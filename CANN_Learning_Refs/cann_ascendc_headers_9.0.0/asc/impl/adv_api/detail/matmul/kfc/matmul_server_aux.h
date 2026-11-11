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
 * \file matmul_server_aux.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/kfc/matmul_server_aux.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul_client.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_AUX_H__
#endif

#ifndef IMPL_MATMUL_KFC_MATMUL_SERVER_AUX_H
#define IMPL_MATMUL_KFC_MATMUL_SERVER_AUX_H

#include "matmul_server_impl.h"
#if defined(USE_WORKSPACE)
#include "matmul_server_impl_c220.h"
#endif
#if defined(USE_SSBUF)
#include "matmul_server_impl_c310.h"
#endif

namespace AscendC {

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE,
    const auto& MM_CFG = CFG_NORM, class MM_CB = AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>,
    MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
struct MatmulInstBase {
    __aicore__ inline MatmulInstBase(){};
};
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
struct MatmulInstShared : MatmulInstBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> {
    __aicore__ inline MatmulInstShared(){};
    AscendC::MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> cubeObj[1];
};
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
struct MatmulInst : MatmulInstBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> {
    __aicore__ inline MatmulInst(){};
    AscendC::MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> cubeObj[MIX_NUM];
};

template <bool SHARED, class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG,
    class MM_CB, MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
struct MatmulInstAux {
    __aicore__ inline MatmulInstAux(){};
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
struct MatmulInstAux<true, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> {
    __aicore__ inline MatmulInstAux(){};
    using MATMUL = MatmulInstShared<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
struct MatmulInstAux<false, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> {
    __aicore__ inline MatmulInstAux(){};
    using MATMUL = MatmulInst<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
class MM_CB = AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
class MatmulServiceAuxBase {
    using SrcT = typename A_TYPE::T;
    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;
    template <class... Args> friend struct AscendC::GetCubeObjConfig;
    constexpr static bool enableMixDualMaster = ToMatmulConfig(MM_CFG).enableMixDualMaster;
    constexpr static bool enableABShare = A_TYPE::ibShare && B_TYPE::ibShare;

public:
    __aicore__ inline MatmulServiceAuxBase() {}
    typename MatmulInstAux<IsSharedMatmul<MM_CFG>(), A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB,
        MATMUL_POLICY>::MATMUL cubeObj;

    // stub functions for MatmulImplBase
    __aicore__ inline void Init(TCubeTiling *cubeTiling, TPipe *tpipe = nullptr)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.Init(cubeTiling, tpipe);
        }
    }
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
    __aicore__ inline void Init(const __gm__ TCubeTiling* cubeTiling, TPipe* tpipe = nullptr)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.Init(cubeTiling, tpipe);
        }
    }
#endif
    __aicore__ inline void SetOrgShape(int orgM, int orgN, int orgK)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetOrgShape(orgM, orgN, orgK);
        }
    }
    __aicore__ inline void SetOrgShape(int orgM, int orgN, int orgKa, int orgKb, int orgKc = 0)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetOrgShape(orgM, orgN, orgKa, orgKb, orgKc);
        }
    }
    __aicore__ inline void SetSingleShape(int singleM, int singleN, int singleK)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetSingleShape(singleM, singleN, singleK);
        }
    }
    __aicore__ inline void SetTail(int tailM = -1, int tailN = -1, int tailK = -1)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetTail(tailM, tailN, tailK);
        }
    }

    __aicore__ inline void SetTensorA(const GlobalTensor<SrcAT>& gm, bool isTransposeA = false)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetTensorA(gm, isTransposeA);
        }
    }

    __aicore__ inline void SetTensorB(const GlobalTensor<SrcBT>& gm, bool isTransposeB = false)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetTensorB(gm, isTransposeB);
        }
    }

    __aicore__ inline void SetBias(const GlobalTensor<BiasT>& biasGlobal)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetBias(biasGlobal);
        }
    }
    __aicore__ inline void SetTensorA(const LocalTensor<SrcAT>& leftMatrix, bool isTransposeA = false)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
            ASSERT("SetTensorA localTensor not support when enableMixDualMaster is enabled");
#endif
        }   
    }
    __aicore__ inline void SetTensorB(const LocalTensor<SrcBT>& rightMatrix, bool isTransposeB = false)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
            ASSERT("SetTensorB localTensor not support when enableMixDualMaster is enabled");
#endif
        } 
    }
    __aicore__ inline void SetBias(const LocalTensor<BiasT>& inputBias)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
            ASSERT("SetBias localTensor not support when enableMixDualMaster is enabled");
#endif
        } 
    }
    __aicore__ inline void SetTensorA(SrcAT aScalar)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetTensorA(aScalar);
        }
    }
    __aicore__ inline void SetTensorB(SrcBT bScalar)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetTensorB(bScalar);
        }
    }
    __aicore__ inline void DisableBias()
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.DisableBias();
        }
    }
    __aicore__ inline void ClearBias()
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.ClearBias();
        }
    }
#if defined(USE_SSBUF)
    template <class T>
    __aicore__ inline void SetSelfDefineData(T dataPtr){
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetSelfDefineData(dataPtr);
        }
    }
#else
    __aicore__ inline void SetSelfDefineData(const uint64_t dataPtr)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetSelfDefineData(dataPtr);
        }
    }
#endif
    __aicore__ inline void SetUserDefInfo(const uint64_t tilingPtr)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetUserDefInfo(tilingPtr);
        }
    }
    __aicore__ inline void SetQuantScalar(const uint64_t quantScalar)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetQuantScalar(quantScalar);
        }
    }
    __aicore__ inline void SetQuantVector(const GlobalTensor<uint64_t>& quantTensor)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetQuantVector(quantTensor);
        }
    }
    __aicore__ inline void SetQuantVector(const LocalTensor<uint64_t>& quantTensor)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetQuantVector(quantTensor);
        }
    }
    template <class T> __aicore__ inline void SetWorkspace(__gm__ T* addr, int size)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "SetWorkspace not support when enableMixDualMaster is enabled");
    }
    template <class T> __aicore__ inline void SetWorkspace(GlobalTensor<T>& addr)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "SetWorkspace not support when enableMixDualMaster is enabled");
    }
    __aicore__ inline void End(){};
    __aicore__ inline void SetHF32(bool enableHF32 = false, int32_t transMode = 0)
    {
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
            cubeObj.cubeObj[0].mul.SetHF32(enableHF32, transMode);
        }
    }

    template <bool sync = true> __aicore__ inline bool Iterate(bool enPartialSum = false)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster && 
            "Iterate not support when enableMixDualMaster is enabled");
        return false;
    };

    template <bool sync = true, typename T> __aicore__ inline bool Iterate(bool enPartialSum,
        const LocalTensor<T>& localCmatrix)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster && 
            "Iterate not support when enableMixDualMaster is enabled");
        return false;
    };
    template <bool sync = true>
    __aicore__ inline void IterateAll(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0,
        bool enSequentialWrite = false, bool waitIterateAll = false, bool fakeMsg = false)
    {
        ASCENDC_ASSERT((!ToMatmulConfig(MM_CFG).isPartialOutput), { KERNEL_LOG(KERNEL_ERROR, "IterateAll is not supported for PartialOutput."); });
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if defined(USE_SSBUF)
            WaitAB();
            cubeObj.cubeObj[0].mul.IterateAll(gm, enAtomic, enSequentialWrite, waitIterateAll, fakeMsg);
            if (sync || waitIterateAll) {
                IterNotify();
            }
#else
            constexpr uint16_t eventID = 9U;
            WaitEvent(eventID);
            cubeObj.cubeObj[0].mul.IterateAll(gm, enAtomic, enSequentialWrite, waitIterateAll, fakeMsg);
            if (sync || waitIterateAll){
                NotifyEvent<PIPE_FIX>(cubeObj.cubeObj[0].instID);
            }
#endif
            cubeObj.cubeObj[0].mul.End();
        }
    }
    template <bool sync = true>
    __aicore__ inline void IterateAll(const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0,
        bool enSequentialWrite = false, bool waitIterateAll = false)
    {
        ASCENDC_ASSERT((!ToMatmulConfig(MM_CFG).isPartialOutput), { KERNEL_LOG(KERNEL_ERROR, "IterateAll is not supported for PartialOutput."); });
        if constexpr (ToMatmulConfig(MM_CFG).enableMixDualMaster) {
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
            ASSERT("IterateAll localTensor not support when enableMixDualMaster is enabled");
#endif
#if defined(USE_SSBUF)
            WaitAB();
            if constexpr (GetPhyType(C_TYPE::pos) == Hardware::UB) {
                CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(
                    cubeObj.cubeObj[0].instID, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP), 0));
                CrossCoreWaitFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(
                    cubeObj.cubeObj[0].instID, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::WAIT_FIXP), 1));
            }
            cubeObj.cubeObj[0].mul.IterateAll(ubCmatrix, enAtomic);
            if (sync || waitIterateAll) {
                IterNotify();
            }
            cubeObj.cubeObj[0].mul.End();

#endif
        }
    }
    __aicore__ inline void WaitIterateAll() {};
    template <bool sync = true, bool doPad = false>
    __aicore__ inline void GetTensorC(const LocalTensor<DstT>& c, uint8_t enAtomic = 0,
        bool enSequentialWrite = false, uint32_t height = 0, uint32_t width = 0, uint32_t srcGap = 0, uint32_t dstGap = 0)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetTensorC not support when enableMixDualMaster is enabled");
    }
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0,
        bool enSequentialWrite = false)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetTensorC not support when enableMixDualMaster is enabled");
    }
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT>& gm, const LocalTensor<DstT>& cLocal,
        uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetTensorC not support when enableMixDualMaster is enabled");
    }
    template <bool sync = true>
    __aicore__ inline GlobalTensor<DstT> GetTensorC(uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetTensorC not support when enableMixDualMaster is enabled");
        GlobalTensor<DstT> global;
        return global;
    };
    template <bool sync = true, bool waitIterateBatch = false>
    __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm, uint32_t batchA, uint32_t batchB,
        bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0,
        const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "IterateBatch not support when enableMixDualMaster is enabled");
    }
    template <bool sync = true>
    __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix, uint32_t batchA, uint32_t batchB,
        bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0,
        const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "IterateBatch not support when enableMixDualMaster is enabled");
    }

    __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm,
        bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0,
        const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "IterateBatch not support when enableMixDualMaster is enabled");
    }

    __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix,
        bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0,
        const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "IterateBatch not support when enableMixDualMaster is enabled");
    }

    template <bool sync = true, bool waitIterateBatch = false>
    __aicore__ inline void IterateNBatch(const uint32_t batchLoop, uint32_t batchA, uint32_t batchB,
        bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0,
        const uint32_t matrixStrideC = 0)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "IterateNBatch not support when enableMixDualMaster is enabled");
    }
    template <bool sync = true>
    __aicore__ inline GlobalTensor<DstT> GetBatchTensorC(uint32_t batchA, uint32_t batchB,
        bool enSequentialWrite = false)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetBatchTensorC not support when enableMixDualMaster is enabled");
        GlobalTensor<DstT> global;
        return global;
    }
    template <bool sync = true>
    __aicore__ inline GlobalTensor<DstT> GetBatchC(uint32_t batchA, uint32_t batchB, bool enSequentialWrite = false)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetBatchC not support when enableMixDualMaster is enabled");
    }
    template <bool sync = true, bool doPad = false>
    __aicore__ inline void GetBatchTensorC(const LocalTensor<DstT>& c, uint32_t batchA, uint32_t batchB,
        bool enSequentialWrite = false, uint32_t height = 0, uint32_t width = 0, uint32_t srcGap = 0,
        uint32_t dstGap = 0)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetBatchTensorC not support when enableMixDualMaster is enabled");
    }
    template <bool sync = true, bool doPad = false>
    __aicore__ inline void GetBatchC(const LocalTensor<DstT>& c, uint32_t batchA, uint32_t batchB,
        bool enSequentialWrite = false, uint32_t height = 0, uint32_t width = 0, uint32_t srcGap = 0,
        uint32_t dstGap = 0)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "GetBatchC not support when enableMixDualMaster is enabled");
    }
    __aicore__ inline void WaitIterateBatch()
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "WaitIterateBatch not support when enableMixDualMaster is enabled");
    }
    __aicore__ inline void SetLocalWorkspace(const LocalTensor<uint8_t>& tmpBuffer)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "SetLocalWorkspace not support when enableMixDualMaster is enabled");
    }
    __aicore__ inline void AsyncGetTensorC(const LocalTensor<DstT>& c)
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "AsyncGetTensorC not support when enableMixDualMaster is enabled");
    }
    __aicore__ inline void WaitGetTensorC()
    {
        ASSERT(!ToMatmulConfig(MM_CFG).enableMixDualMaster &&
            "WaitGetTensorC not support when enableMixDualMaster is enabled");
    }
    template <bool isTurnOnDebug = true>
    __aicore__ inline MatrixOffset GetOffsetC()
    {
        if constexpr (isTurnOnDebug) {
            static_assert(!isTurnOnDebug, "Debug is not supported!");
        }
    }
#if defined(USE_SSBUF)
    __aicore__ inline void SetTensorScaleA(const GlobalTensor<fp8_e8m0_t> &a, bool isTransposeScaleA = false)
    {
        static_assert(!ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorScaleA not support when enableMixDualMaster is enabled.");
    }
    __aicore__ inline void SetTensorScaleA(const LocalTensor<fp8_e8m0_t> &a, bool isTransposeScaleA = false)
    {
        static_assert(!ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorScaleA not support when enableMixDualMaster is enabled.");
    }
    __aicore__ inline void SetTensorScaleB(const GlobalTensor<fp8_e8m0_t> &b, bool isTransposeScaleB = true)
    {
        static_assert(!ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorScaleB not support when enableMixDualMaster is enabled.");
    }
    __aicore__ inline void SetTensorScaleB(const LocalTensor<fp8_e8m0_t> &b, bool isTransposeScaleB = true)
    {
        static_assert(!ToMatmulConfig(MM_CFG).enableMixDualMaster,
            "SetTensorScaleB not support when enableMixDualMaster is enabled.");
    }    
    constexpr static auto CONFIG = ToMatmulConfig(MM_CFG);
private:
    __aicore__ inline void WaitAB()
    {
        if constexpr (GetPhyType(A_TYPE::pos) == Hardware::UB || GetPhyType(B_TYPE::pos) == Hardware::UB ||
            PhyMxScalePosIsUB<A_TYPE>() || PhyMxScalePosIsUB<B_TYPE>() || GetPhyType(BIAS_TYPE::pos) == Hardware::UB) {
            if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                constexpr uint16_t l1SetIdV0 = static_cast<uint16_t>(VEC_WAIT_INTRA_Enum::UB_L1_L1_L0AB);
                constexpr uint16_t l1SetIdV1 = static_cast<uint16_t>(VEC_WAIT_INTRA_Enum::UB_L1_L1_L0AB) + INTRA_NUM;
                CrossCoreSetFlag<INTRA_MODE, PIPE_MTE1>(l1SetIdV0);
                CrossCoreSetFlag<INTRA_MODE, PIPE_MTE1>(l1SetIdV1);
            }
        }
    }
    __aicore__ inline void IterNotify()
    {
        if constexpr (GetPhyType(C_TYPE::pos) != Hardware::L1) {
            CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(
                GetIntraFlagId(cubeObj.cubeObj[0].instID, static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), 0U));
            if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                CrossCoreSetFlag<INTRA_MODE, PIPE_FIX>(GetIntraFlagId(cubeObj.cubeObj[0].instID,
                    static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP), 1U)); // 1 means sub_block 1
            }
        }
    }
#endif
};

// Match Policy with CallBack parameter
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
class MatmulServiceAux
: public MatmulServiceAuxBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> {
public:
    __aicore__ inline MatmulServiceAux() {}
};
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_AUX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_AUX_H__
#endif // __MATMUL_SERVER_H__