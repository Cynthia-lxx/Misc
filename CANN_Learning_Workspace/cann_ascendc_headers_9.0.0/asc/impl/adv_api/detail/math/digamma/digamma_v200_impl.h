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
 * \file digamma_v200_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/digamma/digamma_v200_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/digamma.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_V200_IMPL_H__
#endif
#ifndef IMPL_MATH_DIGAMMA_DIGAMMA_V200_IMPL_H
#define IMPL_MATH_DIGAMMA_DIGAMMA_V200_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {
__aicore__ inline void DigammaCast(const LocalTensor<float>& dst, const LocalTensor<float>& src, RoundMode castType)
{
    Cast<int32_t, float, false>(dst.ReinterpretCast<int32_t>(), src, castType, MASK_PLACEHOLDER, 1, {1, 1, 
        DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
    Cast<float, int32_t, false>(dst, dst.ReinterpretCast<int32_t>(), RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
}
} // namespace AscendC

#endif // IMPL_MATH_DIGAMMA_DIGAMMA_V200_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_V200_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_V200_IMPL_H__
#endif
