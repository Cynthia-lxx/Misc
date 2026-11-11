/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/* !
 * \file reglu_v200_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/reglu/reglu_v200_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/reglu.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REGLU_V200_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_REGLU_REGLU_V200_IMPL_H
#define IMPL_ACTIVATION_REGLU_REGLU_V200_IMPL_H
#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {
template <typename T>
__aicore__ inline void ReGluCast(const LocalTensor<T>& dstTensor, const LocalTensor<float>& srcTensor)
{
    Cast<T, float, false>(dstTensor, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
}
} //  namespace AscendC
#endif // IMPL_ACTIVATION_REGLU_REGLU_V200_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REGLU_V200_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REGLU_V200_IMPL_H__
#endif
