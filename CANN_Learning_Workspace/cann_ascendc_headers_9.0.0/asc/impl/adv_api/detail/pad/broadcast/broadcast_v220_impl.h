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
 * \file broadcast_v220_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/pad/broadcast/broadcast_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/broadcast.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_V220_IMPL_H__
#endif

#ifndef IMPL_PAD_BROADCAST_BROADCAST_V220_IMPL_H
#define IMPL_PAD_BROADCAST_BROADCAST_V220_IMPL_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"

namespace AscendC {
constexpr uint32_t BRCB_ONE_SIZE = 8;
constexpr uint32_t BRCB_HALF_MAX_REPEATE_TIMES = 254;
constexpr uint32_t BRCB_FLOAT_MAX_REPEATE_TIMES = 255;
constexpr uint8_t GATHER_MASK_PATTERN = 7;

template <typename T>
__aicore__ inline void BrcbToOneBlock(const LocalTensor<T> &srcLocal, const uint32_t firstDim,
    uint32_t oneBlockElementNum, LocalTensor<T> &brcbOneBlockTempBuffer)
{
    const uint32_t brcbRepeatTime = (firstDim + BRCB_ONE_SIZE - 1) / BRCB_ONE_SIZE;
    uint32_t brcbMaxRepeatTimes = BRCB_HALF_MAX_REPEATE_TIMES;
    if constexpr (sizeof(T) == sizeof(float)) {
        brcbMaxRepeatTimes = BRCB_FLOAT_MAX_REPEATE_TIMES;
    }
    const uint32_t brcbCount = brcbRepeatTime / brcbMaxRepeatTimes;
    const uint32_t tailBrcbRepeatTime = brcbRepeatTime % brcbMaxRepeatTimes;
    uint32_t brcbSrcOffset = 0;
    uint32_t brcbOneBlockTempBufferOffset = 0;
    for (uint32_t i = 0; i < brcbCount; i++) {
        Brcb(brcbOneBlockTempBuffer[brcbOneBlockTempBufferOffset],
            srcLocal[brcbSrcOffset],
            brcbMaxRepeatTimes,
            {1, DEFAULT_REPEAT_STRIDE});
        brcbOneBlockTempBufferOffset += brcbMaxRepeatTimes * oneBlockElementNum * BRCB_ONE_SIZE;
        brcbSrcOffset += brcbMaxRepeatTimes * BRCB_ONE_SIZE;
    }
    if (tailBrcbRepeatTime != 0) {
        Brcb(brcbOneBlockTempBuffer[brcbOneBlockTempBufferOffset],
            srcLocal[brcbSrcOffset],
            tailBrcbRepeatTime,
            {1, DEFAULT_REPEAT_STRIDE});
    }
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource>
__aicore__ inline void TwoDimBroadCastLastDimAlign220(const LocalTensor<T> &dstLocal, const LocalTensor<T> &srcLocal,
    LocalTensor<T> &tmpBuffer, const uint32_t firstDim, const uint32_t numBlocks)
{
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(T);
    BrcbToOneBlock(srcLocal, firstDim, oneBlockElementNum, tmpBuffer);
    SetVectorMask<T, MaskMode::COUNTER>(numBlocks);
    const CopyRepeatParams copyRepeatParams = {1, 0, (uint16_t)(numBlocks / oneBlockElementNum), 1};  // overflow check
    uint32_t CopyCounts = firstDim / MAX_REPEAT_TIMES;
    uint32_t dstOffset = 0;
    uint32_t brcbOneBlockTempBufferOffset = 0;
    for (uint32_t i = 0; i < CopyCounts; i++) {
        Copy<T, false>(dstLocal[dstOffset],
            tmpBuffer[brcbOneBlockTempBufferOffset],
            MASK_PLACEHOLDER,
            MAX_REPEAT_TIMES,
            copyRepeatParams);
        dstOffset += MAX_REPEAT_TIMES * numBlocks;
        brcbOneBlockTempBufferOffset += MAX_REPEAT_TIMES * oneBlockElementNum;
    }
    uint32_t tailsCopyRepeatTimes = firstDim % MAX_REPEAT_TIMES;
    if (tailsCopyRepeatTimes != 0) {
        Copy<T, false>(dstLocal[dstOffset],
            tmpBuffer[brcbOneBlockTempBufferOffset],
            MASK_PLACEHOLDER,
            tailsCopyRepeatTimes,
            copyRepeatParams);
    }
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource>
__aicore__ inline void TwoDimBroadCastLastDimNotAlign220(const LocalTensor<T> &dstLocal, const LocalTensor<T> &srcLocal,
    LocalTensor<T> &tmpBuffer, const uint32_t firstDim, const uint32_t numBlocks)
{
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(T);
    BrcbToOneBlock(srcLocal, firstDim, oneBlockElementNum, tmpBuffer);
    const uint32_t dstRepeatSize = (numBlocks + oneBlockElementNum - 1) / oneBlockElementNum;
    const uint32_t numBlocksAlign = dstRepeatSize * oneBlockElementNum;
    SetVectorMask<T, MaskMode::COUNTER>(numBlocksAlign);
    const CopyRepeatParams copyRepeatParams = {1, 0, (uint16_t)dstRepeatSize, 1};
    uint32_t CopyCounts = firstDim / MAX_REPEAT_TIMES;
    uint32_t dstOffset = 0;
    uint32_t brcbOneBlockTempBufferOffset = 0;
    auto copyTempBuffer = tmpBuffer[firstDim * oneBlockElementNum];
    for (uint32_t i = 0; i < CopyCounts; i++) {
        Copy<T, false>(copyTempBuffer[dstOffset],
            tmpBuffer[brcbOneBlockTempBufferOffset],
            MASK_PLACEHOLDER,
            MAX_REPEAT_TIMES,
            copyRepeatParams);
        dstOffset += MAX_REPEAT_TIMES * numBlocksAlign;
        brcbOneBlockTempBufferOffset += MAX_REPEAT_TIMES * oneBlockElementNum;
    }
    uint32_t tailsCopyRepeatTimes = firstDim % MAX_REPEAT_TIMES;
    if (tailsCopyRepeatTimes != 0) {
        Copy<T, false>(copyTempBuffer[dstOffset],
            tmpBuffer[brcbOneBlockTempBufferOffset],
            MASK_PLACEHOLDER,
            tailsCopyRepeatTimes,
            copyRepeatParams);
    }
    PipeBarrier<PIPE_V>();
    const GatherMaskParams gatherMaskParams = {
        1, (uint16_t)firstDim, (uint16_t)dstRepeatSize, 0};  // uint32 cast to uint16
    uint64_t rsvdCnt = 0;
    GatherMask(dstLocal, copyTempBuffer, GATHER_MASK_PATTERN, true, numBlocks, gatherMaskParams, rsvdCnt);
    SetMaskCount();
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void GetAlignLoopNumbers(const uint32_t firstDim, const uint32_t numBlocks,
    const uint32_t tmpBufferSize, uint32_t &oneRepeatSize, uint32_t &rangeM, uint32_t &tailM)
{
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(T);
    constexpr uint32_t minBrcbTempBufferSize = oneBlockElementNum * oneBlockElementNum;
    constexpr uint32_t minTmpBufferSize = minBrcbTempBufferSize;
    ASCENDC_ASSERT((tmpBufferSize >= minTmpBufferSize), {
        KERNEL_LOG(KERNEL_ERROR,
            "tmpBufferSize can't smaller than minTmpBufferSize, tmpBufferSize is %u, minTmpBufferSize is %u!",
            tmpBufferSize,
            minTmpBufferSize);
    });
    oneRepeatSize = tmpBufferSize / minTmpBufferSize * oneBlockElementNum;
    rangeM = firstDim / oneRepeatSize;
    tailM = firstDim - oneRepeatSize * rangeM;
}

template <typename T>
__aicore__ inline void GetNotAlignLoopNumbers(const uint32_t firstDim, const uint32_t numBlocks,
    const uint32_t tmpBufferSize, uint32_t &oneRepeatSize, uint32_t &rangeM, uint32_t &tailM)
{
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(T);
    constexpr uint32_t minBrcbTempBufferSize = oneBlockElementNum * oneBlockElementNum;
    const uint32_t dstRepeatSize = (numBlocks + oneBlockElementNum - 1) / oneBlockElementNum;
    const uint32_t numBlocksAlign = dstRepeatSize * oneBlockElementNum;
    const uint32_t minCopyTempBufferSize = oneBlockElementNum * numBlocksAlign;
    const uint32_t minTmpBufferSize = minBrcbTempBufferSize + minCopyTempBufferSize;
    ASCENDC_ASSERT((tmpBufferSize >= minTmpBufferSize), {
        KERNEL_LOG(KERNEL_ERROR,
            "tmpBufferSize can't smaller than minTmpBufferSize, tmpBufferSize is %u, minTmpBufferSize is %u!",
            tmpBufferSize,
            minTmpBufferSize);
    });
    oneRepeatSize = tmpBufferSize / minTmpBufferSize * oneBlockElementNum;
    rangeM = firstDim / oneRepeatSize;
    tailM = firstDim - oneRepeatSize * rangeM;
}

template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void TwoDimBroadCastLastDim(const LocalTensor<T> &dstLocal, const LocalTensor<T> &srcLocal,
    const uint32_t dstShape[dim], const uint32_t srcShape[dim], LocalTensor<T> &tmpBuffer)
{
    const auto firstDim = dstShape[0];
    const auto numBlocks = dstShape[axis];
    uint32_t oneRepeatSize = 0;
    uint32_t rangeM = 0;
    uint32_t tailM = 0;
    uint32_t dstLocalOffset = 0;
    uint32_t srcLocalOffset = 0;
    if (numBlocks * sizeof(T) % ONE_BLK_SIZE == 0) {
        GetAlignLoopNumbers<T>(firstDim, numBlocks, tmpBuffer.GetSize(), oneRepeatSize, rangeM, tailM);
        for (uint32_t i = 0; i < rangeM; i++) {
            TwoDimBroadCastLastDimAlign220<T, isReuseSource>(
                dstLocal[dstLocalOffset], srcLocal[srcLocalOffset], tmpBuffer, oneRepeatSize, numBlocks);
            dstLocalOffset += oneRepeatSize * numBlocks;
            srcLocalOffset += oneRepeatSize;
        }
        
        if (tailM != 0) {
            TwoDimBroadCastLastDimAlign220<T, isReuseSource>(
                dstLocal[dstLocalOffset], srcLocal[srcLocalOffset], tmpBuffer, tailM, numBlocks);
        }
    } else {
        GetNotAlignLoopNumbers<T>(firstDim, numBlocks, tmpBuffer.GetSize(), oneRepeatSize, rangeM, tailM);
        for (uint32_t i = 0; i < rangeM; i++) {
            TwoDimBroadCastLastDimNotAlign220<T, isReuseSource>(
                dstLocal[dstLocalOffset], srcLocal[srcLocalOffset], tmpBuffer, oneRepeatSize, numBlocks);
            dstLocalOffset += oneRepeatSize * numBlocks;
            srcLocalOffset += oneRepeatSize;
        }

        if (tailM != 0) {
            TwoDimBroadCastLastDimNotAlign220<T, isReuseSource>(
                dstLocal[dstLocalOffset], srcLocal[srcLocalOffset], tmpBuffer, tailM, numBlocks);
        }
    }
}

template <typename T>
__aicore__ inline void NoBroad(const LocalTensor<T> &dstLocal, const LocalTensor<T> &srcLocal, const uint32_t size)
{
    SetVectorMask<T, MaskMode::COUNTER>(size);
    Copy<T, false>(dstLocal, srcLocal, MASK_PLACEHOLDER, 1, {1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
}

}  // namespace AscendC
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_V220_IMPL_H__
#endif
