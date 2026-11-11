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
 * \file axpy_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/axpy/axpy_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/axpy.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_AXPY_AXPY_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_AXPY_AXPY_COMMON_IMPL_H
#define IMPL_MATH_AXPY_AXPY_COMMON_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_pop_stack_buffer.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/axpy/axpy_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
 template <typename T, typename U>
__aicore__ inline void AxpyIntrinsicsImpl(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor,
    const U& scalarValue, LocalTensor<float> stackBuffer, uint32_t stackSize)
{
    ASCENDC_ASSERT(false, {
        KERNEL_LOG(KERNEL_ERROR, "Failed to check the data types, current api support data types are "
            "T: half, U: half / T: float, U: float / T: half, U: float."); });
}

/*
 * To improve precision, cast from half to float
 * half mode: dstTensor dataType = half, srcTensor dataType = half
 */
 template <>
__aicore__ inline void AxpyIntrinsicsImpl(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
    const half& scalarValue, LocalTensor<float> stackBuffer, uint32_t stackSize)
{
    LocalTensor<float> tmpSrc = stackBuffer[0];
    LocalTensor<float> tmpDst = stackBuffer[stackSize];

    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;

    Cast<float, half, false>(tmpSrc, srcTensor,
        RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, { 1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();

    Cast<float, half, false>(tmpDst, dstTensor,
        RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, { 1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();

    Muls<float, false>(tmpSrc, tmpSrc, (float)scalarValue, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Add<float, false>(tmpDst, tmpSrc, tmpDst, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Cast<half, float, false>(dstTensor, tmpDst, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
}
/*
 * To improve precision, cast from half to float
 * mix mode: dstTensor dataType = float, srcTensor dataType = half
 */
 template <>
__aicore__ inline void AxpyIntrinsicsImpl(const LocalTensor<float>& dstTensor, const LocalTensor<half>& srcTensor,
    const half& scalarValue, LocalTensor<float> stackBuffer, uint32_t stackSize)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;

    Cast<float, half, false>(stackBuffer, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();

    Muls<float, false>(stackBuffer, stackBuffer, (float)scalarValue, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Add<float, false>(dstTensor, stackBuffer, dstTensor,  MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline uint32_t axpyTmpCalc(uint32_t tmpBufferSize)
{
    uint32_t stackSize = tmpBufferSize;
    if constexpr (sizeof(T) == sizeof(half)) {
        stackSize = tmpBufferSize / 2 / ONE_BLK_SIZE * ONE_BLK_SIZE;
    } else {
        stackSize = tmpBufferSize / ONE_BLK_SIZE * ONE_BLK_SIZE;
    }
    CheckTmpBufferSize(stackSize, 0, tmpBufferSize);
    return stackSize;
}

template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void AxpySub(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U& scalarValue,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    uint32_t bufferSize = sharedTmpBuffer.GetSize();
    CheckTmpBufferSize(bufferSize, 0, bufferSize);

    LocalTensor<float> tmpBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    uint32_t tmpBufferSize = tmpBuffer.GetSize();

    uint32_t stackSize = axpyTmpCalc<T>(tmpBufferSize);

    const uint32_t round = calCount / stackSize;
    const uint32_t tail = calCount % stackSize;

    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, stackSize);

    uint32_t offset = 0;
    for (uint32_t i = 0; i < round; i++) {
        AxpyIntrinsicsImpl(dstTensor[offset], srcTensor[offset], scalarValue, tmpBuffer, stackSize);
        offset = offset + stackSize;
    }

    if (tail != 0) {
        SetVectorMask<T, MaskMode::COUNTER>(0, tail);
        AxpyIntrinsicsImpl(dstTensor[offset], srcTensor[offset], scalarValue, tmpBuffer, stackSize);
    }

    SetMaskNorm();
    ResetMask();
}

template <typename T, typename U, bool isReuseSource>
__aicore__ inline void AxpyImpl(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Axpy, (T, U, isReuseSource), (dstTensor, srcTensor, scalarValue, sharedTmpBuffer, calCount));

    if constexpr (sizeof(U) == sizeof(float)) {
        Axpy<T, U>(dstTensor, srcTensor, scalarValue, calCount);
    } else {
        AxpySub<T, U, isReuseSource>(dstTensor, srcTensor, scalarValue, sharedTmpBuffer, calCount);
    }
}

} // namespace AscendC
#endif // IMPL_MATH_AXPY_AXPY_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_AXPY_AXPY_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_AXPY_AXPY_COMMON_IMPL_H__
#endif
