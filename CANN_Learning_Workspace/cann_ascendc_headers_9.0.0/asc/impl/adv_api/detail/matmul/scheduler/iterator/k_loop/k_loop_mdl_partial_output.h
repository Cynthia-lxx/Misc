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
* \file k_loop_mdl_partial_output.h
* \brief partial output, only for aicore like 310
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/k_loop/k_loop_mdl_partial_output.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_PARTIAL_OUTPUT_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_PARTIAL_OUTPUT_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_PARTIAL_OUTPUT_H

#include "k_loop_intf.h"
#include "k_loop_mdl_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    KLoop is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    KLoop is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, typename TRANS_T, class A_TYPE, const auto& MM_CFG>
class KLoop<IMPL, TRANS_T, A_TYPE, MM_CFG,
            enable_if_t<DoMatmulMDL(MM_CFG) && ToMatmulConfig(MM_CFG).isPartialOutput
            && !HasScalePosition<A_TYPE>::value>>
    : public KLoopMDLBase<IMPL, TRANS_T, A_TYPE, MM_CFG>
{
public:
    using BASE_MODULE = AscendC::Impl::Detail::KLoopMDLBase<IMPL, TRANS_T, A_TYPE, MM_CFG>;
    __aicore__ inline KLoop() = default;
    __aicore__ inline ~KLoop() = default;

    __aicore__ inline void InnerStart()
    {
        BASE_MODULE::outerIdx_ = 0;
        BASE_MODULE::UpdateOuterParams();
        BASE_MODULE::innerIdx_ = 0;
        BASE_MODULE::UpdateInnerParams(AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
    }

    __aicore__ inline bool InnerNext()
    {
        BASE_MODULE::innerIdx_++;
        if (InnerEnd()) {
            return false;
        } else {
            // compute outer from inner
            BASE_MODULE::outerIdx_ = BASE_MODULE::innerIdx_ / BASE_MODULE::minStepK_;
            BASE_MODULE::UpdateOuterParams();
            BASE_MODULE::UpdateInnerParams(AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
            return true;
        }
    }

    __aicore__ inline bool InnerEnd()
    {
        return BASE_MODULE::innerIdx_ >= BASE_MODULE::kIter_;
    }

    __aicore__ inline int32_t GetStepInnerIdx() const
    {
        // loop index within each step
        return BASE_MODULE::innerIdx_ % BASE_MODULE::minStepK_;
    }
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _K_LOOP_MDL_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_PARTIAL_OUTPUT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_PARTIAL_OUTPUT_H__
#endif