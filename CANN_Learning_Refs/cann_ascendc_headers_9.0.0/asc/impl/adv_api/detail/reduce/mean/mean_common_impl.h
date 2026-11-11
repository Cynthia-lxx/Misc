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
 * \file mean_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/reduce/mean/mean_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/mean.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_MEAN_MEAN_COMMON_IMPL_H__
#endif

#ifndef LIB_REDUCE_MEAN_MEAN_COMMON_IMPL_H
#define LIB_REDUCE_MEAN_MEAN_COMMON_IMPL_H

#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "include/adv_api/reduce/mean_utils.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/reduce/mean/mean_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
constexpr uint32_t HALF_NUM_PER = 128;
constexpr uint32_t FLOAT_NUM_PER = 64;

__aicore__ inline void MeanCast(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const MeanParams& meanParams)
{
    uint32_t elementNumPerRep = FLOAT_NUM_PER;
    uint32_t repeatTimes = (meanParams.n + elementNumPerRep - 1) / elementNumPerRep;
    const UnaryRepeatParams unaryParams;
    float scalarValue = static_cast<float>(1) / static_cast<float>(static_cast<int32_t>(meanParams.n));
    LocalTensor<float> TmpTensor = sharedTmpBuffer.ReinterpretCast<float>();
    LocalTensor<half> castTensor = sharedTmpBuffer.ReinterpretCast<half>();
    SetMaskCount();
    for (uint32_t row = 0; row < meanParams.outter; ++row) {
        SetVectorMask<half>(0, meanParams.n);
        Cast<float, half, false>(TmpTensor, srcTensor[row * meanParams.inner], RoundMode::CAST_NONE, MASK_PLACEHOLDER,
                                 1, {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
        PipeBarrier<PIPE_V>();
        RepeatReduceSum<float, false>(TmpTensor[meanParams.inner], TmpTensor, 1,
                                      MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE,
                                      DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        PipeBarrier<PIPE_V>();
        uint32_t reduceNums = repeatTimes;
        while (reduceNums > 1) {
            SetVectorMask<half>(0, reduceNums);
            reduceNums = (reduceNums + elementNumPerRep - 1) / elementNumPerRep;
            RepeatReduceSum<float, false>(TmpTensor[meanParams.inner], TmpTensor[meanParams.inner], 1,
                                      MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE,
                                      DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

            PipeBarrier<PIPE_V>();
        }
        SetVectorMask<half>(0, 1);
        Muls<float, false>(TmpTensor[meanParams.inner], TmpTensor[meanParams.inner],
                       scalarValue, MASK_PLACEHOLDER, 1, unaryParams);
        PipeBarrier<PIPE_V>();
        Cast<half, float, false>(castTensor, TmpTensor[meanParams.inner], RoundMode::CAST_NONE, MASK_PLACEHOLDER,
                                 1, {1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
        PipeBarrier<PIPE_V>();
        RepeatReduceSum<half, false>(dstTensor[row], castTensor, 1, MASK_PLACEHOLDER,
                                     DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    }
    SetMaskNorm();
    ResetMask();
}

template <typename T>
__aicore__ inline void MeanForOneRepeatTime(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
                                            const MeanParams& meanParams, T scalarValue)
{
    SetVectorMask<T>(0, meanParams.n);
    for (uint32_t row = 0; row < meanParams.outter; ++row) {
        RepeatReduceSum<T, false>(dstTensor[row], srcTensor[row * meanParams.inner], 1, MASK_PLACEHOLDER,
                                  DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    }
    PipeBarrier<PIPE_V>();
    SetVectorMask<T>(0, meanParams.outter);
    const UnaryRepeatParams unaryParams;
    Muls<T, false>(dstTensor, dstTensor, scalarValue, MASK_PLACEHOLDER, 1, unaryParams);
    SetMaskNorm();
    ResetMask();
}

template <typename T, typename accType, bool isReuseSource>
__aicore__ inline void MeanCommon(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const MeanParams& meanParams)
{
    uint32_t elementNumPerRep = FLOAT_NUM_PER;
    if constexpr (sizeof(T) == sizeof(half)) {
        elementNumPerRep = HALF_NUM_PER;
    }
    uint32_t repeatTimes = (meanParams.n + elementNumPerRep - 1) / elementNumPerRep;
    T scalarValue = static_cast<T>(static_cast<float>(1) / static_cast<float>(static_cast<int32_t>(meanParams.n)));
    SetMaskCount();
    if (repeatTimes == 1) {
        return MeanForOneRepeatTime(dstTensor, srcTensor, meanParams, scalarValue);
    }
    const UnaryRepeatParams unaryParams;
    LocalTensor<T> TmpTensor = sharedTmpBuffer.ReinterpretCast<T>();
    for (uint32_t row = 0; row < meanParams.outter; ++row) {
        uint32_t reduceNums = repeatTimes;
        SetVectorMask<T>(0, meanParams.n);
        RepeatReduceSum<T, false>(TmpTensor,
            srcTensor[row * meanParams.inner],
            1,
            MASK_PLACEHOLDER,
            DEFAULT_BLK_STRIDE,
            DEFAULT_BLK_STRIDE,
            DEFAULT_BLK_STRIDE,
            DEFAULT_REPEAT_STRIDE);
        PipeBarrier<PIPE_V>();
        while (reduceNums > 1) {
            SetVectorMask<T>(0, reduceNums);
            reduceNums = (reduceNums + elementNumPerRep - 1) / elementNumPerRep;
            if (reduceNums == 1) {
                RepeatReduceSum<T, false>(dstTensor[row], TmpTensor, 1, MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE,
                                          DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
            } else {
                RepeatReduceSum<T, false>(TmpTensor, TmpTensor, 1, MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE,
                                          DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
            }
            PipeBarrier<PIPE_V>();
        }
    }
    SetVectorMask<T>(0, meanParams.outter);
    Muls<T, false>(dstTensor, dstTensor, scalarValue, MASK_PLACEHOLDER, 1, unaryParams);
    SetMaskNorm();
}

template <typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false,
          int32_t reduceDim = -1>
__aicore__ inline void MeanImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const MeanParams& meanParams) {
    uint32_t elementNumPerRep = FLOAT_NUM_PER;
    if constexpr (sizeof(T) == sizeof(half) && sizeof(accType) == sizeof(float))
    {
        uint32_t repeatTimes = (meanParams.n + elementNumPerRep - 1) / elementNumPerRep;
        uint32_t finalWorkSize = meanParams.inner * sizeof(float) + (repeatTimes + ONE_BLK_SIZE - 1) / ONE_BLK_SIZE * ONE_BLK_SIZE;
        CHECK_FUNC_HIGHLEVEL_API(Mean, (T, accType, isReuseSource, isBasicBlock, reduceDim), (dstTensor, srcTensor,
            sharedTmpBuffer, meanParams, finalWorkSize));
        MeanCast(dstTensor, srcTensor, sharedTmpBuffer, meanParams);
    } else {

        if constexpr (sizeof(T) == sizeof(half)) {
            elementNumPerRep = HALF_NUM_PER;
        }
        uint32_t repeatTimes = (meanParams.n + elementNumPerRep - 1) / elementNumPerRep;
        uint32_t finalWorkSize = (repeatTimes + ONE_BLK_SIZE - 1) / ONE_BLK_SIZE * ONE_BLK_SIZE;
        
        CHECK_FUNC_HIGHLEVEL_API(Mean, (T, accType, isReuseSource, isBasicBlock, reduceDim), (dstTensor, srcTensor,
            sharedTmpBuffer, meanParams, finalWorkSize));
        MeanCommon<T, accType, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, meanParams);
    }
}

#pragma end_pipe
}  // namespace AscendC

#endif  // LIB_REDUCE_MEAN_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_MEAN_MEAN_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_MEAN_MEAN_COMMON_IMPL_H__
#endif
