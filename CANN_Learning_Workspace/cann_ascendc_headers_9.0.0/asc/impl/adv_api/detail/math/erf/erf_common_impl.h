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
#pragma message("impl/adv_api/detail/math/erf/erf_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/erf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ERF_ERF_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_ERF_ERF_COMMON_IMPL_H
#define IMPL_MATH_ERF_ERF_COMMON_IMPL_H
#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/erf/erf_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
__aicore__ inline constexpr RoundMode GetErfCastType()
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
    return RoundMode::CAST_ROUND;
#else
    return RoundMode::CAST_NONE;
#endif
}

// Clip x to [-3.92, 3.92]
__aicore__ inline void ErfClip(const LocalTensor<float>& dst, const LocalTensor<float>& src,
    const LocalTensor<float>& tmpBuffer)
{
    constexpr float ERF_BOUNDARY_MAX = 3.92;
    UnaryRepeatParams unaryParams;

    Mins<float, false>(tmpBuffer, src, static_cast<float>(ERF_BOUNDARY_MAX), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Maxs<float, false>(dst, tmpBuffer, static_cast<float>(-ERF_BOUNDARY_MAX), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
}

// P(x) = (((((0.053443748819x^2+0.75517016694e1)x^2+0.10162808918e3)x^2
//          +0.13938061484e4)x^2+0.50637915060e4)x^2+0.29639384698e5)x
__aicore__ inline void ErfComputeP(const LocalTensor<float>& tmpBuffer, const LocalTensor<float>& src,
    const uint32_t calSize)
{
    constexpr float SCALAR_P0 = 0.29639384698e5;
    constexpr float SCALAR_P1 = 0.50637915060e4;
    constexpr float SCALAR_P2 = 0.13938061484e4;
    constexpr float SCALAR_P3 = 0.10162808918e3;
    constexpr float SCALAR_P4 = 0.75517016694e1;
    constexpr float SCALAR_P5 = 0.053443748819;

    UnaryRepeatParams unaryParams;
    BinaryRepeatParams binaryParams;

    LocalTensor<float> tmpBuffer1 = tmpBuffer;
    LocalTensor<float> tmpBuffer2 = tmpBuffer1[calSize];
    LocalTensor<float> tmpBuffer3 = tmpBuffer2[calSize];

    Mul<float, false>(tmpBuffer1, src, src, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Muls<float, false>(tmpBuffer2, tmpBuffer1, static_cast<float>(SCALAR_P5), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmpBuffer3, tmpBuffer2, static_cast<float>(SCALAR_P4), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(tmpBuffer2, tmpBuffer1, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmpBuffer3, tmpBuffer2, static_cast<float>(SCALAR_P3), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(tmpBuffer2, tmpBuffer1, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmpBuffer3, tmpBuffer2, static_cast<float>(SCALAR_P2), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(tmpBuffer2, tmpBuffer1, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmpBuffer3, tmpBuffer2, static_cast<float>(SCALAR_P1), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(tmpBuffer2, tmpBuffer1, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmpBuffer3, tmpBuffer2, static_cast<float>(SCALAR_P0), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(tmpBuffer2, src, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
}

// Q(x) = ((((x^2+0.31212858877e2)x^2+0.39856963806e3)x^2+0.30231248150e4)x^2+0.13243365831e5)x^2+0.26267224157e5
__aicore__ inline void ErfComputeQ(const LocalTensor<float>& tmpBuffer, const LocalTensor<float>& src,
    const uint32_t calSize)
{
    constexpr float SCALAR_Q0 = 0.26267224157e5;
    constexpr float SCALAR_Q1 = 0.13243365831e5;
    constexpr float SCALAR_Q2 = 0.30231248150e4;
    constexpr float SCALAR_Q3 = 0.39856963806e3;
    constexpr float SCALAR_Q4 = 0.31212858877e2;

    UnaryRepeatParams unaryParams;
    BinaryRepeatParams binaryParams;

    LocalTensor<float> tmpBuffer1 = tmpBuffer;
    LocalTensor<float> tmpBuffer3 = tmpBuffer1[calSize * 2];

    Adds<float, false>(tmpBuffer3, tmpBuffer1, static_cast<float>(SCALAR_Q4), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(src, tmpBuffer1, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmpBuffer3, src, static_cast<float>(SCALAR_Q3), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(src, tmpBuffer1, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmpBuffer3, src, static_cast<float>(SCALAR_Q2), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(src, tmpBuffer1, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmpBuffer3, src, static_cast<float>(SCALAR_Q1), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(src, tmpBuffer1, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmpBuffer3, src, static_cast<float>(SCALAR_Q0), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void ErfCompute(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& tmpBuffer,
    const uint32_t calSize)
{
    BinaryRepeatParams binaryParams;
    UnaryRepeatParams unaryParams;

    LocalTensor<T> tmpBuffer1 = tmpBuffer;
    LocalTensor<T> tmpBuffer2 = tmpBuffer1[calSize];
    LocalTensor<T> tmpBuffer3 = tmpBuffer2[calSize];

    // x = Clip(x), Erf(x) = P(x) / Q(x)
    ErfClip(dst, src, tmpBuffer1);

    ErfComputeP(tmpBuffer1, dst, calSize);
    ErfComputeQ(tmpBuffer1, dst, calSize);

    Div<T, false>(dst, tmpBuffer2, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
}

template <>
__aicore__ inline void ErfCompute<half>(const LocalTensor<half>& dst, const LocalTensor<half>& src,
    const LocalTensor<half>& tmpBuffer, const uint32_t calSize)
{
    BinaryRepeatParams binaryParams;
    UnaryRepeatParams unaryParams;

    LocalTensor<float> tmpBuffer1 = tmpBuffer.ReinterpretCast<float>();
    LocalTensor<float> tmpBuffer2 = tmpBuffer1[calSize];
    LocalTensor<float> tmpBuffer3 = tmpBuffer2[calSize];
    LocalTensor<float> tmpBuffer4 = tmpBuffer3[calSize];

    // Cast src from half to float type for getting more precise results.
    Cast<float, half, false>(tmpBuffer4, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE / 2 });
    PipeBarrier<PIPE_V>();

    // x = Clip(x), (x) = P(x) / Q(x)
    ErfClip(tmpBuffer4, tmpBuffer4, tmpBuffer1);

    ErfComputeP(tmpBuffer1, tmpBuffer4, calSize);
    ErfComputeQ(tmpBuffer1, tmpBuffer4, calSize);

    Div<float, false>(tmpBuffer1, tmpBuffer2, tmpBuffer3, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    constexpr RoundMode castType = GetErfCastType();

    Cast<half, float, false>(dst, tmpBuffer1, castType, MASK_PLACEHOLDER, 1,
        { 1, 1, DEFAULT_REPEAT_STRIDE / 2, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void ErfImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(Erf, (T, isReuseSource), (dstTensor, srcTensor, sharedTmpBuffer, calCount));

    constexpr uint8_t ERF_HALF_CALC_PROCEDURE = 8;
    constexpr uint8_t ERF_FLOAT_CALC_PROCEDURE = 3;

    LocalTensor<T> tmpBuffer = sharedTmpBuffer.ReinterpretCast<T>();
    uint32_t bufferSize = sharedTmpBuffer.GetSize();
    uint32_t tmpBufferSize = bufferSize / sizeof(T); // all temporary variables are float type.
    CheckTmpBufferSize(tmpBufferSize, 0, bufferSize);

    uint32_t calSize = 0;
    if constexpr (sizeof(T) == sizeof(half)) {
        calSize = tmpBufferSize / ERF_HALF_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE;
    } else {
        calSize = tmpBufferSize / ERF_FLOAT_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE;
    }
    CheckTmpBufferSize(calSize, 0, bufferSize);

    const uint32_t round = calCount / calSize;
    const uint32_t tail = calCount % calSize;

    SetMaskCount();
    SetVectorMask<half, MaskMode::COUNTER>(0, calSize);

    uint32_t offset = 0;
    for (uint32_t i = 0; i < round; i++) {
        ErfCompute(dstTensor[offset], srcTensor[offset], tmpBuffer, calSize);
        offset = offset + calSize;
    }

    if (tail != 0) {
        SetVectorMask<half, MaskMode::COUNTER>(0, tail);
        ErfCompute(dstTensor[offset], srcTensor[offset], tmpBuffer, calSize);
    }

    SetMaskNorm();
    ResetMask();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void ErfImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }

    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ErfImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
}
} // namespace AscendC
#endif // IMPL_MATH_ERF_ERF_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ERF_ERF_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ERF_ERF_COMMON_IMPL_H__
#endif
