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
 * \file where.h
 * \brief
 */


#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WHERE_H__
#endif

#ifndef LIB_MATH_WHERE_H
#define LIB_MATH_WHERE_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
#include "kernel_tensor.h"
#include "../../../impl/adv_api/detail/math/where/where_c310_impl.h"

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \ingroup Where
 * \param [out] dst, output LocalTensor
 * \param [in] src0, input0 LocalTensor
 * \param [in] src1, input1 LocalTensor
 * \param [in] condition, condition LocalTensor
 * \param [in] count, amount of data to be calculated
 */
template <typename T, typename U, typename S, typename V>
__aicore__ inline void Where(const LocalTensor<T>& dst, const U& src0, const S& src1,
    const LocalTensor<V>& condition, const uint32_t count)
{
    if ASCEND_IS_AIC {
        return;
    }
    WhereImpl<T, U, S, V>(dst, src0, src1, condition, count);
}
#pragma end_pipe
} // namespace AscendC
#endif
#endif // LIB_MATH_WHERE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WHERE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WHERE_H__
#endif