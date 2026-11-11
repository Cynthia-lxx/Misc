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
 * \file batch_loop_single.h
 * \brief
 */


#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/batch_loop/batch_loop_single.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_SINGLE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_SINGLE_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_SINGLE_H

#include "batch_loop_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    BatchLoop is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    BatchLoop is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class INPUT_TYPE, class BIAS_TYPE, const auto &MM_CFG>
class BatchLoop<IMPL, INPUT_TYPE, BIAS_TYPE, MM_CFG,
    enable_if_t<(INPUT_TYPE::layout == LayoutMode::NORMAL &&
    ToMatmulConfig(MM_CFG).batchMode == BatchMode::SINGLE_LARGE_THAN_L1)>>
{
    MATMUL_USE_MODULE(MatmulShapeInfo);

public:
    __aicore__ inline BatchLoop() = default;
    __aicore__ inline ~BatchLoop() = default;

    __aicore__ inline void Init() {}

    __aicore__ inline void SetBatchNum(int32_t batchNumA, int32_t batchNumB)
    {
        batchA_ = batchNumA;
        batchB_ = batchNumB;
        ASSERT(batchA_ > 0 && batchB_ > 0 && (batchA_ % batchB_ == 0 || batchB_ % batchA_ == 0));
        batchNum_ = batchA_ > batchB_ ? batchA_ : batchB_;
    }

    // Single Batch Loop
    __aicore__ inline void OuterStart()
    {
        singleBatchIdx_ = 0;
    }

    __aicore__ inline void OuterNext()
    {
        singleBatchIdx_++;
    }

    __aicore__ inline bool OuterEnd()
    {
        return singleBatchIdx_ >= batchNum_;
    }

    __aicore__ inline int32_t GetOuterIndex() const
    {
        return singleBatchIdx_;
    }

    __aicore__ inline int32_t GetBatchAIndex() const
    {
        return singleBatchIdx_ / Ceil(batchB_, batchA_);
    }

    __aicore__ inline int32_t GetBatchBIndex() const
    {
        return singleBatchIdx_ / Ceil(batchA_, batchB_);
    }

    __aicore__ inline int32_t GetBiasInputOffset() const
    {
        return singleBatchIdx_ * MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
    }

private:
    int32_t batchA_ = 1;
    int32_t batchB_ = 1;
    int32_t batchNum_ = 1;
    int32_t singleBatchIdx_;
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_SINGLE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_SINGLE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_SINGLE_H__
#endif