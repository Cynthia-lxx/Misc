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
 * \file quantize_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZE_UTILS_H__
#endif

#ifndef LIB_QUANTIZATION_QUANTIZE_UTILS_H
#define LIB_QUANTIZATION_QUANTIZE_UTILS_H

#include <cstdint>
#include "kernel_basic_intf.h"

namespace AscendC {
enum class QuantizePolicy : int32_t {
    PER_TENSOR,
    PER_CHANNEL,
    PER_TOKEN,
    PER_GROUP
};

struct QuantizeConfig {
    QuantizePolicy policy;
    bool hasOffset;
    RoundMode roundMode = RoundMode::CAST_RINT;
    int32_t kDim = 1;
};

struct QuantizeParams {
  uint32_t m;
  uint32_t n;
  uint32_t groupSize = 0;
};

}; // namespace AscendC
#endif // LIB_QUANTIZATION_QUANTIZE_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZE_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZE_UTILS_H__
#endif