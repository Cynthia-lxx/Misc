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
 * \file n_loop_mdl_mx.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/n_loop/n_loop_mdl_mx.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_MX_H__
#endif
#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_MX_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_MX_H

#include "n_loop_intf.h"
#include "n_loop_mdl_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    NLoop for ScaleB is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    NLoop is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, const auto& MM_CFG>
class NLoop<IMPL, A_TYPE, MM_CFG, enable_if_t<DoMatmulMDL(MM_CFG) && HasScalePosition<A_TYPE>::value
    && !MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()>> : public NLoopMDLBase<IMPL, A_TYPE, MM_CFG>
{
    MATMUL_USE_MODULE(MatmulShapeTiling);
public:
    using BASE_MODULE = AscendC::Impl::Detail::NLoopMDLBase<IMPL, A_TYPE, MM_CFG>;
    __aicore__ inline NLoop() = default;
    __aicore__ inline ~NLoop() = default;

    __aicore__ inline void Init(int32_t singleShape)
    {
        SetSingleShape(singleShape);
    }

    __aicore__ inline void SetSingleShape(int32_t singleShape)
    {
        BASE_MODULE::SetSingleShape(singleShape);
        const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        int32_t stepN = tiling.GetStepN();
        int32_t baseN = tiling.GetBaseN();
        scaleFactorN_ = tiling.GetScaleFactorN();
        // originTailScaleStepN_ represents the origin size of the GM -> L1 tail block
        tailScaleStepN_ = singleShape % (baseN * stepN * scaleFactorN_);
        if (tailScaleStepN_ == 0) {
            tailScaleStepN_ = baseN * stepN * scaleFactorN_;
        }
    }

    __aicore__ inline void OuterStart()
    {
        BASE_MODULE::outerIndex_ = 0;
        UpdateOuterParams();
    }

    __aicore__ inline bool OuterNext()
    {
        BASE_MODULE::outerIndex_++;
        if (BASE_MODULE::OuterEnd()) {
            return false;
        } else {
            UpdateOuterParams();
            return true;
        }
    }

    /**
     * @description: Get current scaleN outer loop index, used for GetBufferPos in CopyCubeIn
     * @param: void
     * @return: return current scaleN outerIdx
     */
    __aicore__ inline int32_t GetOuterScaleNIdx() const
    {
        return BASE_MODULE::outerIndex_ / scaleFactorN_;
    }

    /**
     * @description: Get next scaleN outer loop index, used for ClearL1BufferCache in SchedulerMDL
     * @param: void
     * @return: return next scaleN outerIdx
     */
    __aicore__ inline int32_t GetNextOuterScaleNIdx() const
    {
        return (BASE_MODULE::outerIndex_ + 1) / scaleFactorN_;
    }

    __aicore__ inline int32_t GetTileShapeScaleN() const
    {
        return tileShapeScaleN_;
    }

    __aicore__ inline int32_t GetScaleFactorN() const
    {
        return scaleFactorN_;
    }

protected:
    __aicore__ inline void UpdateOuterParams()
    {
        BASE_MODULE::UpdateOuterParams();
        auto tilingBaseN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
        auto tilingStepN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN();

        int32_t curScaleOuterIdx = BASE_MODULE::innerStartIdx_ / (tilingStepN * scaleFactorN_);

        if (curScaleOuterIdx + 1 >= Ceil(BASE_MODULE::outerIter_, scaleFactorN_)) {
            tileShapeScaleN_ = tailScaleStepN_;
        } else {
            tileShapeScaleN_ = tilingStepN * tilingBaseN * scaleFactorN_;
        }
    }

private:
    uint8_t scaleFactorN_;
    int32_t tileShapeScaleN_;       // scaleNL1 length
    int32_t tailScaleStepN_; // align scaleNL1 length for GM->L1
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_MX_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_MX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_MX_H__
#endif