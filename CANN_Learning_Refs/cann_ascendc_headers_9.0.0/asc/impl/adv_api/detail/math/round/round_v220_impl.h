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
 * \file round_v220_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/round/round_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/round.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ROUND_ROUND_V220_IMPL_H__
#endif
#ifndef IMPL_MATH_ROUND_ROUND_V220_IMPL_H
#define IMPL_MATH_ROUND_ROUND_V220_IMPL_H
#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../common/check.h"

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
namespace AscendC {
constexpr uint32_t STRIDE_OF_DIFFERENT_DIGITS = 2;

template <typename T, bool isReuseSource = false>
__aicore__ inline void RoundComputeCount(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{}

template <>
__aicore__ inline void RoundComputeCount<float, false>(const LocalTensor<float>& dstTensor,
    const LocalTensor<float>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    SetVectorMask<float, MaskMode::COUNTER>(0, calCount);
    Cast<float, float, false>(dstTensor, srcTensor, RoundMode::CAST_RINT, MASK_PLACEHOLDER, (uint8_t)1,
        { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
}

template <>
__aicore__ inline void RoundComputeCount<half, false>(const LocalTensor<half>& dstTensor,
    const LocalTensor<half>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    // Calculate the amount of data that can be stored in the temporary space and split the data into the entire block
    // and tail block.
    uint32_t sharedTmpBufferSize = sharedTmpBuffer.GetSize();
    uint32_t splitCount = sharedTmpBufferSize / sizeof(float) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    CheckTmpBufferSize(splitCount, 0, sharedTmpBufferSize);

    uint32_t loopCount = calCount / splitCount;
    uint32_t calcTail = calCount % splitCount;

    const LocalTensor<float> &tmpTensor = sharedTmpBuffer.ReinterpretCast<float>();

    SetVectorMask<half, MaskMode::COUNTER>(0, splitCount);
    // In the case of the half data type, there is no direct instruction for the round operation. Therefore, multiple
    // conversions are required.
    for (uint32_t i = 0; i < loopCount; ++i) {
        Cast<float, half, false>(tmpTensor, srcTensor[i * splitCount], RoundMode::CAST_NONE, MASK_PLACEHOLDER,
            (uint8_t)1, { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE / STRIDE_OF_DIFFERENT_DIGITS });
        PipeBarrier<PIPE_V>();

        Cast<float, float, false>(tmpTensor, tmpTensor, RoundMode::CAST_RINT, MASK_PLACEHOLDER, (uint8_t)1,
            { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
        PipeBarrier<PIPE_V>();
        Cast<half, float, false>(dstTensor[i * splitCount], tmpTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER,
            (uint8_t)1, { 1, 1, DEFAULT_REPEAT_STRIDE / STRIDE_OF_DIFFERENT_DIGITS, DEFAULT_REPEAT_STRIDE });
        PipeBarrier<PIPE_V>();
    }
    if (calcTail > 0) {
        SetVectorMask<half, MaskMode::COUNTER>(0, calcTail);
        Cast<float, half, false>(tmpTensor, srcTensor[loopCount * splitCount], RoundMode::CAST_NONE, MASK_PLACEHOLDER,
            (uint8_t)1, { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE / STRIDE_OF_DIFFERENT_DIGITS });
        PipeBarrier<PIPE_V>();
        Cast<float, float, false>(tmpTensor, tmpTensor, RoundMode::CAST_RINT, MASK_PLACEHOLDER, (uint8_t)1,
            { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
        PipeBarrier<PIPE_V>();
        Cast<half, float, false>(dstTensor[loopCount * splitCount], tmpTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER,
            (uint8_t)1, { 1, 1, DEFAULT_REPEAT_STRIDE / STRIDE_OF_DIFFERENT_DIGITS, DEFAULT_REPEAT_STRIDE });
        PipeBarrier<PIPE_V>();
    }
}
} // namespace AscendC
#endif

#endif // IMPL_MATH_ROUND_ROUND_V220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ROUND_ROUND_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ROUND_ROUND_V220_IMPL_H__
#endif
