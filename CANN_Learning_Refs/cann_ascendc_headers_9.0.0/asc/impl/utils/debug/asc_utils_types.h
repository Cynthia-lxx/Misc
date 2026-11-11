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
 * \file asc_utils_types.h
 * \brief
 */
#ifndef IMPL_UTILS_BASE_ASC_UTILS_TYPES_H
#define IMPL_UTILS_BASE_ASC_UTILS_TYPES_H

#include <cstdint>
#include <type_traits>

namespace __asc_aicore {
enum class Hardware : uint8_t { GM, UB, L1, L0A, L0B, L0C, BIAS, FIXBUF, MAX };
}

struct AscTlvHead { // TLV Head include type and length
    uint16_t type;
    uint16_t len;
};

struct AscTlv {
    AscTlvHead head;
    uint32_t value; // feature: PRINT = 1, FFTS = 2, L2CACHE = 3
};

constexpr uint16_t ASC_ONE_DATABLOCK_SIZE = 32;

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_UTILS_TYPES__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_UTILS_TYPES__
#endif

#endif // IMPL_UTILS_BASE_ASC_UTILS_TYPES_H