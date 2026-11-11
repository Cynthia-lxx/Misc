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
 * \file ascend_dequant_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASCEND_DEQUANT_UTILS_H__
#endif

#ifndef LIB_QUANTIZATION_ASCEND_DEQUANT_UTILS_H
#define LIB_QUANTIZATION_ASCEND_DEQUANT_UTILS_H

#include <cstdint>
namespace AscendC {
struct DequantParams {
    uint32_t m;             // outer axis length (do not need to be 32B aligned)              in unit of element num
    uint32_t n;             // inner axis length (must be 32B aligned)                        in unit of element num
    uint32_t calCount;      // in one inner line, calCount elements do dequant calculation    in unit of element num
};

}; // namespace AscendC
#endif // LIB_QUANTIZATION_ASCEND_DEQUANT_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASCEND_DEQUANT_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASCEND_DEQUANT_UTILS_H__
#endif