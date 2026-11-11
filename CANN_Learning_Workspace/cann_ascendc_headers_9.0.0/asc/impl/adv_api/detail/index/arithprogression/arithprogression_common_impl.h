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
 * \file arithprogression_common_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/index/arithprogression/arithprogression_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/index/arithprogression.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_COMMON_IMPL_H__
#endif

#ifndef IMPL_INDEX_ARITHPROGRESSION_ARITHPROGRESSION_COMMON_IMPL_H
#define IMPL_INDEX_ARITHPROGRESSION_ARITHPROGRESSION_COMMON_IMPL_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "kernel_utils.h"
#include "kernel_log.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/index/arithprogression/arithprogression_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
namespace AscendC {
// Generating an underlying arithmetic sequence through scalar operations.
template <typename T>
__aicore__ inline void GetBaseArithProgression(const LocalTensor<T> &dstLocal, const T firstValue, const T diffValue,
    const int32_t count)
{
    for (int i = 0; i < count; i++) {
        dstLocal.SetValue(i, static_cast<T>(firstValue) +
            static_cast<T>(diffValue) * static_cast<T>(i)); // value might be truncated
    }
}

// template specialization for half, 1. cast to float; 2. calculate; 3. cast to half in SetValue
template <>
__aicore__ inline void GetBaseArithProgression(const LocalTensor<half> &dstLocal, const half firstValue,
    const half diffValue, const int32_t count)
{
    for (int i = 0; i < count; i++) {
        dstLocal.SetValue(i, static_cast<float>(firstValue) +
            static_cast<float>(diffValue) * static_cast<float>(i)); // value might be truncated
    }
}

template <typename T>
__aicore__ inline void ArithProgressionImpl(const LocalTensor<T> &dstLocal, const T firstValue, const T diffValue,
    const int32_t count)
{
    CHECK_FUNC_HIGHLEVEL_API(ArithProgression, (T), (dstLocal, firstValue, diffValue, count));

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
    struct UnaryRepeatParams addsParamsStride1(1, 1, 1, 1);
    struct UnaryRepeatParams addsParamsStride8(1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE);

    constexpr int32_t BLOCK_NUM = (ONE_BLK_SIZE / sizeof(T));
    constexpr int32_t REPEAT_NUM = (ONE_REPEAT_BYTE_SIZE / sizeof(T));
    if (count > BLOCK_NUM) {
        // Generates a basic arithmetic sequence of the BLOCK_NUM length for filling in subsequent arithmetic sequences.
        GetBaseArithProgression<T>(dstLocal, firstValue, diffValue, BLOCK_NUM);
        auto eventIdSToV = GetTPipePtr()->FetchEventID(HardEvent::S_V);
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        if (count > REPEAT_NUM) {
            // broadcast from 1 block size to 8 block size
            SetVectorMask<T>(0, (((static_cast<uint64_t>(1)) << static_cast<uint32_t>(BLOCK_NUM)) - 1));
            PipeBarrier<PIPE_V>();
            for (int i = 0; i < DEFAULT_BLK_NUM - 1; i++) {
                Adds<T, false>(dstLocal[(i + 1) * BLOCK_NUM], dstLocal[i * BLOCK_NUM],
                    static_cast<T>(static_cast<float>(diffValue) * static_cast<float>(BLOCK_NUM)), MASK_PLACEHOLDER,
                    (uint16_t)1, addsParamsStride1);
                PipeBarrier<PIPE_V>();
            }
            int32_t repeat = count / REPEAT_NUM;
            int32_t tail = count % REPEAT_NUM;
            ResetMask();
            PipeBarrier<PIPE_V>();
            // Fills the following arithmetic progression with 8 block size arithmetic progressions
            for (int i = 0; i < repeat - 1; i++) {
                Adds<T, false>(dstLocal[(i + 1) * REPEAT_NUM], dstLocal[i * REPEAT_NUM],
                    static_cast<T>(static_cast<float>(diffValue) * static_cast<float>(REPEAT_NUM)), MASK_PLACEHOLDER,
                    (uint16_t)1, addsParamsStride8);
                PipeBarrier<PIPE_V>();
            }
            if (tail > 0) {
                int32_t tail_aligned = (tail + BLOCK_NUM - 1) / BLOCK_NUM * BLOCK_NUM;
                SetVectorMask<T>(tail_aligned);
                PipeBarrier<PIPE_V>();
                Adds<T, false>(dstLocal[repeat * REPEAT_NUM], dstLocal[(repeat - 1) * REPEAT_NUM],
                    static_cast<T>(static_cast<float>(diffValue) * static_cast<float>(REPEAT_NUM)), MASK_PLACEHOLDER,
                    (uint16_t)1, addsParamsStride8);
                PipeBarrier<PIPE_V>();
            }
        } else {
            // Fills the following arithmetic progression
            int32_t countAligned = (count + BLOCK_NUM - 1) / BLOCK_NUM * BLOCK_NUM;
            int32_t repeat = countAligned / BLOCK_NUM;
            SetVectorMask<T>(0, (((static_cast<uint64_t>(1)) << static_cast<uint32_t>(BLOCK_NUM)) - 1));
            PipeBarrier<PIPE_V>();
            for (int i = 0; i < repeat - 1; i++) {
                Adds<T, false>(dstLocal[(i + 1) * BLOCK_NUM], dstLocal[i * BLOCK_NUM],
                    static_cast<T>(static_cast<float>(diffValue) * static_cast<float>(BLOCK_NUM)), MASK_PLACEHOLDER,
                    (uint16_t)1, addsParamsStride1);
                PipeBarrier<PIPE_V>();
            }
        }
    } else {
        // When the length is less than BLOCK_NUM, the arithmetic sequence is generated directly by using a scalar.
        auto eventIdVToS = GetTPipePtr()->FetchEventID(HardEvent::V_S);
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);
        GetBaseArithProgression<T>(dstLocal, firstValue, diffValue, count);
        auto eventIdSToV = GetTPipePtr()->FetchEventID(HardEvent::S_V);
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
    }
}

template <typename T>
__aicore__ inline __in_pipe__(S) __out_pipe__(V, S) void ArithProgression(const LocalTensor<T> &dstLocal,
    const T firstValue, const T diffValue, const int32_t count)
{
    ArithProgressionImpl(dstLocal, firstValue, diffValue, count);
}
} // namespace AscendC

#endif // IMPL_INDEX_ARITHPROGRESSION_ARITHPROGRESSION_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_COMMON_IMPL_H__
#endif
