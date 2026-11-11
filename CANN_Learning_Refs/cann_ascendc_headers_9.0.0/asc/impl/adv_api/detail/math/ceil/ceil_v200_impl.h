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
 * \file ceil_v200_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/ceil/ceil_v200_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/ceil.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CEIL_CEIL_V200_IMPL_H__
#endif
#ifndef IMPL_MATH_CEIL_CEIL_V200_IMPL_H
#define IMPL_MATH_CEIL_CEIL_V200_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002

namespace AscendC {
__aicore__ inline void CeilProcess(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor, const LocalTensor<uint8_t>& tmpTensor)
{
    const LocalTensor<int32_t> intTmpTensor = tmpTensor.ReinterpretCast<int32_t>();
    Cast<int32_t, float, false>(intTmpTensor, srcTensor, RoundMode::CAST_CEIL, MASK_PLACEHOLDER, 1,
        { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();

    Cast<float, int32_t, false>(dstTensor, intTmpTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
}

}  // namespace AscendC
#endif
#endif  // IMPL_MATH_CEIL_CEIL_V200_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CEIL_CEIL_V200_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CEIL_CEIL_V200_IMPL_H__
#endif
