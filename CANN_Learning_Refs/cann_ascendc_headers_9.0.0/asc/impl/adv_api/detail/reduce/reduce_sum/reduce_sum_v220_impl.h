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
#pragma message("impl/adv_api/detail/reduce/reduce_sum/reduce_sum_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_SUM_REDUCE_SUM_V220_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_SUM_REDUCE_SUM_V220_IMPL_H
#define IMPL_REDUCE_REDUCE_SUM_REDUCE_SUM_V220_IMPL_H

#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../reduce_common_util_v220_impl.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/reduce/reduce_sum/reduce_sum_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace Internal {

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumComputeSliceAAxis(const LocalTensor<T>& dst, const LocalTensor<T>& src,
        const LocalTensor<T>& tmpBuf, const uint32_t repeat,
        const uint32_t perRowReduceSize, const uint32_t tailLen, const ReduceParams &params) 
{
    uint32_t curOffset;
    if constexpr (isReuseSource) {
        SetVectorMask<T, MaskMode::COUNTER>(tailLen);
        for (uint32_t i = 0; i < repeat; i++) {
            curOffset = i * params.padLast;
            Add<T, false>(tmpBuf[curOffset], tmpBuf[curOffset],
                src[curOffset + perRowReduceSize], MASK_PLACEHOLDER, 1, params.defaultParam);
        }
        PipeBarrier<PIPE_V>();

        uint16_t blockCount = repeat;
        uint16_t blockLen = perRowReduceSize / B32_DATA_NUM_PER_BLOCK;
        uint16_t srcStride = (params.padLast - perRowReduceSize) / B32_DATA_NUM_PER_BLOCK;
        uint16_t dstStride = 0;
        DataCopy(tmpBuf, tmpBuf, { blockCount, blockLen, srcStride, dstStride });
        PipeBarrier<PIPE_V>();
    } else {
        for (uint32_t i = 0; i < repeat; i++) {
            curOffset = i * params.padLast;
            SetVectorMask<T, MaskMode::COUNTER>(tailLen);
            Add<T, false>(tmpBuf[i * perRowReduceSize], tmpBuf[i * perRowReduceSize],
                src[curOffset + perRowReduceSize], MASK_PLACEHOLDER, 1, params.defaultParam);
        }
        PipeBarrier<PIPE_V>();
    }
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumComputeSliceRAxis(const LocalTensor<T>& dst, const LocalTensor<T>& src,
        const LocalTensor<T>& tmpBuf, const uint32_t repeat,
        const uint32_t perRowReduceSize, const uint32_t tailLenN, const uint32_t tailLenRemain, const ReduceParams &params) 
{
    SetMaskNorm();
    uint32_t srcOffset;
    uint32_t tmpOffset;
    uint32_t perRowSize = perRowReduceSize;
    if constexpr (isReuseSource) {
        perRowSize = params.padLast;
    }
    uint8_t dstRepStride = static_cast<uint8_t>(perRowSize / B32_DATA_NUM_PER_BLOCK);
    uint8_t src0RepStride = dstRepStride;
    uint8_t src1RepStride = static_cast<uint8_t>(params.padLast / B32_DATA_NUM_PER_BLOCK);
    SetVectorMask<T, MaskMode::NORMAL>(B32_DATA_NUM_PER_REPEAT);
    for (uint32_t i = 0; i < tailLenN; i++) {
        srcOffset = perRowReduceSize + i * B32_DATA_NUM_PER_REPEAT;
        tmpOffset = i * B32_DATA_NUM_PER_REPEAT;
        Add<T, false>(tmpBuf[tmpOffset], tmpBuf[tmpOffset], src[srcOffset], MASK_PLACEHOLDER, repeat,
            {1, 1, 1, dstRepStride, src0RepStride, src1RepStride});
    }
    if (tailLenRemain > 0) {
        tmpOffset = tailLenN * B32_DATA_NUM_PER_REPEAT;
        srcOffset = perRowReduceSize + tailLenN * B32_DATA_NUM_PER_REPEAT;
        SetVectorMask<T, MaskMode::NORMAL>(tailLenRemain);
        Add<T, false>(tmpBuf[tmpOffset], tmpBuf[tmpOffset], src[srcOffset], MASK_PLACEHOLDER, repeat,
            {1, 1, 1, dstRepStride, src0RepStride, src1RepStride});
    }
    PipeBarrier<PIPE_V>();
    if constexpr (isReuseSource) {
        uint16_t blockCount = repeat;
        uint16_t blockLen = perRowReduceSize / B32_DATA_NUM_PER_BLOCK;
        uint16_t srcStride = (params.padLast - perRowReduceSize) / B32_DATA_NUM_PER_BLOCK;
        uint16_t dstStride = 0;
        DataCopy(tmpBuf, tmpBuf, { blockCount, blockLen, srcStride, dstStride });
        PipeBarrier<PIPE_V>();
    }
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumComputeTail(const LocalTensor<T>& dst, const LocalTensor<T>& src,
        const LocalTensor<T>& tmpBuf, const uint32_t repeat,
        const uint32_t perRowReduceSize, const ReduceParams &params) 
{
    uint32_t tailLen = params.tail;
    if (params.tail == 0 && perRowReduceSize > 0) {
        tailLen = params.padLast - perRowReduceSize;
    }
    uint32_t tailLenN = tailLen / B32_DATA_NUM_PER_REPEAT;
    uint32_t tailLenRemain = tailLen % B32_DATA_NUM_PER_REPEAT;
    bool isLastAxisSliceLarge = (tailLenN + (tailLenRemain > 0)) < repeat;
    uint32_t vaddMaxRepStrideVal = 255;
    // Add api repStride max val is 255, perRowSize / B32_DATA_NUM_PER_BLOCK should be less than or equal to 255
    bool isLeMaxRepStride = (perRowReduceSize / B32_DATA_NUM_PER_BLOCK) <= vaddMaxRepStrideVal;
    bool performanceSlice = isLastAxisSliceLarge && params.last > B32_DATA_NUM_PER_REPEAT && isLeMaxRepStride;
    if (performanceSlice) {
        ReduceSumComputeSliceRAxis<T, isReuseSource>(dst, src, tmpBuf,
            repeat, perRowReduceSize, tailLenN, tailLenRemain, params);
    } else {
        ReduceSumComputeSliceAAxis<T, isReuseSource>(dst, src, tmpBuf, repeat, perRowReduceSize, tailLen, params);
    }
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumInLargeLast(const LocalTensor<T>& dst, const LocalTensor<T>& src,
        const LocalTensor<T>& tmpBuf, const uint32_t repeat,
        const uint32_t perRowReduceSize, const ReduceParams &params)
{
    if constexpr (!isReuseSource) {
        uint16_t blockCount = repeat;
        uint16_t blockLen = perRowReduceSize / B32_DATA_NUM_PER_BLOCK;
        uint16_t srcStride = (params.padLast - perRowReduceSize) / B32_DATA_NUM_PER_BLOCK;
        uint16_t dstStride = 0;
        DataCopy(tmpBuf, src, { blockCount, blockLen, srcStride, dstStride });
        PipeBarrier<PIPE_V>();
    }
    ReduceSumComputeTail<T, isReuseSource>(dst, src, tmpBuf, repeat, perRowReduceSize, params);
    ResetMask();
    SetMaskCount();
    uint32_t tmpK = perRowReduceSize;
    while (tmpK > B32_DATA_NUM_PER_REPEAT) {
        SetVectorMask<T, MaskMode::COUNTER>(repeat * tmpK);
        PairReduceSum<T, false>(tmpBuf, tmpBuf, 1, MASK_PLACEHOLDER, 1, 1, DEFAULT_REPEAT_STRIDE);
        PipeBarrier<PIPE_V>();
        tmpK >>= 1;
    }
    SetVectorMask<T, MaskMode::COUNTER>(repeat * B32_DATA_NUM_PER_REPEAT);
    BlockReduceSum<T, false>(tmpBuf, tmpBuf, 1, MASK_PLACEHOLDER, 1, 1, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    SetVectorMask<T, MaskMode::COUNTER>(repeat * B32_DATA_NUM_PER_BLOCK);
    BlockReduceSum<T, false>(dst, tmpBuf, 1, MASK_PLACEHOLDER, 1, 1, DEFAULT_REPEAT_STRIDE);
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumArCompute(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LocalTensor<T>& tmpBuf, const uint32_t perRowReduceSize, const ReduceParams &params) 
{
    constexpr uint32_t maxRepeatTimes = 248;
    uint32_t maxRepOffsetN = params.first / maxRepeatTimes;
    uint32_t maxRepeatOffsetTail = params.first % maxRepeatTimes;
    uint32_t srcMaxRepNOffset;
    uint32_t dstMaxRepNOffset;
    uint32_t srcMaxRepTailOffset;
    uint32_t dstMaxRepTailOffset;
    if (params.last <= B32_DATA_NUM_PER_REPEAT) {
        SetMaskNorm();
        SetVectorMask<T, MaskMode::NORMAL>(params.last);
        for (uint32_t i = 0; i < maxRepOffsetN; i++) {
            srcMaxRepNOffset = maxRepeatTimes * i * params.padLast;
            dstMaxRepNOffset = maxRepeatTimes * i;
            WholeReduceSum<T, false>(dst[dstMaxRepNOffset], src[srcMaxRepNOffset],
                MASK_PLACEHOLDER, maxRepeatTimes, 1, 1, params.padLast / params.elePerBlk);
        }
        if (maxRepeatOffsetTail > 0) {
            srcMaxRepTailOffset = (params.first - maxRepeatOffsetTail) * params.padLast;
            dstMaxRepTailOffset = params.first - maxRepeatOffsetTail;
            SetVectorMask<T, MaskMode::NORMAL>(params.last);
            WholeReduceSum<T, false>(dst[dstMaxRepTailOffset], src[srcMaxRepTailOffset],
                MASK_PLACEHOLDER, maxRepeatOffsetTail, 1, 1, params.padLast / params.elePerBlk);
        }
    } else {
        SetMaskCount();
        uint32_t tmpBufMaxRepNOffset;

        for (uint32_t i = 0; i < maxRepOffsetN; i++) {
            srcMaxRepNOffset = maxRepeatTimes * i * params.padLast;
            dstMaxRepNOffset = maxRepeatTimes * i;
            tmpBufMaxRepNOffset = maxRepeatTimes * i * B32_DATA_NUM_PER_REPEAT;
            ReduceSumInLargeLast<T, isReuseSource>(dst[dstMaxRepNOffset], src[srcMaxRepNOffset],
                tmpBuf[tmpBufMaxRepNOffset], maxRepeatTimes, perRowReduceSize, params);
        }
        if (maxRepeatOffsetTail > 0) {
            srcMaxRepTailOffset = (params.first - maxRepeatOffsetTail) * params.padLast;
            dstMaxRepTailOffset = params.first - maxRepeatOffsetTail;
            tmpBufMaxRepNOffset = (params.first - maxRepeatOffsetTail) * B32_DATA_NUM_PER_REPEAT;
            ReduceSumInLargeLast<T, isReuseSource>(dst[dstMaxRepTailOffset],
                src[srcMaxRepTailOffset], tmpBuf[tmpBufMaxRepNOffset],
                maxRepeatOffsetTail, perRowReduceSize, params);
        }
        SetMaskNorm();
    }
    PipeBarrier<PIPE_V>();
    ResetMask();
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumArReusedSrc(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    ReduceParams params)
{
    if (params.last <= B32_DATA_NUM_PER_BLOCK) {
        BlkReduceForLoop<T, ApiMode::API_MODE_SUM>(dst, src, 0, params.first, params.last);
    } else {
        uint32_t perRowReduceSize = params.elePerBlk > params.splitK ? params.elePerBlk : params.splitK;
        if (params.last == params.splitK) {
            perRowReduceSize >>= 1;
        }
        ReduceSumArCompute<T, isReuseSource>(dst, src, src, perRowReduceSize, params);
    }
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumArUnReusedSrc(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LocalTensor<T>& tmpBuf, const ReduceParams &params)
{
    uint32_t perRowReduceSize = params.elePerBlk > params.splitK ? params.elePerBlk : params.splitK;
    if (params.last == params.splitK) {
        perRowReduceSize >>= 1;
    }

    if (params.last <= B32_DATA_NUM_PER_BLOCK) {
        BlkReduceForLoop<T, ApiMode::API_MODE_SUM>(dst, src, 0, params.first, params.last);
    } else {
        ReduceSumArCompute<T, isReuseSource>(dst, src, tmpBuf, perRowReduceSize, params);
    }
}

template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceSumCommon(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[], bool srcInnerPad, const ReduceParams &reduceParams)
{
    uint32_t first = reduceParams.first;
    uint32_t last = reduceParams.last;
    uint32_t padLast = reduceParams.padLast;
    uint32_t elePerBlk =  reduceParams.elePerBlk;
    LocalTensor<T> tmpBuf = sharedTmpBuffer.ReinterpretCast<T>();
    if constexpr (IsSameType<pattern, Pattern::Reduce::AR>::value) {
        ASCENDC_ASSERT((dstTensor.GetSize() >= first), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "dstTensor must be greater than or equal to %u, current size if %u",
                first, dstTensor.GetSize());
        });
        uint32_t splitK = 1 << FindClosestPowerOfTwo(last);
        if (last <= elePerBlk) {
            splitK = 0;
        }
        uint32_t tail = last - splitK;
        if constexpr (isReuseSource) {
            ReduceSumArReusedSrc<T, true>(dstTensor, srcTensor, ReduceParams(
                first, last, padLast, splitK, tail, elePerBlk));
        } else {
            ReduceSumArUnReusedSrc<T, false>(dstTensor, srcTensor, tmpBuf,
                ReduceParams(first, last, padLast, splitK, tail, elePerBlk));
        }
    } else {
        ASCENDC_ASSERT((dstTensor.GetSize() >= last), {
            KERNEL_LOG(KERNEL_ERROR, "dstTensor must be greater than or equal to %u, current size if %u",
            last, dstTensor.GetSize());
        });
        BinaryReduceByFirstAxis<T, isReuseSource, Add<T, false>>(
                dstTensor, srcTensor, tmpBuf, first, last, padLast);
    }
}

template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceSumImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[], bool srcInnerPad) {
    uint32_t last = srcShape[1];
    uint32_t first = srcShape[0];
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    uint32_t padLast = AlignUp(last, elePerBlk);
    static_assert(SupportType<T, float>(), "failed to check the data type, current api supports data type is float!");
    static_assert(SupportType<pattern, Pattern::Reduce::AR, Pattern::Reduce::RA>(), 
        "failed to check the reduce pattern, it only supports AR/RA pattern!");
    CHECK_FUNC_HIGHLEVEL_API(ReduceSum, (T, pattern), (dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad, padLast));
    ReduceParams reduceParams = ReduceParams(first, last, padLast, 0, 0, elePerBlk);
    ReduceSumCommon<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad, reduceParams);
    SetMaskNorm();
    ResetMask();
}

} // namespace Internal
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_SUM_REDUCE_SUM_V220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_SUM_REDUCE_SUM_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_SUM_REDUCE_SUM_V220_IMPL_H__
#endif
