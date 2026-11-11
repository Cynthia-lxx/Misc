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
 * \file kernel_simt_constant.h
 * \brief
 */
#ifndef ASCENDC_MODULE_SIMT_CONSTANT_H
#define ASCENDC_MODULE_SIMT_CONSTANT_H

namespace AscendC
{
namespace Simt
{
namespace ConstantsInternal
{
    constexpr int32_t ONE_INT32 = 1;
    constexpr int64_t ONE_INT64 = 1;
    constexpr uint32_t ONE_UINT32 = 1;
    constexpr uint64_t ONE_UINT64 = 1;

    constexpr float PI = 3.141592653589793;
    constexpr float PI_OF_2 = 1.5707963267948966;
    constexpr float PI_OF_4 = 0.7853981633974483;
    constexpr float PI_OF_8 = 0.39269908169872415480783042290994;
    constexpr float SCALAR_LN2 = 0.69314718055994530941723212145818;

    constexpr uint8_t ONE_BYTE_LEN = 8;
    constexpr uint8_t TWO_BYTE_LEN = 16;
    constexpr uint8_t THREE_BYTE_LEN = 24;
    constexpr uint8_t FOUR_BYTE_LEN = 32;
    constexpr uint8_t EIGHT_BYTE_LEN = 64;
    constexpr int32_t F16_MIN_VAL = -14;
    constexpr uint32_t F16_MAN_BIT_LEN = 10;
    constexpr uint32_t U16_BIT = 16;

    constexpr uint32_t INF = 0x7F800000;
    constexpr uint32_t NEG_INF = 0xFF800000;
    constexpr uint32_t MAX_NAN = 0x7FFFFFFF;
    constexpr uint32_t HALF_INF = 0x7C00;
    constexpr uint32_t HALF_MAX_NAN = 0x7FFF;
    constexpr uint32_t HALF_NEG_INF = 0xFC00;
    constexpr uint32_t B_HALF_INF = 0x7F80;
    constexpr uint32_t B_HALF_NEG_INF = 0xFF80;
    constexpr uint32_t HIGH_16_BIT = 0xFFFF0000;
    constexpr uint32_t U32_MAX_VAL = 0x7FFFFFFF;
    constexpr uint64_t U64_MAX_VAL = 0x7FFFFFFFFFFFFFFF;
    constexpr uint32_t MAN_BIT_FLOAT = 0x7FFFFF;
    constexpr uint32_t EXP_BIT_FLOAT = 0x7F800000;
    constexpr uint32_t NEG_SIGN_BIT = 0x80000000;
    constexpr uint32_t FULL_MASK_B32 = 0xffffffff;
    constexpr int32_t S32_MAX_VAL = 0x7FFFFFFF;
    constexpr int32_t S32_MIN_VAL = 0x80000000;
    constexpr int64_t S64_MAX_VAL = 0x7FFFFFFFFFFFFFFF;
    constexpr int64_t S64_MIN_VAL = 0x8000000000000000;
    constexpr float SIMT_FP32_INF = (__builtin_inff());
    constexpr int32_t SIMT_INT32_INF = 2139095040;
    constexpr float SIMT_INFINITY = (__builtin_inff());

    constexpr uint32_t SIMT_DUMP_BLOCK_NUM = 72;
    constexpr uint32_t SIMT_DUMP_SIZE = 2048;
    constexpr uint32_t SIMT_MAX_THREAD_NUM = 2048;

    constexpr float TWO_OVER_PI = 0.63661975f; 
    constexpr float MINUS_PI_OVER_TWO_HI = -1.5707964f;
    constexpr float MINUS_PI_OVER_TWO_LO = 4.371139e-8f;
} // namespace SimtConstantsInternal
}
}
#endif //ASCENDC_MODULE_SIMT_CONSTANT_H
