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
 * \file batch_loop_batch_large.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/batch_loop/batch_loop_batch_large.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_BATCH_LARGE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LARGE_THAN_L1_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LARGE_THAN_L1_H

#include "batch_loop_intf.h"
#include "batch_loop_batch_base.h"

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
    ToMatmulConfig(MM_CFG).batchMode == BatchMode::BATCH_LARGE_THAN_L1)>>
    : public BatchLoopBase<IMPL, INPUT_TYPE, BIAS_TYPE, MM_CFG>
{
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    using SrcT = typename INPUT_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;

public:
    using BASE_MODULE = AscendC::Impl::Detail::BatchLoopBase<IMPL, INPUT_TYPE, BIAS_TYPE, MM_CFG>;
    __aicore__ inline BatchLoop() = default;
    __aicore__ inline ~BatchLoop() = default;

    __aicore__ inline void Init()
    {
        const auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        CalcBatchNum(tiling.GetALayoutInfoB(), tiling.GetBLayoutInfoB(), tiling.GetBatchNum(), tiling.GetBatchNum());
        UpdateBatchNumParams();
    }

    __aicore__ inline void SetBatchNum(int32_t batchNumA, int32_t batchNumB)
    {
        CalcBatchNum(batchNumA, batchNumB, batchNumA, batchNumB);
        UpdateBatchNumParams();
    }

    __aicore__ inline void OuterNext()
    {
        BASE_MODULE::outerIdx_++;
        BASE_MODULE::dstOffset_ += BASE_MODULE::batchCalcSize_;
        if (oddAndLargerThanL1_ && BASE_MODULE::outerIdx_ == BASE_MODULE::batchOuter_ - 1) {
            const int32_t tail = BASE_MODULE::inputBatchNum_ % BASE_MODULE::batchA_;
            BASE_MODULE::batchA_ = tail == 0 ? mainBatchInner_ : tail;
            BASE_MODULE::batchB_ = BASE_MODULE::batchA_;
            BASE_MODULE::batchNum_ = BASE_MODULE::batchA_;
            BASE_MODULE::batchCalcSize_ = BASE_MODULE::batchNum_ * MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM() *
                MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
            BASE_MODULE::splitSize_ = (BASE_MODULE::batchA_ >= DB_FACTOR) ? DB_FACTOR : 1;
            BASE_MODULE::splitBatchNum_ = BASE_MODULE::batchNum_ / BASE_MODULE::splitSize_;
        }
    }

    __aicore__ inline bool InnerEnd()
    {
        if ((!oddAndLargerThanL1_) || (BASE_MODULE::batchNum_ % DB_FACTOR == 0) || (BASE_MODULE::splitSize_ < DB_FACTOR)) {
            return (BASE_MODULE::innerIdx_ >= BASE_MODULE::splitBatchNum_) || (BASE_MODULE::splitOuterIdx_ * BASE_MODULE::splitBatchNum_ >= BASE_MODULE::batchNum_);
        }
        const auto firstBatchNum = BASE_MODULE::batchNum_ / BASE_MODULE::splitSize_;
        if (BASE_MODULE::splitOuterIdx_ < 1) {
            return BASE_MODULE::innerIdx_ >= firstBatchNum;
        } else {
            return BASE_MODULE::innerIdx_ >= BASE_MODULE::batchNum_ - firstBatchNum;
        }
    }

    __aicore__ inline void CalcBatchOuterIdx(uint32_t& outerIdxA, uint32_t& outerIdxB)
    {
        if (outerLoop_ == 1 || BASE_MODULE::batchA_ == BASE_MODULE::batchB_) {
            outerIdxA = BASE_MODULE::outerIdx_;
            outerIdxB = BASE_MODULE::outerIdx_;
        } else if (BASE_MODULE::batchA_ > BASE_MODULE::batchB_) {
            outerIdxA = BASE_MODULE::outerIdx_;
            outerIdxB = BASE_MODULE::outerIdx_ / outerLoop_;
        } else {
            outerIdxA = BASE_MODULE::outerIdx_ / outerLoop_;
            outerIdxB = BASE_MODULE::outerIdx_;
        }
    }

    __aicore__ inline int32_t GetMainBatchBlockA() const
    {
        return oddAndLargerThanL1_ ? mainBatchInner_ : BASE_MODULE::batchA_; // batchNum main block in outerLoop
    }

    __aicore__ inline int32_t GetMainBatchBlockB() const
    {
        return oddAndLargerThanL1_ ? mainBatchInner_ : BASE_MODULE::batchB_; // batchNum main block in outerLoop
    }

    __aicore__ inline int32_t GetBiasBatchSrcOffset() const
    {
        return BASE_MODULE::outerIdx_ * (oddAndLargerThanL1_ ? mainBatchInner_ : BASE_MODULE::batchNum_) * MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
    }

private:
    __aicore__ inline void CalcBatchNum(int32_t layoutBatchNumA, int32_t layoutBatchNumB,
        int32_t batchNumA, int32_t batchNumB)
    {
        BASE_MODULE::totalBatchNum_ = batchNumA > batchNumB ? batchNumA : batchNumB;

        ASSERT(layoutBatchNumA > 0 && layoutBatchNumB > 0 &&
              (layoutBatchNumA % layoutBatchNumB == 0 || layoutBatchNumB % layoutBatchNumA == 0));
        int32_t aMatrixSingleBatchSize = BASE_MODULE::GetSingleSizeAlignA();
        int32_t bMatrixSingleBatchSize = BASE_MODULE::GetSingleSizeAlignB();
        if ((layoutBatchNumA * aMatrixSingleBatchSize + layoutBatchNumB * bMatrixSingleBatchSize +
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().IsBias() * 
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreN() * sizeof(BiasT)) <= TOTAL_L1_SIZE) {
            BASE_MODULE::batchA_ = layoutBatchNumA;
            BASE_MODULE::batchB_ = layoutBatchNumB;
            return;
        }
        int32_t batchNumLarge;
        int32_t batchNumLess;
        int32_t largeMatrixSingleBatchSize;
        int32_t lessMatrixSingleBatchSize;
        if (layoutBatchNumA >= layoutBatchNumB) {
            batchNumLarge = layoutBatchNumA;
            batchNumLess = layoutBatchNumB;
            largeMatrixSingleBatchSize = aMatrixSingleBatchSize;
            lessMatrixSingleBatchSize = bMatrixSingleBatchSize;
        } else {
            batchNumLarge = layoutBatchNumB;
            batchNumLess = layoutBatchNumA;
            largeMatrixSingleBatchSize = bMatrixSingleBatchSize;
            lessMatrixSingleBatchSize = aMatrixSingleBatchSize;
        }
        CalcBatchAB(batchNumLarge, batchNumLess, largeMatrixSingleBatchSize, lessMatrixSingleBatchSize, layoutBatchNumA >= layoutBatchNumB);
    }

    __aicore__ inline void CalcBatchAB(int32_t batchNumLarge, int32_t batchNumLess, int32_t largeMatrixSingleBatchSize, int32_t lessMatrixSingleBatchSize, bool isBatchALarger)
    {
        int32_t multiples = batchNumLarge / batchNumLess;
        int32_t singleBatchSize = multiples * largeMatrixSingleBatchSize + lessMatrixSingleBatchSize + 
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().IsBias() * 
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreN() * sizeof(BiasT);

        int32_t batchInner = TOTAL_L1_SIZE / singleBatchSize;
        BASE_MODULE::inputBatchNum_ = batchNumLarge;
        oddAndLargerThanL1_ = (multiples == 1) && (BASE_MODULE::inputBatchNum_ % DB_FACTOR != 0);
        if (batchInner <= 0) {
            outerLoop_ = 1;
            while (batchInner <= 0) {
                outerLoop_ += 1;
                while (multiples % outerLoop_ != 0 && outerLoop_ < multiples) {
                    outerLoop_ += 1;
                }
                singleBatchSize = multiples / outerLoop_ * largeMatrixSingleBatchSize + lessMatrixSingleBatchSize +
                    MATMUL_MODULE(MatmulShapeTiling)->GetTiling().IsBias() * 
                    MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreN() * sizeof(BiasT);
                batchInner = TOTAL_L1_SIZE / singleBatchSize;
            }
            multiples /= outerLoop_;
        }
        ASSERT(batchInner > 0);
        if (oddAndLargerThanL1_) {
            mainBatchInner_ = batchInner;
            BASE_MODULE::batchOuter_ = CeilT(batchNumLess, batchInner);
            BASE_MODULE::batchA_ = batchInner;
            BASE_MODULE::batchB_ = batchInner;
        } else {
            while (batchNumLess % batchInner != 0 && batchInner > 0) {
                --batchInner;
            }
            mainBatchInner_ = batchInner;
            BASE_MODULE::batchOuter_ = batchNumLess / batchInner * outerLoop_;
            if (isBatchALarger) {
                BASE_MODULE::batchA_ = multiples * batchInner;
                BASE_MODULE::batchB_ = batchInner;
            } else {
                BASE_MODULE::batchA_ = batchInner;
                BASE_MODULE::batchB_ = multiples * batchInner;
            }
        }
    }

    __aicore__ inline void UpdateBatchNumParams()
    {
        BASE_MODULE::batchNum_ = BASE_MODULE::batchA_ > BASE_MODULE::batchB_ ? BASE_MODULE::batchA_ : BASE_MODULE::batchB_;
        if constexpr (!IsBmmDoubleBuffer<INPUT_TYPE, MM_CFG>()) {
            if (BASE_MODULE::batchOuter_ > 1 && BASE_MODULE::batchA_ == BASE_MODULE::batchB_) {
                BASE_MODULE::splitSize_ = (BASE_MODULE::batchA_ >= DB_FACTOR) ? DB_FACTOR : 1;
                BASE_MODULE::splitBatchNum_ = BASE_MODULE::batchNum_ / BASE_MODULE::splitSize_;
            } else {
                BASE_MODULE::splitSize_ = (BASE_MODULE::batchNum_ >= DB_FACTOR) && (BASE_MODULE::batchA_ % DB_FACTOR == 0) &&
                    (BASE_MODULE::batchB_ % DB_FACTOR == 0) ? DB_FACTOR : 1;
                    BASE_MODULE::splitBatchNum_ = BASE_MODULE::batchNum_ / BASE_MODULE::splitSize_;
            }
        }
    }

    int32_t outerLoop_ = 1;
    bool oddAndLargerThanL1_ = false; // new logical judgment condition for handling odd batchNum && larger than L1
    int32_t mainBatchInner_ = 0; // outerLoop main block
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LARGE_THAN_L1_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_BATCH_LARGE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_BATCH_LARGE_H__
#endif