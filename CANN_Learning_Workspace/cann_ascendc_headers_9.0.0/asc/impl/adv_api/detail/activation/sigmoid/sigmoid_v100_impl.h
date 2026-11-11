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
 * \file sigmoid_v100_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/sigmoid/sigmoid_v100_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/sigmoid.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGMOID_V100_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SIGMOID_SIGMOID_V100_IMPL_H
#define IMPL_ACTIVATION_SIGMOID_SIGMOID_V100_IMPL_H
#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {
template <typename T>
__aicore__ inline void SigmoidIntrinsicsImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LocalTensor<T>& stackBuffer, uint32_t repeatTimes)
{
    struct UnaryRepeatParams repeatParams;
    struct BinaryRepeatParams binaryRepeatParams;
    PipeBarrier<PIPE_V>();
    Muls<T, false>(dst, src, static_cast<T>(-1.0), MASK_PLACEHOLDER, repeatTimes, repeatParams);
    PipeBarrier<PIPE_V>();
    Exp<T, false>(dst, dst, MASK_PLACEHOLDER, repeatTimes, repeatParams);
    PipeBarrier<PIPE_V>();
    Adds<T, false>(dst, dst, static_cast<T>(1), MASK_PLACEHOLDER, repeatTimes, repeatParams);
    Duplicate<T, false>(stackBuffer, static_cast<T>(1.0), MASK_PLACEHOLDER, repeatTimes,
        DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Div<T, false>(dst, stackBuffer, dst, MASK_PLACEHOLDER, repeatTimes, binaryRepeatParams);
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void NormCompute(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LocalTensor<T>& stackBuffer, const uint32_t calCount)
{
    IntriInfo intriInfo = AscendCUtils::CalIntriInfo(sizeof(T), calCount);
    uint32_t calcOffset = 0;
    const auto calcOffsetRounding = MAX_REPEAT_TIMES * DEFAULT_BLK_NUM * intriInfo.c0Count;
    const uint32_t fullMask = intriInfo.c0Count * DEFAULT_BLK_NUM;

    SetVectorMask<T, MaskMode::NORMAL>(fullMask);
    for (uint32_t i = 0; i < intriInfo.repeatRounding; i++) {
        SigmoidIntrinsicsImpl(dst[calcOffset], src[calcOffset], stackBuffer, MAX_REPEAT_TIMES);
        calcOffset += calcOffsetRounding;
    }

    const int32_t calcOffsetRemaining = intriInfo.repeatRemaining * DEFAULT_BLK_NUM * intriInfo.c0Count;
    if (intriInfo.repeatRemaining != 0) {
        SigmoidIntrinsicsImpl(dst[calcOffset], src[calcOffset], stackBuffer,
            intriInfo.repeatRemaining);
        calcOffset += calcOffsetRemaining;
    }

    if (intriInfo.tail != 0) {
        SetVectorMask<T, MaskMode::NORMAL>(intriInfo.tail);
        SigmoidIntrinsicsImpl(dst[calcOffset], src[calcOffset], stackBuffer, 1);
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void SigmoidCompute(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& sharedTmpBuffer, const uint32_t splitSize, const uint32_t loopCount, const uint32_t calcTail)
{
    for (uint32_t i = 0; i < loopCount; ++i) {
        NormCompute(dstTensor[i * splitSize], srcTensor[i * splitSize], sharedTmpBuffer, splitSize);
    }
    if (calcTail > 0) {
        NormCompute(dstTensor[loopCount * splitSize], srcTensor[loopCount * splitSize], sharedTmpBuffer, calcTail);
    }
    ResetMask();
}
} //  namespace AscendC
#endif // IMPL_ACTIVATION_SIGMOID_SIGMOID_V100_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGMOID_V100_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGMOID_V100_IMPL_H__
#endif
