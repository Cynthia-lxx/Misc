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
 * \file matmul_client_impl_aicore.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/kfc/matmul_client_impl_aicore.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_CLIENT_IMPL_AICORE_H__
#endif

#ifndef IMPL_MATMUL_KFC_MATMUL_CLIENT_IMPL_AICORE_H
#define IMPL_MATMUL_KFC_MATMUL_CLIENT_IMPL_AICORE_H

namespace AscendC {

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
class MM_CB = AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
class MatmulClientBase {
    using SrcT = typename A_TYPE::T;
    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using DstT = typename C_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;
    template <class... Args> friend struct AscendC::GetCubeObjConfig;

public:
    __aicore__ inline MatmulClientBase() {}
    MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> mul;

    template <bool sync = true, bool waitIterateBatch = false>
    __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm, uint32_t batchA, uint32_t batchB,
        bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0,
        const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
    {
        mul.SetBatchNum(batchA, batchB);
        mul.IterateBatch(gm, enPartialSum, enAtomic, enSequentialWrite, matrixStrideA, matrixStrideB, matrixStrideC);
        mul.End();
    }

    template <bool sync = true, bool waitIterateBatch = false>
    __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix, uint32_t batchA, uint32_t batchB,
        bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0,
        const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
    {
        mul.SetBatchNum(batchA, batchB);
        mul.IterateBatch(ubCmatrix, enPartialSum, enAtomic, enSequentialWrite, matrixStrideA, matrixStrideB, matrixStrideC);
        mul.End();
    }

    __aicore__ inline void SetTensorA(const GlobalTensor<SrcAT>& gm, bool isTransposeA = false)
    {
        mul.SetTensorA(gm, isTransposeA);
    }

    __aicore__ inline void SetTensorA(const LocalTensor<SrcAT>& leftMatrix, bool isTransposeA = false)
    {
        mul.SetTensorA(leftMatrix, isTransposeA);
    }

    __aicore__ inline void SetTensorA(SrcAT aScalar)
    {
        mul.SetTensorA(aScalar);
    }
    
    __aicore__ inline void SetTensorB(const GlobalTensor<SrcBT>& gm, bool isTransposeB = false)
    {
        mul.SetTensorB(gm, isTransposeB);
    }

    __aicore__ inline void SetTensorB(const LocalTensor<SrcBT>& rightMatrix, bool isTransposeB = false)
    {
        mul.SetTensorB(rightMatrix, isTransposeB);
    }

    __aicore__ inline void SetTensorB(SrcBT bScalar)
    {
        mul.SetTensorB(bScalar);
    }

    __aicore__ inline void SetBias(const GlobalTensor<BiasT>& biasGlobal)
    {
        mul.SetBias(biasGlobal);
    }

    __aicore__ inline void SetBias(const LocalTensor<BiasT>& inputBias)
    {
        mul.SetBias(inputBias);
    }

    __aicore__ inline void SetSubBlockIdx(uint8_t idx)
    {
        mul.SetSubBlockIdx(idx);
    }

    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe = nullptr)
    {
        mul.Init(cubeTiling, tpipe);
    }
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
    __aicore__ inline void Init(const __gm__ TCubeTiling* gmCubeTiling, TPipe* tpipe = nullptr)
    {
        mul.Init(gmCubeTiling, tpipe);
    }
#endif

    __aicore__ inline void SetOrgShape(int orgM, int orgN, int orgK)
    {
        mul.SetOrgShape(orgM, orgN, orgK);
    }

    __aicore__ inline void SetOrgShape(int orgM, int orgN, int orgKa, int orgKb, int orgKc = 0)
    {
        mul.SetOrgShape(orgM, orgN, orgKa, orgKb, orgKc);
    }

    __aicore__ inline void SetSingleShape(int singleM, int singleN, int singleK)
    {
        mul.SetSingleShape(singleM, singleN, singleK);
    }
    __aicore__ inline void SetTail(int tailM = -1, int tailN = -1, int tailK = -1)
    {
        mul.SetTail(tailM, tailN, tailK);
    }

    template <class T>
    __aicore__ inline void SetSelfDefineData(T dataPtr)
    {
        mul.SetSelfDefineData(dataPtr);
    }

    __aicore__ inline void SetSparseIndex(const GlobalTensor<uint8_t>& indexGlobal)
    {
        mul.SetSparseIndex(indexGlobal);
    }

    __aicore__ inline void SetUserDefInfo(const uint64_t tilingPtr)
    {
        mul.SetUserDefInfo(tilingPtr);
    }

    __aicore__ inline void SetQuantScalar(const uint64_t quantScalar)
    {
        mul.SetQuantScalar(quantScalar);
    }

    __aicore__ inline void SetQuantVector(const GlobalTensor<uint64_t>& quantTensor)
    {
        mul.SetQuantVector(quantTensor);
    }

    __aicore__ inline void DisableBias()
    {
        mul.DisableBias();
    }

    __aicore__ inline void ClearBias()
    {
        mul.ClearBias();
    }

    template <bool sync = true> 
    __aicore__ inline bool Iterate(bool enPartialSum = false)
    {
        return mul.Iterate(enPartialSum);
    }

    template <bool sync = true, typename T> 
    __aicore__ inline bool Iterate(bool enPartialSum, const LocalTensor<T>& localCmatrix)
    {
        return mul.Iterate(enPartialSum, localCmatrix);
    }

    template <bool sync = true>
    __aicore__ inline void GetTensorC(const LocalTensor<DstT>& co2Local, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        mul.GetTensorC(co2Local, enAtomic, enSequentialWrite);
    }

    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        mul.GetTensorC(gm, enAtomic, enSequentialWrite);
    }

    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT> &gm, const LocalTensor<DstT> &co2Local,
        uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        mul.GetTensorC(gm, co2Local, enAtomic, enSequentialWrite);
    }

    template <bool sync = true, bool doPad = false>
    __aicore__ inline void GetTensorC(const LocalTensor<DstT>& c, uint8_t enAtomic,
        bool enSequentialWrite, uint32_t height, uint32_t width = 0, uint32_t srcGap = 0,
        uint32_t dstGap = 0)
    {
    }

    template <bool isTurnOnDebug = true>
    __aicore__ inline MatrixOffset GetOffsetC()
    {
        mul.GetOffsetC();
    }

    __aicore__ inline void End()
    {
        mul.End();
    }

    __aicore__ inline void SetHF32(bool enableHF32 = false, int32_t transMode = 0)
    {
        mul.SetHF32(enableHF32, transMode);
    }

    __aicore__ inline uint8_t GetSubBlockIdx()
    {
        mul.GetSubBlockIdx();
    }

    template <class T> __aicore__ inline void SetWorkspace(__gm__ const T* addr, int size)
    {
        mul.SetWorkspace(addr, size);
    }

    template <class T> __aicore__ inline void SetWorkspace(GlobalTensor<T>& addr)
    {
        mul.SetWorkspace(addr);
    }

    __aicore__ inline void SetLocalWorkspace(const LocalTensor<uint8_t>& tmpBuffer)
    {
        mul.SetLocalWorkspace(tmpBuffer);
    }
    
    __aicore__ inline void SetAntiQuantScalar(const SrcT offsetScalar, const SrcT scaleScalar)
    {
        mul.SetAntiQuantScalar(offsetScalar, scaleScalar);
    }

    __aicore__ inline void SetAntiQuantVector(const LocalTensor<SrcT> &offsetTensor,
        const LocalTensor<SrcT> &scaleTensor)
    {
        mul.SetAntiQuantVector(offsetTensor, scaleTensor);
    }

    template <bool sync = true>
    __aicore__ inline void IterateAll(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0,
        bool enSequentialWrite = false, bool waitIterateAll = false, bool fakeMsg = false)
    {
        mul.IterateAll(gm, enAtomic,enSequentialWrite, waitIterateAll, fakeMsg);
    }

    template <bool sync = true>
    __aicore__ inline void IterateAll(const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0)
    {
        mul.IterateAll(ubCmatrix, enAtomic);
    }

    __aicore__ inline void WaitIterateAll()
    {
        event_t eventIDFixToMte2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::FIX_MTE2));
        SetFlag<HardEvent::FIX_MTE2>(eventIDFixToMte2);
        WaitFlag<HardEvent::FIX_MTE2>(eventIDFixToMte2);
    }
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
class MatmulClient
: public MatmulClientBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> {
public:
    __aicore__ inline MatmulClient() {}

private:
    template <class A_TYPE_, class B_TYPE_, class C_TYPE_, class BIAS_TYPE_, const auto &MM_CFG_, class MM_CB_,
        MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY_)>
    friend __aicore__ inline void SetTPipe(
        MatmulClient<A_TYPE_, B_TYPE_, C_TYPE_, BIAS_TYPE_, MM_CFG_, MM_CB_, MATMUL_POLICY_> &mm, TPipe* tpipe);
};

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto &MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void SetTPipe(MatmulClient<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY> &mm,
    TPipe* tpipe)
{
    SetTPipe(mm.mul, tpipe);
}

} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_CLIENT_IMPL_AICORE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_CLIENT_IMPL_AICORE_H__
#endif