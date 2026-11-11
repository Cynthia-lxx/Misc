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
 * \file sign_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/sign/sign_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/sign.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SIGN_SIGN_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_SIGN_SIGN_COMMON_IMPL_H
#define IMPL_MATH_SIGN_SIGN_COMMON_IMPL_H
#include <type_traits>
#include "kernel_basic_intf.h"
#include "kernel_log.h"
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/sign/sign_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

#pragma begin_pipe(V)
namespace AscendC {
constexpr uint32_t SIGN_CALC_PROC = 3;
constexpr uint32_t SIGN_BIT = 8;

template <typename T>
__aicore__ inline void SignComputeImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& tmpBuffer1, const LocalTensor<uint8_t>& tmpBuffer2, const LocalTensor<T>& tmpBuffer3,
    const LocalTensor<T>& tmpBuffer4, uint32_t calCount, uint32_t repeatTimes)
{
    BinaryRepeatParams binaryParams;

    Duplicate<T, false>(dstTensor, static_cast<T>(0), MASK_PLACEHOLDER, 1, 1, 8);
    PipeBarrier<PIPE_V>();

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
    SetMaskNorm();
    ResetMask();
#endif
    Compare<T, uint8_t, false>(
        tmpBuffer1, srcTensor, dstTensor, CMPMODE::LT, MASK_PLACEHOLDER, repeatTimes, binaryParams);
    Compare<T, uint8_t, false>(
        tmpBuffer2, srcTensor, dstTensor, CMPMODE::GT, MASK_PLACEHOLDER, repeatTimes, binaryParams);
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, calCount);
#endif

    Duplicate<T, false>(tmpBuffer3, static_cast<T>(1), MASK_PLACEHOLDER, 1, 1, 8);
    PipeBarrier<PIPE_V>();

    SetCmpMask<T>(tmpBuffer3);
    Select<T, uint8_t>(tmpBuffer4, tmpBuffer1, dstTensor, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Duplicate<T, false>(tmpBuffer3, static_cast<T>(-1), MASK_PLACEHOLDER, 1, 1, 8);
    PipeBarrier<PIPE_V>();

    SetCmpMask<T>(tmpBuffer3);
    Select<T, uint8_t>(dstTensor, tmpBuffer2, dstTensor, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Add<T, false>(dstTensor, tmpBuffer4, dstTensor, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void SignCompute(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    // Only for AI VectorCore.
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(Sign, (T, isReuseSource), (dstTensor, srcTensor, sharedTmpBuffer, calCount));
    uint32_t sharedTmpBufferSize = sharedTmpBuffer.GetSize();
    uint32_t splitCount = sharedTmpBufferSize / sizeof(T) / SIGN_CALC_PROC / ONE_BLK_SIZE * ONE_BLK_SIZE;

    CheckTmpBufferSize(splitCount, 0, sharedTmpBufferSize);

    uint32_t loopCount = calCount / splitCount;
    uint32_t calcTail = calCount % splitCount;

    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, splitCount);
    __ubuf__ uint8_t *tmpBuffer1 = (__ubuf__ uint8_t *)sharedTmpBuffer.GetPhyAddr();
    uint32_t tmpLen = AlignUp(splitCount / SIGN_BIT, ONE_BLK_SIZE);
    __ubuf__ uint8_t *tmpBuffer2 = tmpBuffer1 + tmpLen;
    LocalTensor<T> stackTensor = sharedTmpBuffer[tmpLen * 2].ReinterpretCast<T>();
    __ubuf__ T *tmpBuffer3 = (__ubuf__ T *)stackTensor.GetPhyAddr();
    __ubuf__ T *tmpBuffer4 = tmpBuffer3 + splitCount;

    uint32_t offset = 0;
    uint32_t repeatTimes = (splitCount * sizeof(T) + ONE_REPEAT_BYTE_SIZE - 1) / ONE_REPEAT_BYTE_SIZE;
    for (uint32_t i = 0; i < loopCount; ++i) {
        SignComputeImpl(dstTensor[offset], srcTensor[offset],
            sharedTmpBuffer, sharedTmpBuffer[tmpLen], stackTensor, stackTensor[splitCount], splitCount, repeatTimes);
        offset = offset + splitCount;
    }
    if (calcTail > 0) {
        SetVectorMask<T, MaskMode::COUNTER>(0, calcTail);
        repeatTimes = (calcTail * sizeof(T) + ONE_REPEAT_BYTE_SIZE - 1) / ONE_REPEAT_BYTE_SIZE;
        SignComputeImpl(dstTensor[offset], srcTensor[offset],
            sharedTmpBuffer, sharedTmpBuffer[tmpLen], stackTensor, stackTensor[splitCount], calcTail, repeatTimes);
    }
    SetMaskNorm();
    ResetMask();
}
}  // namespace AscendC
#pragma end_pipe
#endif // IMPL_MATH_SIGN_SIGN_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SIGN_SIGN_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SIGN_SIGN_COMMON_IMPL_H__
#endif
