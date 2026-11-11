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
 * \file scheduler_intf.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/base/scheduler_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_INTF_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_INTF_H
#define IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_INTF_H

#include "../../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    MatmulScheduler is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MatmulScheduler is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG,
    PolicyType POLICY_TYPE = PolicyType::MATMUL_DEFAULT, typename = void>
class MatmulScheduler
{
    using DstT = typename C_TYPE::T;
public:
    /**
     * @description: Compute matmul
     * @param: gm: global tensor to store output
     * @param: enAtomic: atomic mode when get output
     * @param: enSequentialWrite: true if output results to the same basic block
     * @param: fakeMsg: only used in intrablockpartsum scene, true if current computation is fake intrablock scene
     * @return: void
     */
    __aicore__ inline void Schedule(const GlobalTensor<DstT>& gm, uint8_t enAtomic,
        bool enSequentialWrite, bool fakeMsg) {}
    /**
     * @description: Compute one block of matmul
     * @param: enPartialSum: true if current block is based on previous compute results
     * @return: bool: true if current block is still computing
     */
    __aicore__ inline bool ScheduleOnce(bool enPartialSum)
    {
        return false;
    }

    /**
     * @description: Reset scheduler's status
     * @param: void
     * @return: void
     */
    __aicore__ inline void Reset() {}

    /**
     * @description: Get current block's output to local tensor
     * @param: co2Local: local tensor to store output
     * @param: enAtomic: atomic mode when get output
     * @param: enSequentialWrite: true if output results to the same basic block
     * @return: void
     */
    __aicore__ inline void GetResult(const LocalTensor<DstT>& co2Local, uint8_t enAtomic = 0,
        bool enSequentialWrite = false) {}

    /**
     * @description: Get current block's output to local tensor
     * @param: gm: global tensor to store output
     * @param: enAtomic: atomic mode when get output
     * @param: enSequentialWrite: true if output results to the same basic block
     * @return: void
     */
    __aicore__ inline void GetResult(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0,
        bool enSequentialWrite = false) {}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
    /**
     * @description: Get current block's output to local tensor and global tensor
     * @param: gm: global tensor to store output
     * @param: co2Local: local tensor to store output
     * @param: enAtomic: atomic mode when get output
     * @param: enSequentialWrite: true if output results to the same basic block
     * @return: void
     */
    __aicore__ inline void GetResult(const GlobalTensor<DstT>& gm, const LocalTensor<DstT>& co2Local,
        uint8_t enAtomic = 0, bool enSequentialWrite = false) {}
#endif
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_INTF_H__
#endif