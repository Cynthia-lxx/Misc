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
 * \file kernel_struct_gather.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_GATHER_H__
#endif

#ifndef ASCENDC_MODULE_STRUCT_GATHER_H
#define ASCENDC_MODULE_STRUCT_GATHER_H

#include "kernel_macros.h"
#include "utils/kernel_utils_constants.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

namespace AscendC {
struct GatherRepeatParams {
    __aicore__ GatherRepeatParams() {}

    __aicore__ GatherRepeatParams(const uint8_t dstBlkStrideIn, const uint8_t dstRepStrideIn)
        : dstBlkStride(dstBlkStrideIn),
          dstRepStride(dstRepStrideIn)
    {}

    uint32_t blockNumber = DEFAULT_BLK_NUM;
    uint16_t dstRepStride = DEFAULT_REPEAT_STRIDE;
    uint8_t dstBlkStride = DEFAULT_BLK_STRIDE;
    uint8_t src0BlkStride = DEFAULT_BLK_STRIDE;
    uint8_t src1BlkStride = DEFAULT_BLK_STRIDE;
    uint8_t src0RepStride = DEFAULT_REPEAT_STRIDE;
    uint8_t src1RepStride = DEFAULT_REPEAT_STRIDE;
    bool repeatStrideMode = false;
    bool strideSizeMode = false;
};

enum class GatherMaskMode : uint8_t {
    VERSION_V1 = 0,
    VERSION_V2 = 1
};

#if (__NPU_ARCH__ == 2201)
const GatherMaskMode defaultGahterMaskMode = GatherMaskMode::VERSION_V2;
const GatherMaskMode defaultGatherMaskMode = GatherMaskMode::VERSION_V2;
#else
const GatherMaskMode defaultGahterMaskMode = GatherMaskMode::VERSION_V1;
const GatherMaskMode defaultGatherMaskMode = GatherMaskMode::VERSION_V1;
#endif

struct GatherMaskParams {
    __aicore__ GatherMaskParams() {}

    __aicore__ GatherMaskParams(const uint8_t src0BlockStrideIn, const uint16_t repeatTimesIn,
        const uint16_t src0RepeatStrideIn, const uint8_t src1RepeatStrideIn)
        : src0BlockStride(src0BlockStrideIn),
          repeatTimes(repeatTimesIn),
          src0RepeatStride(src0RepeatStrideIn),
          src1RepeatStride(src1RepeatStrideIn)
    {}

    uint8_t src0BlockStride = DEFAULT_BLK_STRIDE;
    uint16_t repeatTimes = 0;
    uint16_t src0RepeatStride = DEFAULT_REPEAT_STRIDE;
    uint8_t src1RepeatStride = DEFAULT_REPEAT_STRIDE;
};
} // namespace AscendC
#endif // ASCENDC_MODULE_STRUCT_GATHER_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_GATHER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_GATHER_H__
#endif
