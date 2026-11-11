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
* \file m_loop_mdl_mx.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/m_loop/m_loop_mdl_mx.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_MX_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_MX_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_MX_H

#include "m_loop_intf.h"
#include "m_loop_mdl_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    MLoop is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MLoopMDLBase is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, const auto& MM_CFG>
class MLoop<IMPL, A_TYPE, MM_CFG, enable_if_t<DoMatmulMDL(MM_CFG) && HasScalePosition<A_TYPE>::value
        && !MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()>>
    : public MLoopMDLBase<IMPL, A_TYPE, MM_CFG>
{
    MATMUL_USE_MODULE(MatmulShapeTiling);
public:
    using BASE_MODULE = AscendC::Impl::Detail::MLoopMDLBase<IMPL, A_TYPE, MM_CFG>;
    __aicore__ inline MLoop() = default;
    __aicore__ inline ~MLoop() = default;

    __aicore__ inline void Init(int32_t singleShape)
    {
        SetSingleShape(singleShape);
    }

    __aicore__ inline void SetSingleShape(int32_t singleShape)
    {
        BASE_MODULE::SetSingleShape(singleShape);
        const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        scaleFactorM_ = tiling.GetScaleFactorM();
        stepScaleM_ = tiling.GetStepM() * scaleFactorM_;
        mainShapeScaleM_ = tiling.GetBaseM() * stepScaleM_;
        tailScaleStepM_ = singleShape % (tiling.GetBaseM() * stepScaleM_);
        if (tailScaleStepM_ == 0) {
            tailScaleStepM_ = mainShapeScaleM_;
        }
    }

    __aicore__ inline void OuterStart()
    {
        BASE_MODULE::outerIndex_ = 0;
        BASE_MODULE::innerIndex_ = 0;
        UpdateOuterParams();
    }

    __aicore__ inline bool OuterNext()
    {
        if constexpr (IsBasicM(MM_CFG)) {
            return false;
        } else {
            BASE_MODULE::outerIndex_++;
            UpdateOuterParams();
            return !BASE_MODULE::OuterEnd();
        }
    }

    __aicore__ inline int32_t GetTileShapeScaleM() const
    {
        return tileShapeScaleM_;
    }

    __aicore__ inline int32_t GetScaleFactorM() const
    {
        return scaleFactorM_;
    }

    /**
     * @description: Get current scaleM outer loop index, used for GetBufferPos in CopyCubeIn
     * @param: void
     * @return: return current scaleM outerIdx
     */
    __aicore__ inline int32_t GetOuterScaleMIdx() const
    {
        return BASE_MODULE::outerIndex_ / stepScaleM_;
    }

    /**
     * @description: Get next scaleM outer loop index, used for ClearL1BufferCache in SchedulerMDL
     * @param: void
     * @return: return next scaleM outerIdx
     */
    __aicore__ inline int32_t GetNextOuterScaleMIdx() const
    {
        return (BASE_MODULE::outerIndex_ + 1) / scaleFactorM_;
    }

protected:
    __aicore__ inline void UpdateOuterParams()
    {
        BASE_MODULE::UpdateOuterParams();
        int32_t curScaleOuterIdx = BASE_MODULE::innerStartIdx_ / stepScaleM_;
        if (curScaleOuterIdx + 1 >= Ceil(BASE_MODULE::outerIter_, scaleFactorM_)) {
            tileShapeScaleM_ = tailScaleStepM_;
        } else {
            tileShapeScaleM_ = mainShapeScaleM_;
        }
    }

private:
    uint8_t scaleFactorM_;
    uint32_t stepScaleM_;
    int32_t tailScaleStepM_; 
    int32_t tileShapeScaleM_;
    int32_t mainShapeScaleM_; 
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_MX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_MX_H__
#endif // _M_LOOP_MDL_MX_H_