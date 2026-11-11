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
 * \file normalize_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZE_UTILS_H__
#endif

#ifndef LIB_NORMALIZATION_NORMALIZE_UTILS_H
#define LIB_NORMALIZATION_NORMALIZE_UTILS_H

#include <cstdint>
namespace AscendC {

#ifndef ASCC_ENUM_REDUCEPATTERN
#define ASCC_ENUM_REDUCEPATTERN
enum class ReducePattern : uint32_t {
    AR = 0,
    RA = 1,
    R,
    ARA,
    ARAR,
    ARARA,
    ARARAR,
    ARARARA,
    ARARARAR,
    ARARARARA,
    RAR,
    RARA,
    RARAR,
    RARARA,
    RARARAR,
    RARARARA,
};
#endif // ASCC_ENUM_REDUCEPATTERN

struct NormalizeConfig {
    ReducePattern reducePattern = ReducePattern::AR;
    int32_t aLength = -1;
    bool isNoBeta = false;
    bool isNoGamma = false;
    bool isOnlyOutput = false;
};

struct NormalizePara {
    uint32_t aLength;
    uint32_t rLength;
    uint32_t rLengthWithPadding;
};

}; // namespace AscendC
#endif // LIB_NORMALIZATION_NORMALIZE_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZE_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZE_UTILS_H__
#endif