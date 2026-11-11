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
* \file k_loop_mdl_mx.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/k_loop/k_loop_mdl_mx.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_MX_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_MX_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_MX_H
 
#include "k_loop_intf.h"
#include "k_loop_mdl_base.h"
 
namespace AscendC {
namespace Impl {
namespace Detail {
/*
    KLoop for Scale A/B is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    KLoop is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, typename TRANS_T, class A_TYPE, const auto& MM_CFG>
class KLoop<IMPL, TRANS_T, A_TYPE, MM_CFG, enable_if_t<(DoMatmulMDL(MM_CFG) && !IsKdimReorderLoad<MM_CFG>) && HasScalePosition<A_TYPE>::value>>
    : public KLoopMDLBase<IMPL, TRANS_T, A_TYPE, MM_CFG>
{
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(CopyCubeOut);
    MATMUL_USE_MODULE(CubeOutBuffer);
    MATMUL_USE_MODULE(CopyCubeInA);
    MATMUL_USE_MODULE(CopyCubeInB);
    MATMUL_USE_MODULE(CopyCubeInScaleA);
    MATMUL_USE_MODULE(CopyCubeInScaleB);
    MATMUL_USE_MODULE(BiasScheduler);
    MATMUL_USE_MODULE(TBufPoolL0);
    MATMUL_USE_MODULE(MatmulSubBlockInfo);
    MATMUL_USE_MODULE(MatmulQuantProcessor);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulCrossCoreSync);
 
public:
    using BASE_MODULE = AscendC::Impl::Detail::KLoopMDLBase<IMPL, TRANS_T, A_TYPE, MM_CFG>;
    __aicore__ inline KLoop() = default;
    __aicore__ inline ~KLoop() = default;
 
    __aicore__ inline void Init(int32_t singleShape)
    {
        SetSingleShape(singleShape);
    }

    __aicore__ inline void SetSingleShape(int32_t singleShape)
    {
        BASE_MODULE::SetSingleShape(singleShape);
        const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        int32_t stepKa = tiling.GetStepKa();
        int32_t stepKb = tiling.GetStepKb();
        int32_t baseK = tiling.GetBaseK();
        scaleFactorKa_ = tiling.GetScaleFactorKa();
        scaleFactorKb_ = tiling.GetScaleFactorKb();
        if (BASE_MODULE::kIter_ > stepKa * scaleFactorKa_) {
            if constexpr (!DoMatmulSpecialMDL(MM_CFG)) {
                ASCENDC_ASSERT(tiling.GetScaleFactorM() == 1,
                    { KERNEL_LOG(KERNEL_ERROR, "scaleFactorM is %d, which can only be 1.", tiling.GetScaleFactorM()); });
            }
        }
        // originTailScaleStepKa_ represents the origin size of the GM -> L1 tail block
        tailScaleStepKa_ = singleShape % (baseK * stepKa * scaleFactorKa_);
        tailScaleStepKb_ = singleShape % (baseK * stepKb * scaleFactorKb_);
        if (tailScaleStepKa_ == 0) {
            tailScaleStepKa_ = baseK * stepKa * scaleFactorKa_;
        }
        if (tailScaleStepKb_ == 0) {
            tailScaleStepKb_ = baseK * stepKb * scaleFactorKb_;
        }

        tailScaleStepKa_ = Ceil(tailScaleStepKa_, MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
        tailScaleStepKb_ = Ceil(tailScaleStepKb_, MX_BASEK_FACTOR) * MX_EVEN_FACTOR;

        isScaleA1KFullLoad_ = (stepKa * scaleFactorKa_) >= BASE_MODULE::kIter_;
        isScaleB1KFullLoad_ = (stepKb * scaleFactorKb_) >= BASE_MODULE::kIter_;
    }
 
    __aicore__ inline void OuterStart()
    {
        BASE_MODULE::outerIdx_ = 0;
        UpdateOuterParams();
    }
 
    __aicore__ inline bool OuterNext()
    {
        BASE_MODULE::outerIdx_++;
        if (BASE_MODULE::OuterEnd()) {
            return false;
        } else {
            UpdateOuterParams();
            return true;
        }
    }
 
    __aicore__ inline bool IsScaleAKL1FullLoad() const
    {
        return isScaleA1KFullLoad_;
    }
 
    __aicore__ inline bool IsScaleBKL1FullLoad() const
    {
        return isScaleB1KFullLoad_;
    }

    /**
     * @description: Get current scaleKa outer loop index, used for GetBufferPos in CopyCubeIn
     * @param: void
     * @return: return current scaleKa outerIdx
     */
    __aicore__ inline int32_t GetOuterScaleKaIdx() const
    {
        return BASE_MODULE::outerIdx_ / (BASE_MODULE::kaStepFactor_ * scaleFactorKa_);
    }
 
    /**
     * @description: Get current scaleKb outer loop index, used for GetBufferPos in CopyCubeIn
     * @param: void
     * @return: return current scaleKb outerIdx
     */
    __aicore__ inline int32_t GetOuterScaleKbIdx() const
    {
        return BASE_MODULE::outerIdx_ / (BASE_MODULE::kbStepFactor_ * scaleFactorKb_);
    }

    /**
     * @description: Get next scaleKa outer loop index, used for ClearL1BufferCache in SchedulerMDL
     * @param: void
     * @return: return next scaleKa outerIdx
     */
    __aicore__ inline int32_t GetNextOuterScaleKaIdx() const
    {
        return (BASE_MODULE::outerIdx_ + 1) / (BASE_MODULE::kaStepFactor_ * scaleFactorKa_);
    }
 
    /**
     * @description: Get next scaleKb outer loop index, used for ClearL1BufferCache in SchedulerMDL
     * @param: void
     * @return: return next scaleKb outerIdx
     */
    __aicore__ inline int32_t GetNextOuterScaleKbIdx() const
    {
        return (BASE_MODULE::outerIdx_ + 1) / (BASE_MODULE::kbStepFactor_ * scaleFactorKb_);
    }
 
    __aicore__ inline int32_t GetTileShapeScaleKa() const
    {
        return tileShapeScaleKa_;
    }
 
    __aicore__ inline int32_t GetTileShapeScaleKb() const
    {
        return tileShapeScaleKb_;
    }
 
    __aicore__ inline int32_t GetScaleFactorKa() const
    {
        return scaleFactorKa_;
    }
 
    __aicore__ inline int32_t GetScaleFactorKb() const
    {
        return scaleFactorKb_;
    }

    __aicore__ inline void InnerStart()
    {
        if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
            BASE_MODULE::outerIdx_ = 0;
            UpdateOuterParams();
            BASE_MODULE::innerIdx_ = 0;
        } else {
            BASE_MODULE::innerIdx_ = BASE_MODULE::innerStartIdx_;
        }
        BASE_MODULE::UpdateInnerParams(AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
    }

    __aicore__ inline bool InnerEnd()
    {
        if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
            return BASE_MODULE::innerIdx_ >= BASE_MODULE::kIter_;
        }
        return BASE_MODULE::innerIdx_ >= BASE_MODULE::innerStartIdx_ + BASE_MODULE::innerIter_;
    }

    __aicore__ inline bool InnerNext()
    {
        BASE_MODULE::innerIdx_++;
        if (InnerEnd()) {
            return false;
        } else {
            if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
                // compute outer from inner
                BASE_MODULE::outerIdx_ = BASE_MODULE::innerIdx_ / BASE_MODULE::minStepK_;
                UpdateOuterParams();
            }
            BASE_MODULE::UpdateInnerParams(AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
            return true;
        }
    }

    __aicore__ inline int32_t GetStepInnerIdx() const
    {
        // loop index within each step
        return BASE_MODULE::innerIdx_ % BASE_MODULE::minStepK_;
    }

protected:
    __aicore__ inline void UpdateOuterParams()
    {
        BASE_MODULE::UpdateOuterParams();
        auto tilingStepKa = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa();
        auto tilingStepKb = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb();
        auto tilingBaseK = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        // for scaleA/scaleB
        scaleFactorKa_ = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetScaleFactorKa();
        scaleFactorKb_ = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetScaleFactorKb();

        int32_t curScaleKaOuterIdx = BASE_MODULE::innerStartIdx_ / (tilingStepKa * scaleFactorKa_);
        int32_t curScaleKbOuterIdx = BASE_MODULE::innerStartIdx_ / (tilingStepKb * scaleFactorKb_);

        if (curScaleKaOuterIdx + 1 >= Ceil(BASE_MODULE::outerKaIter_, scaleFactorKa_)) {
            tileShapeScaleKa_ = tailScaleStepKa_;
        } else {
            tileShapeScaleKa_ = Ceil(tilingStepKa * tilingBaseK * scaleFactorKa_, MX_K_FACTOR);
        }

        if (curScaleKbOuterIdx + 1 >= Ceil(BASE_MODULE::outerKbIter_, scaleFactorKb_)) {
            tileShapeScaleKb_ = tailScaleStepKb_;
        } else {
            tileShapeScaleKb_ = Ceil(tilingStepKb * tilingBaseK * scaleFactorKb_, MX_K_FACTOR);
        }
    }
 
private:
    int32_t tileShapeScaleKa_;       // scaleKaL1 length
    int32_t tileShapeScaleKb_;       // scaleKbL1 length
    uint8_t scaleFactorKa_;
    uint8_t scaleFactorKb_;
 
    int32_t tailScaleStepKa_; // align scaleKaL1 length for GM->L1
    int32_t tailScaleStepKb_; // align scaleKbL1 length for GM->L1
    bool isScaleA1KFullLoad_, isScaleB1KFullLoad_;
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_MX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_MX_H__
#endif // _K_LOOP_MDL_MX_H_