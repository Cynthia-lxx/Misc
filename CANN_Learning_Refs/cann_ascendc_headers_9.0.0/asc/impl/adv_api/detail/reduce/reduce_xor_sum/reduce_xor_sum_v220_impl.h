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
 * \file reduce_xor_sum_v200_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/reduce/reduce_xor_sum/reduce_xor_sum_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce_xor_sum.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_XOR_SUM_REDUCE_XOR_SUM_V220_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_XOR_SUM_REDUCE_XOR_SUM_V220_IMPL_H
#define IMPL_REDUCE_REDUCE_XOR_SUM_REDUCE_XOR_SUM_V220_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {
__aicore__ inline void CastInt162Float(const LocalTensor<float>& dst, const LocalTensor<int16_t>& src)
{
    Cast<float, int16_t, false>(dst, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
                          {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
}

__aicore__ inline void CastFloat2Int16(const LocalTensor<int16_t>& dst, const LocalTensor<float>& src)
{
    Cast<int16_t, float, false>(dst, src, RoundMode::CAST_ROUND, MASK_PLACEHOLDER, 1,
                          {1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
}
} //  namespace AscendC
#endif // IMPL_REDUCE_REDUCE_XOR_SUM_REDUCE_XOR_SUM_V220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_XOR_SUM_REDUCE_XOR_SUM_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_XOR_SUM_REDUCE_XOR_SUM_V220_IMPL_H__
#endif
