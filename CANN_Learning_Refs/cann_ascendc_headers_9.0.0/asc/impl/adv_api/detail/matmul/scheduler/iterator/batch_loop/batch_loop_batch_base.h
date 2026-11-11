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
 * \file batch_loop_batch_base.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/batch_loop/batch_loop_batch_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_BATCH_BASE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_BASE_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_BASE_H

#include "../../../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    BatchLoopBase is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    BatchLoopBase is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class INPUT_TYPE, class BIAS_TYPE, const auto &MM_CFG>
class BatchLoopBase
{
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    using SrcT = typename INPUT_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;

public:
    __aicore__ inline BatchLoopBase() = default;
    __aicore__ inline ~BatchLoopBase() = default;

    __aicore__ inline void Init()
    {
        const auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        if constexpr (IsBmmDoubleBuffer<INPUT_TYPE, MM_CFG>()) {
            auto batchNum = tiling.GetBatchNum();
            splitSize_ = (batchNum % DB_FACTOR == 0) ? DB_FACTOR : 1;
            splitBatchNum_ = batchNum / splitSize_;
        }
        batchA_ = tiling.GetBatchNum();
        batchB_ = tiling.GetBatchNum();
        totalBatchNum_ = batchA_;
        batchNum_ = batchA_;
    }

    __aicore__ inline void SetBatchNum(int32_t batchNumA, int32_t batchNumB)
    {
        CalcBatchNum(batchNumA, batchNumB);
        batchNum_ = batchA_ > batchB_ ? batchA_ : batchB_;
    }

    __aicore__ inline void SetNBatchOutNum(int32_t nBatchOutNum)
    {
        nBatchOutNum_ = nBatchOutNum;
    }

    // Outer Loop
    __aicore__ inline void OuterStart()
    {
        outerIdx_ = 0;
        dstOffset_ = 0;
        batchCalcSize_ = batchNum_ * MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM() *
            MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
    }

    __aicore__ inline void OuterNext()
    {
        outerIdx_++;
        dstOffset_ += batchCalcSize_;
    }

    __aicore__ inline bool OuterEnd()
    {
        return outerIdx_ >= batchOuter_;
    }

    __aicore__ inline uint32_t GetOuterIndex() const
    {
        return outerIdx_;
    }

    __aicore__ inline uint32_t GetDstOffset() const
    {
        return dstOffset_;
    }

    __aicore__ inline int32_t GetBatchNum() const
    {
        return batchNum_;
    }

    template <InputTypeTag tag, const auto& MM_CFG_ALIAS = MM_CFG>
    __aicore__ inline enable_if_t<IsBmmDoubleBuffer<INPUT_TYPE, MM_CFG_ALIAS>(), int32_t>
    GetBatchNumBySplitIdx(int32_t splitIdx) const
    {
        auto batchNum = tag == InputTypeTag::A ? batchA_ : batchB_;
        if (batchNum > splitBatchNum_) {
            if (splitIdx == 0) {
                return splitBatchNum_;
            } else {
                return batchNum - splitBatchNum_;
            } 
        }
        return batchNum;
    }

    __aicore__ inline int32_t GetBatchA() const
    {
        return batchA_;
    }

    __aicore__ inline int32_t GetBatchB() const
    {
        return batchB_;
    }

    __aicore__ inline int32_t GetMainBatchBlockA() const
    {
        return batchA_; // batchNum main block in outerLoop
    }

    __aicore__ inline int32_t GetMainBatchBlockB() const
    {
        return batchB_; // batchNum main block in outerLoop
    }

    __aicore__ inline int32_t GetBiasBatchSrcOffset() const
    {
        return outerIdx_ * batchNum_ * MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
    }

    // Double Buffer Loop
    __aicore__ inline void SplitStart()
    {
        // Check that the total amount of data to be transferred is less than L1.
        const auto &tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        ASSERT((batchA_ * tiling.GetSingleCoreM() * tiling.GetSingleCoreK() + batchB_ * tiling.GetSingleCoreN() *
            tiling.GetSingleCoreK()) * sizeof(SrcT) + tiling.IsBias() * tiling.GetSingleCoreN() * 
            sizeof(BiasT) <= TOTAL_L1_SIZE);

        splitOuterIdx_ = 0;
        splitBatchIdx_ = 0;
    }

    __aicore__ inline void SplitNext()
    {
        splitOuterIdx_++;
        UpdateSplitParams();
    }

    __aicore__ inline bool SplitEnd()
    {
        if constexpr (IsBmmDoubleBuffer<INPUT_TYPE, MM_CFG>()) { 
            return splitOuterIdx_ >= splitSize_ || (splitOuterIdx_ == 1 && batchNum_ < splitBatchNum_);
        } else {
            return splitOuterIdx_ >= splitSize_;
        } 
    }

    template <InputTypeTag tag>
    __aicore__ inline uint32_t GetSplitIndex() const
    {
        if constexpr (IsBmmDoubleBuffer<INPUT_TYPE, MM_CFG>()) {
            if constexpr (tag == InputTypeTag::A) {
                return splitBatchNum_ >= batchA_ ? 0 : splitOuterIdx_;
            } else {
                return splitBatchNum_ >= batchB_ ? 0 : splitOuterIdx_;
            }
        } else {
            return splitOuterIdx_;
        }
    }

    __aicore__ inline uint32_t GetSplitIndex() const
    {
        return splitOuterIdx_;
    }

    __aicore__ inline int32_t GetSplitSize() const
    {
        return splitSize_;
    }

    __aicore__ inline int32_t GetSplitBatchNum() const
    {
        return splitBatchNum_;
    }

    // Inner Loop
    __aicore__ inline void InnerStart()
    {
        innerIdx_ = 0;
        UpdateInnerParams();
    }

    __aicore__ inline void InnerNext()
    {
        innerIdx_++;
        UpdateInnerParams();
    }

    __aicore__ inline bool InnerEnd()
    {
        if constexpr (IsBmmDoubleBuffer<INPUT_TYPE, MM_CFG>()) {
            return (innerIdx_ >= splitBatchNum_) || (splitOuterIdx_ * splitBatchNum_ >= batchNum_) || (innerBatchIdx_ >= batchNum_);
        } else {
            return (innerIdx_ >= splitBatchNum_) || (splitOuterIdx_ * splitBatchNum_ >= batchNum_);
        }
    }

    __aicore__ inline uint32_t GetInnerIndex() const
    {
        return innerIdx_;
    }

    __aicore__ inline uint32_t GetBatchIndex() const
    {
        return innerBatchIdx_;
    }

    __aicore__ inline bool IsNeedNBatchOut() const
    {
        if (batchOutCacheNum_ == nBatchOutNum_) {
            return true;
        } else if (batchOutOffsetNum_ + batchOutCacheNum_ == totalBatchNum_) {
            return true;
        }
        return false;
    }

    __aicore__ inline int32_t GetBatchOutCacheNum() const
    {
        return batchOutCacheNum_;
    }

    __aicore__ inline void SetBatchOutCacheNum(int32_t cacheNum)
    {
        batchOutCacheNum_ = cacheNum;
    }

    __aicore__ inline int32_t GetBatchOutOffsetNum() const
    {
        return batchOutOffsetNum_;
    }

    __aicore__ inline void SetBatchOutOffsetNum(int32_t offsetNum)
    {
        batchOutOffsetNum_ = offsetNum;
    }

    template <InputTypeTag tag>
    __aicore__ inline bool NeedCache() const
    {
        if constexpr (IsBmmDoubleBuffer<INPUT_TYPE, MM_CFG>()) {
            if constexpr (tag == InputTypeTag::A) {
                return batchA_ <= splitBatchNum_;
            } else {
                return batchB_ <= splitBatchNum_;
            }
        } else {
            return false;
        }
    }

private:
    __aicore__ inline void CalcBatchNum(int32_t batchNumA, int32_t batchNumB)
    {
        totalBatchNum_ = batchNumA > batchNumB ? batchNumA : batchNumB;
        ASSERT(batchNumA > 0 && batchNumB > 0 &&
            (batchNumA % batchNumB == 0 || batchNumB % batchNumA == 0));
        batchA_ = batchNumA;
        batchB_ = batchNumB;
    }

    __aicore__ inline void UpdateSplitParams()
    {
        splitBatchIdx_ += splitBatchNum_;
    }

    __aicore__ inline void UpdateInnerParams()
    {
        innerBatchIdx_ = innerIdx_ + splitBatchIdx_;
    }

protected:

    __aicore__ inline int32_t GetSingleSizeAlignA()
    {
        const auto matmulShapeInfo = MATMUL_MODULE(MatmulShapeInfo);
        if (matmulShapeInfo->IsTransposeA()) {
            if constexpr (IsSameType<SrcT, int8_t>::value) {
                return CeilAlign(matmulShapeInfo->GetSingleCoreM(), c0Size_) *
                    CeilAlign(matmulShapeInfo->GetSingleCoreK(), c0Size_) * sizeof(SrcT);
            } else {
                return CeilAlign(matmulShapeInfo->GetSingleCoreM(), c0Size_) *
                    CeilAlign(matmulShapeInfo->GetSingleCoreK(), BLOCK_CUBE) * sizeof(SrcT);
            }
        } else {
            return CeilAlign(matmulShapeInfo->GetSingleCoreM(), BLOCK_CUBE) *
                CeilAlign(matmulShapeInfo->GetSingleCoreK(), c0Size_) * sizeof(SrcT);
        }
    }

    __aicore__ inline int32_t GetSingleSizeAlignB()
    {
        const auto matmulShapeInfo = MATMUL_MODULE(MatmulShapeInfo);
        if (matmulShapeInfo->IsTransposeB()) {
            return CeilAlign(matmulShapeInfo->GetSingleCoreK(), c0Size_) *
                CeilAlign(matmulShapeInfo->GetSingleCoreN(), BLOCK_CUBE) * sizeof(SrcT);
        } else {
            if constexpr (IsSameType<SrcT, int8_t>::value) {
                return CeilAlign(matmulShapeInfo->GetSingleCoreK(), c0Size_) *
                    CeilAlign(matmulShapeInfo->GetSingleCoreN(), c0Size_) * sizeof(SrcT);
            } else {
                return CeilAlign(matmulShapeInfo->GetSingleCoreK(), BLOCK_CUBE) *
                    CeilAlign(matmulShapeInfo->GetSingleCoreN(), c0Size_) * sizeof(SrcT);
            }
        }
    }

    int32_t batchA_; // outerLoop main/tail block
    int32_t batchB_; // outerLoop main/tail block
    int32_t batchNum_; // outerLoop main/tail block
    int32_t batchOuter_ = 1;
    constexpr static int32_t c0Size_ = AuxGetC0Size<typename INPUT_TYPE::T>();

    // outer loop params
    uint32_t outerIdx_;
    int32_t batchCalcSize_;
    uint32_t dstOffset_;

    // split loop params
    uint32_t splitOuterIdx_;
    int32_t splitSize_; // 2 for double buffer, 1 otherwise
    int32_t splitBatchNum_; // batch num per split size
    uint32_t splitBatchIdx_; // global view batch index within split loop

    // inner loop params
    uint32_t innerIdx_;
    uint32_t innerBatchIdx_; // global view batch index within inner loop

    // nBatchOut params
    int32_t totalBatchNum_;
    int32_t nBatchOutNum_ = 1;
    int32_t batchOutCacheNum_ = 0;
    int32_t batchOutOffsetNum_ = 0;

    int32_t inputBatchNum_ = 0; 
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_BASE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_BATCH_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_BATCH_LOOP_BATCH_LOOP_BATCH_BASE_H__
#endif