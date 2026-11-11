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
 * \file softmax_common_reduce.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/softmax_common/softmax_common_reduce.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_REDUCE_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_REDUCE_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_REDUCE_H
#include "softmax_common_utils.h"

namespace AscendC {

__aicore__ inline void ReduceMaxBlockNZImpl(const LocalTensor<float>& dst, const LocalTensor<float>& src,
    const ReduceLastND& reduceParam)
{
    SetMaskCount();
    SetVectorMask<float, MaskMode::COUNTER>(0, reduceParam.srcM * FLOAT_NUM_PER_BLK);

    Max<float, false>(dst, src, src[FLOAT_NUM_PER_BLK], 1, 1,
        { B16_BYTE_SIZE, B16_BYTE_SIZE, B16_BYTE_SIZE, DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE,
        DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE, DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE });
    PipeBarrier<PIPE_V>();
    BlockReduceMax<float, false>(dst, dst, reduceParam.srcM / FLOAT_NUM_PER_BLK, MASK_PLACEHOLDER,
        DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE, B16_BYTE_SIZE, DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE);

    SetMaskNorm();
    ResetMask();
}

__aicore__ inline void ReduceSumBlockNZImpl(const LocalTensor<float>& dst, const LocalTensor<float>& src,
    const ReduceLastND& reduceParam)
{
    SetMaskCount();
    SetVectorMask<float, MaskMode::COUNTER>(0, reduceParam.srcM * FLOAT_NUM_PER_BLK);

    Add<float, false>(dst, src, src[FLOAT_NUM_PER_BLK], 1, 1,
        { B16_BYTE_SIZE, B16_BYTE_SIZE, B16_BYTE_SIZE, DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE,
        DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE, DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE });
    PipeBarrier<PIPE_V>();
    BlockReduceSum<float, false>(dst, dst, reduceParam.srcM / FLOAT_NUM_PER_BLK, MASK_PLACEHOLDER,
        DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE, B16_BYTE_SIZE, DEFAULT_REPEAT_STRIDE * B16_BYTE_SIZE);

    SetMaskNorm();
    ResetMask();
}

__aicore__ inline void BigBlockReduceMax(const LocalTensor<float>& dst, const LocalTensor<float>& src,
    const uint32_t splitBlock, const uint32_t splitM, const uint32_t splitK)
{
    for (uint32_t i = 0; i < splitM; i++) {
        BlockReduceMax<float, false>(dst[FLOAT_REPEAT_SIZE * i], src[i * splitK], FLOAT_NUM_PER_BLK, MASK_PLACEHOLDER,
            1, 1, DEFAULT_REPEAT_STRIDE);
    }
    uint8_t remainRepeat = splitBlock - FLOAT_NUM_PER_BLK;
    if (remainRepeat == 0) {
        return;
    }
    PipeBarrier<PIPE_V>();
    for (uint32_t j = 0; j < splitM; ++j) {
        Max<float, false>(dst[j * FLOAT_REPEAT_SIZE], src[SOFTMAX_FLOAT_SPECIAL_BLOCKREDUCE_LEN + j * splitK],
            dst[j * FLOAT_REPEAT_SIZE], 1, remainRepeat, { 1, 1, 1, 0, DEFAULT_REPEAT_STRIDE, 0 });
    }
}

__aicore__ inline void BigBlockReduceSum(const LocalTensor<float>& dst, const LocalTensor<float>& src,
    const uint32_t splitBlock, const uint32_t splitM, const uint32_t splitK)
{
    for (uint32_t i = 0; i < splitM; i++) {
        BlockReduceSum<float, false>(dst[FLOAT_REPEAT_SIZE * i], src[i * splitK], FLOAT_NUM_PER_BLK, MASK_PLACEHOLDER,
            1, 1, DEFAULT_REPEAT_STRIDE);
    }
    uint8_t remainRepeat = splitBlock - FLOAT_NUM_PER_BLK;
    if (remainRepeat == 0) {
        return;
    }
    PipeBarrier<PIPE_V>();
    for (uint32_t j = 0; j < splitM; ++j) {
        Add<float, false>(dst[j * FLOAT_REPEAT_SIZE], src[SOFTMAX_FLOAT_SPECIAL_BLOCKREDUCE_LEN + j * splitK],
            dst[j * FLOAT_REPEAT_SIZE], 1, remainRepeat, { 1, 1, 1, 0, DEFAULT_REPEAT_STRIDE, 0 });
    }
}

}; // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_REDUCE_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_REDUCE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_REDUCE_H__
#endif
