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
 * \file n_loop_mdl_outer_product.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/iterator/n_loop/n_loop_mdl_outer_product.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_OUTER_PRODUCT_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_OUTER_PRODUCT_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_OUTER_PRODUCT_H

#include "n_loop_intf.h"
#include "n_loop_mdl_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    NLoop is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    NLoop is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, const auto& MM_CFG>
class NLoop<IMPL, A_TYPE, MM_CFG, enable_if_t<DoMatmulMDL(MM_CFG) &&
    MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()>>
    : public NLoopMDLBase<IMPL, A_TYPE, MM_CFG>
{
    MATMUL_USE_MODULE(MatmulShapeTiling);
public:
    using BASE_MODULE = AscendC::Impl::Detail::NLoopMDLBase<IMPL, A_TYPE, MM_CFG>;
    __aicore__ inline NLoop() = default;
    __aicore__ inline ~NLoop() = default;

    __aicore__ inline bool InnerNext()
    {
        if constexpr (IsBasicN(MM_CFG)) {
            return false;
        } else {
            if (IsL0DoubleBuffer()) {
                BASE_MODULE::innerIndex_ += DB_FACTOR;
            } else {
                ++BASE_MODULE::innerIndex_;
            }
            BASE_MODULE::UpdateInnerParams();
            CalcDBLoopNum();
            return !BASE_MODULE::InnerEnd();
        }
    }

    __aicore__ inline void InnerStart()
    {
        BASE_MODULE::innerIndex_ = BASE_MODULE::innerStartIdx_;
        CalcDBLoopNum();
        BASE_MODULE::UpdateInnerParams();
    }

    __aicore__ inline bool IsL0DoubleBuffer()
    {
        if constexpr(ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_N) {
            return false; // ORDER_N do not support n axis l0 double buffer
        } else {
            if (BASE_MODULE::outerIndex_ + 1 < BASE_MODULE::outerIter_) {
                return BASE_MODULE::innerIndex_ + DB_FACTOR <= BASE_MODULE::innerStartIdx_ + BASE_MODULE::innerIter_;
            } else {
                return (BASE_MODULE::innerIndex_ + DB_FACTOR < BASE_MODULE::totalIter_) ||
                    (BASE_MODULE::innerIndex_ + DB_FACTOR == BASE_MODULE::totalIter_ &&
                    BASE_MODULE::tailBaseShape_ == MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN());
            }
        }
    }

    __aicore__ inline uint32_t GetL0DBLoopNum() const
    {
        return l0dbLoopNum;
    }

    __aicore__ inline int32_t GetOuterScaleNIdx() const
    {
        return BASE_MODULE::outerIndex_;
    }

    __aicore__ inline int32_t GetNextOuterScaleNIdx() const
    {
        return BASE_MODULE::outerIndex_ + 1;
    }

    __aicore__ inline int32_t GetTileShapeScaleN() const
    {
        return BASE_MODULE::GetTileShape();
    }

    __aicore__ inline int32_t GetScaleFactorN() const
    {
        return 1;
    }

private:
    __aicore__ inline void CalcDBLoopNum()
    {
        l0dbLoopNum = IsL0DoubleBuffer() ? DB_FACTOR : 1;
    }

    // DBLoop
    uint32_t l0dbLoopNum = 1;
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _N_LOOP_MDL_OUTER_PRODUCT_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_OUTER_PRODUCT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_N_LOOP_N_LOOP_MDL_OUTER_PRODUCT_H__
#endif