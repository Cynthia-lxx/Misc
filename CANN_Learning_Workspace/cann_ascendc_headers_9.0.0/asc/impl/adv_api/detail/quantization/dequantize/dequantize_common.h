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
 * \file dequantize_common.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/quantization/dequantize/dequantize_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/dequantize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANTIZE_DEQUANTIZE_COMMON_H__
#endif

#ifndef LIB_DEQUANTIZE_IMPL_DEQUANTIZE_COMMON_H
#define LIB_DEQUANTIZE_IMPL_DEQUANTIZE_COMMON_H

namespace AscendC {
enum class DequantizePolicy : int32_t {
    PER_TENSOR,
    PER_CHANNEL,
    PER_TOKEN,
    PER_GROUP,
};

struct DequantizeConfig {
    DequantizePolicy policy;
    bool hasOffset = false;
    int32_t kDim = 1;
};

struct DequantizeParams {
    uint32_t m;
    uint32_t n;
    uint32_t groupSize = 0;
};
} // namespace AscendC
#endif // LIB_DEQUANTIZE_IMPL_DEQUANTIZE_COMMON_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANTIZE_DEQUANTIZE_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANTIZE_DEQUANTIZE_COMMON_H__
#endif
