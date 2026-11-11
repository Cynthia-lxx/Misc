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
 * \file ceil_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/ceil/ceil_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/ceil.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CEIL_CEIL_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_CEIL_CEIL_COMMON_IMPL_H
#define IMPL_MATH_CEIL_CEIL_COMMON_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/ceil/ceil_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "ceil_v220_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "ceil_v200_impl.h"
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
namespace AscendC {

constexpr uint32_t CEIL_HALF_CALC_PROCEDURE = 2;

__aicore__ inline void CeilProcess(
    const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor, const LocalTensor<uint8_t>& tmpTensor)
{
    const LocalTensor<float> floatTmpTensor = tmpTensor.ReinterpretCast<float>();
    // In the case of the half data type, there is no direct instruction for the round operation. Therefore, multiple
    // conversions are required.
    Cast<float, half, false>(floatTmpTensor, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
    CeilProcess(floatTmpTensor, floatTmpTensor, tmpTensor);
    Cast<half, float, false>(dstTensor, floatTmpTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void CeilImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Ceil, (T, isReuseSource), (dstTensor, srcTensor, sharedTmpBuffer, calCount));

    // Calculate the amount of data that can be stored in the temporary space and split the data into the entire block
    // and tail block.
    uint32_t tmpBufferSize = sharedTmpBuffer.GetSize();
    uint32_t splitCount = tmpBufferSize / sizeof(T);
    if constexpr (sizeof(T) == sizeof(half)) {
        splitCount = splitCount / CEIL_HALF_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE;
    } else {
        splitCount = splitCount / ONE_BLK_SIZE * ONE_BLK_SIZE;
    }
    CheckTmpBufferSize(splitCount, 0, tmpBufferSize);

    uint32_t loopCount = calCount / splitCount;
    uint32_t calcTail = calCount % splitCount;

    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, splitCount);
    for (uint32_t i = 0; i < loopCount; ++i) {
        CeilProcess(dstTensor[i * splitCount], srcTensor[i * splitCount], sharedTmpBuffer);
    }
    if (calcTail > 0) {
        SetVectorMask<T>(0, calcTail);
        CeilProcess(dstTensor[loopCount * splitCount], srcTensor[loopCount * splitCount], sharedTmpBuffer);
    }

    SetMaskNorm();
    ResetMask();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void CeilImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // alloc tmp buffer using stack
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ret = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ret), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    CeilImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
}
} // namespace AscendC
#endif

#endif // IMPL_MATH_CEIL_CEIL_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CEIL_CEIL_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CEIL_CEIL_COMMON_IMPL_H__
#endif
