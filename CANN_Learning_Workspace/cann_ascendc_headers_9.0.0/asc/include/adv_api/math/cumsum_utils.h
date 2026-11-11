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
 * \file cumsum_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_UTILS_H__
#endif

#ifndef LIB_MATH_CUMSUM_UTILS_H
#define LIB_MATH_CUMSUM_UTILS_H

#include <cstdint>

namespace AscendC {
enum class CumSumAlgorithm {
    CUMSUM_ALGORITHM_LINEBYLINE = 0,
    CUMSUM_ALGORITHM_SKLANSKY = 1
};

struct CumSumConfig {
    bool isLastAxis{true};
    bool isReuseSource{false};
    bool outputLastRow{false};
    CumSumAlgorithm algorithm{CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
};

struct CumSumInfo {
    uint32_t outter{0};
    uint32_t inner{0};  // 32-byte aligned
};

}; // namespace AscendC
#endif // LIB_MATH_CUMSUM_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_UTILS_H__
#endif