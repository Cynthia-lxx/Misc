/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_COMMON_H__
#endif

#ifndef LIB_REDUCE_REDUCE_COMMON_H
#define LIB_REDUCE_REDUCE_COMMON_H

#include "../../../impl/adv_api/detail/reduce/reduce_common_util_impl.h"

namespace AscendC {
namespace Pattern {
namespace Reduce {
/*
 * @brief Reduce pattern
 * R means reduce axis, A means non-reduce axis
 * each A/R represents a dimension
*/
struct R : private Detail::PatternConstInfo<Detail::PATTERN_R, true, false, 1> {};
struct RA : private Detail::PatternConstInfo<Detail::PATTERN_RA, false, true, Detail::DIM_TWO> {};
struct AR : private Detail::PatternConstInfo<Detail::PATTERN_AR, true, false, Detail::DIM_TWO> {};
struct ARA : private Detail::PatternConstInfo<Detail::PATTERN_ARA, true, true, Detail::DIM_THREE> {};
struct ARAR : private Detail::PatternConstInfo<Detail::PATTERN_ARAR, true, false, Detail::DIM_FOUR> {};
struct ARARA : private Detail::PatternConstInfo<Detail::PATTERN_ARARA, true, true, Detail::DIM_FIVE> {};
struct ARARAR : private Detail::PatternConstInfo<Detail::PATTERN_ARARAR, true, false, Detail::DIM_SIX> {};
struct ARARARA : private Detail::PatternConstInfo<Detail::PATTERN_ARARARA, true, true, Detail::DIM_SEVEN> {};
struct ARARARAR : private Detail::PatternConstInfo<Detail::PATTERN_ARARARAR, true, false, Detail::DIM_EIGHT> {};
struct ARARARARA : private Detail::PatternConstInfo<Detail::PATTERN_ARARARARA, true, true, Detail::DIM_NINE> {};
struct RAR : private Detail::PatternConstInfo<Detail::PATTERN_RAR, false, false, Detail::DIM_THREE> {};
struct RARA : private Detail::PatternConstInfo<Detail::PATTERN_RARA, false, true, Detail::DIM_FOUR> {};
struct RARAR : private Detail::PatternConstInfo<Detail::PATTERN_RARAR, false, false, Detail::DIM_FIVE> {};
struct RARARA : private Detail::PatternConstInfo<Detail::PATTERN_RARARA, false, true, Detail::DIM_SIX> {};
struct RARARAR : private Detail::PatternConstInfo<Detail::PATTERN_RARARAR, false, false, Detail::DIM_SEVEN> {};
struct RARARARA : private Detail::PatternConstInfo<Detail::PATTERN_RARARARA, false, true, Detail::DIM_EIGHT> {};
} // namespace Reduce
} // namespace Pattern
} // namespace AscendC
#endif  // LIB_REDUCE_REDUCE_COMMON_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_COMMON_H__
#endif