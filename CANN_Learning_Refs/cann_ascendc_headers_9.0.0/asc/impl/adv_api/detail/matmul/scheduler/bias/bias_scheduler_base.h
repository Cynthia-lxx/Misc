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
* \file bias_scheduler_base.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/bias/bias_scheduler_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_BASE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_BASE_H
#define IMPL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_BASE_H

#include "../../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {

/**
 * BiasScheduler: responsible for copy bias data management.
 * This module provides abilities to copy bias data in C2 or L0C.
 * We retain the freedom to make incompatible changes, but do not guarantee the stability.
 * BiasScheduler is only for internal usage, does not support extension or customized specialization!
 */
template <typename IMPL, class A_TYPE, class B_TYPE, class BIAS_TYPE, const auto &MM_CFG>
class BiasSchedulerBase
{
    MATMUL_USE_MODULE(MatmulShapeTiling);
    using BiasT = typename BIAS_TYPE::T;
    using TensorT = typename Conditional<(PhyPosIsGM(BIAS_TYPE::pos) || (PhyPosIsUB(BIAS_TYPE::pos) && !MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1()
        && !MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape())), GlobalTensor<BiasT>, LocalTensor<BiasT>>::type;

public:
    __aicore__ inline BiasSchedulerBase() = default;
    __aicore__ inline ~BiasSchedulerBase() = default;

    __aicore__ inline void SetBias(bool enableBias = false)
    {
        ASCENDC_ASSERT(((int32_t)enableBias <= MATMUL_MODULE(MatmulShapeTiling)->GetTiling().IsBias()), {
            KERNEL_LOG(KERNEL_ERROR, "when tiling_.IsBias() is false, not allowed to set enableBias to true.");
        });
        enableBias_ = enableBias;
    }

    __aicore__ inline bool IsBias() const
    {
        return enableBias_;
    }

    __aicore__ inline void SetInput(const TensorT& srcTensor)
    {
        srcTensor_ = srcTensor;
    }

    __aicore__ inline void SetSingleOffset(int32_t offset = 0)
    {
        singleOffset_ = offset;
    }

public:
    TensorT srcTensor_;
    int32_t singleOffset_ {0};
    bool enableBias_ { false };
};

}  // namespace Detail
}  // namespace Impl
}  // namespace Gemm
#endif // _BIAS_SCHEDULER_BASE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_BASE_H__
#endif