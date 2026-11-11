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
 * \file groupnorm_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/normalization/groupnorm/groupnorm_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_GROUPNORM_GROUPNORM_COMMON_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_GROUPNORM_GROUPNORM_COMMON_IMPL_H
#define IMPL_NORMALIZATION_GROUPNORM_GROUPNORM_COMMON_IMPL_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/normalization/groupnorm/groupnorm_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
    namespace {
        constexpr uint32_t GROUPNORM_MASK_MAX_VAL = 64;
        constexpr uint32_t GROUPNORM_MASK_SMALLEST_VAL = 8;
        constexpr uint32_t GROUPNORM_MASK_STEP_VAL = 8;
        constexpr uint32_t GROUPNORM_ONE_BLK_SIZE = 8;
    } // namespace

template <typename T> struct GroupNormParams
{
    __aicore__ GroupNormParams(){};
    LocalTensor<T> tempTensorA;
    LocalTensor<T> tempTensorB;
    LocalTensor<T> tempTensorC;
    LocalTensor<T> meanTmpTensor;
    LocalTensor<T> varianceTmpTensor;
};

__aicore__ inline uint32_t GetGroupNormWholeReduceMask1(const GroupNormTiling& tiling)
{
    uint32_t mask1{0};
    if (tiling.dhwAlignSize > GROUPNORM_MASK_MAX_VAL) {
        mask1 = GROUPNORM_MASK_MAX_VAL;
        while (mask1 != 0 && tiling.dhwAlignSize % mask1 != 0) {
            mask1 -= GROUPNORM_MASK_STEP_VAL;
        }
        return mask1;
    }
    return tiling.dhwAlignSize;
}

__aicore__ inline void GetGroupNormOutputMean(const LocalTensor<float>& x_in,
    const LocalTensor<float>& tmp, const LocalTensor<float>& mean,
    const GroupNormTiling& tiling, const int32_t loopCount, const int32_t mvOffset)
{
    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint32_t buffIndex = i * tiling.dhwAlignSize;
        ReduceSum<float>(mean[i + loopCount * tiling.bsCurLength + mvOffset], 
            x_in[buffIndex], tmp[buffIndex], tiling.dhwAlignSize);
        PipeBarrier<PIPE_V>();
    }  

    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint64_t maskOffset = static_cast<uint64_t>(1) << (i + loopCount * tiling.bsCurLength + mvOffset);
        uint64_t mask[1] = {maskOffset};
        Muls(mean, mean, tiling.factor, mask, 1, {1, 1, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL});
    }
    
    // mean will be used to GetValue() to get scalar value
    auto eventIdVToS = GetTPipePtr()->FetchEventID(HardEvent::V_S);
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
}

__aicore__ inline void GetGroupNormOutputVar(const LocalTensor<float>& x_in,
    const LocalTensor<float>& tmp1, const LocalTensor<float>& tmp2,
    const LocalTensor<float>& mean, const LocalTensor<float>& var, const GroupNormTiling& tiling, 
    const int32_t loopCount, const int32_t mvOffset)
{
    for (uint32_t i = 0; i < tiling.d * tiling.bsCurLength; ++i) {
        uint32_t buffIndex = i * tiling.hwAlignSize;
        Adds(tmp1[buffIndex], x_in[buffIndex], 
            -1.0f * mean.GetValue((i + loopCount * tiling.d * tiling.bsCurLength + mvOffset * tiling.d) / tiling.d), tiling.hw);
        PipeBarrier<PIPE_V>();
    }

    Mul(tmp2, tmp1, tmp1, tiling.bshCurLength);
    PipeBarrier<PIPE_V>();

    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint32_t buffIndex = i * tiling.dhwAlignSize;
        ReduceSum<float>(var[i + loopCount * tiling.bsCurLength + mvOffset], 
            tmp2[buffIndex], tmp2[buffIndex], tiling.dhwAlignSize);
        PipeBarrier<PIPE_V>();
    }      

    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint64_t maskOffset = static_cast<uint64_t>(1) << (i + loopCount * tiling.bsCurLength + mvOffset);
        uint64_t mask[1] = {maskOffset};
        Muls(var, var, tiling.factor, mask, 1, {1, 1, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL});
        PipeBarrier<PIPE_V>();
    }      
}

__aicore__ inline void GetGroupNormOutputPre(const LocalTensor<float>& inout,
    const LocalTensor<float>& tmp, const LocalTensor<float>& tempOnes, const LocalTensor<float>& variance,
    const GroupNormTiling& tiling, const float epsilon, const int32_t loopCount, const int32_t mvOffset)
{
    for (uint32_t i = 0; i < tiling.bsCurLength; i++) {
        uint64_t maskMvOffset = i + loopCount * tiling.bsCurLength + mvOffset;
        uint32_t maskMod = maskMvOffset / GROUPNORM_MASK_MAX_VAL;
        uint32_t loopIndex = maskMod * GROUPNORM_MASK_MAX_VAL;
        maskMvOffset = maskMvOffset - maskMod * GROUPNORM_MASK_MAX_VAL;
        uint64_t maskOffset = static_cast<uint64_t>(1) << (maskMvOffset);
        uint64_t mask[1] = {maskOffset};

        Adds<float, true>(tmp, variance[loopIndex], epsilon, mask, 1, {1, 1, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL});
        PipeBarrier<PIPE_V>();
        
        Duplicate<float, true>(tempOnes, 1.0f, mask, 1, 1, 1);
        PipeBarrier<PIPE_V>();

        Sqrt<float, true>(tmp, tmp, mask, 1, {1, 1, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL});
        PipeBarrier<PIPE_V>();

        Div<float, true>(tmp, tempOnes, tmp, mask, 1, 
            {1, 1, 1, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL});
        PipeBarrier<PIPE_V>();
    }

    auto eventIdVToS = GetTPipePtr()->FetchEventID(HardEvent::V_S);
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);

    // pre norm
    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint32_t buffIndex = i * tiling.dhwAlignSize;
        Muls<float, true>(inout[buffIndex], inout[buffIndex], 
            tmp.GetValue((i + loopCount * tiling.bsCurLength + mvOffset) % GROUPNORM_MASK_MAX_VAL), tiling.dhwAlignSize);
        PipeBarrier<PIPE_V>();
    }

    // tmp will be written later 
    auto eventIdSToV = GetTPipePtr()->FetchEventID(HardEvent::S_V);
    SetFlag<HardEvent::V_S>(eventIdSToV);
    WaitFlag<HardEvent::V_S>(eventIdSToV);

    PipeBarrier<PIPE_V>();
}

__aicore__ inline void GetGroupNormOutput(const LocalTensor<float>& inout,
    const LocalTensor<float>& gamma, const LocalTensor<float>& beta,
    const GroupNormTiling& tiling, const int32_t loopCount, const int32_t mvOffset)
{
    size_t channelIndex = loopCount * tiling.bsCurLength * tiling.d + mvOffset * tiling.d;
    for (uint32_t channel_offset = 0; channel_offset < tiling.bsCurLength * tiling.d; ++channel_offset) {
        Muls(inout[channel_offset * tiling.hwAlignSize], inout[channel_offset * tiling.hwAlignSize],
        gamma.GetValue(channelIndex % tiling.c), tiling.hw);
        channelIndex += 1;
        PipeBarrier<PIPE_V>();
    }       

    channelIndex = loopCount * tiling.bsCurLength * tiling.d + mvOffset * tiling.d;
    for (uint32_t channel_offset = 0; channel_offset < tiling.bsCurLength * tiling.d; ++channel_offset) {
        Adds(inout[channel_offset * tiling.hwAlignSize], inout[channel_offset * tiling.hwAlignSize],
        beta.GetValue(channelIndex % tiling.c), tiling.hw);
        channelIndex += 1;
        PipeBarrier<PIPE_V>();
    }    
}

__aicore__ inline void GroupNormExe(const LocalTensor<half>& inputX,
    const LocalTensor<half>& gamma, const LocalTensor<half>& beta,
    const LocalTensor<half>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance,
    const half epsilon, const GroupNormTiling& tiling, const GroupNormParams<float>& params, const int32_t loopCount, 
    const int32_t mvOffset)
{
    LocalTensor<float> tempTensorA = params.tempTensorA;
    LocalTensor<float> tempTensorB = params.tempTensorB;
    LocalTensor<float> tempTensorC = params.tempTensorC;
    Duplicate(tempTensorA, 0.0f, tiling.bshCurLength);
    PipeBarrier<PIPE_V>();
    Cast<float, half>(tempTensorB, inputX, RoundMode::CAST_NONE, tiling.inputRoundSize);
    PipeBarrier<PIPE_V>();

    GetGroupNormOutputMean(tempTensorB, tempTensorC, outputMean, tiling, loopCount, mvOffset);

    GetGroupNormOutputVar(tempTensorB, tempTensorB, tempTensorC, outputMean, outputVariance, tiling, loopCount, mvOffset);

    GetGroupNormOutputPre(tempTensorB, tempTensorA, tempTensorC, outputVariance, tiling, 
                            static_cast<float>(epsilon), loopCount, mvOffset);

    Cast<float, half>(tempTensorA, gamma, RoundMode::CAST_NONE, tiling.c);
    PipeBarrier<PIPE_V>();
    Cast<float, half>(tempTensorC, beta, RoundMode::CAST_NONE, tiling.c);
    PipeBarrier<PIPE_V>();

    GetGroupNormOutput(tempTensorB, tempTensorA, tempTensorC, tiling, loopCount, mvOffset);
   
    Cast<half, float>(output, tempTensorB, RoundMode::CAST_NONE, tiling.inputRoundSize);
    PipeBarrier<PIPE_V>();
}


__aicore__ inline void GroupNormExe(const LocalTensor<float>& inputX,
    const LocalTensor<float>& gamma, const LocalTensor<float>& beta,
    const LocalTensor<float>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance,
    const float epsilon, const GroupNormTiling& tiling, const GroupNormParams<float>& params, const int32_t loopCount, 
    const int32_t mvOffset)
{
    LocalTensor<float> tempTensorA = params.tempTensorA;
    LocalTensor<float> tempTensorB = params.tempTensorB;
    LocalTensor<float> tempTensorC = params.tempTensorC;

    GetGroupNormOutputMean(inputX, output, outputMean, tiling, loopCount, mvOffset);

    Duplicate(output, 0.0f, tiling.bshCurLength);
    PipeBarrier<PIPE_V>();

    GetGroupNormOutputVar(inputX, output, tempTensorC, outputMean, outputVariance, tiling, loopCount, mvOffset);

    GetGroupNormOutputPre(output, tempTensorA, tempTensorB, outputVariance, tiling, epsilon, loopCount, mvOffset);

    GetGroupNormOutput(output, gamma, beta, tiling, loopCount, mvOffset);
}

__aicore__ inline void GroupNormExeSmallShape(const LocalTensor<half>& inputX,
    const LocalTensor<half>& gamma, const LocalTensor<half>& beta,
    const LocalTensor<half>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance,
    const half epsilon, const GroupNormTiling& tiling, const GroupNormParams<float>& params, const int32_t loopCount, 
    const int32_t mvOffset)
{
    LocalTensor<float> tempTensorA = params.tempTensorA;
    LocalTensor<float> tempTensorB = params.tempTensorB;
    LocalTensor<float> tempTensorC = params.tempTensorC;
    Duplicate(tempTensorA, 0.0f, tiling.inputRoundSize * tiling.numberOfTmpBuf);
    PipeBarrier<PIPE_V>();

    Cast<float, half>(tempTensorB, inputX, RoundMode::CAST_NONE, tiling.inputRoundSize);
    PipeBarrier<PIPE_V>();

    uint32_t mask1 = GetGroupNormWholeReduceMask1(tiling);
    ASCENDC_ASSERT((mask1 > 0), { KERNEL_LOG(KERNEL_ERROR, "mask1 must > 0!"); });

    uint32_t repeat1 = tiling.dhwAlignSize / mask1 * tiling.meanVarRoundSize;
    uint32_t mask2 = tiling.dhwAlignSize / mask1 * GROUPNORM_MASK_SMALLEST_VAL;
    PipeBarrier<PIPE_V>();  

    WholeReduceSum<float, true>(tempTensorC, tempTensorB, mask1, repeat1, 
                                GROUPNORM_MASK_SMALLEST_VAL, DEFAULT_BLK_STRIDE, mask1 / GROUPNORM_MASK_SMALLEST_VAL);
    PipeBarrier<PIPE_V>();

    WholeReduceSum<float, true>(outputMean[loopCount * tiling.bsCurLength + mvOffset], tempTensorC, mask2, tiling.bsCurLength, 
                                DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, mask2 / GROUPNORM_MASK_SMALLEST_VAL);
    PipeBarrier<PIPE_V>();

    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint64_t maskMvOffset = i + loopCount * tiling.bsCurLength + mvOffset;
        uint32_t maskMod = maskMvOffset / GROUPNORM_MASK_MAX_VAL;
        uint64_t maskOffset = static_cast<uint64_t>(1) << (maskMvOffset - maskMod * GROUPNORM_MASK_MAX_VAL);
        uint64_t mask[1] = {maskOffset};
        Muls(outputMean[GROUPNORM_MASK_MAX_VAL * maskMod], outputMean[GROUPNORM_MASK_MAX_VAL * maskMod], tiling.factor, 
            mask, 1, {1, 1, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL});
        PipeBarrier<PIPE_V>();
    }
    
    auto eventIdVToS = GetTPipePtr()->FetchEventID(HardEvent::V_S);
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);

    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint32_t buffIndex = i * tiling.dhwAlignSize;
        Adds(tempTensorB[buffIndex], tempTensorB[buffIndex], -1.0f * outputMean.GetValue(i + loopCount * tiling.bsCurLength + mvOffset), 
            tiling.hw, tiling.d, {1, 1, static_cast<uint8_t>(tiling.hwAlignSize / GROUPNORM_ONE_BLK_SIZE), 
            static_cast<uint8_t>(tiling.hwAlignSize / GROUPNORM_ONE_BLK_SIZE)});
        PipeBarrier<PIPE_V>();
    }  

    Mul(tempTensorC, tempTensorB, tempTensorB, tiling.bshCurLength);
    PipeBarrier<PIPE_V>();

    WholeReduceSum<float, true>(tempTensorA, tempTensorC, mask1, repeat1, 
                                GROUPNORM_MASK_SMALLEST_VAL, DEFAULT_BLK_STRIDE, mask1 / GROUPNORM_MASK_SMALLEST_VAL);
    PipeBarrier<PIPE_V>();

    WholeReduceSum<float, true>(outputVariance[loopCount * tiling.bsCurLength + mvOffset], tempTensorA, mask2, tiling.bsCurLength, 
                                DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, mask2 / GROUPNORM_MASK_SMALLEST_VAL);
    PipeBarrier<PIPE_V>();

    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint64_t maskMvOffset = i + loopCount * tiling.bsCurLength + mvOffset;
        uint32_t maskMod = maskMvOffset / GROUPNORM_MASK_MAX_VAL;
        uint64_t maskOffset = static_cast<uint64_t>(1) << (maskMvOffset - maskMod * GROUPNORM_MASK_MAX_VAL);
        uint64_t mask[1] = {maskOffset};

        Muls(outputVariance[GROUPNORM_MASK_MAX_VAL * maskMod], outputVariance[GROUPNORM_MASK_MAX_VAL * maskMod], tiling.factor, 
            mask, 1, {1, 1, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL});
        PipeBarrier<PIPE_V>();
    }

    GetGroupNormOutputPre(tempTensorB, tempTensorA, tempTensorC, outputVariance, tiling, static_cast<float>(epsilon), loopCount, mvOffset);

    Cast<float, half>(tempTensorA, gamma, RoundMode::CAST_NONE, tiling.c);
    PipeBarrier<PIPE_V>();
    Cast<float, half>(tempTensorC, beta, RoundMode::CAST_NONE, tiling.c);
    PipeBarrier<PIPE_V>();

    GetGroupNormOutput(tempTensorB, tempTensorA, tempTensorC, tiling, loopCount, mvOffset);
   
    Cast<half, float>(output, tempTensorB, RoundMode::CAST_NONE, tiling.inputRoundSize);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void GroupNormExeSmallShape(const LocalTensor<float>& inputX,
    const LocalTensor<float>& gamma, const LocalTensor<float>& beta,
    const LocalTensor<float>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance,
    const float epsilon, const GroupNormTiling& tiling, const GroupNormParams<float>& params, const int32_t loopCount, 
    const int32_t mvOffset)
{
    LocalTensor<float> tempTensorA = params.tempTensorA;
    LocalTensor<float> tempTensorB = params.tempTensorB;
    LocalTensor<float> tempTensorC = params.tempTensorC;

    if (mvOffset) {
        for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
            uint64_t maskMvOffset = i + loopCount * tiling.bsCurLength + mvOffset;
            uint32_t maskMod = maskMvOffset / GROUPNORM_MASK_MAX_VAL;
            uint64_t maskOffset = static_cast<uint64_t>(1) << (maskMvOffset - maskMod * GROUPNORM_MASK_MAX_VAL);
            uint64_t mask[1] = {maskOffset};

            Duplicate(output[GROUPNORM_MASK_MAX_VAL * maskMod], 0.0f, mask, 1, 1, GROUPNORM_MASK_SMALLEST_VAL);
            PipeBarrier<PIPE_V>();
        }
    } else {
        Duplicate(output, 0.0f, tiling.inputRoundSize);
        PipeBarrier<PIPE_V>();
    }
    
    Duplicate(tempTensorC, 0.0f, tiling.inputRoundSize);
    PipeBarrier<PIPE_V>();
    uint32_t mask1 = GetGroupNormWholeReduceMask1(tiling);
    ASCENDC_ASSERT((mask1 > 0), { KERNEL_LOG(KERNEL_ERROR, "mask1 must > 0!"); });

    uint32_t repeat1 = tiling.dhwAlignSize / mask1 * tiling.meanVarRoundSize;
    uint32_t mask2 = tiling.dhwAlignSize / mask1 * GROUPNORM_MASK_SMALLEST_VAL;
    PipeBarrier<PIPE_V>();

    WholeReduceSum<float, true>(tempTensorC, inputX, mask1, repeat1, 
                                GROUPNORM_MASK_SMALLEST_VAL, DEFAULT_BLK_STRIDE, mask1 / GROUPNORM_MASK_SMALLEST_VAL);
    PipeBarrier<PIPE_V>();

    WholeReduceSum<float, true>(outputMean[loopCount * tiling.bsCurLength + mvOffset], tempTensorC, mask2, tiling.bsCurLength, 
                                DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, mask2 / GROUPNORM_MASK_SMALLEST_VAL);
    PipeBarrier<PIPE_V>();

    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint64_t maskMvOffset = i + loopCount * tiling.bsCurLength + mvOffset;
        uint32_t maskMod = maskMvOffset / GROUPNORM_MASK_MAX_VAL;
        uint64_t maskOffset = static_cast<uint64_t>(1) << (maskMvOffset - maskMod * GROUPNORM_MASK_MAX_VAL);
        uint64_t mask[1] = {maskOffset};
        Muls(outputMean[GROUPNORM_MASK_MAX_VAL * maskMod], outputMean[GROUPNORM_MASK_MAX_VAL * maskMod], tiling.factor, 
            mask, 1, {1, 1, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL});
    }
    
    auto eventIdVToS = GetTPipePtr()->FetchEventID(HardEvent::V_S);
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);

    auto repeatStride = tiling.hwAlignSize / GROUPNORM_ONE_BLK_SIZE;
    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint32_t buffIndex = i * tiling.dhwAlignSize;
        Adds(output[buffIndex], inputX[buffIndex], -1.0f * outputMean.GetValue(i + loopCount * tiling.bsCurLength + mvOffset), tiling.hw, tiling.d,
        {1, 1, static_cast<uint8_t>(repeatStride), static_cast<uint8_t>(repeatStride)});

        PipeBarrier<PIPE_V>();
    }

    Mul(tempTensorC, output, output, tiling.bshCurLength);
    PipeBarrier<PIPE_V>();

    Duplicate(tempTensorA, 0.0f, tiling.inputRoundSize);
    PipeBarrier<PIPE_V>();

    WholeReduceSum<float, true>(tempTensorA, tempTensorC, mask1, repeat1, 
                                GROUPNORM_MASK_SMALLEST_VAL, DEFAULT_BLK_STRIDE, mask1 / GROUPNORM_MASK_SMALLEST_VAL);
    PipeBarrier<PIPE_V>();

    WholeReduceSum<float, true>(outputVariance[loopCount * tiling.bsCurLength + mvOffset], tempTensorA, mask2, tiling.bsCurLength, 
                                DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, mask2 / GROUPNORM_MASK_SMALLEST_VAL);
    PipeBarrier<PIPE_V>();

    for (uint32_t i = 0; i < tiling.bsCurLength; ++i) {
        uint64_t maskMvOffset = i + loopCount * tiling.bsCurLength + mvOffset;
        uint32_t maskMod = maskMvOffset / GROUPNORM_MASK_MAX_VAL;
        uint64_t maskOffset = static_cast<uint64_t>(1) << (maskMvOffset - maskMod * GROUPNORM_MASK_MAX_VAL);
        uint64_t mask[1] = {maskOffset};
        Muls(outputVariance[GROUPNORM_MASK_MAX_VAL * maskMod], outputVariance[GROUPNORM_MASK_MAX_VAL * maskMod], tiling.factor, 
            mask, 1, {1, 1, GROUPNORM_MASK_SMALLEST_VAL, GROUPNORM_MASK_SMALLEST_VAL});
        PipeBarrier<PIPE_V>();
    }
    
    GetGroupNormOutputPre(output, tempTensorA, tempTensorB, outputVariance, tiling, epsilon, loopCount, mvOffset);

    GetGroupNormOutput(output, gamma, beta, tiling, loopCount, mvOffset);
}

template <bool isReuseSource = false>
__aicore__ inline void GetGroupNormNDTensorInfo(const LocalTensor<half>& inputX,
    const LocalTensor<half>& outputMean, const LocalTensor<half>& outputVariance,
    const LocalTensor<float>& stackBuffer, const GroupNormTiling& tiling, GroupNormParams<float>& params)
{
    params.tempTensorA = stackBuffer[tiling.firstTmpStartPos];
    params.tempTensorB = stackBuffer[tiling.secondTmpStartPos];
    params.tempTensorC = stackBuffer[tiling.thirdTmpStartPos];
    params.meanTmpTensor = stackBuffer[tiling.meanTmpTensorPos];
    params.varianceTmpTensor = stackBuffer[tiling.varianceTmpTensorPos];

    ASCENDC_ASSERT((tiling.thirdTmpStartPos + tiling.oneTmpSize <= tiling.tmpBufSize), {
        KERNEL_LOG(KERNEL_ERROR, "thirdTmpStartPos + oneTmpSize is (%d) should <= tmpBufSize is (%d)",
        tiling.thirdTmpStartPos + tiling.oneTmpSize, tiling.tmpBufSize);
    });
    ASCENDC_ASSERT((stackBuffer.GetSize() >= tiling.tmpBufSize), {
        KERNEL_LOG(KERNEL_ERROR, "stackBuffer.GetSize is (%d) should >= tmpBufSize is (%d)",
        stackBuffer.GetSize(), tiling.tmpBufSize);
    });
}

template <bool isReuseSource = false>
__aicore__ inline void GetGroupNormNDTensorInfo(const LocalTensor<float>& inputX,
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance,
    const LocalTensor<float>& stackBuffer, const GroupNormTiling& tiling, GroupNormParams<float>& params)
{
    params.meanTmpTensor = outputMean;
    params.varianceTmpTensor = outputVariance;

    if constexpr (isReuseSource) {
        params.tempTensorA = inputX;
        params.tempTensorB = stackBuffer[tiling.firstTmpStartPos];
        params.tempTensorC = stackBuffer[tiling.secondTmpStartPos];

        ASCENDC_ASSERT((tiling.secondTmpStartPos + tiling.oneTmpSize <= tiling.tmpBufSize), {
            KERNEL_LOG(KERNEL_ERROR, "secondTmpStartPos + oneTmpSize is (%d) should <= tmpBufSize is (%d)",
            tiling.secondTmpStartPos + tiling.oneTmpSize, tiling.tmpBufSize);
        });
    } else {
        params.tempTensorA = stackBuffer[tiling.firstTmpStartPos];
        params.tempTensorB = stackBuffer[tiling.secondTmpStartPos];
        params.tempTensorC = stackBuffer[tiling.thirdTmpStartPos];
       
        ASCENDC_ASSERT((tiling.thirdTmpStartPos + tiling.oneTmpSize <= tiling.tmpBufSize), {
            KERNEL_LOG(KERNEL_ERROR, "thirdTmpStartPos + oneTmpSize is (%d) should <= tmpBufSize is (%d)",
            tiling.thirdTmpStartPos + tiling.oneTmpSize, tiling.tmpBufSize);
        });
    }
   
    ASCENDC_ASSERT((stackBuffer.GetSize() >= tiling.tmpBufSize), {
        KERNEL_LOG(KERNEL_ERROR, "stackBuffer.GetSize is (%d) should >= tmpBufSize is (%d)",
        stackBuffer.GetSize(), tiling.tmpBufSize);
    });
}

__aicore__ inline void GetOutputMeanVariance(const LocalTensor<half>& outputMean,
    const LocalTensor<half>& outputVariance, const GroupNormTiling& tiling, const GroupNormParams<float>& params)
{
    Cast<half, float>(outputMean, params.meanTmpTensor, RoundMode::CAST_NONE, tiling.n * tiling.g);
    Cast<half, float>(outputVariance, params.varianceTmpTensor, RoundMode::CAST_NONE, tiling.n * tiling.g);
}

template <typename T>
__aicore__ inline void GroupNormNDCommon(const LocalTensor<T>& inputX,
    const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const T epsilon, GroupNormTiling& tiling, const GroupNormParams<float>& params)
{
    uint32_t inputOffset = 0;
    uint32_t mvOffset = 0;

    if (tiling.smallShape) {
        for (uint32_t index = 0; index < tiling.loopRound; index++) {
            GroupNormExeSmallShape(inputX[inputOffset], gamma, beta, output[inputOffset],
            params.meanTmpTensor,
            params.varianceTmpTensor, epsilon, tiling, params, index, mvOffset);

            inputOffset += tiling.inputRoundSize;
        }
    } else {
        for (uint32_t index = 0; index < tiling.loopRound; index++) {
            GroupNormExe(inputX[inputOffset], gamma, beta, output[inputOffset],
            params.meanTmpTensor,
            params.varianceTmpTensor, epsilon, tiling, params, index, mvOffset);

            inputOffset += tiling.inputRoundSize;
        }
    }

    if (tiling.inputTailSize > 0) {
        tiling.bshCurLength = tiling.inputTailSize;
        tiling.bsCurLength = tiling.meanVarTailSize;

        inputOffset = tiling.inputTailPos;
        mvOffset = tiling.meanVarTailPos;

        if (tiling.smallShape) {
            GroupNormExeSmallShape(inputX[inputOffset], gamma, beta, output[inputOffset],
            params.meanTmpTensor,
            params.varianceTmpTensor, epsilon, tiling, params, 0, mvOffset);
        } else {
            GroupNormExe(inputX[inputOffset], gamma, beta, output[inputOffset],
            params.meanTmpTensor,
            params.varianceTmpTensor, epsilon, tiling, params, 0, mvOffset);
        }

        // revert to normal round size from tail size, for the next iteration calculation
        tiling.bshCurLength = tiling.inputRoundSize;
        tiling.bsCurLength = tiling.meanVarRoundSize;
    }

    if constexpr (sizeof(T) == sizeof(half)) {
        GetOutputMeanVariance(outputMean, outputVariance, tiling, params);
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void GroupNormImpl(const LocalTensor<T>& output,
    const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, GroupNormTiling& tiling)
{
    CHECK_FUNC_HIGHLEVEL_API(GroupNorm, (T, isReuseSource), (output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer,
        epsilon, tiling));
    ASCENDC_ASSERT((tiling.oneTmpSize > 0), { KERNEL_LOG(KERNEL_ERROR, "tiling.oneTmpSize must > 0!"); });

    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<float> stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    ASCENDC_ASSERT((stackBuffer.GetSize() > 0),{ KERNEL_LOG(KERNEL_ERROR, "sharedTmpBuffer Size must > 0!"); });

    GroupNormParams<float> params;
    GetGroupNormNDTensorInfo<isReuseSource>(inputX, outputMean, outputVariance, stackBuffer, tiling, params);

    GroupNormNDCommon<T>(inputX, gamma, beta, output, outputMean, outputVariance, epsilon, tiling, params);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void GroupNormImpl(const LocalTensor<T>& output,
    const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
    const T epsilon, GroupNormTiling& tiling)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    GroupNormImpl<T, isReuseSource>(output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling);
}

} // namespace AscendC
#endif // IMPL_NORMALIZATION_GROUPNORM_GROUPNORM_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_GROUPNORM_GROUPNORM_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_GROUPNORM_GROUPNORM_COMMON_IMPL_H__
#endif
