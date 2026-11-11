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
 * \file matmul_server.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/kfc/matmul_server.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul_client.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_H__
#endif

#ifndef IMPL_MATMUL_KFC_MATMUL_SERVER_H
#define IMPL_MATMUL_KFC_MATMUL_SERVER_H

#include "kernel_operator_common_intf_impl.h"
#include "kernel_operator_cache_intf.h"
#include "matmul_server_utils.h"
#include "../utils/matmul_config_utils.h"
#include "../utils/matmul_utils.h"

namespace AscendC {

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG = CFG_NORM,
    class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
class MatmulService {
    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using SrcT = typename A_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;
    using TILING_TYPE = typename std::remove_cv<typename std::remove_reference<decltype(MM_CFG)>::type>::type;
#if defined(USE_SSBUF)
    using IMPL = MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;
    using UserDefDataType = typename MATMUL_POLICY<MM_CFG, IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>::UserDefDataType;
#endif
public:
    __aicore__ inline MatmulService() {}
    template <class T>
    __aicore__ inline void InitKfc(TPipe* tpipe, T* tiling, KFC_COMM_SERVER_PTR kfc, int32_t instID, GM_ADDR workspace)
    {
        ASSERT(instID >= 0 && "instID should be not less than 0 when init kfc matmul server");
        this->instID = instID;
        if constexpr (!ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            ASSERT(kfc != nullptr && "kfc cannot be nullptr when init kfc matmul server");
            this->kfcCommSrv = kfc;
            mul.SetSubBlockIdx(kfcCommSrv->subBlockID);
#if defined(USE_WORKSPACE)
            ASSERT(workspace != nullptr && "workspace cannot be nullptr when init kfc matmul server");
            this->workspace = workspace;
            if constexpr (!ToMatmulConfig(MM_CFG).enableInit) {
                msgAux.msg0.setOrgShape = false;
                msgAux.msg1.setOrgShape = false;
            }
#endif
            this->devEvtID = instID;
            // A and B both enable ibShare no need to use cache 
            if constexpr ((A_TYPE::ibShare || B_TYPE::ibShare) && !(A_TYPE::ibShare && B_TYPE::ibShare)) {
                if (kfcCommSrv->subBlockID == 0) {
                    gCache.Init();
                }
            }
        } else {
            mul.SetSubBlockIdx(0);
        }
        if constexpr (IsSameTypeV<TILING_TYPE, MatmulApiStaticTiling>) {
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
            if constexpr (IsSameTypeV<T, const __gm__ TCubeTiling>) {
                TCubeTiling cubeTiling;
                CopyTiling<A_TYPE, B_TYPE, MM_CFG>(tiling, cubeTiling);
                tiling_.SetTiling(&cubeTiling);
                mul.Init(tiling_.GetTiling(), nullptr);
            } else {
#endif
                tiling_.SetTiling((TCubeTiling *)tiling);
                mul.Init(tiling_.GetTiling(), nullptr);
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
            }
#endif
#if defined(USE_SSBUF)
            InitL1Addr();
#endif
        } else if (tiling) {
            tiling_.SetTiling((TCubeTiling *)tiling);
            mul.Init(tiling_.GetTiling(), nullptr);
#if defined(USE_SSBUF)
            InitL1Addr();
#endif
        }
    }
    __aicore__ inline void Init(MSG_POS KfcMsg* msg);
    __aicore__ inline void SetSubBlockIdx(uint8_t idx)
    {
        mul.SetSubBlockIdx(idx);
    }

    __aicore__ inline void SetOrgShape(MSG_POS KfcMsg* msg);
    __aicore__ inline void SetSingleShape(__gm__ KfcMsg* msg)
    {
        if (msg->body.setTail) {
            mul.SetSingleShape(msg->body.singleM, msg->body.singleN, msg->body.singleK);
        }
    }

    __aicore__ inline void SetTail(MsgTmpPos MatmulConfigParams* body)
    {
        if (body->setTail) {
            mul.SetTail(body->singleM, body->singleN, body->singleK);
        }
    }

    __aicore__ inline void SetHF32(MSG_POS KfcMsg* msg)
    {
        mul.SetHF32(static_cast<bool>(msg->body.enHF32), static_cast<int32_t>(msg->body.hf32TransMode));
    }

    __aicore__ inline void SetTensorA(MsgTmpPos MatmulConfigParams* body);
    __aicore__ inline void SetTensorA(MsgTmpPos MatmulConfigParams* body, const uint64_t size, const uint64_t offset);
    __aicore__ inline void SetQuantVector(MsgTmpPos MatmulConfigParams* body)
    {
        if (!body->setQuant) {
            return;
        }
        int quantMode = body->quantMode;
        if (quantMode == 1) {
            uint64_t quantScalar = body->quantScalar;
            mul.SetQuantScalar(quantScalar);
        } else if (quantMode == 2) {
            const uint64_t size = static_cast<uint64_t>(body->quantSize);
            GlobalTensor<uint64_t> quantGlobal;
            quantGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ uint64_t*>(body->quantAddr), size);
            mul.SetQuantVector(quantGlobal);
        } else if (quantMode == 3) {
            const uint64_t size = static_cast<uint64_t>(body->quantSize);
            LocalTensor<uint64_t> quantL1 = GetLocalTensor<uint64_t, TPosition::TSCM>(body->quantAddr, size);
            mul.SetQuantVector(quantL1);
        }
    }
    __aicore__ inline void SetBatchNum(MsgTmpPos MatmulConfigParams* body)
    {
        if constexpr (A_TYPE::layout == LayoutMode::NONE) {
            return;
        }
        if (!body->setBatch) {
            return;
        }
        mul.SetBatchNum(body->batchA, body->batchB);
    }

#if defined(USE_SSBUF)
    __aicore__ inline void SetUserDefInfo(MSG_POS KfcMsg* msg)
    {
        if (msg->userCustomData == 1) {
            mul.SetUserDefInfo(msg->userDefInfo.tilingPtr);
            return;
        }
        UserDefDataType userData;
        uint32_t *ptr = reinterpret_cast<uint32_t *>(&userData);
        MSG_POS uint32_t *ptrMsg = reinterpret_cast<MSG_POS uint32_t *>(&(msg->body));
        for (int i = 0; i < sizeof(UserDefDataType) / sizeof(uint32_t); i++) {
            *(ptr + i) = *(ptrMsg + i);
        }

        mul.SetSelfDefineData(userData);
    }

    __aicore__ inline void SetSelfDefineData(MSG_POS KfcMsg* msg, MsgTmpPos MatmulConfigParams* body)
    {
        if (body->userInfoType == 0) {
            return;
        }
        UserDefDataType userData;
        uint32_t *ptr = reinterpret_cast<uint32_t *>(&userData);
        if constexpr (sizeof(UserDefDataType) == 4) {
            *ptr = msg->userCustomData;
        } else if constexpr (sizeof(UserDefDataType) == 8) {
            *ptr = msg->userCustomData;
            *(ptr + 1) = body->userCustomData;
        }
        mul.SetSelfDefineData(userData);
    }
#else
    __aicore__ inline void SetSelfDefineData(__gm__ KfcMsg* msg)
    {
        GlobalTensor<int64_t> msgGlobal;
        msgGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int64_t*>(msg) + sizeof(int64_t));
        DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(msgGlobal);
        mul.SetSelfDefineData(msg->body.dataPtr);
        if constexpr (!ToMatmulConfig(MM_CFG).enableReuse) {
            GlobalTensor<uint32_t> dataGlobal;
            dataGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ uint32_t*>(msg->body.dataPtr));
            DataCacheCleanAndInvalid<uint32_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(dataGlobal);
        }
    }

    __aicore__ inline void SetUserDefInfo(__gm__ KfcMsg* msg)
    {
        mul.SetUserDefInfo(msg->userDefInfo.tilingPtr);
    }
#endif

    __aicore__ inline void SetTensorB(MsgTmpPos MatmulConfigParams* body);
    __aicore__ inline void SetTensorB(MsgTmpPos MatmulConfigParams* body, const uint64_t size, const uint64_t offset);
    __aicore__ inline void SetBias(MsgTmpPos MatmulConfigParams* body);
    __aicore__ inline void SetBias(MsgTmpPos MatmulConfigParams* body, const uint64_t offset);
    __aicore__ inline bool GetTensorC(MSG_POS KfcMsg* msg);
    __aicore__ inline uint16_t GetInstID()
    {
        return instID;
    }
    __aicore__ inline void IterateSetMessage(MSG_POS KfcMsg* msg, MsgTmpPos MatmulConfigParams* body)
    {
#if defined(USE_WORKSPACE)
        if constexpr (!ToMatmulConfig(MM_CFG).enableInit) {
            if (mul.GetSubBlockIdx() == 0 && msgAux.msg0.setOrgShape) {
                mul.SetOrgShape(msgAux.msg0.orgM, msgAux.msg0.orgN, msgAux.msg0.orgKa,
                    msgAux.msg0.orgKb, msgAux.msg0.orgKc);
            } else if (mul.GetSubBlockIdx() == 1 && msgAux.msg1.setOrgShape) {
                mul.SetOrgShape(msgAux.msg1.orgM, msgAux.msg1.orgN, msgAux.msg1.orgKa,
                    msgAux.msg1.orgKb, msgAux.msg1.orgKc);
            }
        }
#endif
        if (body->isFirstIter) {
            SetTensorA(body);
            SetTensorB(body);
            if constexpr (ToMatmulConfig(MM_CFG).enableSetBias) {
                SetBias(body);
            }
            if constexpr (ToMatmulConfig(MM_CFG).enableSetTail) {
                SetTail(body);
            }
            if constexpr (ToMatmulConfig(MM_CFG).enableQuantVector) {
                SetQuantVector(body);
            }
            if constexpr (((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_BATCH) != 0) ||
                ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_N_BATCH) != 0)) {
                if constexpr (A_TYPE::layout != LayoutMode::NONE) {
                    SetBatchNum(body);
                }
            }
#if defined(USE_SSBUF)
            if constexpr (ToMatmulConfig(MM_CFG).enableSetOrgShape) {
                if (body->setOrgShape) {
                    mul.SetOrgShape(body->orgM, body->orgN, body->orgKa, body->orgKb, body->orgKc);
                }
            }
            if constexpr (HasScalePosition<A_TYPE>::value) {
                SetTensorScaleA(*body);
            }
            if constexpr (HasScalePosition<B_TYPE>::value) {
                SetTensorScaleB(*body);
            }
            if constexpr (ToMatmulConfig(MM_CFG).enableSetDefineData) {
                SetSelfDefineData(msg, body);
            }
#else
            if constexpr (ToMatmulConfig(MM_CFG).enableSetDefineData) {
                SetSelfDefineData(msg);
            }
#endif
        }
    }

    __aicore__ inline void IterateSetMessage(MsgTmpPos MatmulConfigParams* body, const uint64_t batchASize, const uint64_t batchBSize,
        const uint64_t offsetA = 0, const uint64_t offsetB = 0, const uint64_t offsetBias = 0)
    {
        if (body->isFirstIter) {
            SetTensorA(body, batchASize, offsetA);
            SetTensorB(body, batchBSize, offsetB);
            SetBias(body, offsetBias);
            SetTail(body);
            SetQuantVector(body);
            if constexpr (A_TYPE::layout != LayoutMode::NONE) {
                SetBatchNum(body);
            }
        }
    }
    __aicore__ inline bool IterateBatch(MSG_POS KfcMsg* msg);
    __aicore__ inline void StartIterateNBatch(MsgTmpPos MatmulConfigParams* body, uint32_t &cntIterator);
    __aicore__ inline bool IterateNBatch(MSG_POS KfcMsg* msg);
    __aicore__ inline void GetOffsetSize(MsgTmpPos MatmulConfigParams* body, KFC_Enum funID, uint32_t sync,
        uint64_t &offsetSize, uint32_t &enSequentialWrite, bool hasSetWorkspace = false);
    __aicore__ inline bool StartIterate(MsgTmpPos MatmulConfigParams* body, KFC_Enum funID, uint32_t sync, uint32_t &cntIterator);
    __aicore__ inline bool Iterate(MSG_POS KfcMsg* msg, KFC_Enum funID);
#if defined(__ASCENDC_ENABLE_SUPER_KERNEL__)
    __aicore__ inline void SuperKernelEventCount(uint16_t eventID);
#endif
    __aicore__ inline void QuantCacheRefresh(__gm__ KfcMsg* msg)
    {
        if constexpr (((IsSameType<SrcT, int4b_t>::value || IsSameType<SrcT, int8_t>::value) && IsSameType<DstT, half>::value) ||
            ((IsSameType<SrcT, half>::value || IsSameType<SrcT, bfloat16_t>::value) && IsSameType<DstT, int8_t>::value) ||
            (IsSameType<SrcT, int8_t>::value && (IsSameType<DstT, uint8_t>::value || IsSameType<DstT, int8_t>::value))) {
            GlobalTensor<int64_t> msgGlobal;
            msgGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int64_t*>(msg) + sizeof(int64_t));
            DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(msgGlobal);
        }
    }

    __aicore__ inline bool IterateIntraBlockPartSum(MSG_POS KfcMsg* msg, KFC_Enum funID)
    {
        if constexpr (A_TYPE::layout != LayoutMode::NONE) {
            return true;
        }
        QuantCacheRefresh(msg);
        IterateSetMessage(msg, &(msg->body));
        if (mul.GetSubBlockIdx() == 0) {
            return true;
        }
        uint64_t size;
        if constexpr (ToMatmulConfig(MM_CFG).singleCoreMN != 0) {
            size = ToMatmulConfig(MM_CFG).singleCoreMN;
        } else {
            size = tiling_.GetSingleCoreM() * tiling_.GetSingleCoreN();
        }

        GlobalTensor<DstT> cGlobal;
        cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT*>(msg->body.cAddr), size);
        mul.IterateAll(cGlobal, msg->body.enAtomic, msg->body.enSequentialWrite,
            msg->body.waitIterateAll, msg->body.iterateFakeMsg);

        uint16_t eventID0 = static_cast<uint16_t>(this->devEvtID * 2 + 0);
        uint16_t eventID1 = static_cast<uint16_t>(this->devEvtID * 2 + 1);
        if (msg->body.sync || msg->body.waitIterateAll) {
            ASSERT(funID == KFC_Enum::MMFUN_ITERATE_ALL);
            NotifyEvent<PIPE_FIX>(eventID0);
            NotifyEvent<PIPE_FIX>(eventID1);
        }
        if (!msg->body.iterateFakeMsg) {
            mul.End();
        }
        TRACE_STOP(TraceId::MatMul_CALC);
        return true;
    }

    __aicore__ inline bool IsSharedObj()
    {
        if constexpr (!ToMatmulConfig(MM_CFG).enableInit || ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            return true;
        }
        return false;
    }

    __aicore__ inline bool IsEnableMixHdAbility()
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            return true;
        }
        return false;
    }

    template <uint8_t enableHardPoll = 0>
    __aicore__ inline bool SkipMsg(KFC_Enum funID, bool &freeMsg, int &lastMsgId, const int subBlockID)
    {
#if defined(USE_SSBUF)
        if constexpr (enableHardPoll == 1) {
            return false;
        }
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            if (lastMsgId == subBlockID) {
                freeMsg = false;
                return true;
            } else if (subBlockID == 1) {
                lastMsgId = 1;
                return true;
            }
            lastMsgId = 0;
            return false;
        }
#else
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            return false;
        }
#endif
        if constexpr (A_TYPE::ibShare || B_TYPE::ibShare || ToMatmulConfig(MM_CFG).intraBlockPartSum) {
            if (funID == KFC_Enum::MMFUN_ITERATE_ALL) {
                if (lastMsgId == subBlockID) {
                    freeMsg = false;
                    return true;
                }
                lastMsgId = subBlockID;
                return false;
            }
            return false;
        } else {
            return false;
        }
    }

    template <uint8_t enableHardPoll = 0>
    __aicore__ inline bool LockMsgQueue(KFC_Enum funID, bool &freeMsg, int &lastMsgId, const int subBlockID,
        MSG_POS KfcMsg *msg = nullptr)
    {
#if defined(USE_SSBUF)
        if constexpr (!(A_TYPE::ibShare && B_TYPE::ibShare)) {
            if (funID == KFC_Enum::MMFUN_ITERATE) {
                if (msg->body.cAddr == 0) {
                    return true;
                }
            }
        }
        if constexpr (enableHardPoll == 1) {
            return true;
        }
#else
        if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
            return true;
        }
#endif
        return false;
    }

    __aicore__ inline bool Process(MSG_POS KfcMsg* msg, KFC_Enum funID)
    {
        if constexpr (((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_ALL) != 0) ||
            ((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_NORMAL) != 0)) {
            if ((static_cast<uint16_t>(funID) & static_cast<uint16_t>(KFC_Enum::MMFUN_MASK)) ==
                static_cast<uint16_t>(KFC_Enum::MMFUN_MASK)) {
                if constexpr (ToMatmulConfig(MM_CFG).intraBlockPartSum) {
#if defined(USE_WORKSPACE)
                    return IterateIntraBlockPartSum(msg, funID);
#endif
                } else {
                    return Iterate(msg, funID);
                }
            }
        }
        if constexpr (((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_BATCH) != 0) &&
                    (A_TYPE::layout != LayoutMode::NONE)) {
            if (funID == KFC_Enum::MMFUN_ITERATE_BATCH_ALL) {
                return IterateBatch(msg);
            }
        }
        if constexpr (ToMatmulConfig(MM_CFG).enableEnd) {
            if (funID == KFC_Enum::MMFUN_END) {
                mul.End();
            }
        }
        if constexpr (ToMatmulConfig(MM_CFG).enableGetTensorC) {
            if (funID == KFC_Enum::MMFUN_GET_TENSOR_C) {
                return GetTensorC(msg);
            }
        }
#if defined(USE_WORKSPACE)
        if constexpr (ToMatmulConfig(MM_CFG).enableSetOrgShape) {
            if (funID == KFC_Enum::MMFUN_SET_ORG_SHAPE) {
                SetOrgShape(msg);
                return true;
            }
        }
#endif
        if constexpr (ToMatmulConfig(MM_CFG).enableInit) {
            if (funID == KFC_Enum::MMFUN_INIT) {
                Init(msg);
                return true;
            }
        }
        if constexpr (((ToMatmulConfig(MM_CFG).iterateMode & IterateMode::ITERATE_MODE_N_BATCH) != 0) &&
                      (A_TYPE::layout != LayoutMode::NONE)) {
            if (funID == KFC_Enum::MMFUN_ITERATE_N_BATCH_ALL) {
                return IterateNBatch(msg);
            }
        }
        if constexpr (ToMatmulConfig(MM_CFG).enableSetDefineData) {
            if (funID == KFC_Enum::MMFUN_SET_USER_DEF_INFO) {
                SetUserDefInfo(msg);
                return true;
            }
        }
        if (funID == KFC_Enum::MMFUN_SET_HF32) {
            SetHF32(msg);
            return true;
        }
        ASSERT("illegal function ID.");
        return true;
    }

    template <class Dtype, TPosition Tpos>
    __aicore__ LocalTensor<Dtype> GetLocalTensor(uint64_t addr, const uint64_t size)
    {
        LocalTensor<Dtype> localTensor;
        TBuffAddr tbufOutTmp;
        tbufOutTmp.logicPos = (uint8_t)(Tpos);
        tbufOutTmp.bufferAddr = addr;
#if ASCENDC_CPU_DEBUG
#if defined(USE_SSBUF)
        if constexpr (Tpos == TPosition::TSCM) {
            // 8 bit for intraID, 32 bit for addr
            addr = addr & 0xffffffff;
            // size need to be equals to a non-zero val
            tbufOutTmp.dataLen = TOTAL_L1_SIZE * sizeof(Dtype);
        } else {
            tbufOutTmp.dataLen = size * sizeof(Dtype);
        }
#else
        tbufOutTmp.dataLen = size * sizeof(Dtype);
#endif
        if constexpr (PhyPosIsUB(Tpos)) {
            tbufOutTmp.absAddr = reinterpret_cast<uint8_t*>(addr);
        } else if constexpr (Tpos == TPosition::TSCM) {
            tbufOutTmp.absAddr = GetTPipePtr()->GetBaseAddr((uint8_t)(Tpos)) + addr;
        }
#endif
        localTensor.SetAddr(tbufOutTmp);
        return localTensor;
    }

#if defined(USE_SSBUF)
    __aicore__ inline void GetMsgFromSSbuf(MSG_POS KfcMsg* msg, MatmulConfigParams &body);
    __aicore__ inline void InitL1Addr();
    __aicore__ inline void CopyL1Addr2SSBUF(MSG_POS MsgMatmulL1Addr *matmulL1AddrMsg_, MatrixL1Addr *matrixL1Addr_);
    __aicore__ inline void WaitAB(MatmulConfigParams &body);
    __aicore__ inline void IterNotify();
    __aicore__ inline void SetTensorScaleA(MatmulConfigParams &body);
    __aicore__ inline void SetTensorScaleB(MatmulConfigParams &body);
#endif
public:
    MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> mul;
private:
    GM_ADDR workspace;
    KFC_COMM_SERVER_PTR kfcCommSrv;
    MatmulTiling<MM_CFG> tiling_;
    TCubeTiling tmpTiling_; // for compatible with init interface
    typename IBShareCache<IsIBShare<A_TYPE, B_TYPE>()>::ShareCache gCache;
    typename ShareMatmulAux<!ToMatmulConfig(MM_CFG).enableInit>::MSG msgAux;
public:
    uint16_t instID;
private:
    uint16_t devEvtID;
#if defined(USE_SSBUF)
    uint8_t enPartialSum_;
    uint8_t isSyncIterate_;
#endif
};
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_H__
#endif // __MATMUL_SERVER_H__