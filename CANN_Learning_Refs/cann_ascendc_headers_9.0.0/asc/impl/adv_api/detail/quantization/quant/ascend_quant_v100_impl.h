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
 * \file ascend_quant_v100_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/quantization/quant/ascend_quant_v100_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_quant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_V100_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_QUANT_ASCEND_QUANT_V100_IMPL_H
#define IMPL_QUANTIZATION_QUANT_ASCEND_QUANT_V100_IMPL_H
#include "ascend_quant_pre_impl.h"
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
// per tensor intrinsics
__aicore__ inline void AscendQuantIntrinsicsImpl(const LocalTensor<int8_t>& dstTensor,
    const LocalTensor<half>& srcTensor, const LocalTensor<half>& stackBuffer,
    half scale, half offset, uint8_t repeatTimes)
{
    UnaryRepeatParams unaryParams;
    UnaryRepeatParams f162s8Param;
    f162s8Param.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    PipeBarrier<PIPE_V>();
    Muls<half, false>(stackBuffer, srcTensor, scale, MASK_PLACEHOLDER, repeatTimes, unaryParams);
    PipeBarrier<PIPE_V>();
    Adds<half, false>(stackBuffer, stackBuffer, offset, MASK_PLACEHOLDER, repeatTimes, unaryParams);
    PipeBarrier<PIPE_V>();
    Cast<int8_t, half, false>(dstTensor, stackBuffer, RoundMode::CAST_NONE, MASK_PLACEHOLDER, repeatTimes, f162s8Param);
    PipeBarrier<PIPE_V>();
}
__aicore__ inline void AscendQuantIntrinsicsImpl(const LocalTensor<int8_t>& dst, const LocalTensor<float>& src,
    const LocalTensor<half>& stackBuffer, half scale, half offset, uint8_t repeatTimes)
{
    UnaryRepeatParams f322f16Param;
    f322f16Param.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams unaryf16Params;
    unaryf16Params.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    unaryf16Params.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams f162s8Param;
    f162s8Param.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    f162s8Param.dstRepStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
    PipeBarrier<PIPE_V>();
    Cast<half, float, false>(stackBuffer, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, repeatTimes, f322f16Param);
    PipeBarrier<PIPE_V>();
    Muls<half, false>(stackBuffer, stackBuffer, scale, MASK_PLACEHOLDER, repeatTimes, unaryf16Params);
    PipeBarrier<PIPE_V>();
    Adds<half, false>(stackBuffer, stackBuffer, offset, MASK_PLACEHOLDER, repeatTimes, unaryf16Params);
    PipeBarrier<PIPE_V>();
    Cast<int8_t, half, false>(dst, stackBuffer, RoundMode::CAST_NONE, MASK_PLACEHOLDER, repeatTimes, f162s8Param);
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void NormCompute(const LocalTensor<int8_t>& dst, const LocalTensor<T>& src,
    const LocalTensor<half>& stackBuffer, const half scale, const half offset, const uint32_t calCount)
{
    IntriInfo intriInfo = AscendCUtils::CalIntriInfo(sizeof(T), calCount);
    uint32_t calcOffset = 0;
    const auto calcOffsetRounding = MAX_REPEAT_TIMES * DEFAULT_BLK_NUM * intriInfo.c0Count;
    const uint32_t fullMask = intriInfo.c0Count * DEFAULT_BLK_NUM;

    SetVectorMask<T, MaskMode::NORMAL>(fullMask);
    for (uint32_t i = 0; i < intriInfo.repeatRounding; i++) {
        AscendQuantIntrinsicsImpl(dst[calcOffset], src[calcOffset], stackBuffer, scale, offset,
            MAX_REPEAT_TIMES);
        calcOffset += calcOffsetRounding;
    }

    const int32_t calcOffsetRemaining = intriInfo.repeatRemaining * DEFAULT_BLK_NUM * intriInfo.c0Count;
    if (intriInfo.repeatRemaining != 0) {
        AscendQuantIntrinsicsImpl(dst[calcOffset], src[calcOffset], stackBuffer, scale, offset,
            intriInfo.repeatRemaining);
        calcOffset += calcOffsetRemaining;
    }

    if (intriInfo.tail != 0) {
        SetVectorMask<T, MaskMode::NORMAL>(intriInfo.tail);
        AscendQuantIntrinsicsImpl(dst[calcOffset], src[calcOffset], stackBuffer, scale, offset, 1);
    }
}

// api impl
template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
__aicore__ inline void AscendQuantImpl(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const float scale, const float offset, const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(AscendQuant, (T, isReuseSource, config),
        (dstTensor, srcTensor, sharedTmpBuffer, scale, offset, calCount));

    uint32_t splitSize = sharedTmpBuffer.GetSize() / sizeof(half) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    ASCENDC_ASSERT((splitSize != 0), { KERNEL_LOG(KERNEL_ERROR,
        "Insufficient temporary space, current operation is not enough, please check the host tiling.");
    });
    uint32_t loopCount = calCount / splitSize;
    for (uint32_t i = 0; i < loopCount; ++i) {
        NormCompute(dstTensor[splitSize * i], srcTensor[splitSize * i],
            sharedTmpBuffer.ReinterpretCast<half>(), static_cast<half>(scale), static_cast<half>(offset),
            splitSize);
    }
    if (calCount % splitSize > 0) {
        NormCompute(dstTensor[splitSize * loopCount], srcTensor[splitSize * loopCount],
            sharedTmpBuffer.ReinterpretCast<half>(), static_cast<half>(scale), static_cast<half>(offset),
            calCount % splitSize);
    }

    ResetMask();
}
template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
__aicore__ inline void AscendQuantImpl(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor,
    const T offset, const uint32_t scaleCount, const uint32_t calCount)
{
    ASCENDC_ASSERT(false,
                    { KERNEL_LOG(KERNEL_ERROR, "This device does not support per channel quant!"); });
}
template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
__aicore__ inline void AscendQuantImpl(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor,
    const LocalTensor<T>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount,
    const uint32_t calCount)
{
    ASCENDC_ASSERT(false,
                    { KERNEL_LOG(KERNEL_ERROR, "This device does not support per channel quant!"); });
}
} //  namespace AscendC
#endif // IMPL_QUANTIZATION_QUANT_ASCEND_QUANT_V100_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_V100_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_V100_IMPL_H__
#endif
