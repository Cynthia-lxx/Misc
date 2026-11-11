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
 * \file scheduler_n_buffer.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/base/scheduler_n_buffer.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_N_BUFFER_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_SCHEDULER_N_BUFFER_H
#define IMPL_MATMUL_SCHEDULER_SCHEDULER_N_BUFFER_H

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
    enable_if_t<!MatmulFeatureTrait<MM_CFG>::IsNeedUB() &&
    DoMatmulMDL(MM_CFG) && POLICY_TYPE == PolicyType::MATMUL_NBUFFER_33>>
    : public MatmulMDLSchedulerCommon<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>
{
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(BiasScheduler);
    MATMUL_USE_MODULE(CubeOutBuffer);
    MATMUL_USE_MODULE(CopyCubeInA);
    MATMUL_USE_MODULE(CopyCubeInB);

public:
    using BASE_MODULE =
        AscendC::Impl::Detail::MatmulMDLSchedulerCommon<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;

    template <class T>
    __aicore__ inline bool Schedule(const T& dst, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        const auto nLoop = MATMUL_MODULE(NLoop);
        const auto mLoop = MATMUL_MODULE(MLoop);
        for (nLoop->OuterStart(); !nLoop->OuterEnd(); nLoop->OuterNext()) {
            for (nLoop->InnerStart(); !nLoop->InnerEnd(); nLoop->InnerNext()) {
                for (mLoop->InnerStart(); !mLoop->InnerEnd(); mLoop->InnerNext()) {
                    MATMUL_MODULE(BiasScheduler)->SetBias(MATMUL_MODULE(BiasScheduler)->IsBias());
                    MATMUL_MODULE(CubeOutBuffer)->AllocTensor();
                    BASE_MODULE::ReduceK(false);
                    BASE_MODULE::GetResult(dst, enAtomic, enSequentialWrite);
                }
            }
            MATMUL_MODULE(CopyCubeInB)->ClearLoadData();
        }
        MATMUL_MODULE(CopyCubeInA)->Reset();
        return true;
    }
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC

#endif // IMPL_MATMUL_SCHEDULER_SCHEDULER_N_BUFFER_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_N_BUFFER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_N_BUFFER_H__
#endif