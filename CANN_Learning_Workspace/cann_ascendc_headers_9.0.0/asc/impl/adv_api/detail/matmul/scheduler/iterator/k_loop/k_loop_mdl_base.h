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
* \file k_loop_mdl_base.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/k_loop/k_loop_mdl_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_BASE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_BASE_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_BASE_H

#include "k_loop_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    KLoopMDLBase is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    KLoopMDLBase is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, typename TRANS_T, class A_TYPE, const auto& MM_CFG>
class KLoopMDLBase
{
    MATMUL_USE_MODULE(MatmulShapeTiling);

public:
    __aicore__ inline KLoopMDLBase() = default;
    __aicore__ inline ~KLoopMDLBase() = default;

    __aicore__ inline void Init(int32_t singleShape)
    {
        SetSingleShape(singleShape);
    }

    __aicore__ inline void SetSingleShape(int32_t singleShape) {
        SetSingleShapeImpl(singleShape, AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
    }

    __aicore__ inline void OuterStart()
    {
        outerIdx_ = 0;
        UpdateOuterParams();
    }

    __aicore__ inline bool OuterNext()
    {
        outerIdx_++;
        if (OuterEnd()) {
            return false;
        } else {
            UpdateOuterParams();
            return true;
        }
    }

    __aicore__ inline bool OuterEnd()
    {
        return outerIdx_ >= outIter_;
    }

    __aicore__ inline bool FirstOuterIter() const
    {
        return outerIdx_ == 0;
    }

    __aicore__ inline bool LastOuterIter() const
    {
        return outerIdx_ + 1 == outIter_;
    }

    __aicore__ inline void InnerStart()
    {
        innerIdx_ = innerStartIdx_;
        UpdateInnerParams(AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
    }

    __aicore__ inline bool InnerNext()
    {
        innerIdx_++;
        if (InnerEnd()) {
            return false;
        } else {
            UpdateInnerParams(AscendC::Std::integral_constant<PolicyType, IMPL::POLICY::POLICY_TYPE>{});
            return true;
        }
    }

    __aicore__ inline bool InnerEnd()
    {
        return innerIdx_ >= innerStartIdx_ + innerIter_;
    }

    __aicore__ inline bool FirstInnerIter() const
    {
        return innerIdx_ == 0;
    }

    __aicore__ inline int32_t GetTotalIter() const
    {
        return kIter_;
    }

    __aicore__ inline bool IsAKL1FullLoad() const
    {
        return isA1KFullLoad_;
    }

    __aicore__ inline bool IsBKL1FullLoad() const
    {
        return isB1KFullLoad_;
    }

    __aicore__ inline int32_t GetInnerStartIdx() const
    {
        return innerStartIdx_;
    }

    __aicore__ inline int32_t GetOuterIter() const
    {
        return outIter_;
    }

    __aicore__ inline int32_t GetInnerIter() const
    {
        return innerIter_;
    }

    __aicore__ inline int32_t GetOuterIdx() const
    {
        return outerIdx_;
    }

    /**
     * @description: Get current ka outer loop index, used for GetBufferPos in CopyCubeIn
     * @param: void
     * @return: return current ka outerIdx
     */
    __aicore__ inline int32_t GetOuterKaIdx() const
    {
        return outerIdx_ / kaStepFactor_;
    }

    /**
     * @description: Get current kb outer loop index, used for GetBufferPos in CopyCubeIn
     * @param: void
     * @return: return current kb outerIdx
     */
    __aicore__ inline int32_t GetOuterKbIdx() const
    {
        return outerIdx_ / kbStepFactor_;
    }

    /**
     * @description: Get next ka outer loop index, used for ClearL1BufferCache in SchedulerMDL
     * @param: void
     * @return: return next ka outerIdx
     */
    __aicore__ inline int32_t GetNextOuterKaIdx() const
    {
        return (outerIdx_ + 1) / kaStepFactor_;
    }

    /**
     * @description: Get next kb outer loop index, used for ClearL1BufferCache in SchedulerMDL
     * @param: void
     * @return: return next kb outerIdx
     */
    __aicore__ inline int32_t GetNextOuterKbIdx() const
    {
        return (outerIdx_ + 1) / kbStepFactor_;
    }

    __aicore__ inline int32_t GetInnerIdx() const
    {
        return innerIdx_;
    }

    __aicore__ inline int32_t GetTileShapeA() const
    {
        return tileShapeA_;
    }

    /**
     * @description: Get specified loop index's kaL1 length, used when Preload is enabled
     * @param: curOuterIdx: specified outer loop index
     * @return: return kaL1 length
     */
    __aicore__ inline int32_t GetTileShapeAOf(int32_t curOuterIdx) const
    {
        if constexpr (IMPL::POLICY::POLICY_TYPE == PolicyType::MATMUL_NBUFFER_33) {
            return tileShapeA_;
        } else {
            return (curOuterIdx + 1 >= outerKaIter_) ? tailStepKa_ :
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa() *
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        }
    }

    __aicore__ inline int32_t GetTileShapeB() const
    {
        return tileShapeB_;
    }

    /**
     * @description: Get specified loop index's kbL1 length, used when Preload is enabled
     * @param: curOuterIdx: specified outer loop index
     * @return: return kbL1 length
     */
    __aicore__ inline int32_t GetTileShapeBOf(int32_t curOuterIdx) const
    {
        if constexpr (IMPL::POLICY::POLICY_TYPE == PolicyType::MATMUL_NBUFFER_33) {
            return tileShapeB_;
        } else {
            return (curOuterIdx + 1 >= outerKbIter_) ? tailStepKb_ :
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb() *
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        }
    }

    __aicore__ inline int32_t GetTileBlockShapeA() const
    {
        return tileBlockShapeA_;
    }

    __aicore__ inline int32_t GetTileBlockShapeB() const
    {
        return tileBlockShapeB_;
    }

    __aicore__ inline int32_t GetBaseShape() const
    {
        return baseShape_;
    }

    __aicore__ inline int32_t GetBaseBlockShape() const
    {
        return baseBlockShape_;
    }

protected:
    template <PolicyType P>
    __aicore__ inline enable_if_t<P != PolicyType::MATMUL_NBUFFER_33, void>
    SetSingleShapeImpl(int32_t singleShape, AscendC::Std::integral_constant<PolicyType, P>)
    {
        const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        int32_t stepKa = tiling.GetStepKa();
        int32_t stepKb = tiling.GetStepKb();
        int32_t baseK = tiling.GetBaseK();
        kIter_ = IsBasicK(MM_CFG) ? 1 : Ceil(static_cast<uint32_t>(singleShape), static_cast<uint32_t>(baseK));
        ASCENDC_ASSERT((kIter_ > 0),
            { KERNEL_LOG(KERNEL_ERROR, "kIter_ is %d , which should be larger than 0", kIter_); });
        if (kIter_ > stepKa) {
            if constexpr (!DoMatmulSpecialMDL(MM_CFG)) {
                ASCENDC_ASSERT((tiling.GetStepM() == 1),
                        { KERNEL_LOG(KERNEL_ERROR, "stepM is %d which can only be 1", tiling.GetStepM()); });
            }
        }
        if (kIter_ > stepKb) {
            if constexpr (!DoMatmulSpecialMDL(MM_CFG)) {
                ASCENDC_ASSERT((tiling.GetStepN() == 1),
                    { KERNEL_LOG(KERNEL_ERROR, "stepN is %d which can only be 1", tiling.GetStepN()); });
            }
        }
        if constexpr (NoTailK(MM_CFG)) {
            tailK_ = baseK;
        } else {
            tailK_ = singleShape % baseK;
            tailK_ = (tailK_ == 0) ? baseK : tailK_;
        }
        // get outer loop params
        minStepK_ = stepKa > stepKb ? stepKb : stepKa;
        kaStepFactor_ = stepKa / minStepK_;
        kbStepFactor_ = stepKb / minStepK_;
        ASCENDC_ASSERT((kaStepFactor_ >= 1 && kbStepFactor_ >= 1), { KERNEL_LOG(KERNEL_ERROR,
            "kaStepFactor_ %d and kbStepFactor_ %d should be no less than 1", kaStepFactor_, kbStepFactor_); });
        outerKaIter_ = Ceil(static_cast<uint32_t>(singleShape), static_cast<uint32_t>(baseK * stepKa));
        outerKbIter_ = Ceil(static_cast<uint32_t>(singleShape), static_cast<uint32_t>(baseK * stepKb));
        ASCENDC_ASSERT((outerKaIter_ % outerKbIter_ == 0 || outerKbIter_ % outerKaIter_ == 0), {
            KERNEL_LOG(KERNEL_ERROR, "outerKaIter_ %d ,  outerKbIter_ is %d, "
            "outerKaIter_ and outerKbIter_ should be in multiple relationship.", outerKaIter_, outerKbIter_);
        });
        outIter_ = outerKaIter_ > outerKbIter_ ? outerKaIter_ : outerKbIter_;
        tailStepKa_ = singleShape % (baseK * stepKa);
        tailStepKb_ = singleShape % (baseK * stepKb);
        if (tailStepKa_ == 0) {
            tailStepKa_ = baseK * stepKa;
        }
        if (tailStepKb_ == 0) {
            tailStepKb_ = baseK * stepKb;
        }
        isA1KFullLoad_ = stepKa >= kIter_;
        isB1KFullLoad_ = stepKb >= kIter_;
    }

    __aicore__ inline void SetSingleShapeImpl(int32_t singleShape,
        AscendC::Std::integral_constant<PolicyType, PolicyType::MATMUL_NBUFFER_33>)
    {
        const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        int32_t stepKa = tiling.GetStepKa();
        int32_t stepKb = tiling.GetStepKb();
        int32_t baseK = tiling.GetBaseK();
        int32_t baseNum = Ceil(static_cast<uint32_t>(singleShape), static_cast<uint32_t>(baseK));
        ASCENDC_ASSERT((stepKa == baseNum && stepKa <= N_BUFFER_33_FACTOR),
            { KERNEL_LOG(KERNEL_ERROR, "stepKa is %d , which should be less than or equal to 3, "
            "and exactly equal to singleCoreK / baseK (currently is %d)", stepKa, baseNum); });
        ASCENDC_ASSERT((stepKa == stepKb),
            { KERNEL_LOG(KERNEL_ERROR, "stepKb %d should be equal to stepKa %d", stepKb, stepKa); });

        if constexpr (IsBasicK(MM_CFG)) {
            kIter_ = 1;
        } else {
            kIter_ = baseNum;
        }
        ASCENDC_ASSERT((kIter_ > 0 && kIter_ <= N_BUFFER_33_FACTOR),
            { KERNEL_LOG(KERNEL_ERROR, "kIter_ is %d , which should be larger than 0 and less than or equal to 3",
            kIter_); });

        if constexpr (NoTailK(MM_CFG)) {
            tailK_ = baseK;
        } else {
            tailK_ = singleShape % baseK;
            if (tailK_ == 0) {
                tailK_ = baseK;
            }
        }

        kaStepFactor_ = 1;
        kbStepFactor_ = 1;
        isA1KFullLoad_ = true;
        isB1KFullLoad_ = true;
        outIter_ = 1;
        innerIter_ = kIter_;
        innerStartIdx_ = 0;
        tileShapeA_ = singleShape;
        tileShapeB_ = singleShape;
        tileBlockShapeA_ = Ceil(tileShapeA_, c0Size_);
        tileBlockShapeB_ = Ceil(tileShapeB_, c0Size_);
    }

    __aicore__ inline void UpdateOuterParams()
    {
        if constexpr (IMPL::POLICY::POLICY_TYPE == PolicyType::MATMUL_NBUFFER_33) {
            baseShape_ = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
            baseBlockShape_ = Ceil(baseShape_, c0Size_);
            return;
        }
        auto tilingStepKa = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa();
        auto tilingStepKb = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb();
        innerStartIdx_ = outerIdx_ * minStepK_;
        int32_t curKaOuterIdx = innerStartIdx_ / tilingStepKa;
        int32_t curKbOuterIdx = innerStartIdx_ / tilingStepKb;
        ASCENDC_ASSERT((innerStartIdx_ >= curKaOuterIdx * tilingStepKa), {
            KERNEL_LOG(KERNEL_ERROR, "k is %d , minStepK_ is %d, curKaOuterIdx is %d, stepKa is %d,"
            "(k * minStepK_) should >= (curKaOuterIdx * stepKa)", outerIdx_, minStepK_, curKaOuterIdx, tilingStepKa);
        });
        ASCENDC_ASSERT((innerStartIdx_ >= curKbOuterIdx * tilingStepKb), {
            KERNEL_LOG(KERNEL_ERROR, "k is %d , minStepK_ is %d, curKbOuterIdx is %d, stepKb is %d,"
            "(k * minStepK_) should >= (curKbOuterIdx * stepKb)", outerIdx_, minStepK_, curKbOuterIdx, tilingStepKb);
        });

        auto tilingBaseK = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        tileShapeA_ =
            (curKaOuterIdx + 1 >= outerKaIter_) ? tailStepKa_ : tilingStepKa * tilingBaseK;
        tileShapeB_ =
            (curKbOuterIdx + 1 >= outerKbIter_) ? tailStepKb_ : tilingStepKb * tilingBaseK;
        tileBlockShapeA_ = Ceil(tileShapeA_, c0Size_);
        tileBlockShapeB_ = Ceil(tileShapeB_, c0Size_);

        // update inner loop common params
        baseSize_ = (outerIdx_ + 1 == kIter_) ? tailK_ : MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        baseShape_ = baseSize_;
        baseBlockShape_ = Ceil(baseSize_, c0Size_);
        int32_t baseBlockSize = baseBlockShape_ * c0Size_;
        int32_t tileShape = tileShapeA_ > tileShapeB_ ? tileShapeB_ : tileShapeA_;
        innerIter_ = tileShape / baseBlockSize;
        innerTailK_ = tileShape - innerIter_ * baseBlockSize;
        if (innerTailK_ == 0) {
            innerTailK_ = baseBlockSize;
        } else {
            innerIter_ = innerIter_ + 1;
        }
    }

    template <PolicyType P>
    __aicore__ inline enable_if_t<P != PolicyType::MATMUL_NBUFFER_33, void>
    UpdateInnerParams(AscendC::Std::integral_constant<PolicyType, P>)
    {
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            baseShape_ = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        } else {
            baseShape_ = (innerIdx_ == innerStartIdx_ + innerIter_ -1) ? innerTailK_ : baseSize_;
        }
    }

    __aicore__ inline void UpdateInnerParams(AscendC::Std::integral_constant<PolicyType, PolicyType::MATMUL_NBUFFER_33>)
    {
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            baseShape_ = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        } else {
            baseShape_ = (innerIdx_ == innerIter_ -1) ? tailK_ :
                            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        }
        baseBlockShape_ = Ceil(baseShape_, c0Size_);
    }

protected:
    int32_t tailK_;
    int32_t tailStepKa_;
    int32_t tailStepKb_;
    int32_t minStepK_;         // lesser value of stepKa and stepKb
    int32_t baseSize_;         // kL1 base size, used for updating baseShape_
    int32_t innerTailK_;       // kL1 tail size of current outer loop, used for updating baseShape_

    int32_t tileShapeA_;       // kaL1 length
    int32_t tileShapeB_;       // kbL1 length
    int32_t tileBlockShapeA_;  // kaL1 block num
    int32_t tileBlockShapeB_;  // kbL1 block num
    int32_t kaStepFactor_;     // indicates the coefficient of stepka and minStepK_
    int32_t kbStepFactor_;     // indicates the coefficient of stepkb and minStepK_

    int32_t baseShape_;        // kL0 length
    int32_t baseBlockShape_;   // kL0 block num

    int32_t kIter_;           // total iterations counts
    int32_t outIter_;         // outer loop counts, greater value of outerKaIter_ and outerKbIter_;
    int32_t innerIter_;       // inner loop counts
    int32_t outerKaIter_;     // outer ka loop counts
    int32_t outerKbIter_;     // outer kb loop counts
    int32_t outerIdx_ {0};        // current outer loop index
    int32_t innerStartIdx_;   // inner loop start index of current outer loop, used for indicating k's index
                               // when load to L1 and calculating L1 offset when load to l0
    int32_t innerIdx_ {0};        // current inner loop index
    bool isA1KFullLoad_, isB1KFullLoad_;

    constexpr static int32_t c0Size_ = AuxGetC0Size<typename A_TYPE::T>();
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _K_LOOP_MDL_BASE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_MDL_BASE_H__
#endif