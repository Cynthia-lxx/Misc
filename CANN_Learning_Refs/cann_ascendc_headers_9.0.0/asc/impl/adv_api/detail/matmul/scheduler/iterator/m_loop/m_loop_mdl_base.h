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
 * \file m_loop_mdl_base.h
 * \brief m_loop base class for mdl and mdl_outer_product
 */


#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/m_loop/m_loop_mdl_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_BASE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_BASE_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_BASE_H

#include "../../../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    MLoopMDLBase is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MLoopMDLBase is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, const auto& MM_CFG>
class MLoopMDLBase
{
    MATMUL_USE_MODULE(MatmulShapeTiling);
public:
    __aicore__ inline MLoopMDLBase() = default;
    __aicore__ inline ~MLoopMDLBase() = default;

    __aicore__ inline void Init(int32_t singleShape)
    {
        SetSingleShape(singleShape);
    }

    __aicore__ inline void SetSingleShape(int32_t singleShape) {
        SetSingleShapeImpl(singleShape, AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
    }

    __aicore__ inline uint32_t GetTotalIter() const
    {
        return totalIter_;
    }

    __aicore__ inline bool OuterNext()
    {
        if constexpr (IsBasicM(MM_CFG)) {
            return false;
        } else {
            outerIndex_++;
            UpdateOuterParams();
            return !OuterEnd();
        }
    }

    __aicore__ inline void OuterStart()
    {
        outerIndex_ = 0;
        innerIndex_ = 0;
        UpdateOuterParams();
    }

    __aicore__ inline bool OuterEnd()
    {
        if constexpr (IsBasicM(MM_CFG)) {
            return true;
        } else {
            return outerIndex_ + 1 > outerIter_;
        }
    }

    __aicore__ inline bool IsLastOuterIter() const
    {
        if constexpr (IMPL::POLICY::POLICY_TYPE == PolicyType::MATMUL_NBUFFER_33) {
            return innerIndex_ + 1 >= innerIter_;
        } else {
            return outerIndex_ + 1 >= outerIter_;
        }
    }

    __aicore__ inline uint32_t GetOuterIdx() const
    {
        if constexpr (IMPL::POLICY::POLICY_TYPE == PolicyType::MATMUL_NBUFFER_33) {
            return innerIndex_;
        } else {
            return outerIndex_;
        }
    }

    __aicore__ inline uint32_t GetOuterIter() const
    {
        if constexpr (IMPL::POLICY::POLICY_TYPE == PolicyType::MATMUL_NBUFFER_33) {
            return innerIter_;
        } else {
            return outerIter_;
        }
    }

    __aicore__ inline int32_t GetTileShape() const
    {
        return tileShape_;
    }

    __aicore__ inline int32_t GetTileShapeOf(int32_t outerIdx) const
    {
        return (outerIdx + 1 >= outerIter_) ? tailTileShape_ : mainTileShape_;
    }

    __aicore__ inline int32_t GetTileBlockShape() const
    {
        return tileBlockShape_;
    }

    __aicore__ inline bool InnerNext()
    {
        if constexpr (IsBasicM(MM_CFG)) {
            return false;
        } else {
            ++innerIndex_;
            UpdateInnerParams();
            return !InnerEnd();
        }
    }

    __aicore__ inline void InnerStart()
    {
        innerIndex_ = innerStartIdx_;
        UpdateInnerParams();
    }

    __aicore__ inline bool InnerEnd()
    {
        return innerIndex_ >= innerStartIdx_ + innerIter_;
    }

    __aicore__ inline bool IsLastInnerIter()
    {
        return innerIndex_ == innerStartIdx_ + innerIter_ - 1;
    }

    __aicore__ inline uint32_t GetInnerIdx() const
    {
        return innerIndex_;
    }

    __aicore__ inline uint32_t GetInnerIter() const
    {
        return innerIter_;
    }

    __aicore__ inline int32_t GetBaseShape() const
    {
        return baseShape_;
    }

    __aicore__ inline int32_t GetBaseBlockShape() const
    {
        return baseBlockShape_;
    }

    __aicore__ inline void UpdateInnerParams() {
        UpdateInnerParamsImpl(AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
    }

    __aicore__ inline bool IsAML1FullLoad() const
    {
        return isA1MFullLoad_;
    }

private:
    template <PolicyType P>
    __aicore__ inline enable_if_t<P != PolicyType::MATMUL_NBUFFER_33, void>
    SetSingleShapeImpl(int32_t singleShape, AscendC::Std::integral_constant<PolicyType, P>)
    {
        if constexpr (ToMatmulConfig(MM_CFG).singleCoreM != 0 && ToMatmulConfig(MM_CFG).basicM != 0 &&
            !ToMatmulConfig(MM_CFG).enableSetTail) {
            SetSingleShapeFromCFG(AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
        } else {
            SetSingleShapeFromTiling(singleShape, AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
        }
        ASCENDC_ASSERT((totalIter_ > 0), {
            KERNEL_LOG(KERNEL_ERROR, "invalid singleCoreM, totalIter_ is %d , which should be larger than 0",
                totalIter_);
        });
    }

    __aicore__ inline void SetSingleShapeImpl(int32_t singleShape,
        AscendC::Std::integral_constant<PolicyType, PolicyType::MATMUL_NBUFFER_33>)
    {
        if constexpr (ToMatmulConfig(MM_CFG).singleCoreM != 0 && ToMatmulConfig(MM_CFG).basicM != 0 &&
            !ToMatmulConfig(MM_CFG).enableSetTail) {
            SetSingleShapeFromCFG(AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
        } else {
            SetSingleShapeFromTiling(singleShape, AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
        }
        ASCENDC_ASSERT((totalIter_ <= N_BUFFER_33_FACTOR), {
            KERNEL_LOG(KERNEL_ERROR, "Invalid singleCoreM or baseM, totalIter_ is %d , "
                "which should be less than or equal to 3.", totalIter_);
        });
        ASCENDC_ASSERT((totalIter_ > 0), {
            KERNEL_LOG(KERNEL_ERROR, "Invalid singleCoreM, totalIter_ is %d , which should be larger than 0",
                totalIter_);
        });
    }

    template <PolicyType P>
    __aicore__ inline enable_if_t<P != PolicyType::MATMUL_NBUFFER_33, void>
    SetSingleShapeFromCFG(AscendC::Std::integral_constant<PolicyType, P>)
    {
        totalIter_ = GetMIter(MM_CFG);
        outerIter_ = Ceil(static_cast<uint32_t>(ToMatmulConfig(MM_CFG).singleCoreM),
                            static_cast<uint32_t>(ToMatmulConfig(MM_CFG).basicM * ToMatmulConfig(MM_CFG).stepM));
        tailBaseShape_ = ToMatmulConfig(MM_CFG).singleCoreM % ToMatmulConfig(MM_CFG).basicM;
        if (tailBaseShape_ == 0) {
            tailBaseShape_ = ToMatmulConfig(MM_CFG).basicM;
        }
        mainTileShape_ = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM() *
                        MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM();
        tailTileShape_ = ToMatmulConfig(MM_CFG).singleCoreM % mainTileShape_;
        if (tailTileShape_ == 0) {
            tailTileShape_ = mainTileShape_;
        }
        isA1MFullLoad_ = ToMatmulConfig(MM_CFG).stepM >= totalIter_;
    }

    __aicore__ inline void SetSingleShapeFromCFG(AscendC::Std::integral_constant<PolicyType, PolicyType::MATMUL_NBUFFER_33>)
    {
        ASCENDC_ASSERT((ToMatmulConfig(MM_CFG).stepM == N_BUFFER_33_FACTOR), {
            KERNEL_LOG(KERNEL_ERROR, "StepM %d should be equal to 3 in case of MATMUL_NBUFFER_33 Policy.",
                ToMatmulConfig(MM_CFG).stepM);
        });
        totalIter_ = GetMIter(MM_CFG);
        mainTileShape_ = ToMatmulConfig(MM_CFG).basicM;
        tailTileShape_ = ToMatmulConfig(MM_CFG).singleCoreM % mainTileShape_;
        if (tailTileShape_ == 0) {
            tailTileShape_ = mainTileShape_;
        }
        tailBaseShape_ = tailTileShape_;

        outerIter_ = 1;
        innerIter_ = totalIter_;
        isA1MFullLoad_ = true;
    }

    template <PolicyType P>
    __aicore__ inline enable_if_t<P != PolicyType::MATMUL_NBUFFER_33, void>
    SetSingleShapeFromTiling(int32_t singleShape, AscendC::Std::integral_constant<PolicyType, P>)
    {
        auto tilingBaseM = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        totalIter_ = singleShape / tilingBaseM;
        tailBaseShape_ = singleShape - totalIter_ * tilingBaseM;
        if (tailBaseShape_ == 0) {
            tailBaseShape_ = tilingBaseM;
        } else {
            totalIter_ += 1;
        }
        mainTileShape_ = tilingBaseM * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM();
        outerIter_ = Ceil(static_cast<uint32_t>(singleShape), static_cast<uint32_t>(mainTileShape_));
        tailTileShape_ = singleShape % mainTileShape_;
        if (tailTileShape_ == 0) {
            tailTileShape_ = mainTileShape_;
        }
        isA1MFullLoad_ = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() >= totalIter_;
    }

    __aicore__ inline void SetSingleShapeFromTiling(int32_t singleShape,
        AscendC::Std::integral_constant<PolicyType, PolicyType::MATMUL_NBUFFER_33>)
    {
        ASCENDC_ASSERT((MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() <= N_BUFFER_33_FACTOR), {
            KERNEL_LOG(KERNEL_ERROR, "StepM %d should be less than or equal to 3 in case of MATMUL_NBUFFER_33 Policy.",
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM());
        });
        mainTileShape_ = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        totalIter_ = singleShape / mainTileShape_;
        tailTileShape_ = singleShape % mainTileShape_;
        if (tailTileShape_ == 0) {
            tailTileShape_ = mainTileShape_;
        } else {
            totalIter_ += 1;
        }
        tailBaseShape_ = tailTileShape_;

        outerIter_ = 1;
        innerIter_ = totalIter_;
        isA1MFullLoad_ = true;
    }

    template <PolicyType P>
    __aicore__ inline enable_if_t<P != PolicyType::MATMUL_NBUFFER_33, void>
    UpdateInnerParamsImpl(AscendC::Std::integral_constant<PolicyType, P>)
    {
        if constexpr (IsBasicM(MM_CFG)) {
            baseShape_ = tailBaseShape_;
        } else {
            baseShape_ = (innerIndex_ + 1 == totalIter_) ? tailBaseShape_ :
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        }
        baseBlockShape_ = Ceil(baseShape_, BLOCK_CUBE);
    }

    __aicore__ inline void UpdateInnerParamsImpl(AscendC::Std::integral_constant<PolicyType, PolicyType::MATMUL_NBUFFER_33>)
    {
        if constexpr (IsBasicM(MM_CFG)) {
            tileShape_ = tailTileShape_;
            baseShape_ = tailBaseShape_;
        } else {
            tileShape_ = (innerIndex_ + 1 == totalIter_) ? tailTileShape_ : mainTileShape_;
            baseShape_ = (innerIndex_ + 1 == totalIter_) ? tailBaseShape_ :
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        }
        tileBlockShape_ = Ceil(tileShape_, BLOCK_CUBE);
        baseBlockShape_ = Ceil(baseShape_, BLOCK_CUBE);
    }

protected:
    __aicore__ inline void UpdateOuterParams()
    {
        if constexpr (IsBasicM(MM_CFG)) {
            innerStartIdx_ = 0;
            innerIter_ = 1;
            tileShape_ = tailTileShape_;
        } else {
            innerStartIdx_ = outerIndex_ * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM();
            innerIter_ = (totalIter_ - innerStartIdx_) > MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() ?
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() : (totalIter_ - innerStartIdx_);
            tileShape_ = (outerIndex_ + 1 >= outerIter_) ? tailTileShape_ : mainTileShape_;
        }
        tileBlockShape_ = Ceil(tileShape_, BLOCK_CUBE);
    }
    uint32_t totalIter_;
    // OuterLoop
    uint32_t outerIndex_ = 0;
    uint32_t outerIter_;
    // InnerLoop
    uint32_t innerIndex_ = 0;
    uint32_t innerIter_;
    uint32_t innerStartIdx_ = 0;
    // Shape
    int32_t mainTileShape_;
    int32_t tailTileShape_;
    int32_t tileShape_;
    int32_t tileBlockShape_;
    int32_t baseShape_;
    int32_t tailBaseShape_;
    int32_t baseBlockShape_;
    bool isA1MFullLoad_;
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _M_LOOP_MDL_BASE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_MDL_BASE_H__
#endif