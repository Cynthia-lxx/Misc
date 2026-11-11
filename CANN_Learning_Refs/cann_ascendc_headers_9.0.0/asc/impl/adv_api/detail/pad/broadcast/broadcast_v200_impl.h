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
#pragma message("impl/adv_api/detail/pad/broadcast/broadcast_v200_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/broadcast.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_V200_IMPL_H__
#endif

#ifndef IMPL_PAD_BROADCAST_BROADCAST_V200_IMPL_H
#define IMPL_PAD_BROADCAST_BROADCAST_V200_IMPL_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "broadcast_common_utils.h"

namespace AscendC {
template <typename T>
__aicore__ inline void GetAlignLoopNumbers200(const uint32_t firstDim, const uint32_t numBlocks, uint32_t tmpBufferSize,
    uint32_t &oneRepeatSize, uint32_t &rangeM, uint32_t &tailM)
{
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(T);
    tmpBufferSize -= oneBlockElementNum;
    ASCENDC_ASSERT(
        (tmpBufferSize > 0), { KERNEL_LOG(KERNEL_ERROR, "tmpBufferSize should bigger than oneBlockElementNum!"); });
    const uint32_t minTmpBufferSize = oneBlockElementNum * ((numBlocks + ONE_VOR_BLOCK_DIM - 1) / ONE_VOR_BLOCK_DIM);
    ASCENDC_ASSERT((tmpBufferSize > minTmpBufferSize), {
        KERNEL_LOG(
            KERNEL_ERROR, "tmpBufferSize %u should bigger than minTmpBufferSize %u!", tmpBufferSize, minTmpBufferSize);
    });
    oneRepeatSize = tmpBufferSize / minTmpBufferSize * oneBlockElementNum;
    rangeM = firstDim / oneRepeatSize;
    tailM = firstDim - oneRepeatSize * rangeM;
}

template <typename T>
__aicore__ inline void BroadCastTranse(
    const LocalTensor<T> &dstLocal, const LocalTensor<T> &srcLocal, const uint32_t firstDim, const uint32_t numBlocks)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    TransposeParamsExt param = {1, (uint16_t)numBlocks, 1, (uint16_t)firstDim, TransposeType::TRANSPOSE_NCHW2NHWC};
    Transpose(dstLocal, srcLocal, sharedTmpBuffer, param);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void TwoDimBroadCastLastDimAlign200(const LocalTensor<T> &dstLocal, const LocalTensor<T> &srcLocal,
    const LocalTensor<T> &zeroTemp, const LocalTensor<T> &tmpBuffer, const uint32_t firstDim, const uint32_t numBlocks)
{
    TwoDimBroadCastDimAlign<T, isReuseSource>(tmpBuffer, srcLocal, zeroTemp, numBlocks, firstDim);
    BroadCastTranse<T>(dstLocal, tmpBuffer, firstDim, numBlocks);
    PipeBarrier<PIPE_V>();
}

template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void TwoDimBroadCastLastDim(const LocalTensor<T> &dstLocal, const LocalTensor<T> &srcLocal,
    const uint32_t dstShape[dim], const uint32_t srcShape[dim], LocalTensor<T> &tmpBuffer)
{
    const auto firstDim = dstShape[0];
    const auto numBlocks = dstShape[axis];
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(T);
    constexpr uint32_t FIRST_DIM_LOOP_LIMITE = MAX_REPEAT_NUM * oneBlockElementNum;

    auto zeroTemp = tmpBuffer;
    const uint32_t blockSize = ONE_BLK_SIZE / sizeof(T);
    Duplicate(zeroTemp.template ReinterpretCast<uint16_t>(), (uint16_t)0, ONE_BLK_SIZE / sizeof(uint16_t));
    PipeBarrier<PIPE_V>();

    if (firstDim >= FIRST_DIM_LOOP_LIMITE) {
        LoopBroadCast<T>(tmpBuffer[blockSize], srcLocal, zeroTemp, firstDim, numBlocks);
        BroadCastTranse<T>(dstLocal, tmpBuffer[blockSize], firstDim, numBlocks);
        PipeBarrier<PIPE_V>();
        return;
    }

    if (firstDim * sizeof(T) % ONE_BLK_SIZE == 0) {
        uint32_t oneRepeatSize = 0;
        uint32_t rangeM = 0;
        uint32_t tailM = 0;
        uint32_t dstLocalOffset = 0;
        uint32_t srcLocalOffset = 0;
        GetAlignLoopNumbers200<T>(firstDim, numBlocks, tmpBuffer.GetSize(), oneRepeatSize, rangeM, tailM);
        for (uint32_t i = 0; i < rangeM; i++) {
            TwoDimBroadCastLastDimAlign200<T, isReuseSource>(dstLocal[dstLocalOffset],
                srcLocal[srcLocalOffset],
                zeroTemp,
                tmpBuffer[blockSize],
                oneRepeatSize,
                numBlocks);
            dstLocalOffset += oneRepeatSize * numBlocks;
            srcLocalOffset += oneRepeatSize;
        }

        if (tailM != 0) {
            TwoDimBroadCastLastDimAlign200<T, isReuseSource>(
                dstLocal[dstLocalOffset], srcLocal[srcLocalOffset], zeroTemp, tmpBuffer[blockSize], tailM, numBlocks);
        }
    } else {
        KERNEL_LOG(KERNEL_ERROR, "Non-alignment is not supported.");
    }
}

template <typename T>
__aicore__ inline void NoBroad(const LocalTensor<T> &dstLocal, const LocalTensor<T> &srcLocal, const uint32_t size)
{
    SetVectorMask<T, MaskMode::COUNTER>(size);
    DataCopy<T>(dstLocal, srcLocal, size);
    PipeBarrier<PIPE_V>();
}

}  // namespace AscendC

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_V200_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_V200_IMPL_H__
#endif
