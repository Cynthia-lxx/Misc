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
 * \file m_loop_basic.h
 * \brief
 */


#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/m_loop/m_loop_basic.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BASIC_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BASIC_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BASIC_H

#include "m_loop_intf.h"
#include "m_loop_norm_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    MLoop is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MLoop is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, const auto& MM_CFG>
class MLoop<IMPL, A_TYPE, MM_CFG, enable_if_t<IsBasicBlockEnable<MM_CFG>>> : public MLoopNormBase<IMPL, A_TYPE, MM_CFG>
{
    MATMUL_USE_MODULE(MatmulShapeTiling);
public:
    using BASE_MODULE = AscendC::Impl::Detail::MLoopNormBase<IMPL, A_TYPE, MM_CFG>;
    __aicore__ inline MLoop() = default;
    __aicore__ inline ~MLoop() = default;

    __aicore__ inline void Init(int32_t singleShape)
    {
        SetSingleShape(singleShape);
    }

    __aicore__ inline void SetSingleShape(int32_t singleShape)
    {
        BASE_MODULE::baseShape_ = ToMatmulConfig(MM_CFG).basicM;
        BASE_MODULE::baseBlockShape_ = Ceil(BASE_MODULE::baseShape_, BLOCK_CUBE);
        BASE_MODULE::totalIter_ = Ceil(static_cast<uint32_t>(singleShape), static_cast<uint32_t>(BASE_MODULE::baseShape_));
        ASCENDC_ASSERT((BASE_MODULE::totalIter_ > 0), {
            KERNEL_LOG(KERNEL_ERROR, "invalid singleCoreM, totalIter_ is %d , which should be larger than 0",
                BASE_MODULE::totalIter_);
        });
    }

    __aicore__ inline bool OuterNext()
    {
        BASE_MODULE::innerStartIdx_ += BASE_MODULE::innerIter_;
        UpdateOuterParams();
        return !BASE_MODULE::OuterEnd();
    }

    __aicore__ inline void OuterStart()
    {
        BASE_MODULE::innerStartIdx_ = 0;
        UpdateOuterParams();
    }

    __aicore__ inline bool OuterEnd()
    {
        return BASE_MODULE::innerStartIdx_ >= BASE_MODULE::totalIter_;
    }

    __aicore__ inline uint32_t GetOuterIdx() const
    {
        return Ceil(BASE_MODULE::innerStartIdx_, GetStepM());
    }

    __aicore__ inline uint32_t GetOuterIter() const
    {
        return Ceil(BASE_MODULE::totalIter_, GetStepM());
    }

    __aicore__ inline bool InnerNext()
    {
        ++BASE_MODULE::innerIndex_;
        return !BASE_MODULE::InnerEnd();
    }

    __aicore__ inline void InnerStart()
    {
        BASE_MODULE::innerIndex_ = BASE_MODULE::innerStartIdx_;
    }
private:
    __aicore__ inline uint32_t GetStepM() const
    {
        if constexpr (ToMatmulConfig(MM_CFG).stepM != 0) {
            return ToMatmulConfig(MM_CFG).stepM;
        } else {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM();
        }
    }

    __aicore__ inline void UpdateOuterParams()
    {
        auto stepM = GetStepM();
        BASE_MODULE::innerIter_ = (BASE_MODULE::totalIter_ - BASE_MODULE::innerStartIdx_) > stepM ? stepM :
            (BASE_MODULE::totalIter_ - BASE_MODULE::innerStartIdx_);
    }
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _M_LOOP_BASIC_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BASIC_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_M_LOOP_M_LOOP_BASIC_H__
#endif