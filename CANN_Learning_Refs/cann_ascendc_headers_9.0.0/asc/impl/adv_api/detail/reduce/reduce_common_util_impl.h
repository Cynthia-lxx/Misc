/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/reduce/reduce_common_util_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_UTIL_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_COMMON_UTIL_IMPL_H
#define IMPL_REDUCE_REDUCE_COMMON_UTIL_IMPL_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "../common/check.h"

namespace AscendC {
namespace Pattern {
namespace Detail {
constexpr int32_t DIM_TWO = 2;
constexpr int32_t DIM_THREE = 3;
constexpr int32_t DIM_FOUR = 4;
constexpr int32_t DIM_FIVE = 5;
constexpr int32_t DIM_SIX = 6;
constexpr int32_t DIM_SEVEN = 7;
constexpr int32_t DIM_EIGHT = 8;
constexpr int32_t DIM_NINE = 9;
constexpr int32_t PATTERN_R = 0;
constexpr int32_t PATTERN_RA = 1;
constexpr int32_t PATTERN_AR = 2;
constexpr int32_t PATTERN_ARA = 3;
constexpr int32_t PATTERN_ARAR = 4;
constexpr int32_t PATTERN_ARARA = 5;
constexpr int32_t PATTERN_ARARAR = 6;
constexpr int32_t PATTERN_ARARARA = 7;
constexpr int32_t PATTERN_ARARARAR = 8;
constexpr int32_t PATTERN_ARARARARA = 9;
constexpr int32_t PATTERN_RAR = 10;
constexpr int32_t PATTERN_RARA = 11;
constexpr int32_t PATTERN_RARAR = 12;
constexpr int32_t PATTERN_RARARA = 13;
constexpr int32_t PATTERN_RARARAR = 14;
constexpr int32_t PATTERN_RARARARA = 15;

template <int32_t id, bool firstA, bool tailA, int32_t dim>
struct PatternConstInfo {
constexpr static int32_t ID = id;
constexpr static bool FirstA = firstA;
constexpr static bool TailA = tailA;
constexpr static int32_t Dim = dim;
};
} // namespace Detail
} // namespace Pattern

namespace Internal {
// api mode
enum class ApiMode : uint8_t {
    API_MODE_SUM = 0,
    API_MODE_MIN,
    API_MODE_MAX,
    API_MODE_ANY,
    API_MODE_ALL
};

// Find the closest power of two, except 0.
__aicore__ inline uint32_t FindClosestPowerOfTwo(uint32_t n)
{
    ASCENDC_ASSERT(n != 0, { KERNEL_LOG(KERNEL_ERROR, "input n must be non-zero!"); });
    constexpr uint32_t totalShiftBits = 63;
    return totalShiftBits - ScalarCountLeadingZero(n);
}

template <class T>
__aicore__ inline void ComputeMaskBit(uint32_t oneBlkMask, uint32_t oneBlkElems, uint32_t blkNum,
    uint64_t& maskLow, uint64_t& maskHigh)
{
    // Get bit mask on each blkNum, oneBlkElems means number of elements in one block
    // only support half/float
    if constexpr (sizeof(T) == sizeof(half)) {
        uint32_t maskLBlkNum = blkNum > HALF_DEFAULT_REPEAT_STRIDE ? HALF_DEFAULT_REPEAT_STRIDE : blkNum;
        uint32_t maskHBlkNum = blkNum - maskLBlkNum;
        for (int32_t k = 0; k < maskLBlkNum; k++) {
            maskLow += (((1ULL << oneBlkMask) - 1ULL) << (k * oneBlkElems));
        }
        for (int32_t k = 0; k < maskHBlkNum; k++) {
            maskHigh += (((1ULL << oneBlkMask) - 1ULL) << (k * oneBlkElems));
        }
    } else if constexpr (sizeof(T) == sizeof(float)) {
        for (int32_t k = 0; k < blkNum; k++) {
            maskLow += (((1ULL << oneBlkMask) - 1ULL) << (k * oneBlkElems));
        }
    }
}

template <class T, ApiMode apiMode, MaskMode maskMode = MaskMode::NORMAL>
__aicore__ inline void BlockReduceCompute(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const int32_t repeat, const uint64_t mask[], const int32_t blkStride, const int32_t repStride)
{
    if constexpr (maskMode == MaskMode::NORMAL) {
        if constexpr (apiMode == ApiMode::API_MODE_SUM) {
            BlockReduceSum(dstTensor, srcTensor, repeat, mask, 1, 1, DEFAULT_REPEAT_STRIDE);
        } else if constexpr (apiMode == ApiMode::API_MODE_MIN || apiMode == ApiMode::API_MODE_ALL) {
            BlockReduceMin(dstTensor, srcTensor, repeat, mask, 1, 1, DEFAULT_REPEAT_STRIDE);
        } else if constexpr (apiMode == ApiMode::API_MODE_MAX || apiMode == ApiMode::API_MODE_ANY) {
            BlockReduceMax(dstTensor, srcTensor, repeat, mask, 1, 1, DEFAULT_REPEAT_STRIDE);
        }
    } else {
        if constexpr (apiMode == ApiMode::API_MODE_MIN || apiMode == ApiMode::API_MODE_ALL) {
            BlockReduceMin<T, false>(dstTensor, srcTensor, 1, MASK_PLACEHOLDER, 1, blkStride, repStride);
        } else if constexpr (apiMode == ApiMode::API_MODE_MAX || apiMode == ApiMode::API_MODE_ANY) {
            BlockReduceMax<T, false>(dstTensor, srcTensor, 1, MASK_PLACEHOLDER, 1, blkStride, repStride);
        }
    }
}

template <class T, ApiMode apiMode>
__aicore__ inline void WholeReduceCompute(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const int32_t repeat, const int32_t mask, const int32_t repStride)
{
    if constexpr (apiMode == ApiMode::API_MODE_MIN || apiMode == ApiMode::API_MODE_ALL) {
        WholeReduceMin(dstTensor, srcTensor, mask, repeat, 1, 1, repStride, ReduceOrder::ORDER_ONLY_VALUE);
    } else if constexpr (apiMode == ApiMode::API_MODE_MAX || apiMode == ApiMode::API_MODE_ANY) {
        WholeReduceMax(dstTensor, srcTensor, mask, repeat, 1, 1, repStride, ReduceOrder::ORDER_ONLY_VALUE);
    }
}
} // namespace Internal
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_COMMON_UTIL_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_UTIL_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_UTIL_IMPL_H__
#endif
