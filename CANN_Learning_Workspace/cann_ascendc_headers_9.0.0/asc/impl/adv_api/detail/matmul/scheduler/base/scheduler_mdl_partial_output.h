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
 * \file scheduler_mdl_partial_output.h
 * \brief partial output, only for aicore like 310
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/base/scheduler_mdl_partial_output.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_PARTIAL_OUTPUT_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_PARTIAL_OUTPUT_H
#define IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_PARTIAL_OUTPUT_H
 
#include "scheduler_intf.h"
#include "scheduler_mdl_common.h"
namespace AscendC {
namespace Impl {
namespace Detail {
 
/*
    MatmulScheduler is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MatmulScheduler is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG,
    PolicyType POLICY_TYPE>
class MatmulScheduler<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE,
    enable_if_t<DoMatmulMDL(MM_CFG) && ToMatmulConfig(MM_CFG).isPartialOutput && !isMxMatmul<A_TYPE, B_TYPE>>>
    : public MatmulMDLSchedulerCommon<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>
{
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(TBufPoolL0);
    MATMUL_USE_MODULE(BiasScheduler);
    MATMUL_USE_MODULE(CubeOutBuffer);
 
    using TransAT = typename A_TYPE::T;
    using TransBT = typename decltype(GetTransBDataType<A_TYPE, B_TYPE, MM_CFG>())::T;
    using BiasT = typename BIAS_TYPE::T;
    using DstT = typename C_TYPE::T;
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
 
public:
    using BASE_MODULE =
        AscendC::Impl::Detail::MatmulMDLSchedulerCommon<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>;

    __aicore__ inline bool ScheduleOnce(bool enPartialSum)
    {
        MATMUL_MODULE(BiasScheduler)->SetBias(MATMUL_MODULE(BiasScheduler)->IsBias() && !enPartialSum);
        if (!MoveNext()) {
            return false;
        }
        if (!enPartialSum) {
            MATMUL_MODULE(CubeOutBuffer)->AllocTensor();
        }
        PartialK(enPartialSum);
        return true;
    }
 
private:
    __aicore__ inline bool MoveNext()
    {
        if (unlikely(BASE_MODULE::isFirstIter_)) {
            MATMUL_MODULE(KLoop)->InnerStart();
            return BASE_MODULE::MoveOnFirstIterate();
        } else {
            if (MATMUL_MODULE(KLoop)->InnerNext()) {
                return true;
            }

            MATMUL_MODULE(KLoop)->InnerStart();
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::UNDEF) {
                if (likely(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                    static_cast<int>(IterateOrder::ORDER_M))) {
                    return BASE_MODULE::MoveOnIterateOrderM();
                } else {
                    ASCENDC_ASSERT((MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                        static_cast<int>(IterateOrder::ORDER_N)), {
                        KERNEL_LOG(KERNEL_ERROR, "iterateOrder is %d , which should be ORDER_N",
                        MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder());
                    });
                    return BASE_MODULE::MoveOnIterateOrderN();
                }
            } else if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                return BASE_MODULE::MoveOnIterateOrderM();
            } else {
                return BASE_MODULE::MoveOnIterateOrderN();
            }
        }
    }
 
    __aicore__ inline void PartialK(bool enPartialSum)
    {
        PartialKMultiIter(enPartialSum);
    }

    __aicore__ inline void PartialKMultiIter(bool enPartialSum)
    {
        // init split params for left and right matrix
        SplitParams aL0Params = BASE_MODULE::InitSplitAParams();
        SplitParams bL0Params = BASE_MODULE::InitSplitBParams();
        bool isATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA();
        bool isBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB();

        // curKaOuterIdx and curKbOuterIdx are used to decide if left or right matrix need to clear its l1 buffer
        int32_t curKaOuterIdx = MATMUL_MODULE(KLoop)->GetOuterKaIdx();
        int32_t curKbOuterIdx = MATMUL_MODULE(KLoop)->GetOuterKbIdx();

        // CopyIn
        LocalTensor<TransAT> a1;
        LocalTensor<TransBT> b1;
        BASE_MODULE::CopyIn(a1, b1);
        LocalTensor<BiasT> bias = BASE_MODULE::SplitBias(bL0Params.axisL0Len);
        Compute(a1, b1, bias, enPartialSum, isATranspose, isBTranspose, aL0Params, bL0Params);

        BASE_MODULE::DoPreloadAWait();
        BASE_MODULE::ClearL1BufferCache(curKaOuterIdx, curKbOuterIdx);
        BASE_MODULE::ResetCopyInBuffer();
    }

    __aicore__ inline void Compute(const LocalTensor<TransAT>& a1, const LocalTensor<TransBT>& b1, LocalTensor<BiasT>& bias, 
        const bool enPartialSum, const bool isATranspose, const bool isBTranspose, SplitParams& aL0Params, SplitParams& bL0Params)
    {
        // sL0CInit and sL0CLast are used for Split
        bool sL0CInit = false;
        bool sL0CLast = false;
        BASE_MODULE::SplitPrepare(enPartialSum, isATranspose, isBTranspose, aL0Params, bL0Params, sL0CInit, sL0CLast);
        // prepare for Split
        int32_t kL1Stride = MATMUL_MODULE(KLoop)->GetBaseBlockShape() * BASE_MODULE::c0Size_;
        aL0Params.kAxisL1Offset += MATMUL_MODULE(KLoop)->GetStepInnerIdx() * kL1Stride;
        bL0Params.kAxisL1Offset += MATMUL_MODULE(KLoop)->GetStepInnerIdx() * kL1Stride;
        ComputeKDB(a1, b1, aL0Params, bL0Params, isATranspose, isBTranspose, sL0CInit, sL0CLast);
    }
 
    __aicore__ inline void ComputeKDB(const LocalTensor<TransAT>& a1, const LocalTensor<TransBT>& b1,
        const SplitParams& aL0Params, const SplitParams& bL0Params,
        const bool isATranspose, const bool isBTranspose, const bool sL0CInit, const bool sL0CLast)
    {
        MATMUL_MODULE(TBufPoolL0)->Allocate();
        LocalTensor<TransAT> a2 = BASE_MODULE::SplitA(a1, aL0Params, isATranspose);
        LocalTensor<TransBT> b2 = BASE_MODULE::SplitB(b1, bL0Params, isBTranspose);
        MATMUL_MODULE(TBufPoolL0)->EnQue();
        MATMUL_MODULE(TBufPoolL0)->DeQue();
        BASE_MODULE::CubeCompute(MATMUL_MODULE(CubeOutBuffer)->GetTensor(), a2, b2, aL0Params.axisL0Len,
            bL0Params.axisL0Len, MATMUL_MODULE(KLoop)->GetBaseShape(), isATranspose, isBTranspose, sL0CInit, sL0CLast);
        MATMUL_MODULE(TBufPoolL0)->Free();
        MATMUL_MODULE(BiasScheduler)->Free();
    }
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC

#endif // IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_PARTIAL_OUTPUT_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_PARTIAL_OUTPUT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_PARTIAL_OUTPUT_H__
#endif