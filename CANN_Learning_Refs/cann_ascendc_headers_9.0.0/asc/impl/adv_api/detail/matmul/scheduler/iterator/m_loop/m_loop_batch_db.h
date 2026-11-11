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
 * \file m_loop_batch_db.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/m_loop/m_loop_batch_db.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BATCH_DB_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BATCH_DB_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BATCH_DB_H

#include "m_loop_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, class INPUT_TYPE, const auto &MM_CFG>
class MLoop<IMPL, INPUT_TYPE, MM_CFG,
    enable_if_t<(IsBmmBatchScheduler<INPUT_TYPE, MM_CFG> && (ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT))>> {
    MATMUL_USE_MODULE(MatmulShapeTiling);
public:
    __aicore__ inline MLoop() = default;
    __aicore__ inline ~MLoop() = default;

    __aicore__ inline void Init(int32_t batchDbShape)
    {
        SetSingleShape(batchDbShape);
    }

    __aicore__ inline void SetSingleShape(int32_t singleShape)
    {
        mIter_ = Ceil(static_cast<uint32_t>(singleShape), static_cast<uint32_t>(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM()));
        tailM_ = singleShape % MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        if (tailM_ == 0) {
            tailM_ = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        }
        isL0aDB = 1;
        ASCENDC_ASSERT((mIter_ > 0), {
            KERNEL_LOG(KERNEL_ERROR, "invalid singleCoreM, mIter_ is %d , which should be larger than 0",
                mIter_);
        });
    }

    __aicore__ inline int32_t GetTotalIter() const
    {
        return mIter_;
    }

    __aicore__ inline bool OuterNext()
    {
        mIdx_ = mIdx_ + isL0aDB;
        if ((tailM_ == MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM() && mIter_ % DB_FACTOR == 0) || mIdx_ < mIter_ - DB_FACTOR) {
            isL0aDB = DB_FACTOR;
        } else {
            isL0aDB = 1;
        }
        UpdateOuterParams();
        return !OuterEnd();
    }

    __aicore__ inline void OuterStart()
    {
        mIdx_ = 0;
        if ((tailM_ == MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM() && mIter_ % DB_FACTOR == 0) || mIdx_ < mIter_ - DB_FACTOR) {
            isL0aDB = DB_FACTOR;
        } else {
            isL0aDB = 1;
        }
        UpdateOuterParams();
    }

    __aicore__ inline bool OuterEnd()
    {
        return mIdx_ >= mIter_;
    }

    __aicore__ inline bool InnerNext()
    {
        mIdx_ = mIdx_ + 1;
        UpdateOuterParams();
        return !OuterEnd();
    }

    __aicore__ inline void InnerStart()
    {
        mIdx_ = 0;
        isL0aDB = 1;
        UpdateOuterParams();
    }

    __aicore__ inline bool InnerEnd()
    {
        return mIdx_ >= mIter_;
    }

    __aicore__ inline void UpdateOuterParams()
    {
        baseShape_ = (mIdx_ + 1 == mIter_) ? tailM_ : MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        baseBlockShape_ = Ceil(baseShape_, BLOCK_CUBE);
    }

    // Params Function
    __aicore__ inline int32_t GetOuterIdx() const
    {
        return mIdx_;
    }

    __aicore__ inline int32_t GetInnerIdx() const
    {
        return mIdx_;
    }

    __aicore__ inline int32_t GetTileShape() const
    {
        return baseShape_;
    }

    __aicore__ inline int32_t GetTileBlockShape() const
    {
        return baseBlockShape_;
    }

    __aicore__ inline int32_t GetBaseShape() const
    {
        return baseShape_;
    }

    __aicore__ inline int32_t GetBaseBlockShape() const
    {
        return baseBlockShape_;
    }

    __aicore__ inline int32_t GetTail() const
    {
        return tailM_;
    }

    __aicore__ inline int32_t GetL0DBLoopNum() const
    {
        return isL0aDB;
    }

private:
    int32_t mIter_;
    int32_t tailM_;

    int32_t mIdx_;
    // shape
    int32_t baseShape_;
    int32_t baseBlockShape_;
    int32_t isL0aDB;
};
}
}
}
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BATCH_DB_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BATCH_DB_H__
#endif // IMPL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BATCH_DB_H