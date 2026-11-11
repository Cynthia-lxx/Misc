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
 * \file softmax_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_UTILS_H__
#endif

#ifndef LIB_ACTIVATION_SOFTMAX_UTILS_H
#define LIB_ACTIVATION_SOFTMAX_UTILS_H

#include <cstdint>
#include "kernel_basic_intf.h"

namespace AscendC {

enum class SoftmaxMode {
    SOFTMAX_NORMAL = 0,
    SOFTMAX_OUTPUT_WITHOUT_BRC = 1,
};

struct SoftmaxConfig {
    __aicore__ constexpr SoftmaxConfig(const bool isCheckTilingIn)
    {
        isCheckTiling = isCheckTilingIn;
    }
    __aicore__ constexpr SoftmaxConfig(const bool isCheckTilingIn, const uint32_t oriSrcMIn, const uint32_t oriSrcKIn)
    {
        isCheckTiling = isCheckTilingIn;
        oriSrcM = oriSrcMIn;
        oriSrcK = oriSrcKIn;
    }
    __aicore__ constexpr SoftmaxConfig(const bool isCheckTilingIn, const uint32_t oriSrcMIn, const uint32_t oriSrcKIn, const enum SoftmaxMode modeIn)
    {
        isCheckTiling = isCheckTilingIn;
        oriSrcM = oriSrcMIn;
        oriSrcK = oriSrcKIn;
        mode = modeIn;
    }
    // to judge if match or not of input shape and tiling, if not match, api will recompute tiling, default to judge
    bool isCheckTiling = true;
    uint32_t oriSrcM = 0;
    uint32_t oriSrcK = 0;
    SoftmaxMode mode = SoftmaxMode::SOFTMAX_NORMAL;
};

constexpr SoftmaxConfig SOFTMAX_DEFAULT_CFG = { true, 0, 0 , SoftmaxMode::SOFTMAX_NORMAL};

struct SoftMaxParams {
    uint32_t srcM{ 0 };
    uint32_t srcK{ 0 };
    uint32_t oriSrcM{ 0 };
    uint32_t oriSrcK{ 0 };
    uint32_t loopCnt{ 1 };
    uint32_t splitMeanCnt{ 8 };
    float alpha{ 0.9375 };
};

struct SoftMaxShapeInfo {
    uint32_t srcM{ 0 };
    uint32_t srcK{ 0 };
    uint32_t oriSrcM{ 0 };
    uint32_t oriSrcK{ 0 };
};

}; // namespace AscendC
#endif // LIB_ACTIVATION_SOFTMAX_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_UTILS_H__
#endif