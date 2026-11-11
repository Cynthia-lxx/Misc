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
 * \file layernorm_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_UTILS_H__
#endif

#ifndef LIB_NORMALIZATION_LAYERNORM_UTILS_H
#define LIB_NORMALIZATION_LAYERNORM_UTILS_H

#include <cstdint>
#include "kernel_basic_intf.h"
namespace AscendC {

struct LayerNormConfig {
    bool isNoBeta = false;
    bool isNoGamma = false;
    bool isOnlyOutput = false;
    bool isOutputRstd = true;
};

struct WelfordUpdateConfig {
    __aicore__ constexpr WelfordUpdateConfig(const bool isInplaceIn): isInplace(isInplaceIn) {}
    bool isInplace = false;
};

constexpr WelfordUpdateConfig WFUPDATE_DEFAULT_CFG = {false};

struct LayerNormPara {
    uint32_t aLength;
    uint32_t rLength;
    uint32_t rLengthWithPadding;
};

struct WelfordUpdateParam {
    uint32_t rnLength;
    uint32_t abLength;
    uint32_t abComputeLength;
    float nRec;
};

}; // namespace AscendC
#endif // LIB_NORMALIZATION_LAYERNORM_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_UTILS_H__
#endif