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
 * \file batch_loop_inf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/batch_loop/batch_loop_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_INTF_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_INTF_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_INTF_H

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    BatchLoop is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    BatchLoop is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class INPUT_TYPE, class BIAS_TYPE, const auto &MM_CFG, typename = void>
class BatchLoop
{
    public:
    __aicore__ inline BatchLoop() = default;
    __aicore__ inline ~BatchLoop() = default;

    /**
     * @description: Init BatchLoop params, should be called when matmul is inited
     * @param: void
     * @return: void
     */
    __aicore__ inline void Init() {}

    /**
     * @description: Set batchA & batchB and update params of BatchLoop when batchA/batchB is changed
     * @param: batchNumA: input matrix batch num
     * @param: batchNumB: weight matrix batch num
     * @return: void
     */
    __aicore__ inline void SetBatchNum(int32_t batchNumA, int32_t batchNumB) {}

    /**
     * @description: Start outer loop for BatchLoop
     * @param: void
     * @return: void
     */
    __aicore__ inline void OuterStart() {}

    /**
     * @description: Proceed outer loop for BatchLoop
     * @param: void
     * @return: void
     */
    __aicore__ inline void OuterNext() {}

    /**
     * @description: Whether outer loop is end for BatchLoop
     * @param: void
     * @return: return true if outer loop is end
     */
    __aicore__ inline bool OuterEnd()
    {
        return false;
    }

    /**
     * @description: Get current outer loop index for BatchLoop
     * @param: void
     * @return: return outer loop index
     */
    __aicore__ inline uint32_t GetOuterIndex() const
    {
        return 0;
    }
};
}
}
} // IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_INTF_H
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_INTF_H__
#endif