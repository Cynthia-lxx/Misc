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
 * \file fmod_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/fmod/fmod_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/fmod.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FMOD_FMOD_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_FMOD_FMOD_COMMON_IMPL_H
#define IMPL_MATH_FMOD_FMOD_COMMON_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "include/adv_api/math/trunc.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/fmod/fmod_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace {
constexpr uint32_t SRC0_IDX = 1;
constexpr uint32_t SRC1_IDX = 2;
constexpr uint32_t TRUNC_IDX = 3;
}

__aicore__ inline void FmodCompute(const LocalTensor<float>& dstTensor, const LocalTensor<float>& src0Tensor,
    const LocalTensor<float>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t stackSize,
    const uint32_t calCount)
{
    PipeBarrier<PIPE_V>();

    Div(dstTensor, src0Tensor, src1Tensor, calCount);
    PipeBarrier<PIPE_V>();

    Trunc(dstTensor, dstTensor, sharedTmpBuffer, calCount);
    PipeBarrier<PIPE_V>();

    Mul(dstTensor, dstTensor, src1Tensor, calCount);
    PipeBarrier<PIPE_V>();

    Sub(dstTensor, src0Tensor, dstTensor, calCount);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void FmodCompute(const LocalTensor<half>& dstTensor, const LocalTensor<half>& src0Tensor,
    const LocalTensor<half>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t stackSize,
    const uint32_t calCount)
{
    // floatTmpTensor<float>    = | dst | src0 | src1 |
    // sharedTmpBuffer<uint8_t> = | dst | src0 | src1 | trunc |
    LocalTensor<float> floatTmpTensor = sharedTmpBuffer.ReinterpretCast<float>();
    LocalTensor<float> tmpSrc0 = floatTmpTensor[SRC0_IDX * stackSize]; // Allocate stackSize space
    LocalTensor<float> tmpSrc1 = floatTmpTensor[SRC1_IDX * stackSize];

    PipeBarrier<PIPE_V>();

    Cast<float, half>(tmpSrc0, src0Tensor, RoundMode::CAST_NONE, calCount);

    Cast<float, half>(tmpSrc1, src1Tensor, RoundMode::CAST_NONE, calCount);
    PipeBarrier<PIPE_V>();

    FmodCompute(floatTmpTensor, tmpSrc0, tmpSrc1, sharedTmpBuffer[TRUNC_IDX * stackSize * sizeof(float)], stackSize, calCount);

    Cast<half, float>(dstTensor, floatTmpTensor, RoundMode::CAST_NONE, calCount);
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void FmodImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor,
    const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(Fmod, (T, isReuseSource), (dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, calCount));

    if constexpr (sizeof(T) == sizeof(float)) {
        FmodCompute(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, src0Tensor.GetSize(), calCount);
        return;
    }

    constexpr uint32_t maxLiveNodeCount = 8; // The corresponding maxLiveNodeCount for half is 8, extra is 3 * 2 + trunc 2.
    uint32_t bufferSize = sharedTmpBuffer.GetSize();
    uint32_t stackSize =
        bufferSize / sizeof(T) / maxLiveNodeCount / ONE_BLK_SIZE * ONE_BLK_SIZE; // divided by how many counts
    CheckTmpBufferSize(stackSize, 0, bufferSize);
    stackSize = stackSize > src0Tensor.GetSize() ? src0Tensor.GetSize() : stackSize; // No more than localTensor

    const uint32_t round = calCount / stackSize;
    const uint32_t tail = calCount % stackSize;

    for (uint32_t i = 0; i < round; ++i) {
        FmodCompute(dstTensor[i * stackSize], src0Tensor[i * stackSize], src1Tensor[i * stackSize], sharedTmpBuffer,
            stackSize, stackSize);
    }
    if (tail > 0) {
        FmodCompute(dstTensor[round * stackSize], src0Tensor[round * stackSize], src1Tensor[round * stackSize],
            sharedTmpBuffer, stackSize, tail);
    }
}
} // namespace AscendC
#endif // IMPL_MATH_FMOD_FMOD_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FMOD_FMOD_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FMOD_FMOD_COMMON_IMPL_H__
#endif
