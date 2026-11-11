/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/* !
 * \file matmul_server_impl_c310.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/kfc/matmul_server_impl_c310.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul_client.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_IMPL_C310_H__
#endif

#ifndef IMPL_MATMUL_KFC_MATMUL_SERVER_IMPL_C310_H
#define IMPL_MATMUL_KFC_MATMUL_SERVER_IMPL_C310_H

#include "matmul_server.h"

namespace AscendC {
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::Init(MSG_POS KfcMsg *msg)
{
    // C310 get tiling from special addr in ssbuf, no need to flush cacheline
    if constexpr (!ToMatmulConfig(MM_CFG).enableInit) {
        return;
    } else {
        MSG_POS TilingInfo *tilingSSbuf = reinterpret_cast<MSG_POS TilingInfo *>(GetTilingAddr(mul.GetSubBlockIdx()));
        auto temp1 = ((MSG_POS uint64_t *)(&(tilingSSbuf->tCubeTiling)));
        tiling_.SetTiling(&tmpTiling_);
        auto temp2 = (uint64_t *)(&tmpTiling_); // need to be same as c220
        for (int i = 0; i < sizeof(TCubeTiling) / sizeof(uint64_t); i++, temp1++, temp2++) {
            *temp2 = *temp1;
        }
        tilingSSbuf->valid = 0; // set ssbuf ok flag for next mm init 
        mul.Init(&tmpTiling_, nullptr);
        InitL1Addr();
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::GetOffsetSize(
    MsgTmpPos MatmulConfigParams *body, KFC_Enum funID, uint32_t sync, uint64_t &offsetSize, uint32_t &enSequentialWrite, bool hasSetWorkspace)
{
    bool isIterate = (funID == KFC_Enum::MMFUN_ITERATE);
    if constexpr ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_ALL) != 0) {
        if constexpr (GetPhyType(C_TYPE::pos) == Hardware::UB) {
            // On the scene of (defaultMode + iterate + async + setWorkspace + outUb)
            if constexpr (ToMatmulConfig(MM_CFG).iterateMode == IterateMode::ITERATE_MODE_DEFAULT) { // for performance 
                if (unlikely(body->cAddr != 0 && isIterate && hasSetWorkspace && sync == 0)) {
                    enSequentialWrite = 1;
                    offsetSize = GetBaseOffsetC<MM_CFG>(enSequentialWrite, tiling_.GetBaseM(), tiling_.GetBaseN());
                }
            }
        } else {
            offsetSize = GetBaseOffsetC<MM_CFG>(enSequentialWrite, tiling_.GetBaseM(), tiling_.GetBaseN());
        }
    } else {
        if constexpr (GetPhyType(C_TYPE::pos) != Hardware::UB) {
            if (sync == 0) {
                // For asynchronous Iterate, the offset must be used for address calculation and the size is
                // baseM x baseN.
                if constexpr (ToMatmulConfig(MM_CFG).baseMN != 0) {
                    offsetSize = ToMatmulConfig(MM_CFG).baseMN;
                } else {
                    offsetSize = tiling_.GetBaseM() * tiling_.GetBaseN();
                }
                enSequentialWrite = 1;
            }
        }
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline bool MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::Iterate(
    MSG_POS KfcMsg* msg, KFC_Enum funID)
{
    if constexpr (A_TYPE::layout != LayoutMode::NONE) {
        return true;
    }

    MatmulConfigParams body;
    GetMsgFromSSbuf(msg, body);
    WaitAB(body);
    IterateSetMessage(msg, &body);

    auto enSequentialWrite = body.enSequentialWrite;
    auto enAtomic = body.enAtomic;
    auto sync = body.sync;
    auto enPartialSum = body.enPartialSum;
    auto hasSetWorkspace = body.hasSetWorkspace;

    GlobalTensor<DstT> cGlobal;
    LocalTensor<DstT> cLocal;
    uint64_t size;
    if constexpr (ToMatmulConfig(MM_CFG).singleCoreMN != 0) {
        size = ToMatmulConfig(MM_CFG).singleCoreMN;
    } else {
        size = tiling_.GetSingleCoreM() * tiling_.GetSingleCoreN();
    }

    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT *>(body.cAddr), size);
    if constexpr (GetPhyType(C_TYPE::pos) == Hardware::L1 || GetPhyType(C_TYPE::pos) == Hardware::UB) {
        cLocal = GetLocalTensor<typename C_TYPE::T, C_TYPE::pos>(body.cAddr, size);
    }

    // calculate offset
    uint64_t offset = 0;
    uint64_t offsetSize = 0;
    GetOffsetSize(&body, funID, sync, offsetSize, enSequentialWrite, hasSetWorkspace);

    // execute iterate
    bool isFirstIterate = true;
    TRACE_START(TraceId::MatMul_CALC);
    // Asynchronous and configure the workspace
    bool isIterate = (funID == KFC_Enum::MMFUN_ITERATE);
    while (mul.Iterate(enPartialSum)) {
        // Process iterateAll and iterate together, save code to decrease icachemiss
        if constexpr ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_NORMAL) != 0) {
            if (isIterate && (msg->body.cAddr == 0 || !hasSetWorkspace)) {
                enPartialSum_ = enPartialSum;
                isSyncIterate_ = sync;
                TRACE_STOP(TraceId::MatMul_CALC);
                if constexpr (A_TYPE::ibShare && B_TYPE::ibShare && IsBasic(ToMatmulConfig(MM_CFG)) &&
                    (GetPhyType(C_TYPE::pos) == Hardware::GM)) {
                    if (sync == 0) {
                        mul.End();
                    }
                }
                return false;  // The queue is not switched, and no message needs to be returned.
            }
        }

        // On the scene of (defaultMode + iterate + async + setWorkspace + outUb)
        if constexpr (GetPhyType(C_TYPE::pos) == Hardware::UB &&
                      ToMatmulConfig(MM_CFG).iterateMode == IterateMode::ITERATE_MODE_DEFAULT) {
            if (unlikely(isIterate && body.cAddr != 0 && hasSetWorkspace && sync == 0)) {
                mul.GetTensorC(cGlobal[offset], (uint8_t)(enAtomic), enSequentialWrite);
                offset += offsetSize;
                CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(
                    GetIntraFlagId(instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
                continue;
            }
        }

        if constexpr ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_ALL) != 0) {
            if constexpr (GetPhyType(C_TYPE::pos) == Hardware::L1) {
                mul.GetTensorC(cLocal[offset], (uint8_t)(enAtomic), enSequentialWrite);
            } else if constexpr (GetPhyType(C_TYPE::pos) == Hardware::UB) {
                if (isFirstIterate) {
                    CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(
                        instID, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
                    if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                        CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(
                            instID + static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) + INTRA_NUM);
                    }
                    isFirstIterate = false;
                }
                mul.GetTensorC(cLocal, (uint8_t)(enAtomic), enSequentialWrite);
            } else {
                mul.GetTensorC(cGlobal[offset], (uint8_t)(enAtomic), enSequentialWrite);
            }
            if constexpr (GetPhyType(C_TYPE::pos) != Hardware::UB) {
                offset += offsetSize;
            }
        }
    }

    // Now release UB, actual is gmï¿?gm->l1
    if constexpr ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_ALL) != 0) {
        if (sync || body.waitIterateAll) {
            ASSERT(funID == KFC_Enum::MMFUN_ITERATE_ALL);
            IterNotify();
        }
    }
    mul.End();
    TRACE_STOP(TraceId::MatMul_CALC);
    return true;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::StartIterateNBatch(
    MsgTmpPos MatmulConfigParams* body, uint32_t &cntIterator)
{
    const uint64_t size = tiling_.GetSingleCoreM() * tiling_.GetSingleCoreN();
    uint64_t singleBatchASize = 0;
    uint64_t singleBatchBSize = 0;
    uint32_t batchC = GetBatchCNum(body->batchA, body->batchB, tiling_.GetALayoutInfoG(), tiling_.GetBLayoutInfoG(), tiling_.GetCLayoutInfoG());
    uint64_t batchOffsetBias = tiling_.GetCLayoutInfoS2() * batchC * sizeof(typename BIAS_TYPE::T);
    BmmOffset batchLoopOffset;
    for (uint32_t loopIdx = 0U; loopIdx < body->batchLoop; loopIdx++) {
        const uint64_t biasOffset = batchOffsetBias * loopIdx;
        batchLoopOffset.offA = CalcNBatchoffset<A_TYPE>(body->batchA, loopIdx, tiling_.GetALayoutInfoN(), tiling_.GetALayoutInfoG(), tiling_.GetALayoutInfoD(), tiling_.GetALayoutInfoS());
        batchLoopOffset.offB = CalcNBatchoffset<B_TYPE>(body->batchB, loopIdx, tiling_.GetBLayoutInfoN(), tiling_.GetBLayoutInfoG(), tiling_.GetBLayoutInfoD(), tiling_.GetBLayoutInfoS());
        batchLoopOffset.offC = CalcNBatchoffset<C_TYPE>(batchC, loopIdx, tiling_.GetCLayoutInfoN(), tiling_.GetCLayoutInfoG(), tiling_.GetCLayoutInfoS2(), tiling_.GetCLayoutInfoS1());
        
        IterateSetMessage(body, singleBatchASize, singleBatchBSize, batchLoopOffset.offA, batchLoopOffset.offB, biasOffset);
        GlobalTensor<DstT> cGlobal;
        cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT*>(body->cAddr + batchLoopOffset.offC), size);
        mul.IterateBatch(cGlobal, body->enPartialSum, (uint8_t)(body->enAtomic),
            body->enSequentialWrite, body->matrixStrideA,
            body->matrixStrideB, body->matrixStrideC);
        cntIterator++;
        if (cntIterator < INC_PROCESS_CHECK && (!body->sync && !body->waitIterateBatch)) {
            CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(
                GetIntraFlagId(instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
        }
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline bool MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::GetTensorC(MSG_POS KfcMsg* msg)
{
    if constexpr (A_TYPE::layout != LayoutMode::NONE) {
        return true;
    }
    uint64_t size;
    if constexpr (ToMatmulConfig(MM_CFG).singleCoreMN != 0) {
        size = ToMatmulConfig(MM_CFG).singleCoreMN;
    } else {
        size = tiling_.GetSingleCoreM() * tiling_.GetSingleCoreN();
    }
    uint8_t enAtomic = static_cast<uint8_t>(msg->body.enAtomic);
    bool enSequentialWrite = static_cast<bool>(msg->body.enSequentialWrite);
    if constexpr (NeedTransitByGm(C_TYPE::pos)) {
        GlobalTensor<DstT> cGlobal;
        cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT *>(msg->body.cAddr), size);
        mul.GetTensorC(cGlobal, enAtomic, enSequentialWrite);
    } else {
        const auto& cLocal = GetLocalTensor<typename C_TYPE::T, C_TYPE::pos>(msg->body.cAddr, size);
        // C310 direct l0c->ub need to sync pipe with AIV
        if constexpr (GetPhyType(C_TYPE::pos) == Hardware::UB) {
            CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(
                GetIntraFlagId(instID, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
            if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(instID + static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) +
                                                        INTRA_NUM);
            }
        }
        mul.GetTensorC(cLocal, enAtomic, enSequentialWrite);
    }

    if constexpr (A_TYPE::ibShare && B_TYPE::ibShare && IsBasic(ToMatmulConfig(MM_CFG)) &&
            (GetPhyType(C_TYPE::pos) == Hardware::GM)) {
        // L0c accumulation and async no need to wait copy out, so no need to set fixpipe
        if (isSyncIterate_) {
            CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(
                GetIntraFlagId(instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
            CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(instID + static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP) +
                                                   INTRA_NUM);
        }
    } else {
        CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(
            GetIntraFlagId(instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(instID + static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP) +
                                                   INTRA_NUM);
        }
    }

    if constexpr (GetPhyType(C_TYPE::pos) == Hardware::GM || GetPhyType(C_TYPE::pos) == Hardware::UB) {
        if constexpr (!(IsBasic(ToMatmulConfig(MM_CFG)))) {
            if (isSyncIterate_) {
                return false;
            }
            if (mul.Iterate(enPartialSum_)) {
                return false;
            }
        }
        if constexpr (!(A_TYPE::ibShare && B_TYPE::ibShare && IsBasic(ToMatmulConfig(MM_CFG)) &&
                        (GetPhyType(C_TYPE::pos) == Hardware::GM))) {
            mul.End();
        }
        // No data is available, switch the message queue.
        return true;
    }
    return false;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::GetMsgFromSSbuf(
    MSG_POS KfcMsg* msg, MatmulConfigParams &body)
{
    // copy msg from ssbuf to stack for higher performance, because there is no cache in ssbuf
    uint64_t *ptr = reinterpret_cast<uint64_t *>(&body);
    MSG_POS uint64_t *ptrMsg = reinterpret_cast<MSG_POS uint64_t *>(&(msg->body));

    if constexpr (ToMatmulConfig(MM_CFG).enableQuantVector ||
                 (ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_BATCH) != 0 ||
                 (ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_N_BATCH) != 0 ||
                  HasScalePosition<A_TYPE>::value || HasScalePosition<B_TYPE>::value) {
        RecvSSbufData<15>(ptr, ptrMsg);
    } else if constexpr (ToMatmulConfig(MM_CFG).enableSetBias) {
        RecvSSbufData<9>(ptr, ptrMsg);
    } else if constexpr (ToMatmulConfig(MM_CFG).enableSetTail) {
        RecvSSbufData<8>(ptr, ptrMsg);
    } else if constexpr (ToMatmulConfig(MM_CFG).enableSetOrgShape) {
        RecvSSbufData<7>(ptr, ptrMsg);
    } else {
        RecvSSbufData<4>(ptr, ptrMsg);
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::IterNotify()
{
    if constexpr (GetPhyType(C_TYPE::pos) != Hardware::L1) {
        CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(
            GetIntraFlagId(instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(instID,
                static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), 1U)); // 1 means sub_block 1
        }
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::InitL1Addr()
{
    // need to init first in mm impl to get l1 addr
    if constexpr (GetPhyType(A_TYPE::pos) == Hardware::UB || GetPhyType(B_TYPE::pos) == Hardware::UB ||
        PhyMxScalePosIsUB<A_TYPE>() || PhyMxScalePosIsUB<B_TYPE>() || GetPhyType(BIAS_TYPE::pos) == Hardware::UB) {
        MatrixL1Addr matrixL1Addr = KfcGetMatrixL1Addr(mul);
        MSG_POS MsgMatmulL1Addr *matmulL1AddrMsg =
            (MSG_POS MsgMatmulL1Addr *)GetMatmulL1AddrMsg(mul.GetSubBlockIdx(), instID);
        CopyL1Addr2SSBUF(matmulL1AddrMsg, &matrixL1Addr);
        if constexpr (!ToMatmulConfig(MM_CFG).enableInit) {
            matrixL1Addr = KfcGetMatrixL1Addr(mul);
            matmulL1AddrMsg = (MSG_POS MsgMatmulL1Addr *)GetMatmulL1AddrMsg(1, instID);
            CopyL1Addr2SSBUF(matmulL1AddrMsg, &matrixL1Addr);
        }
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::CopyL1Addr2SSBUF(
    MSG_POS MsgMatmulL1Addr *matmulL1AddrMsg_, MatrixL1Addr *matrixL1Addr_)
{
    if constexpr (GetPhyType(A_TYPE::pos) == Hardware::UB) {
        matmulL1AddrMsg_->l1aAddr = matrixL1Addr_->l1aAddr;
    }
    if constexpr (GetPhyType(B_TYPE::pos) == Hardware::UB) {
        matmulL1AddrMsg_->l1bAddr = matrixL1Addr_->l1bAddr;
    }
    if constexpr (GetPhyType(BIAS_TYPE::pos) == Hardware::UB) {
        matmulL1AddrMsg_->l1biasAddr = matrixL1Addr_->l1biasAddr;
    }
    if constexpr (PhyMxScalePosIsUB<A_TYPE>()) {
        matmulL1AddrMsg_->l1aScaleAddr = matrixL1Addr_->l1aScaleAddr;
    }
    if constexpr (PhyMxScalePosIsUB<B_TYPE>()) {
        matmulL1AddrMsg_->l1bScaleAddr = matrixL1Addr_->l1bScaleAddr;
    }

    matmulL1AddrMsg_->valid = 1;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::WaitAB(
    MatmulConfigParams &body)
{
    // Make sure data copy done from UB->GM
    if constexpr ((GetPhyType(A_TYPE::pos) == Hardware::GM && GetPhyType(A_TYPE::srcPos) == Hardware::UB) ||
        (GetPhyType(B_TYPE::pos) == Hardware::GM && GetPhyType(B_TYPE::srcPos) == Hardware::UB) ||
        (GetPhyType(BIAS_TYPE::pos) == Hardware::GM && GetPhyType(BIAS_TYPE::srcPos) == Hardware::UB)) {
        // GM->L1 wait UB->GM
        CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(
            GetIntraFlagId(0, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::GM_L1_UB_GM), mul.GetSubBlockIdx()));
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            CrossCoreWaitFlag<INTRA_MODE, PIPE_MTE2>(static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::GM_L1_UB_GM) + INTRA_NUM);
        }
    }

    // iterateNorm & iterateDefault only set flag at firstIter, UB in only
    if constexpr (((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_NORMAL) != 0) &&
        (GetPhyType(A_TYPE::pos) == Hardware::UB || GetPhyType(B_TYPE::pos) == Hardware::UB ||
        PhyMxScalePosIsUB<A_TYPE>() || PhyMxScalePosIsUB<B_TYPE>() || GetPhyType(BIAS_TYPE::pos) == Hardware::UB)) {
        if (!body.isFirstIter) {
            return;
        }
    }

    // First iterate need let aiv know, its ok to copy UB->L1
    if constexpr (GetPhyType(A_TYPE::pos) == Hardware::UB || GetPhyType(B_TYPE::pos) == Hardware::UB ||
        PhyMxScalePosIsUB<A_TYPE>() || PhyMxScalePosIsUB<B_TYPE>() || GetPhyType(BIAS_TYPE::pos) == Hardware::UB) {
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            // Only one mm obj in server
            constexpr uint16_t l1SetIdV0 = static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::UB_L1_L1_L0AB);
            constexpr uint16_t l1SetIdV1 = static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::UB_L1_L1_L0AB) + INTRA_NUM;
            CrossCoreSetFlag<INTRA_MODE, PIPE_MTE1>(l1SetIdV0);
            CrossCoreSetFlag<INTRA_MODE, PIPE_MTE1>(l1SetIdV1);
        } else {
            CrossCoreSetFlag<INTRA_MODE, PIPE_MTE1>(
                GetIntraFlagId(0, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::UB_L1_L1_L0AB), mul.GetSubBlockIdx()));
        }
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SetTensorScaleA(
    MatmulConfigParams &body)
{
    const bool isTransScaleA = body.quantMode & 0b01;
    // use addr to send intraId in C310
    if constexpr (PhyMxScalePosIsL1<A_TYPE>()) {
        KfcSetIntraScaleAId(mul, body.quantAddr >> VALID_ADDR_BITS_NUM);
    }
    if constexpr (PhyMxScalePosIsGM<A_TYPE>()) {
        GlobalTensor<fp8_e8m0_t> scaleAGlobal;
        scaleAGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ fp8_e8m0_t *>(body.quantAddr), 0);
        mul.SetTensorScaleA(scaleAGlobal, isTransScaleA);
    } else if constexpr (PhyMxScalePosIsUB<A_TYPE>() || PhyMxScalePosIsL1<A_TYPE>()) {
        const auto &scaleALocal = GetLocalTensor<fp8_e8m0_t, A_TYPE::scalePosition>(body.quantAddr, 0);
        mul.SetTensorScaleA(scaleALocal, isTransScaleA);
        return;
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SetTensorScaleB(
    MatmulConfigParams &body)
{
    const bool isTransScaleB = (body.quantMode >> 1) & 0b01;
    // use addr to send intraId in C310
    if constexpr (PhyMxScalePosIsL1<B_TYPE>()) {
        KfcSetIntraScaleBId(mul, body.quantScalar >> VALID_ADDR_BITS_NUM);
    }
    if constexpr (PhyMxScalePosIsGM<B_TYPE>()) {
        GlobalTensor<fp8_e8m0_t> scaleBGlobal;
        scaleBGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ fp8_e8m0_t *>(body.quantScalar), 0);
        mul.SetTensorScaleB(scaleBGlobal, isTransScaleB);
    } else if constexpr (PhyMxScalePosIsUB<B_TYPE>() || PhyMxScalePosIsL1<B_TYPE>()) {
        const auto &scaleBLocal = GetLocalTensor<fp8_e8m0_t, B_TYPE::scalePosition>(body.quantScalar, 0);
        mul.SetTensorScaleB(scaleBLocal, isTransScaleB);
        return;
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline bool MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::IterateBatch(MSG_POS KfcMsg* msg)
{
    if constexpr (A_TYPE::layout == LayoutMode::NONE) {
        return true;
    }

    MatmulConfigParams tmpBody;
    GetMsgFromSSbuf(msg, tmpBody);
    auto *body = &tmpBody;
    WaitAB(*body);

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    // quant mode is not supported for quantization parameters in Batch MM
    ASSERT(body->setQuant != 1);
#endif

    IterateSetMessage(msg, body);
    uint64_t size = tiling_.GetSingleCoreM() * tiling_.GetSingleCoreN();

    if constexpr (NeedTransitByGm(C_TYPE::pos)) {
        GlobalTensor<DstT> cGlobal;
        cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT *>(body->cAddr), size);
        mul.IterateBatch(cGlobal,body->enPartialSum, (uint8_t)(body->enAtomic),body->enSequentialWrite,
            body->matrixStrideA, body->matrixStrideB, body->matrixStrideC);
        mul.End();
    }
    
    if constexpr (GetPhyType(C_TYPE::pos) == Hardware::UB) {
        LocalTensor<DstT> cLocal = GetLocalTensor<typename C_TYPE::T, C_TYPE::pos>(body->cAddr, size);
        CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(
            GetIntraFlagId(instID, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(instID + static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP) + 
                                                    INTRA_NUM);
        }
        mul.IterateBatch(cLocal,body->enPartialSum, (uint8_t)(body->enAtomic), body->enSequentialWrite,
                         body->matrixStrideA, body->matrixStrideB, body->matrixStrideC);
        mul.End();
    }
    if (body->sync || body->waitIterateBatch) {
        IterNotify();
    }
    return true;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline bool MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::IterateNBatch(MSG_POS KfcMsg* msg)
{
    if constexpr (!ToMatmulConfig(MM_CFG).isNBatch) {
        return true;
    }
    MatmulConfigParams body;
    GetMsgFromSSbuf(msg, body);
    WaitAB(body);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    // quant mode is not supported for quantization parameters in Batch MM
    ASSERT(body.setQuant != 1);
#endif
    uint32_t cntIterator = 0;
    StartIterateNBatch(&body, cntIterator);
    if (body.sync || body.waitIterateBatch) {
        CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(
            GetIntraFlagId(instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
    } else if (cntIterator >= INC_PROCESS_CHECK) {
        CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(
            GetIntraFlagId(instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), mul.GetSubBlockIdx()));
    }
    return true;
}

#if defined(__ASCENDC_ENABLE_SUPER_KERNEL__)
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SuperKernelEventCount(uint16_t eventID)
{
    (void)eventID;
}
#endif
} // namespace AscendC
#endif // IMPL_MATMUL_KFC_MATMUL_SERVER_IMPL_C310_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_IMPL_C310_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_IMPL_C310_H__
#endif