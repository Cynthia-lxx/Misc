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
 * \file is_finite.h
 * \brief
 */


#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_IS_FINITE_H__
#endif

#ifndef LIB_MATH_IS_FINITE_H
#define LIB_MATH_IS_FINITE_H
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
#include "kernel_tensor.h"

#include "../../../impl/adv_api/detail/math/isfinite/is_finite_common_impl.h"

namespace AscendC {

/*!
 * \ingroup isFinite
 * \brief do isFinite elementwisely.
 * \tparam T: input dataType, support float/half/bf16
 * \tparam U: output dataType, support float/half/bf16/bool
 * \param [out] dst: output LocalTensor
 * \param [in]  src: base LocalTensor
 * \param [in]  calCount: amount of output data to be calculated
 */

template<typename T, typename U>
__aicore__ inline void IsFinite(const LocalTensor<U>& dst, const LocalTensor<T>& src, uint32_t calCount)
{
    IsFiniteImpl<T, U>(dst, src, calCount);
}

}  // namespace AscendC
#endif
#endif  // LIB_MATH_IS_FINITE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_IS_FINITE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_IS_FINITE_H__
#endif