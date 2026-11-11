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
#pragma message("impl/adv_api/detail/reduce/reduce_common_util_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_UTIL_V220_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_COMMON_UTIL_V220_IMPL_H
#define IMPL_REDUCE_REDUCE_COMMON_UTIL_V220_IMPL_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "reduce_common_util_impl.h"
#include "../common/check.h"

namespace AscendC {
namespace Internal {

template <typename T, ApiMode apiMode>
__aicore__ inline void DoReduceLessThanBlk(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    uint32_t firstAxis, uint32_t lastAxis)
{
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    constexpr uint32_t elePerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    uint32_t firstBlkRepeat = DivCeil(firstAxis, DEFAULT_BLK_NUM);
    uint32_t blkMaxRepeat = DivCeil(firstBlkRepeat, MAX_REPEAT_TIMES);
    uint32_t blkRepeatTail =
        firstBlkRepeat % MAX_REPEAT_TIMES == 0 ? MAX_REPEAT_TIMES : firstBlkRepeat % MAX_REPEAT_TIMES;
    uint32_t mainBlkNum = firstAxis < DEFAULT_BLK_NUM ? firstAxis : DEFAULT_BLK_NUM;
    uint64_t mainMaskLow = 0;
    uint64_t mainMaskHigh = 0;
    ComputeMaskBit<T>(lastAxis, elePerBlk, mainBlkNum, mainMaskLow, mainMaskHigh);
    uint64_t mainMask[] = { mainMaskLow, mainMaskHigh };
    uint32_t tailBlkNum = firstAxis % DEFAULT_BLK_NUM;
    if (tailBlkNum == 0 || firstAxis < DEFAULT_BLK_NUM) {
        uint32_t blkMainRepeat = MAX_REPEAT_TIMES;
        for (int32_t i = 0; i < blkMaxRepeat; i++) {
            blkMainRepeat = i == blkMaxRepeat - 1 ? blkRepeatTail : MAX_REPEAT_TIMES;
            BlockReduceCompute<T, apiMode>(dstTensor[i * MAX_REPEAT_TIMES * DEFAULT_BLK_NUM],
                srcTensor[i * MAX_REPEAT_TIMES * elePerRep], blkMainRepeat, mainMask, 1,
                DEFAULT_REPEAT_STRIDE);
            PipeBarrier<PIPE_V>();
        }
    } else {
        uint64_t tailMaskLow = 0;
        uint64_t tailMaskHigh = 0;
        ComputeMaskBit<T>(lastAxis, elePerBlk, tailBlkNum, tailMaskLow, tailMaskHigh);
        uint64_t tailMask[] = { tailMaskLow, tailMaskHigh };
        for (int32_t i = 0; i < blkMaxRepeat; i++) {
            if (i == blkMaxRepeat - 1) {
                BlockReduceCompute<T, apiMode>(dstTensor[i * MAX_REPEAT_TIMES * DEFAULT_BLK_NUM],
                    srcTensor[i * MAX_REPEAT_TIMES * elePerRep], blkRepeatTail - 1, mainMask, 1,
                    DEFAULT_REPEAT_STRIDE);
                PipeBarrier<PIPE_V>();
                BlockReduceCompute<T, apiMode>(dstTensor[(i * MAX_REPEAT_TIMES + blkRepeatTail - 1) * DEFAULT_BLK_NUM],
                    srcTensor[(i * MAX_REPEAT_TIMES + blkRepeatTail - 1) * elePerRep], 1, tailMask, 1,
                    DEFAULT_REPEAT_STRIDE);
            } else {
                BlockReduceCompute<T, apiMode>(dstTensor[i * MAX_REPEAT_TIMES * DEFAULT_BLK_NUM],
                    srcTensor[i * MAX_REPEAT_TIMES * elePerRep], MAX_REPEAT_TIMES, mainMask, 1,
                    DEFAULT_REPEAT_STRIDE);
                PipeBarrier<PIPE_V>();
            }
        }
    }
}

template <typename T, ApiMode apiMode>
__aicore__ inline void DoReduceOneBlk(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    uint32_t firstAxis, uint32_t lastAxis)
{
    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, firstAxis * lastAxis);
    BlockReduceCompute<T, apiMode, MaskMode::COUNTER>(dstTensor, srcTensor, 1, MASK_PLACEHOLDER_LIST, 1,
        DEFAULT_REPEAT_STRIDE);
}

template <typename T, void (*func)(const LocalTensor<T> &, const LocalTensor<T> &, const LocalTensor<T> &, uint64_t,
    const uint8_t, const BinaryRepeatParams &)>
__aicore__ inline void AccValOnBlk(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& tmpTensor, const BinaryRepeatParams& mainParams, const BinaryRepeatParams& tailParams,
    uint32_t firstAxis, uint32_t lastAxis, uint32_t tmpOffset, uint32_t padLast)
{
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    uint32_t firstRepeat = DivCeil(firstAxis, MAX_REPEAT_TIMES);
    uint32_t firstRepeatTail = firstAxis % MAX_REPEAT_TIMES == 0 ? MAX_REPEAT_TIMES : firstAxis % MAX_REPEAT_TIMES;
    uint32_t blkCount = lastAxis / elePerBlk;
    uint32_t blkTail = lastAxis % elePerBlk;
    for (int32_t i = 1; i < blkCount; i++) {
        SetVectorMask<T, MaskMode::COUNTER>(0, firstAxis * elePerBlk);
        func(tmpTensor, tmpTensor, srcTensor[i * elePerBlk], MASK_PLACEHOLDER, 1, mainParams);
        PipeBarrier<PIPE_V>();
    }
    if (blkTail != 0) {
        SetMaskNorm();
        SetVectorMask<T, MaskMode::NORMAL>(blkTail);
        uint32_t blkRepeat = MAX_REPEAT_TIMES;
        for (int32_t i = 0; i < firstRepeat; i++) {
            blkRepeat = i == firstRepeat - 1 ? firstRepeatTail : MAX_REPEAT_TIMES;
            func(tmpTensor[i * MAX_REPEAT_TIMES * tmpOffset], tmpTensor[i * MAX_REPEAT_TIMES * tmpOffset],
                srcTensor[i * MAX_REPEAT_TIMES * padLast + blkCount * elePerBlk], blkTail, blkRepeat, tailParams);
            PipeBarrier<PIPE_V>();
        }
        SetMaskCount();
    }
}

template <typename T, bool isReuseSource, ApiMode apiMode,
    void (*func)(const LocalTensor<T> &, const LocalTensor<T> &, const LocalTensor<T> &, uint64_t, const uint8_t,
        const BinaryRepeatParams &)>
__aicore__ inline void DoReduceByBlk(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& tmpTensor, uint32_t firstAxis, uint32_t lastAxis, uint32_t padLast)
{
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    uint8_t blkStridePerRow = padLast / elePerBlk;
    uint8_t blkStridePerRep = (padLast / elePerBlk) * DEFAULT_BLK_NUM;
    SetMaskCount();
    if constexpr (!isReuseSource) {
        UnaryRepeatParams blockUnaryParams{ 1, blkStridePerRow, DEFAULT_REPEAT_STRIDE, blkStridePerRep };
        SetVectorMask<T, MaskMode::COUNTER>(0, firstAxis * elePerBlk);
        Adds<T, false>(tmpTensor, srcTensor, static_cast<T>(0), MASK_PLACEHOLDER, 1, blockUnaryParams);
        PipeBarrier<PIPE_V>();
        BinaryRepeatParams blockMainParams{ 1, 1, blkStridePerRow, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, blkStridePerRep};
        BinaryRepeatParams blockTailParams{ 1, 1, 1, 1, 1, blkStridePerRow };
        AccValOnBlk<T, func>(dstTensor, srcTensor, tmpTensor, blockMainParams, blockTailParams, firstAxis, lastAxis, elePerBlk, padLast);
        SetVectorMask<T, MaskMode::COUNTER>(0, firstAxis * elePerBlk);
        BlockReduceCompute<T, apiMode, MaskMode::COUNTER>(dstTensor, tmpTensor, 1, MASK_PLACEHOLDER_LIST, 1,
            DEFAULT_REPEAT_STRIDE);
    } else {
        BinaryRepeatParams blockMainParams{ blkStridePerRow, blkStridePerRow, blkStridePerRow, blkStridePerRep,
            blkStridePerRep, blkStridePerRep};
        BinaryRepeatParams blockTailParams{ 1, 1, 1, blkStridePerRow, blkStridePerRow, blkStridePerRow };
        AccValOnBlk<T, func>(dstTensor, srcTensor, srcTensor, blockMainParams, blockTailParams, firstAxis, lastAxis, padLast, padLast);
        SetVectorMask<T, MaskMode::COUNTER>(0, firstAxis * elePerBlk);
        BlockReduceCompute<T, apiMode, MaskMode::COUNTER>(dstTensor, srcTensor, 1, MASK_PLACEHOLDER_LIST, blkStridePerRow,
            blkStridePerRep);
    }
}

template <typename T, ApiMode apiMode>
__aicore__ inline void GetReduceValOnRep(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& tmpTensor, uint32_t firstAxis, uint32_t tmpOffset, uint32_t repStride)
{
    constexpr uint32_t elePerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    uint32_t firstRepeat = DivCeil(firstAxis, MAX_REPEAT_TIMES);
    uint32_t firstRepeatTail = firstAxis % MAX_REPEAT_TIMES == 0 ? MAX_REPEAT_TIMES : firstAxis % MAX_REPEAT_TIMES;
    if constexpr (IsSameType<T, half>::value) {
        SetMaskNorm();
        uint32_t blockRepeat = MAX_REPEAT_TIMES;
        for (int32_t i = 0; i < firstRepeat; i++) {
            blockRepeat = i == firstRepeat - 1 ? firstRepeatTail : MAX_REPEAT_TIMES;
            WholeReduceCompute<T, apiMode>(dstTensor[i * MAX_REPEAT_TIMES], tmpTensor[i * MAX_REPEAT_TIMES * tmpOffset],
                blockRepeat, elePerRep, repStride);
            PipeBarrier<PIPE_V>();
        }
    } else {
        SetVectorMask<T, MaskMode::COUNTER>(0, firstAxis * elePerRep);
        BlockReduceCompute<T, apiMode, MaskMode::COUNTER>(tmpTensor, tmpTensor, 1, MASK_PLACEHOLDER_LIST, 1,
            repStride);
        PipeBarrier<PIPE_V>();
        SetVectorMask<T, MaskMode::COUNTER>(0, firstAxis * DEFAULT_BLK_NUM);
        BlockReduceCompute<T, apiMode, MaskMode::COUNTER>(dstTensor, tmpTensor, 1, MASK_PLACEHOLDER_LIST, 1,
            DEFAULT_REPEAT_STRIDE);
    }
}

template <typename T, ApiMode apiMode,
    void (*func)(const LocalTensor<T> &, const LocalTensor<T> &, const LocalTensor<T> &, uint64_t, const uint8_t,
    const BinaryRepeatParams &)>
__aicore__ inline void AccValOnRep(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& tmpTensor, const BinaryRepeatParams& binaryParams, uint32_t firstAxis, uint32_t lastAxis,
    uint32_t tmpOffset, uint32_t repStride, uint32_t padLast)
{
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    constexpr uint32_t elePerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    uint32_t firstRepeat = DivCeil(firstAxis, MAX_REPEAT_TIMES);
    uint32_t firstRepeatTail = firstAxis % MAX_REPEAT_TIMES == 0 ? MAX_REPEAT_TIMES : firstAxis % MAX_REPEAT_TIMES;
    uint32_t repCount = lastAxis / elePerRep;
    uint32_t repTail = lastAxis % elePerRep;
    for (int32_t i = 1; i < repCount; i++) {
        SetVectorMask<T, MaskMode::COUNTER>(0, firstAxis * elePerRep);
        func(tmpTensor, tmpTensor, srcTensor[i * elePerRep], MASK_PLACEHOLDER, 1, binaryParams);
        PipeBarrier<PIPE_V>();
    }
    if (repTail != 0) {
        SetMaskNorm();
        SetVectorMask<T, MaskMode::NORMAL>(repTail);
        uint32_t repRepeat = MAX_REPEAT_TIMES;
        for (int32_t i = 0; i < firstRepeat; i++) {
            repRepeat = i == firstRepeat - 1 ? firstRepeatTail : MAX_REPEAT_TIMES;
            func(tmpTensor[i * MAX_REPEAT_TIMES * tmpOffset], tmpTensor[i * MAX_REPEAT_TIMES * tmpOffset],
                srcTensor[i * MAX_REPEAT_TIMES * padLast + repCount * elePerRep], repTail, repRepeat, binaryParams);
            PipeBarrier<PIPE_V>();
        }
        SetMaskCount();
    }
    GetReduceValOnRep<T, apiMode>(dstTensor, srcTensor, tmpTensor, firstAxis, tmpOffset, repStride);
}

template <typename T, bool isReuseSource, ApiMode apiMode,
    void (*func)(const LocalTensor<T> &, const LocalTensor<T> &, const LocalTensor<T> &, uint64_t, const uint8_t,
        const BinaryRepeatParams &)>
__aicore__ inline void DoReduceByRep(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& tmpTensor, uint32_t firstAxis, uint32_t lastAxis, uint32_t padLast)
{
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    constexpr uint32_t elePerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    uint8_t repStridePerRow = padLast / elePerBlk;
    SetMaskCount();
    if constexpr (!isReuseSource) {
        UnaryRepeatParams repeatUnaryParams{ 1, 1, DEFAULT_REPEAT_STRIDE, repStridePerRow };
        SetVectorMask<T, MaskMode::COUNTER>(0, firstAxis * elePerRep);
        Adds<T, false>(tmpTensor, srcTensor, static_cast<T>(0), MASK_PLACEHOLDER, 1, repeatUnaryParams);
        PipeBarrier<PIPE_V>();
        BinaryRepeatParams binaryParams{ 1, 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, repStridePerRow};
        AccValOnRep<T, apiMode, func>(dstTensor, srcTensor, tmpTensor, binaryParams, firstAxis, lastAxis, elePerRep,
            DEFAULT_REPEAT_STRIDE, padLast);
    } else {
        BinaryRepeatParams binaryParams{ 1, 1, 1, repStridePerRow, repStridePerRow, repStridePerRow};
        AccValOnRep<T, apiMode, func>(dstTensor, srcTensor, srcTensor, binaryParams, firstAxis, lastAxis, padLast,
            repStridePerRow, padLast);
    }
}

template <typename T, bool isReuseSource, ApiMode apiMode,
    void (*func)(const LocalTensor<T> &, const LocalTensor<T> &, const LocalTensor<T> &, uint64_t, const uint8_t,
        const BinaryRepeatParams &)>
__aicore__ inline void DoLongLastReduce(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& tmpTensor, uint32_t firstAxis, uint32_t lastAxis, uint32_t padLast)
{
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    constexpr uint32_t elePerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    uint32_t repCount = DivCeil(lastAxis, elePerRep);
    uint32_t repTail = lastAxis % elePerRep == 0 ? elePerRep : lastAxis % elePerRep;
    BinaryRepeatParams defaultParams;
    UnaryRepeatParams defaultUnaryParams;
    SetMaskCount();
    if constexpr (!isReuseSource) {
        SetVectorMask<T, MaskMode::COUNTER>(0, elePerRep);
        for (int32_t i = 0; i < firstAxis; i++) {
            Adds<T, false>(tmpTensor[i * elePerRep], srcTensor[i * padLast], static_cast<T>(0), MASK_PLACEHOLDER,
                1, defaultUnaryParams);
            PipeBarrier<PIPE_V>();
        }
        uint32_t mask = elePerRep;
        for (int32_t i = 1; i < repCount; i++) {
            mask = i == repCount - 1 ? repTail : elePerRep;
            SetVectorMask<T, MaskMode::COUNTER>(0, mask);
            for (int32_t j = 0; j < firstAxis; j++) {
                func(tmpTensor[j * elePerRep], tmpTensor[j * elePerRep],
                    srcTensor[j * padLast + i * elePerRep], MASK_PLACEHOLDER, 1, defaultParams);
                PipeBarrier<PIPE_V>();
            }
        }
        GetReduceValOnRep<T, apiMode>(dstTensor, srcTensor, tmpTensor, firstAxis, elePerRep, DEFAULT_REPEAT_STRIDE);
    } else {
        uint32_t mask = elePerRep;
        for (int32_t i = 0; i < firstAxis; i++) {
            for (int32_t j = 1; j < repCount; j++) {
                mask = j == repCount - 1 ? repTail : elePerRep;
                SetVectorMask<T, MaskMode::COUNTER>(0, mask);
                if (j == 1) {
                    func(srcTensor[i * elePerRep], srcTensor[i * padLast], srcTensor[i * padLast + j * elePerRep],
                        MASK_PLACEHOLDER, 1, defaultParams);
                    PipeBarrier<PIPE_V>();
                } else {
                    func(srcTensor[i * elePerRep], srcTensor[i * elePerRep], srcTensor[i * padLast + j * elePerRep],
                        MASK_PLACEHOLDER, 1, defaultParams);
                    PipeBarrier<PIPE_V>();
                }
            }
        }
        GetReduceValOnRep<T, apiMode>(dstTensor, srcTensor, srcTensor, firstAxis, elePerRep, DEFAULT_REPEAT_STRIDE);
    }
}

template <typename T, bool isReuseSource, ApiMode apiMode,
    void (*func)(const LocalTensor<T> &, const LocalTensor<T> &, const LocalTensor<T> &, uint64_t, const uint8_t,
    const BinaryRepeatParams &)>
__aicore__ inline void BlockReduceByLastAxis(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& tmpTensor, uint32_t firstAxis, uint32_t lastAxis, uint32_t padLast)
{
    ASCENDC_ASSERT((dstTensor.GetSize() >= firstAxis), {
        KERNEL_LOG(KERNEL_ERROR, "dstTensor must be greater than or equal to %u, current size if %u",
            firstAxis, dstTensor.GetSize());
    });
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    constexpr uint32_t elePerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    if (lastAxis < elePerBlk) {
        DoReduceLessThanBlk<T, apiMode>(dstTensor, srcTensor, firstAxis, lastAxis);
    } else if (lastAxis == elePerBlk) {
        DoReduceOneBlk<T, apiMode>(dstTensor, srcTensor, firstAxis, lastAxis);
    } else if (lastAxis > elePerBlk && lastAxis < elePerRep) {
        DoReduceByBlk<T, isReuseSource, apiMode, func>(dstTensor, srcTensor, tmpTensor, firstAxis, lastAxis, padLast);
    } else if (lastAxis >= elePerRep && lastAxis <= MAX_REPEAT_TIMES * elePerBlk) {
        DoReduceByRep<T, isReuseSource, apiMode, func>(dstTensor, srcTensor, tmpTensor, firstAxis, lastAxis, padLast);
    } else {
        DoLongLastReduce<T, isReuseSource, apiMode, func>(dstTensor, srcTensor, tmpTensor, firstAxis, lastAxis,
            padLast);
    }
}

struct ReduceParams {
public:
    __aicore__ ReduceParams() {}
    __aicore__ ReduceParams(uint32_t first, uint32_t last,
        uint32_t padLast, uint32_t splitK, uint32_t tail, uint32_t elePerBlk)
    {
        this->first = first;
        this->last = last;
        this->padLast = padLast;
        this->splitK = splitK;
        this->tail = tail;
        this->elePerBlk = elePerBlk;
    }
    uint32_t first = 0;
    uint32_t last = 0;
    uint32_t padLast = 0;
    uint32_t splitK = 0;
    uint32_t tail = 0;
    uint32_t elePerBlk = 0;
    BinaryRepeatParams defaultParam = { DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE,
                DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE };
    UnaryRepeatParams defaultUnaryParam = { DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE,
                DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE };
};

template <class T, ApiMode apiMode>
__aicore__ inline void BlkReduceForLoop(const LocalTensor<T>& dst,
    const LocalTensor<T>& tmp, uint32_t srcOffset, uint32_t first, uint32_t last) {
    constexpr uint32_t blkReduceDstStride = 8;  // elements uint
    uint32_t srcPerBlkElements = ONE_BLK_SIZE/sizeof(T);
    uint64_t maskHigh = 0;
    uint32_t oneRepElements = srcPerBlkElements * DEFAULT_BLK_NUM;
    uint32_t nMaxRepBlkNum = first / (MAX_REPEAT_TIMES * DEFAULT_BLK_NUM);
    uint32_t tailMaxRepBlkNum = first % (MAX_REPEAT_TIMES * DEFAULT_BLK_NUM);
    uint32_t tailNBlkNum = tailMaxRepBlkNum / DEFAULT_BLK_NUM;
    uint32_t tailRemainOfBlkNum = tailMaxRepBlkNum % DEFAULT_BLK_NUM;
    uint32_t dstOffset = 0;
    uint32_t blkReduceSrcOffset = 0;
    uint32_t oneBlkMask = last > srcPerBlkElements ? srcPerBlkElements : last;
    uint64_t maskLow = 0;
    ComputeMaskBit<T>(oneBlkMask, srcPerBlkElements, DEFAULT_BLK_NUM, maskLow, maskHigh);

    uint64_t blkReduceMask[] = { maskLow, maskHigh };
    for (int k = 0; k < nMaxRepBlkNum; k++) {
        BlockReduceCompute<T, apiMode>(dst[dstOffset], tmp[srcOffset], MAX_REPEAT_TIMES, blkReduceMask,
            DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        PipeBarrier<PIPE_V>();
    }
    if (tailNBlkNum > 0) {
        dstOffset = nMaxRepBlkNum * MAX_REPEAT_TIMES * blkReduceDstStride;
        blkReduceSrcOffset = srcOffset + nMaxRepBlkNum * MAX_REPEAT_TIMES * DEFAULT_BLK_NUM * srcPerBlkElements;
        BlockReduceCompute<T, apiMode>(dst[dstOffset], tmp[blkReduceSrcOffset], tailNBlkNum, blkReduceMask,
            DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        PipeBarrier<PIPE_V>();
    }
    if (tailRemainOfBlkNum > 0) {
        maskLow = 0;
        maskHigh = 0;
        uint32_t tailBlkReduceRep = 1;
        ComputeMaskBit<T>(oneBlkMask, srcPerBlkElements, tailRemainOfBlkNum, maskLow, maskHigh);
        uint64_t tailMask[] = { maskLow, maskHigh };
        dstOffset = tailNBlkNum * blkReduceDstStride + (nMaxRepBlkNum * MAX_REPEAT_TIMES * blkReduceDstStride);
        blkReduceSrcOffset = srcOffset + tailNBlkNum * oneRepElements + (nMaxRepBlkNum * MAX_REPEAT_TIMES * oneRepElements);
        BlockReduceCompute<T, apiMode>(dst[dstOffset], tmp[blkReduceSrcOffset], tailBlkReduceRep, tailMask,
            DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        PipeBarrier<PIPE_V>();
    }
}

template <typename T, bool isReuseSource, ApiMode apiMode,
    void (*func)(const LocalTensor<half>&, const LocalTensor<half>&, const LocalTensor<half>&, uint64_t, const uint8_t,
        const BinaryRepeatParams&)>
__aicore__ inline void BinaryReduceAnyAllCompute(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& tmp, const ReduceParams& params)
{
    half halfZero = 0.0;
    LocalTensor<half> tmpBuf = tmp.template ReinterpretCast<half>();
    uint32_t tmpK;
    constexpr uint32_t halfBlkElements = 16;
    SetMaskCount();
    for (int i = 0; i < params.first; i++) {
        SetVectorMask<uint8_t, MaskMode::COUNTER>(params.padLast);
        Cast<half, uint8_t, false>(tmpBuf, src[i*params.padLast], RoundMode::CAST_NONE,
            MASK_PLACEHOLDER, 1,
            { DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE });
        PipeBarrier<PIPE_V>();
        if (params.tail > 0 && params.splitK > 0) {
            SetVectorMask<half, MaskMode::COUNTER>(params.tail);
            func(tmpBuf, tmpBuf, tmpBuf[params.splitK], MASK_PLACEHOLDER, 1, params.defaultParam);
            PipeBarrier<PIPE_V>();
        }
        tmpK = params.splitK;
        while (tmpK > halfBlkElements) {
            tmpK >>= 1;
            SetVectorMask<half, MaskMode::COUNTER>(tmpK);
            func(tmpBuf, tmpBuf, tmpBuf[tmpK], MASK_PLACEHOLDER, 1, params.defaultParam);
            PipeBarrier<PIPE_V>();
        }
        SetVectorMask<half, MaskMode::COUNTER>(halfBlkElements);
        Adds<half, false>(tmpBuf[params.padLast + i * halfBlkElements], tmpBuf, halfZero,
            MASK_PLACEHOLDER, 1, params.defaultUnaryParam);
        PipeBarrier<PIPE_V>();
    }
    SetMaskNorm();
    ResetMask();
    BlkReduceForLoop<half, apiMode>(tmpBuf, tmpBuf, params.padLast, params.first, params.last);
    SetMaskCount();
    SetVectorMask<half, MaskMode::COUNTER>(params.first);
    Cast<uint8_t, half, false>(dst, tmpBuf, RoundMode::CAST_NONE,
        MASK_PLACEHOLDER, 1,
        { DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
    SetMaskNorm();
    ResetMask();
}

template <typename T, bool isReuseSource,
    void (*func)(const LocalTensor<T> &, const LocalTensor<T> &, const LocalTensor<T> &, uint64_t, const uint8_t,
    const BinaryRepeatParams &)>
__aicore__ inline void BinaryReduceByFirstAxis(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& tmpTensor, uint32_t firstAxis, uint32_t lastAxis, uint32_t padLast)
{
    ASCENDC_ASSERT((dstTensor.GetSize() >= lastAxis), {
        KERNEL_LOG(KERNEL_ERROR, "dstTensor must be greater than or equal to %u, current size if %u",
            lastAxis, dstTensor.GetSize());
    });
    BinaryRepeatParams defaultParam;
    UnaryRepeatParams defaultUnaryParam;
    uint32_t k = FindClosestPowerOfTwo(firstAxis);
    uint32_t splitK = 1 << k;
    uint32_t remain = firstAxis - splitK;
    SetMaskCount();
    if constexpr (isReuseSource) {
        // reduce the tail part
        if (remain != 0) {
            SetVectorMask<T, MaskMode::COUNTER>(0, padLast * remain);
            func(srcTensor, srcTensor, srcTensor[splitK * padLast], MASK_PLACEHOLDER, 1, defaultParam);
            PipeBarrier<PIPE_V>();
        }
    } else {
        CheckTmpBufferSize(tmpTensor.GetSize(), 0, tmpTensor.GetSize());
        // reduce the tail part
        if (remain != 0) {
            SetVectorMask<T, MaskMode::COUNTER>(0, splitK * padLast);
            Adds<T, false>(tmpTensor, srcTensor, static_cast<T>(0), MASK_PLACEHOLDER, 1,
                           defaultUnaryParam);
            PipeBarrier<PIPE_V>();
            SetVectorMask<T, MaskMode::COUNTER>(0, padLast * remain);
            func(tmpTensor, tmpTensor, srcTensor[splitK * padLast], MASK_PLACEHOLDER, 1, defaultParam);
            PipeBarrier<PIPE_V>();
        } else if (splitK > 1) { // binary reduce the first part from the srcTensor
            splitK >>= 1;
            SetVectorMask<T, MaskMode::COUNTER>(0, padLast * splitK);
            func(tmpTensor, srcTensor, srcTensor[splitK * padLast], MASK_PLACEHOLDER, 1, defaultParam);
            PipeBarrier<PIPE_V>();
        } else { // move src to dst directly if axis size is one.
            SetVectorMask<T, MaskMode::COUNTER>(0, lastAxis);
            Adds<T, false>(dstTensor, srcTensor, static_cast<T>(0), MASK_PLACEHOLDER, 1, defaultUnaryParam);
            PipeBarrier<PIPE_V>();
            return;
        }
    }
    // binary reduce the remain 2^k axis 
    LocalTensor<T> currBuff = isReuseSource ? srcTensor : tmpTensor;
    while (splitK > 1) {
        splitK >>= 1;
        SetVectorMask<T, MaskMode::COUNTER>(0, padLast * splitK);
        func(currBuff, currBuff, currBuff[splitK * padLast], MASK_PLACEHOLDER, 1, defaultParam);
        PipeBarrier<PIPE_V>();
    }
    SetVectorMask<T, MaskMode::COUNTER>(0, lastAxis);
    Adds<T, false>(dstTensor, currBuff, static_cast<T>(0), MASK_PLACEHOLDER, 1, defaultUnaryParam);
    PipeBarrier<PIPE_V>();
}
} // namespace Internal
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_COMMON_UTIL_V220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_UTIL_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_UTIL_V220_IMPL_H__
#endif
