/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/*!
 * \file matmul_server_impl_c220.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/kfc/matmul_server_impl_c220.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul_client.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_IMPL_C220_H__
#endif

#ifndef IMPL_MATMUL_KFC_MATMUL_SERVER_IMPL_C220_H
#define IMPL_MATMUL_KFC_MATMUL_SERVER_IMPL_C220_H

#include "matmul_server.h"

namespace AscendC {
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::GetOffsetSize(
    MsgTmpPos MatmulConfigParams* body, KFC_Enum funID, uint32_t sync, uint64_t &offsetSize,
    uint32_t &enSequentialWrite, bool hasSetWorkspace)
{
    if constexpr ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_NORMAL) == 0) {
        ASSERT(body->cAddr != 0); // The output address must be configured.
        if constexpr (ToMatmulConfig(MM_CFG).baseMN != 0) {
            offsetSize = enSequentialWrite ? ToMatmulConfig(MM_CFG).baseMN : 0;
        } else {
            offsetSize = enSequentialWrite ? (tiling_.GetBaseM() * tiling_.GetBaseN()) : 0;
        }
    } else {
        if (funID == KFC_Enum::MMFUN_ITERATE_ALL) {
            ASSERT(body->cAddr != 0); // The output address must be configured.
            if constexpr (ToMatmulConfig(MM_CFG).baseMN != 0) {
                offsetSize = enSequentialWrite ? ToMatmulConfig(MM_CFG).baseMN : 0;
            } else {
                offsetSize = enSequentialWrite ? (tiling_.GetBaseM() * tiling_.GetBaseN()) : 0;
            }
        } else if (sync == 0) {
            // For asynchronous Iterate, the offset must be used for address calculation and
            // the size is baseM x baseN.
            if constexpr (ToMatmulConfig(MM_CFG).baseMN != 0) {
                offsetSize = ToMatmulConfig(MM_CFG).baseMN;
            } else {
                offsetSize = tiling_.GetBaseM() * tiling_.GetBaseN();
            }
            enSequentialWrite = 1;
        }
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline bool MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::StartIterate(
    MsgTmpPos MatmulConfigParams* body, KFC_Enum funID, uint32_t sync, uint32_t &cntIterator)
{
    uint64_t size;
    if constexpr (ToMatmulConfig(MM_CFG).singleCoreMN != 0) {
        size = ToMatmulConfig(MM_CFG).singleCoreMN;
    } else {
        size = tiling_.GetSingleCoreM() * tiling_.GetSingleCoreN();
    }

    GlobalTensor<DstT> cGlobal;
    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT*>(body->cAddr), size);
    LocalTensor<DstT> cLocal;
    if constexpr (PhyPosIsL1(C_TYPE::pos)) {
        cLocal = GetLocalTensor<typename C_TYPE::T, C_TYPE::pos>(body->cAddr, size);
    }
    uint64_t offset = 0;
    uint64_t offsetSize = 0;
    auto enSequentialWrite = body->enSequentialWrite;
    auto enAtomic = body->enAtomic;
    auto enPartialSum = body->enPartialSum;
    GetOffsetSize(body, funID, sync, offsetSize, enSequentialWrite);
    TRACE_START(TraceId::MatMul_CALC);
    // Asynchronous and configure the workspace
    while (mul.Iterate(enPartialSum)) {
        if constexpr ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_NORMAL) != 0) {
            if (unlikely(cntIterator == 0) && unlikely(funID == KFC_Enum::MMFUN_ITERATE && sync == 1)) {
                TRACE_STOP(TraceId::MatMul_CALC);
                return false; // The queue is not switched, and no message needs to be returned.
            }
        }
        if constexpr (PhyPosIsL1(C_TYPE::pos)) {
            mul.GetTensorC(cLocal[offset], (uint8_t)(enAtomic), enSequentialWrite);
        } else {
            mul.GetTensorC(cGlobal[offset], (uint8_t)(enAtomic), enSequentialWrite);
        }
        cntIterator++;
        if constexpr ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_NORMAL) != 0) {
            if (cntIterator < INC_PROCESS_CHECK && funID == KFC_Enum::MMFUN_ITERATE) {
                uint16_t eventID = static_cast<uint16_t>(this->devEvtID * 2 + mul.GetSubBlockIdx());
                NotifyEvent<PIPE_FIX>(eventID);
#if defined(__ASCENDC_ENABLE_SUPER_KERNEL__)
                SuperKernelEventCount(eventID);
#endif
            }
        }
        offset += offsetSize;
    }
    return true;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::Init(MSG_POS KfcMsg* msg)
{
    if constexpr (!ToMatmulConfig(MM_CFG).enableInit) {
        return;
    }
    ASSERT(msg != nullptr && "msg cannot be nullptr when init matmul server");
    ASSERT(msg->tilingInfo.tilingAddr != nullptr && "tiling cannot be nullptr when init matmul server");
    auto temp1 = ((__gm__ uint32_t*)(msg->tilingInfo.tilingAddr));
    tiling_.SetTiling(&tmpTiling_);
    auto temp2 = (uint32_t*)(tiling_.GetTiling());

    constexpr uint32_t tCubeTilingSize = ConstCeil(sizeof(TCubeTiling), CACHE_LINE_SIZE) * CACHE_LINE_SIZE;
    GlobalTensor<int64_t> tilingGlobal;
    for (int i = 0; i < tCubeTilingSize; i += CACHE_LINE_SIZE) {
        Barrier();
        tilingGlobal.SetGlobalBuffer((__gm__ int64_t *)(msg->tilingInfo.tilingAddr + i));
        DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(tilingGlobal);
    }

    for (int i = 0; i < sizeof(TCubeTiling) / sizeof(uint32_t); i++, temp1++, temp2++) {
        *temp2 = *temp1;
    }
    mul.Init(this->tiling_.GetTiling(), nullptr);
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SetOrgShape(MSG_POS KfcMsg* msg)
{
    if constexpr (!ToMatmulConfig(MM_CFG).enableInit) {
        if (mul.GetSubBlockIdx() == 0) {
            msgAux.msg0.orgM = msg->orgShape.orgM;
            msgAux.msg0.orgN = msg->orgShape.orgN;
            msgAux.msg0.orgKa = msg->orgShape.orgKa;
            msgAux.msg0.orgKb = msg->orgShape.orgKb;
            msgAux.msg0.orgKc = msg->orgShape.orgKc;
            msgAux.msg0.setOrgShape = true;
        } else {
            msgAux.msg1.orgM = msg->orgShape.orgM;
            msgAux.msg1.orgN = msg->orgShape.orgN;
            msgAux.msg1.orgKa = msg->orgShape.orgKa;
            msgAux.msg1.orgKb = msg->orgShape.orgKb;
            msgAux.msg1.orgKc = msg->orgShape.orgKc;
            msgAux.msg1.setOrgShape = true;
        }
    } else {
        mul.SetOrgShape(msg->orgShape.orgM, msg->orgShape.orgN, msg->orgShape.orgKa, msg->orgShape.orgKb,
            msg->orgShape.orgKc);
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
    if constexpr (ToMatmulConfig(MM_CFG).baseMN != 0) {
        size = ToMatmulConfig(MM_CFG).baseMN;
    } else {
        size = tiling_.GetBaseM() * tiling_.GetBaseN();
    }
    if constexpr (PhyPosIsL1(C_TYPE::pos)) {
        const auto& cLocal = GetLocalTensor<typename C_TYPE::T, C_TYPE::pos>(msg->body.cAddr, size);
        mul.GetTensorC(cLocal, (uint8_t)(msg->body.enAtomic), msg->body.enSequentialWrite);
    } else {
        GlobalTensor<DstT> cGlobal;
        cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT*>(msg->body.cAddr), size);
        mul.GetTensorC(cGlobal, (uint8_t)(msg->body.enAtomic), msg->body.enSequentialWrite);
    }
    // Now release UB
    if constexpr (PhyPosIsUB(C_TYPE::pos)) {
        if (unlikely(msg->ubAddr >= 0)) {
            kfcCommSrv->FreeUB(msg->ubAddr);
        }
    }
    if (msg->body.sync == 1) { // Synchronize
        uint16_t eventID = static_cast<uint16_t>(this->devEvtID * 2 + mul.GetSubBlockIdx());
        NotifyEvent<PIPE_FIX>(eventID);
    }
    return false;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline bool MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::IterateBatch(MSG_POS KfcMsg* msg)
{
    if constexpr (A_TYPE::layout == LayoutMode::NONE) {
        return true;
    }
    // In the batch scenario, messages occupy 128 bytes. After the update, messages occupy 64 bytes.
    GlobalTensor<int64_t> msgGlobalTensor;
    msgGlobalTensor.SetGlobalBuffer(reinterpret_cast<__gm__ int64_t*>(msg) + sizeof(int64_t));
    DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(msgGlobalTensor);
    __gm__ auto *body = &(msg->body);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if (body->setQuant == 1) {
        ASSERT(body->quantMode != 1);  // scalar mode is not supported for quantization parameters in
        // Batch MM
    }
#endif
    IterateSetMessage(msg, body);
    uint64_t size = tiling_.GetSingleCoreM() * tiling_.GetSingleCoreN();

    GlobalTensor<DstT> cGlobal;
    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT*>(body->cAddr), size);
    mul.IterateBatch(cGlobal, body->enPartialSum, (uint8_t)(body->enAtomic),body->enSequentialWrite,
                    body->matrixStrideA, body->matrixStrideB, body->matrixStrideC);
    mul.End();

    // Now release UB
    if constexpr (PhyPosIsUB(A_TYPE::pos) || PhyPosIsUB(B_TYPE::pos) ||
        PhyPosIsUB(BIAS_TYPE::pos) || PhyPosIsUB(C_TYPE::pos)) {
        if (unlikely(msg->ubAddr >= 0)) {
            kfcCommSrv->FreeUB(msg->ubAddr);
        }
    }
    if (body->sync || body->waitIterateBatch) {
        uint16_t eventID = static_cast<uint16_t>(this->devEvtID * 2 + mul.GetSubBlockIdx());
        NotifyEvent<PIPE_FIX>(eventID);
    }
    return true;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::StartIterateNBatch(
    MsgTmpPos MatmulConfigParams* body, uint32_t &cntIterator)
{
    const uint64_t size = tiling_.GetSingleCoreM() * tiling_.GetSingleCoreN();
    uint64_t singleBatchASize = (uint64_t)(body->sizeAmatrix) / body->batchLoop;
    uint64_t singleBatchBSize = (uint64_t)(body->sizeBmatrix) / body->batchLoop;
    uint32_t batchC = body->batchA > body->batchB ? body->batchA : body->batchB;
    bool layoutGCondition = tiling_.GetCLayoutInfoG() == 1 &&
                            (tiling_.GetBLayoutInfoG() != 1 || tiling_.GetALayoutInfoG() != 1);
    if (layoutGCondition) {
        int32_t layoutG = tiling_.GetBLayoutInfoG() > tiling_.GetALayoutInfoG() ? tiling_.GetBLayoutInfoG() : tiling_.GetALayoutInfoG();
        batchC = batchC / layoutG;
    }
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
            uint16_t eventID = static_cast<uint16_t>(this->devEvtID * 2 + mul.GetSubBlockIdx());
            NotifyEvent<PIPE_FIX>(eventID);
        }
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline bool MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::IterateNBatch(MSG_POS KfcMsg* msg)
{
    if constexpr (!ToMatmulConfig(MM_CFG).isNBatch) {
        return true;
    }
    GlobalTensor<int64_t> msgGlobal;
    msgGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int64_t*>(msg) + sizeof(int64_t));
    DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(msgGlobal);
    __gm__ auto *body = &(msg->body);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if (msg->body.setQuant == 1) {
        ASSERT(msg->body.quantMode != 1);  // scalar mode is not supported for quantization parameters in
        // Batch MM
    }
#endif
    uint32_t cntIterator = 0;
    StartIterateNBatch(body, cntIterator);
    // Now release UB
    if constexpr (PhyPosIsUB(A_TYPE::pos) || PhyPosIsUB(B_TYPE::pos) ||
        PhyPosIsUB(BIAS_TYPE::pos) || PhyPosIsUB(C_TYPE::pos)) {
        if (unlikely(msg->ubAddr >= 0)) {
            kfcCommSrv->FreeUB(msg->ubAddr);
        }
    }
    uint16_t eventID = static_cast<uint16_t>(this->devEvtID * 2 + mul.GetSubBlockIdx());
    if (msg->body.sync || msg->body.waitIterateBatch) {
        NotifyEvent<PIPE_FIX>(eventID);
    } else if (cntIterator >= INC_PROCESS_CHECK) {
        NotifyEvent<PIPE_FIX>(eventID);
    }
    return true;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline bool MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::Iterate(
    MSG_POS KfcMsg* msg, KFC_Enum funID)
{
    if constexpr (A_TYPE::layout != LayoutMode::NONE) {
        return true;
    }
    if constexpr ((A_TYPE::ibShare == true) || (B_TYPE::ibShare == true)) {
        if (msg->body.iterateFakeMsg) {
            if (funID == KFC_Enum::MMFUN_ITERATE_ALL) { // fake msg
                uint16_t eventID = static_cast<uint16_t>(this->devEvtID * 2 + kfcCommSrv->subBlockID);
                NotifyEvent<PIPE_FIX>(eventID);
#if defined(__ASCENDC_ENABLE_SUPER_KERNEL__)
                SuperKernelEventCount(eventID);
#endif
                return true;
            }
        }
    } else {
        ASSERT(!msg->body.iterateFakeMsg &&"Only Ib share mode support fake msg.");
    }
    SyncCubeWithVec<A_TYPE::ibShare, B_TYPE::ibShare>();
    QuantCacheRefresh(msg);
    IterateSetMessage(msg, &(msg->body));
    uint32_t cntIterator = 0;
    auto sync = msg->body.sync;
    if(!StartIterate(&(msg->body), funID, sync, cntIterator)) {
        return false;
    }
    // Now release UB
    if constexpr (PhyPosIsUB(A_TYPE::pos) || PhyPosIsUB(B_TYPE::pos) ||
        PhyPosIsUB(BIAS_TYPE::pos) || PhyPosIsUB(C_TYPE::pos)) {
        if (unlikely(msg->ubAddr >= 0)) {
            kfcCommSrv->FreeUB(msg->ubAddr);
        }
    }

    uint16_t eventID = static_cast<uint16_t>(this->devEvtID * 2 + mul.GetSubBlockIdx());
    if (sync || msg->body.waitIterateAll) {
        ASSERT(funID == KFC_Enum::MMFUN_ITERATE_ALL);
        NotifyEvent<PIPE_FIX>(eventID);
#if defined(__ASCENDC_ENABLE_SUPER_KERNEL__)
        SuperKernelEventCount(eventID);
#endif
    } else if (cntIterator >= INC_PROCESS_CHECK && funID == KFC_Enum::MMFUN_ITERATE) {
        NotifyEvent<PIPE_FIX>(eventID);
#if defined(__ASCENDC_ENABLE_SUPER_KERNEL__)
        SuperKernelEventCount(eventID);
#endif
    }
    mul.End();
    TRACE_STOP(TraceId::MatMul_CALC);
    return true;
}

#if defined(__ASCENDC_ENABLE_SUPER_KERNEL__)
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SuperKernelEventCount(uint16_t eventID)
{
    auto msgRcvStart = reinterpret_cast<__gm__ int64_t *>(kfcCommSrv->GetSecondBuffStart());
    auto superKernelEvent = reinterpret_cast<__gm__ SuperKernelWaitEventCnt *>(msgRcvStart);
    int32_t count = superKernelEvent->eventCnt[this->devEvtID] + 1;
    superKernelEvent->eventId[this->devEvtID] = eventID;
    superKernelEvent->eventCnt[this->devEvtID] = count;
}
#endif
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_IMPL_C220_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_IMPL_C220_H__
#endif // IMPL_MATMUL_KFC_MATMUL_SERVER_IMPL_C220_H