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
 * \file floor_v200_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/floor/floor_v200_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/floor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FLOOR_FLOOR_V200_IMPL_H__
#endif
#ifndef IMPL_MATH_FLOOR_FLOOR_V200_IMPL_H
#define IMPL_MATH_FLOOR_FLOOR_V200_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/floor/floor_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
__aicore__ inline void FloorCommonProcess(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
    const LocalTensor<int32_t>& intTmpTensor)
{
    Cast<int32_t, float, false>(intTmpTensor, srcTensor, RoundMode::CAST_FLOOR, MASK_PLACEHOLDER, 1,
        { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
    Cast<float, int32_t, false>(dstTensor, intTmpTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void FloorProcess(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
    const LocalTensor<uint8_t>& tmpTensor)
{
    const LocalTensor<float> floatTmpTensor = tmpTensor.ReinterpretCast<float>();

    // In the case of the half data type, there is no direct instruction for the round operation. Therefore, multiple
    // conversions are required.
    Cast<float, half, false>(floatTmpTensor, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();

    FloorCommonProcess(floatTmpTensor, floatTmpTensor, tmpTensor.ReinterpretCast<int32_t>());

    Cast<half, float, false>(dstTensor, floatTmpTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        { 1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void FloorImpl(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Floor, (half, isReuseSource), (dstTensor, srcTensor, sharedTmpBuffer, calCount));

    // Calculate the amount of data that can be stored in the temporary space and split the data into the entire block
    // and tail block.
    uint32_t tmpBufferSize = sharedTmpBuffer.GetSize();
    uint32_t splitCount = tmpBufferSize / sizeof(float) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    CheckTmpBufferSize(splitCount, 0, tmpBufferSize);
    uint32_t loopCount = calCount / splitCount;
    uint32_t calcTail = calCount % splitCount;

    SetMaskCount();
    SetVectorMask<half, MaskMode::COUNTER>(0, splitCount);
    for (uint32_t i = 0; i < loopCount; ++i) {
        FloorProcess(dstTensor[i * splitCount], srcTensor[i * splitCount], sharedTmpBuffer);
    }
    if (calcTail > 0) {
        SetVectorMask<half>(0, calcTail);
        FloorProcess(dstTensor[loopCount * splitCount], srcTensor[loopCount * splitCount], sharedTmpBuffer);
    }
    SetMaskNorm();
    ResetMask();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void FloorImpl(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
    const uint32_t calCount)
{
    // alloc tmp buffer using stack
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ret = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ret), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    FloorImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void FloorImpl(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
    const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Floor, (float, isReuseSource), (dstTensor, srcTensor, calCount));

    SetMaskCount();
    SetVectorMask<float, MaskMode::COUNTER>(0, calCount);
    FloorCommonProcess(dstTensor, srcTensor, dstTensor.ReinterpretCast<int32_t>());
    SetMaskNorm();
    ResetMask();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void FloorImpl(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    (void)sharedTmpBuffer;
    FloorImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
}
}  // namespace AscendC
#endif  // IMPL_MATH_FLOOR_FLOOR_V200_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FLOOR_FLOOR_V200_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FLOOR_FLOOR_V200_IMPL_H__
#endif
