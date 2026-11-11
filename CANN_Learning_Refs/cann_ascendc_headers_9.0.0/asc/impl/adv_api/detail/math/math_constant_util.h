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
 * \file math_constant_util.h
 * \brief defined common used math related constant value.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/math_constant_util.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/round.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_MATH_CONSTANT_UTIL_H__
#endif
#ifndef IMPL_MATH_MATH_CONSTANT_UTIL_H
#define IMPL_MATH_MATH_CONSTANT_UTIL_H

#include <cstdint>

namespace AscendC {

constexpr float NUM_ONE = 1.0;
constexpr float NEG_ONE = -1.0;
constexpr float HALF_PI = 1.5707963267948966192313216916398;
constexpr float BOUNDARY = 0.70710678118654752440084436210485;

constexpr uint8_t ASIN_HALF_CALC_PROCEDURE = 6;
constexpr uint8_t ASIN_FLOAT_CALC_PROCEDURE = 4;
constexpr uint32_t ASIN_TAYLOR_EXPAND_COUNT = 7;
// Coefficient values of taylor expansion of asin.
constexpr float kCOEF[] = {
    1.0,
    0.16666666666666666666666666666667,
    0.075,
    0.04464285714285714285714285714286,
    0.03038194444444444444444444444444,
    0.02237215909090909090909090909091,
    0.01735276442307692307692307692308,
    0.01396484375,
};
} // namespace AscendC
#endif // IMPL_MATH_MATH_CONSTANT_UTIL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_MATH_CONSTANT_UTIL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_MATH_CONSTANT_UTIL_H__
#endif