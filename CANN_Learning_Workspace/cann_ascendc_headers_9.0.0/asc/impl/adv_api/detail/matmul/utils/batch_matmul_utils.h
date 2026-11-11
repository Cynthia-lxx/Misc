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
* \file batch_matmul_utils.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/utils/batch_matmul_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_BATCH_MATMUL_UTILS_H__
#endif

#ifndef IMPL_MATMUL_UTILS_BATCH_MATMUL_UTILS_H
#define IMPL_MATMUL_UTILS_BATCH_MATMUL_UTILS_H

#include "matmul_config_utils.h"
#include "matmul_type_def.h"

namespace AscendC {

template <typename A_TYPE, const auto& MM_CFG>
constexpr bool IsBmmEnableScheduler = DoMatmulNorm(MM_CFG) &&
    ((A_TYPE::layout != LayoutMode::NONE && ToMatmulConfig(MM_CFG).batchMode == BatchMode::BATCH_LESS_THAN_L1) ||
    (A_TYPE::layout == LayoutMode::NORMAL && ToMatmulConfig(MM_CFG).batchMode == BatchMode::BATCH_LARGE_THAN_L1) ||
    (A_TYPE::layout == LayoutMode::NORMAL && ToMatmulConfig(MM_CFG).batchMode == BatchMode::SINGLE_LARGE_THAN_L1));

template <typename A_TYPE, const auto& MM_CFG>
constexpr bool IsBmmBatchScheduler = DoMatmulNorm(MM_CFG) &&
    ((A_TYPE::layout != LayoutMode::NONE && ToMatmulConfig(MM_CFG).batchMode != BatchMode::SINGLE_LARGE_THAN_L1));

template <typename A_TYPE, const auto& MM_CFG>
constexpr bool IsBmmSingleScheduler = DoMatmulNorm(MM_CFG) &&
    (A_TYPE::layout == LayoutMode::NORMAL && ToMatmulConfig(MM_CFG).batchMode == BatchMode::SINGLE_LARGE_THAN_L1);

struct BatchOffsetInfo
{
    int32_t modA;
    int32_t divisorA;
    int32_t alignA;
    int32_t modB;
    int32_t divisorB;
    int32_t alignB;
    int32_t modBias;
    int32_t divisorBias;
    int32_t alignBias;
    bool setBiasFlag {false};
};

struct SplitParams
{
    int16_t axisL1Len;
    int16_t kAxisL1Len;
    int16_t axisL1Offset;
    int16_t kAxisL1Offset;
    int16_t axisL0Len;
};

struct BatchSchedulerContext
{
    int32_t offsetA;
    int32_t offsetB;
    int32_t offsetBias;
    uint32_t reduceGNum;
    bool isReduceG;
    SplitParams aL0Params;
    SplitParams bL0Params;
};

struct BmmOffset {
    uint64_t offA = 0;
    uint64_t offB = 0;
    uint64_t offC = 0;
};

// It is invoked by the matmulV3 operator and cannot be removed at present
__aicore__ inline uint16_t CeilDiv(uint16_t num1, uint16_t num2)
{
    ASSERT(num2 > 0);
    return (num1 + num2 - 1) / num2;
}

// It is invoked by the matmulV3 operator and cannot be removed at present
__aicore__ inline uint16_t CeilAlign(uint16_t num1, uint16_t num2)
{
    ASSERT(num2 > 0);
    return CeilDiv(num1, num2) * num2;
}

template <class INPUT_TYPE>
__aicore__ inline uint64_t CalcNBatchoffset(uint32_t batchValue, uint32_t loopIdx, uint32_t layoutInfoN, uint32_t layoutInfoG, uint32_t layoutInfoD, uint32_t layoutInfoS)
{
    uint32_t alignedSingleCoreN = layoutInfoD;
    if constexpr (INPUT_TYPE::format == CubeFormat::ND_ALIGN) {
        alignedSingleCoreN = CeilAlign(layoutInfoD, AscendCUtils::GetC0Count(sizeof(typename INPUT_TYPE::T)));
    }
    uint64_t offset;
    if constexpr (INPUT_TYPE::layout == LayoutMode::BNGS1S2 || INPUT_TYPE::layout == LayoutMode::NORMAL) {
        offset = alignedSingleCoreN * layoutInfoS * batchValue * loopIdx * sizeof(typename INPUT_TYPE::T);
    } else if constexpr (INPUT_TYPE::layout == LayoutMode::SBNGD) {
        offset = alignedSingleCoreN * batchValue * loopIdx * sizeof(typename INPUT_TYPE::T);
    } else if constexpr (INPUT_TYPE::layout == LayoutMode::BSNGD) {
        uint64_t layoutBIdx = loopIdx * batchValue / (layoutInfoN * layoutInfoG);
        uint64_t layoutNGOff = loopIdx * batchValue % (layoutInfoN * layoutInfoG);
        offset = (layoutBIdx * alignedSingleCoreN * layoutInfoS * layoutInfoN * layoutInfoG + layoutNGOff * alignedSingleCoreN) * sizeof(typename INPUT_TYPE::T); 
    } 
    return offset;
}

__aicore__ inline uint64_t GetBatchCNum(uint32_t batchA, uint32_t batchB, uint32_t aLayoutInfoG, uint32_t bLayoutInfoG, uint32_t cLayoutInfoG)
{
    uint32_t batchC = batchA > batchB ? batchA : batchB;
    bool layoutGCondition = cLayoutInfoG == 1 &&
                            (aLayoutInfoG != 1 || bLayoutInfoG != 1);
    if (layoutGCondition) {
        int32_t layoutG = bLayoutInfoG > aLayoutInfoG ? bLayoutInfoG : aLayoutInfoG;
        batchC = batchC / layoutG;
    }
    return batchC;
}
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_BATCH_MATMUL_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_BATCH_MATMUL_UTILS_H__
#endif // IMPL_MATMUL_UTILS_BATCH_MATMUL_UTILS_H