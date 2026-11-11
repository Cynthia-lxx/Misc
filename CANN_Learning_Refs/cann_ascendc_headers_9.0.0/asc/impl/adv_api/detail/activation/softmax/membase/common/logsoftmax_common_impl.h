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
 * \file logsoftmax_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/membase/common/logsoftmax_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/logsoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGSOFTMAX_COMMON_IMPL_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_MEMBASE_COMMON_LOGSOFTMAX_COMMON_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_MEMBASE_COMMON_LOGSOFTMAX_COMMON_IMPL_H
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "../v220/softmax_common_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "../v200/softmax_common_impl.h"
#endif

namespace AscendC {
constexpr float SCALAR_NATURE_LOG_10 = 0.4342944819;
 __aicore__ inline void GenericLogNZImpl(const LocalTensor<float>& dst, const LocalTensor<float>& src,
     const uint32_t originalSrcM, const uint32_t srcK)
{
    if (srcK < SOFTMAX_SUB_DIV_ROW_COLUMN_SIZE) {
        const uint8_t blockStride = srcK / FLOAT_NUM_PER_BLK;
        SetMaskCount();
        SetVectorMask<float>(0, originalSrcM * FLOAT_NUM_PER_BLK);
        for (uint8_t j = 0; j < blockStride; j++) {
            Ln<float, false>(dst[j * FLOAT_NUM_PER_BLK], src[j * FLOAT_NUM_PER_BLK], MASK_PLACEHOLDER, 1,
                { blockStride, blockStride, static_cast<uint8_t>(srcK), static_cast<uint8_t>(srcK)});
            PipeBarrier<PIPE_V>();
            Muls<float, false>(dst[j * FLOAT_NUM_PER_BLK], src[j * FLOAT_NUM_PER_BLK],
                static_cast<float>(SCALAR_NATURE_LOG_10), MASK_PLACEHOLDER, 1,
                { blockStride, blockStride, static_cast<uint8_t>(srcK), static_cast<uint8_t>(srcK)});
            PipeBarrier<PIPE_V>();
        }
    } else {
        SetMaskCount();
        SetVectorMask<float>(0, srcK);
        for (int j = 0; j < originalSrcM; j++) {
            Ln<float, false>(dst[j * srcK], src[j * srcK], MASK_PLACEHOLDER, 1,
                { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
            PipeBarrier<PIPE_V>();
            Muls<float, false>(dst[j * srcK], src[j * srcK], static_cast<float>(SCALAR_NATURE_LOG_10),
                MASK_PLACEHOLDER, 1, { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
            PipeBarrier<PIPE_V>();
        }
    }
    SetMaskNorm();
    ResetMask();
}

__aicore__ inline void LogSoftMaxGenericNZImpl(const LocalTensor<float>& dst, const LocalTensor<float>& sumTensor,
    const LocalTensor<float>& maxTensor, const LocalTensor<float>& src, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, const uint32_t& offset1, const uint32_t& offset2,
    const uint32_t& splitCount, const ReduceLastND& reduceParam)
{
    const UnaryRepeatParams unaryParams;
    const uint64_t splitOffset = tiling.splitM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    const uint64_t splitNZBlockCount = tiling.srcK / SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    const uint64_t lastSplitNZBlockOffset = splitOffset * (splitNZBlockCount - 1);
    const uint64_t lastBlockMaskLen = reduceParam.originalSrcK % SOFTMAX_SHAPE_NZ_BASIC_COUNT != 0 ?
        reduceParam.originalSrcK % SOFTMAX_SHAPE_NZ_BASIC_COUNT :
        SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    const uint64_t copyBlockCount = splitCount / SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    GenericLogNZImpl(dst, dst, reduceParam.originalSrcM, tiling.srcK);
}

__aicore__ inline void LogSoftMaxGenericNZReduceMaxImpl(const LocalTensor<float>& tmpBuffer0,
    const LocalTensor<float>& tmpBuffer1, const LocalTensor<half>& maxTensor, const uint32_t& offset2,
    const uint32_t& splitCount, uint64_t mask[2], const ReduceLastND& reduceParam)
{
    ReduceMaxLastNZImpl(tmpBuffer1, tmpBuffer0, mask, reduceParam);
    PipeBarrier<PIPE_V>();

    SetMaskCount();
    SetVectorMask<float, MaskMode::COUNTER>(0, splitCount);
    Cast<half, float, false>(maxTensor[offset2], tmpBuffer1, FLOAT2HALF_ROUND_MODE, MASK_PLACEHOLDER, 1,
        { 1, 1, HALF_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
}

__aicore__ inline void LogSoftMaxGenericNZSubImpl(const uint32_t& splitNZBlockCount,
    const LocalTensor<float>& tmpBuffer0, const LocalTensor<float>& tmpBuffer1, const uint32_t& splitOffset,
    const uint32_t& lastSplitNZBlockOffset, uint64_t mask[2],
    const uint32_t& lastBlockMaskLen, const uint32_t& splitCount)
{
    for (uint32_t j = 0; j < splitNZBlockCount - 1; j++) {
        Sub<float, false>(tmpBuffer0[splitOffset * j], tmpBuffer0[splitOffset * j], tmpBuffer1, MASK_PLACEHOLDER, 1,
            { 1, 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    }
    SetMaskNorm();
    ResetMask();
    BinaryComputeWithSpecialMask(tmpBuffer0[lastSplitNZBlockOffset], tmpBuffer0[lastSplitNZBlockOffset], tmpBuffer1,
        mask, lastBlockMaskLen, splitCount, Sub<float>);

    PipeBarrier<PIPE_V>();
}

__aicore__ inline void LogSoftMaxGenericNZReduceSumImpl(const LocalTensor<float>& tmpBuffer0,
    const LocalTensor<float>& tmpBuffer1, const LocalTensor<half>& sumTensor, const uint32_t& offset2,
    const uint32_t& splitCount, uint64_t mask[2], const ReduceLastND& reduceParam)
{
    ReduceSumLastNZImpl(tmpBuffer1, tmpBuffer0, mask, reduceParam);
    PipeBarrier<PIPE_V>();

    SetMaskCount();
    SetVectorMask<float, MaskMode::COUNTER>(0, splitCount);
    Cast<half, float, false>(sumTensor[offset2], tmpBuffer1, FLOAT2HALF_ROUND_MODE, MASK_PLACEHOLDER, 1,
        { 1, 1, HALF_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void LogSoftMaxGenericNZDivImpl(const uint32_t& splitNZBlockCount,
    const LocalTensor<float>& tmpBuffer0, const LocalTensor<float>& tmpBuffer1, const uint32_t& splitOffset,
    const uint32_t& lastSplitNZBlockOffset, uint64_t mask[2], const uint32_t& lastBlockMaskLen,
    const uint32_t& splitCount)
{
    for (uint32_t j = 0; j < splitNZBlockCount - 1; j++) {
        Div<float, false>(tmpBuffer0[splitOffset * j], tmpBuffer0[splitOffset * j], tmpBuffer1, MASK_PLACEHOLDER, 1,
            { 1, 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    }
    SetMaskNorm();
    ResetMask();
    BinaryComputeWithSpecialMask(tmpBuffer0[lastSplitNZBlockOffset], tmpBuffer0[lastSplitNZBlockOffset], tmpBuffer1,
        mask, lastBlockMaskLen, splitCount, Div<float>);
}

__aicore__ inline void LogSoftMaxGenericNZLogImpl(const uint32_t& splitNZBlockCount,
    const LocalTensor<float>& tmpBuffer0, const LocalTensor<half>& dst, const uint32_t& splitOffset,
    const uint32_t& splitCount, const SoftMaxTiling& tiling, const uint32_t& offset1)
{
    SetMaskCount();
    SetVectorMask<float, MaskMode::COUNTER>(0, splitCount);
    PipeBarrier<PIPE_V>();

    for (uint32_t j = 0; j < splitNZBlockCount; j++) {
        Ln<float, false>(tmpBuffer0[splitOffset * j], tmpBuffer0[splitOffset * j], MASK_PLACEHOLDER, 1,
            { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
        PipeBarrier<PIPE_V>();
        Muls<float, false>(tmpBuffer0[splitOffset * j], tmpBuffer0[splitOffset * j],
            static_cast<float>(SCALAR_NATURE_LOG_10), MASK_PLACEHOLDER, 1,
            { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
        PipeBarrier<PIPE_V>();
        Cast<half, float, false>(dst[offset1 + j * tiling.srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT],
            tmpBuffer0[splitOffset * j], FLOAT2HALF_ROUND_MODE, MASK_PLACEHOLDER, 1,
            { 1, 1, HALF_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    }
    SetMaskNorm();
    ResetMask();
}

__aicore__ inline void LogSoftMaxGenericNZExpImpl(const uint32_t& splitNZBlockCount,
    const LocalTensor<float>& tmpBuffer0, const LocalTensor<float>& tmpBuffer1, const uint32_t& splitOffset,
    const uint32_t& lastSplitNZBlockOffset, uint64_t mask[2], const uint32_t& lastBlockMaskLen,
    const uint32_t& splitCount)
{
    SetMaskCount();
    SetVectorMask<float, MaskMode::COUNTER>(0, splitCount);

    for (uint32_t j = 0; j < splitNZBlockCount - 1; j++) {
        Exp<float, false>(tmpBuffer0[splitOffset * j], tmpBuffer0[splitOffset * j], MASK_PLACEHOLDER, 1,
            { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE });
    }
    SetMaskNorm();
    ResetMask();
    UnaryComputeWithSpecialMask(tmpBuffer0[lastSplitNZBlockOffset], tmpBuffer0[lastSplitNZBlockOffset], mask,
        lastBlockMaskLen, splitCount, Exp<float>);

    PipeBarrier<PIPE_V>();
}

__aicore__ inline void LogSoftMaxGenericNZImpl(const LocalTensor<half>& dst, const LocalTensor<half>& sumTensor,
    const LocalTensor<half>& maxTensor, const LocalTensor<half>& src, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, uint64_t mask[2], const uint32_t& offset1, const uint32_t& offset2,
    const uint32_t& splitCount, const ReduceLastND& reduceParam)
{
    LocalTensor<float> tmpBuffer0 = workLocal;
    LocalTensor<float> tmpBuffer1 = workLocal[tiling.splitSize];
    const uint64_t splitOffset = tiling.splitM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    const uint64_t splitNZBlockCount = tiling.srcK / SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    const uint64_t lastSplitNZBlockOffset = splitOffset * (splitNZBlockCount - 1);
    const uint64_t lastBlockMaskLen = reduceParam.originalSrcK % SOFTMAX_SHAPE_NZ_BASIC_COUNT != 0 ?
        reduceParam.originalSrcK % SOFTMAX_SHAPE_NZ_BASIC_COUNT :
        SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    SetMaskCount();
    SetVectorMask<float, MaskMode::COUNTER>(0, splitCount);
    for (uint64_t j = 0; j < splitNZBlockCount; j++) {
        Cast<float, half, false>(tmpBuffer0[splitOffset * j],
            src[offset1 + j * tiling.srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT], RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
            { 1, 1, DEFAULT_REPEAT_STRIDE, HALF_REPEAT_STRIDE });
    }
    SetMaskNorm();
    ResetMask();

    PipeBarrier<PIPE_V>();
    LogSoftMaxGenericNZReduceMaxImpl(tmpBuffer0, tmpBuffer1, maxTensor, offset2, splitCount, mask, reduceParam);

    LogSoftMaxGenericNZSubImpl(splitNZBlockCount, tmpBuffer0, tmpBuffer1, splitOffset, lastSplitNZBlockOffset,
        mask, lastBlockMaskLen, splitCount);

    LogSoftMaxGenericNZExpImpl(splitNZBlockCount, tmpBuffer0, tmpBuffer1, splitOffset, lastSplitNZBlockOffset,
        mask, lastBlockMaskLen, splitCount);

    LogSoftMaxGenericNZReduceSumImpl(tmpBuffer0, tmpBuffer1, sumTensor, offset2, splitCount, mask, reduceParam);

    LogSoftMaxGenericNZDivImpl(splitNZBlockCount, tmpBuffer0, tmpBuffer1, splitOffset, lastSplitNZBlockOffset,
        mask, lastBlockMaskLen, splitCount);

    LogSoftMaxGenericNZLogImpl(splitNZBlockCount, tmpBuffer0, dst, splitCount, splitOffset, tiling, offset1);
}

__aicore__ inline bool LogSoftMaxTilingFunc(const uint32_t workLocalSize, const LastAxisShapeND& ndinfo,
    AscendC::tiling::LogSoftMaxTiling& softmaxTiling, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    bool isDataFormatNZ = false)
{
    ASCENDC_ASSERT((dataTypeSize2 != 0),
                   { KERNEL_LOG(KERNEL_ERROR, "logsoftmax maxTensor&sumTensor type is zero."); });
    const uint32_t elementNumPerBlk = ONE_BLK_SIZE / dataTypeSize2;
    softmaxTiling.srcM = ndinfo.m;
    softmaxTiling.srcK = ndinfo.k;
    softmaxTiling.srcSize = ndinfo.m * ndinfo.k;
    softmaxTiling.outMaxM = ndinfo.m;
    softmaxTiling.outMaxK = elementNumPerBlk;
    softmaxTiling.outMaxSize = ndinfo.m * elementNumPerBlk;
    if (isDataFormatNZ) {
        softmaxTiling.reduceM = workLocalSize / (SOFTMAX_SHAPE_NZ_BASIC_COUNT + ndinfo.k);
    } else {
        softmaxTiling.reduceM = CalculateNDSplitM(workLocalSize, dataTypeSize1, elementNumPerBlk, ndinfo);
    }

    if (softmaxTiling.reduceM < ndinfo.m && softmaxTiling.reduceM > SOFTMAX_BASIC_TILE_NUM) {
        softmaxTiling.reduceM = softmaxTiling.reduceM / SOFTMAX_BASIC_TILE_NUM * SOFTMAX_BASIC_TILE_NUM;
    }
    softmaxTiling.reduceM = softmaxTiling.reduceM < ndinfo.m ? softmaxTiling.reduceM : ndinfo.m;
    softmaxTiling.reduceK = elementNumPerBlk;
    softmaxTiling.reduceSize = softmaxTiling.reduceM * elementNumPerBlk;

    softmaxTiling.splitM = softmaxTiling.reduceM;
    softmaxTiling.splitK = ndinfo.k;
    softmaxTiling.splitSize = softmaxTiling.reduceM * ndinfo.k;
    ASCENDC_ASSERT((softmaxTiling.reduceM > 0),
                   { KERNEL_LOG(KERNEL_ERROR, "softmax need min tmpbuffer is not enough."); });
    softmaxTiling.rangeM = ndinfo.m / softmaxTiling.reduceM;
    softmaxTiling.tailM = ndinfo.m % softmaxTiling.reduceM;

    softmaxTiling.tailSplitSize = softmaxTiling.tailM * ndinfo.k;
    softmaxTiling.tailReduceSize = softmaxTiling.tailM * elementNumPerBlk;
    return true;
}

__aicore__ inline void GenericLogNDImpl(const LocalTensor<float>& dst, const LocalTensor<float>& src,
    const uint32_t originalSrcM, const uint32_t srcK)
{
    if (srcK < SOFTMAX_SUB_DIV_ROW_COLUMN_SIZE) {
        const uint8_t blockStride = srcK / FLOAT_NUM_PER_BLK;
        SetMaskCount();
        SetVectorMask<float>(0, originalSrcM * FLOAT_NUM_PER_BLK);
        for (uint8_t j = 0; j < blockStride; j++) {
            Ln<float, false>(dst[j * FLOAT_NUM_PER_BLK], src[j * FLOAT_NUM_PER_BLK], MASK_PLACEHOLDER, 1,
                { blockStride, blockStride, static_cast<uint8_t>(srcK), static_cast<uint8_t>(srcK)});
            PipeBarrier<PIPE_V>();
            Muls<float, false>(dst[j * FLOAT_NUM_PER_BLK], src[j * FLOAT_NUM_PER_BLK],
                static_cast<float>(SCALAR_NATURE_LOG_10), MASK_PLACEHOLDER, 1,
                { blockStride, blockStride, static_cast<uint8_t>(srcK), static_cast<uint8_t>(srcK)});
            PipeBarrier<PIPE_V>();
        }
    } else {
        SetMaskCount();
        SetVectorMask<float>(0, srcK);
        for (int j = 0; j < originalSrcM; j++) {
            Ln<float, false>(dst[j * srcK], src[j * srcK], MASK_PLACEHOLDER, 1,
                { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
            PipeBarrier<PIPE_V>();
            Muls<float, false>(dst[j * srcK], src[j * srcK], static_cast<float>(SCALAR_NATURE_LOG_10), 1,
                MASK_PLACEHOLDER, { 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
            PipeBarrier<PIPE_V>();
        }
    }
    SetMaskNorm();
    ResetMask();
}

__aicore__ inline void LogSoftMaxGenericNDImpl(const LocalTensor<float>& dst, const LocalTensor<float>& sumTensor,
    const LocalTensor<float>& maxTensor, const LocalTensor<float>& src, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, const uint32_t& offset1, const uint32_t& offset2, const uint32_t& splitSize,
    const uint32_t& reduceSize, const ReduceLastND& reduceParam)
{
    const LocalTensor<float>& tmpBuffer0 = workLocal; // need splitM * 64
    const UnaryRepeatParams unaryParams;

    NewReduceMaxLastNDImpl(maxTensor[offset2], src[offset1], tmpBuffer0, reduceParam);
    PipeBarrier<PIPE_V>();

    GenericSubNDImpl(dst[offset1], src[offset1], maxTensor[offset2], reduceParam.originalSrcM, tiling.srcK,
        tiling.reduceK);

    PipeBarrier<PIPE_V>();
    Exp(dst[offset1], dst[offset1], splitSize);
    PipeBarrier<PIPE_V>();
    NewReduceSumLastNDImpl(sumTensor[offset2], dst[offset1], tmpBuffer0, reduceParam);
    PipeBarrier<PIPE_V>();

    GenericDivNDImpl(dst[offset1], dst[offset1], sumTensor[offset2], reduceParam.originalSrcM, tiling.srcK,
        tiling.reduceK);
    PipeBarrier<PIPE_V>();

    GenericLogNDImpl(dst[offset1], dst[offset1], reduceParam.originalSrcM, tiling.srcK);
}

__aicore__ inline void LogSoftMaxGenericNDImpl(const LocalTensor<half>& dst, const LocalTensor<half>& sumTensor,
    const LocalTensor<half>& maxTensor, const LocalTensor<half>& src, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, const uint32_t& offset1, const uint32_t& offset2, const uint32_t& splitSize,
    const uint32_t& reduceSize, const ReduceLastND& reduceParam)
{
    const LocalTensor<float>& tmpBuffer0 = workLocal;
    const LocalTensor<float>& tmpBuffer2 = workLocal[tiling.splitSize];
    const LocalTensor<float>& tmpBuffer3 = workLocal[tiling.splitSize + tiling.reduceSize]; // need splitM * 64
    const UnaryRepeatParams unaryParams;
    Cast(tmpBuffer0, src[offset1], RoundMode::CAST_NONE, splitSize);
    PipeBarrier<PIPE_V>();
    NewReduceMaxLastNDImpl(tmpBuffer2, tmpBuffer0, tmpBuffer3, reduceParam);

    PipeBarrier<PIPE_V>();
    Cast(maxTensor[offset2], tmpBuffer2, FLOAT2HALF_ROUND_MODE, reduceSize);
    PipeBarrier<PIPE_V>();

    GenericSubNDImpl(tmpBuffer0, tmpBuffer0, tmpBuffer2, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);

    PipeBarrier<PIPE_V>();
    Exp(tmpBuffer0, tmpBuffer0, splitSize);
    PipeBarrier<PIPE_V>();

    NewReduceSumLastNDImpl(tmpBuffer2, tmpBuffer0, tmpBuffer3, reduceParam);
    PipeBarrier<PIPE_V>();

    Cast(sumTensor[offset2], tmpBuffer2, FLOAT2HALF_ROUND_MODE, reduceSize);
    PipeBarrier<PIPE_V>();

    GenericDivNDImpl(tmpBuffer0, tmpBuffer0, tmpBuffer2, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
    PipeBarrier<PIPE_V>();

    GenericLogNDImpl(tmpBuffer0, tmpBuffer0, reduceParam.originalSrcM, tiling.srcK);

    Cast(dst[offset1], tmpBuffer0, FLOAT2HALF_ROUND_MODE, splitSize);
}

__aicore__ inline void LogSoftMaxGenericNDImpl(const LocalTensor<half>& dst, const LocalTensor<float>& sumTensor,
    const LocalTensor<float>& maxTensor, const LocalTensor<half>& src, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, const uint32_t& offset1, const uint32_t& offset2, const uint32_t& splitSize,
    const ReduceLastND& reduceParam)
{
    const LocalTensor<float>& tmpBuffer0 = workLocal;
    const LocalTensor<float>& tmpBuffer1 = workLocal[tiling.splitSize]; // need splitM * 64
    const UnaryRepeatParams unaryParams;

    Cast(tmpBuffer0, src[offset1], RoundMode::CAST_NONE, splitSize);
    PipeBarrier<PIPE_V>();
    NewReduceMaxLastNDImpl(maxTensor[offset2], tmpBuffer0, tmpBuffer1, reduceParam);
    PipeBarrier<PIPE_V>();

    GenericSubNDImpl(tmpBuffer0, tmpBuffer0, maxTensor[offset2], reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);

    PipeBarrier<PIPE_V>();
    Exp(tmpBuffer0, tmpBuffer0, splitSize);

    PipeBarrier<PIPE_V>();
    NewReduceSumLastNDImpl(sumTensor[offset2], tmpBuffer0, tmpBuffer1, reduceParam);
    PipeBarrier<PIPE_V>();

    GenericDivNDImpl(tmpBuffer0, tmpBuffer0, sumTensor[offset2], reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);

    PipeBarrier<PIPE_V>();

    GenericLogNDImpl(tmpBuffer0, tmpBuffer0, reduceParam.originalSrcM, tiling.srcK);

    Cast(dst[offset1], tmpBuffer0, FLOAT2HALF_ROUND_MODE, splitSize);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LogSoftMaxNZImpl(const LocalTensor<T>& dst, const LocalTensor<T>& sumTensor,
    const LocalTensor<T>& maxTensor, const LocalTensor<T>& src, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    SetMaskNorm();
    ResetMask();
    ReduceLastND reduceParam = { tiling.splitM, originalSrcShape.k, tiling.splitM,
        tiling.splitK, tiling.reduceM, tiling.reduceK };
    uint32_t offset1 = 0;
    uint32_t offset2 = 0;
    uint32_t splitSize = tiling.splitSize;
    uint32_t reduceSize = tiling.reduceSize;
    PipeBarrier<PIPE_V>();
    for (uint32_t i = 0; i <= tiling.rangeM; i++) {
        LogSoftMaxGenericNZImpl(dst, sumTensor, maxTensor, src, workLocal, tiling, offset1, offset2,
            splitSize, reduceParam);
        offset1 += tiling.splitSize;
        offset2 += tiling.reduceSize;
        if (i == (tiling.rangeM - 1)) {
            if (tiling.tailM == 0) {
                break;
            }
            offset2 = tiling.rangeM * tiling.reduceSize;
            offset1 = tiling.rangeM * tiling.splitSize;
            splitSize = tiling.tailSplitSize;
            reduceSize = tiling.tailReduceSize;
            reduceParam.originalSrcM = tiling.tailM;
            reduceParam.srcM = tiling.tailM;
            reduceParam.dstM = tiling.tailM;
            PipeBarrier<PIPE_V>();
        }
    }
}

__aicore__ inline void LogSoftMaxNZImpl(const LocalTensor<half>& dst, const LocalTensor<half>& sumTensor,
    const LocalTensor<half>& maxTensor, const LocalTensor<half>& src, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    SetMaskNorm();
    ResetMask();
    const ReduceLastND& mainReduceParam = { tiling.splitM, originalSrcShape.k, tiling.splitM,
        tiling.splitK, tiling.splitM,      SOFTMAX_SHAPE_NZ_BASIC_COUNT };
    const ReduceLastND& tailReduceParam = { tiling.tailM,  originalSrcShape.k, tiling.splitM,
        tiling.splitK, tiling.splitM,      SOFTMAX_SHAPE_NZ_BASIC_COUNT };
    uint32_t lastBlockMaskLen = originalSrcShape.k % SOFTMAX_SHAPE_NZ_BASIC_COUNT != 0 ?
        originalSrcShape.k % SOFTMAX_SHAPE_NZ_BASIC_COUNT :
        SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint64_t mask[2] = { 0, 0 };
    CreateSpecialFormatMask(mask[0], lastBlockMaskLen, FLOAT_REPEAT_SIZE / SOFTMAX_SHAPE_NZ_BASIC_COUNT);

    uint32_t offset1 = 0;
    uint32_t offset2 = 0;
    uint32_t splitCount = tiling.splitM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    for (uint32_t i = 0; i < tiling.rangeM; i++) {
        offset1 = i * splitCount;
        offset2 = i * tiling.reduceSize;
        LogSoftMaxGenericNZImpl(dst, sumTensor, maxTensor, src, workLocal, tiling, mask, offset1, offset2, splitCount,
            mainReduceParam);
    }
    PipeBarrier<PIPE_V>();
    if (tiling.tailM != 0) {
        offset1 = tiling.rangeM * splitCount;
        offset2 = tiling.rangeM * tiling.reduceSize;
        splitCount = tiling.tailM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
        LogSoftMaxGenericNZImpl(dst, sumTensor, maxTensor, src, workLocal, tiling, mask, offset1, offset2, splitCount,
            tailReduceParam);
    }
}

__aicore__ inline void LogSoftMaxNDImpl(const LocalTensor<half>& dst, const LocalTensor<float>& sumTensor,
    const LocalTensor<float>& maxTensor, const LocalTensor<half>& src, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    SetMaskNorm();
    ResetMask();
    PipeBarrier<PIPE_V>();
    ReduceLastND reduceParam = { tiling.splitM, originalSrcShape.k, tiling.splitM,
        tiling.splitK, tiling.reduceM, tiling.reduceK };
    uint32_t offset1 = 0;
    uint32_t offset2 = 0;
    uint32_t splitSize = tiling.splitSize;
    uint32_t reduceSize = tiling.reduceSize;
    for (uint32_t i = 0; i < tiling.rangeM; i++) {
        LogSoftMaxGenericNDImpl(dst, sumTensor, maxTensor, src, workLocal, tiling, offset1, offset2, splitSize,
            reduceParam);
        offset1 += tiling.splitSize;
        offset2 += tiling.reduceSize;
        if (i == (tiling.rangeM - 1)) {
            if (tiling.tailM == 0) {
                break;
            }
            offset2 = tiling.rangeM * tiling.reduceSize;
            offset1 = tiling.rangeM * tiling.splitSize;
            splitSize = tiling.tailSplitSize;
            reduceSize = tiling.tailReduceSize;
            reduceParam.originalSrcM = tiling.tailM;
            reduceParam.srcM = tiling.tailM;
            reduceParam.dstM = tiling.tailM;
            PipeBarrier<PIPE_V>();
        }
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LogSoftMaxNDImpl(const LocalTensor<T>& dst, const LocalTensor<T>& sumTensor,
    const LocalTensor<T>& maxTensor, const LocalTensor<T>& src, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    SetMaskNorm();
    ResetMask();
    ReduceLastND reduceParam = { tiling.splitM, originalSrcShape.k, tiling.splitM,
        tiling.splitK, tiling.reduceM, tiling.reduceK };
    uint32_t offset1 = 0;
    uint32_t offset2 = 0;
    uint32_t splitSize = tiling.splitSize;
    uint32_t reduceSize = tiling.reduceSize;
    PipeBarrier<PIPE_V>();
    for (uint32_t i = 0; i <= tiling.rangeM; i++) {
        LogSoftMaxGenericNDImpl(dst, sumTensor, maxTensor, src, workLocal, tiling, offset1, offset2, splitSize,
            reduceSize, reduceParam);
        offset1 += tiling.splitSize;
        offset2 += tiling.reduceSize;
        if (i == (tiling.rangeM - 1)) {
            if (tiling.tailM == 0) {
                break;
            }
            offset2 = tiling.rangeM * tiling.reduceSize;
            offset1 = tiling.rangeM * tiling.splitSize;
            splitSize = tiling.tailSplitSize;
            reduceSize = tiling.tailReduceSize;
            reduceParam.originalSrcM = tiling.tailM;
            reduceParam.srcM = tiling.tailM;
            reduceParam.dstM = tiling.tailM;
            PipeBarrier<PIPE_V>();
        }
    }
}
}
#endif // IMPL_ACTIVATION_SOFTMAX_MEMBASE_COMMON_LOGSOFTMAX_COMMON_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGSOFTMAX_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGSOFTMAX_COMMON_IMPL_H__
#endif
