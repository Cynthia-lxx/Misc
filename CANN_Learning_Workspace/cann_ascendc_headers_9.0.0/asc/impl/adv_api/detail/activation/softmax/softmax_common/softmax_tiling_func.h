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
 * \file softmax_tiling_func.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/softmax_common/softmax_tiling_func.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_TILING_FUNC_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_TILING_FUNC_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_TILING_FUNC_H
#include "softmax_common_utils.h"
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {

__aicore__ inline bool SoftMaxTilingFunc(const uint32_t workLocalSize, const SoftMaxShapeInfo& ndinfo,
    SoftMaxTiling& softmaxTiling, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, bool isBasicBlock = false,
    bool isDataFormatNZ = false)
{
    ASCENDC_ASSERT((dataTypeSize2 != 0),
                   { KERNEL_LOG(KERNEL_ERROR, "softmax maxTensor&sumTensor type is zero."); });
    const uint32_t elementNumPerBlk = ONE_BLK_SIZE / dataTypeSize2;
    const uint32_t srcM = ndinfo.srcM;
    const uint32_t srcK = ndinfo.srcK;
    const uint32_t oriSrcM = ndinfo.oriSrcM;

    softmaxTiling.srcM = srcM;
    softmaxTiling.srcK = srcK;
    softmaxTiling.srcSize = srcM * srcK;
    softmaxTiling.outMaxM = srcM;
    softmaxTiling.outMaxK = elementNumPerBlk;
    softmaxTiling.outMaxSize = srcM * elementNumPerBlk;
    if (isDataFormatNZ) {
        softmaxTiling.reduceM = workLocalSize / (SOFTMAX_SHAPE_NZ_BASIC_COUNT + srcK);
    } else {
        softmaxTiling.reduceM = CalculateNDSplitM(workLocalSize, dataTypeSize1, elementNumPerBlk, {srcM, srcK},
            isBasicBlock);
    }

    if (softmaxTiling.reduceM < oriSrcM && softmaxTiling.reduceM > SOFTMAX_BASIC_TILE_NUM) {
        softmaxTiling.reduceM = softmaxTiling.reduceM / SOFTMAX_BASIC_TILE_NUM * SOFTMAX_BASIC_TILE_NUM;
    }
    softmaxTiling.reduceM = softmaxTiling.reduceM < oriSrcM ? softmaxTiling.reduceM : oriSrcM;
    softmaxTiling.reduceK = elementNumPerBlk;
    softmaxTiling.reduceSize = softmaxTiling.reduceM * elementNumPerBlk;

    softmaxTiling.splitM = softmaxTiling.reduceM;
    softmaxTiling.splitK = srcK;
    softmaxTiling.splitSize = softmaxTiling.reduceM * srcK;
    ASCENDC_ASSERT((softmaxTiling.reduceM > 0),
                   { KERNEL_LOG(KERNEL_ERROR, "softmax need min tmpbuffer is not enough."); });
    softmaxTiling.rangeM = oriSrcM / softmaxTiling.reduceM;
    softmaxTiling.tailM = oriSrcM % softmaxTiling.reduceM;

    softmaxTiling.tailSplitSize = softmaxTiling.tailM * srcK;
    softmaxTiling.tailReduceSize = softmaxTiling.tailM * elementNumPerBlk;
    return true;
}

__aicore__ inline bool SoftMaxFlashTilingFunc(const uint32_t workLocalSize, const LastAxisShapeND& ndinfo,
    SoftMaxTiling& softmaxTiling, const uint32_t elementNumPerBlk, bool isUpdate = false, bool isBasicBlock = false)
{
    softmaxTiling.srcM = ndinfo.m;
    softmaxTiling.srcK = ndinfo.k;
    softmaxTiling.srcSize = ndinfo.m * ndinfo.k;

    softmaxTiling.outMaxM = ndinfo.m;
    softmaxTiling.outMaxK = elementNumPerBlk;
    softmaxTiling.outMaxSize = ndinfo.m * elementNumPerBlk;

    if (!isUpdate) {
        softmaxTiling.reduceM =
            workLocalSize / (elementNumPerBlk * SOFTMAX_COMPUTE_DIM + ndinfo.k * SOFTMAX_COMPUTE_DIM);
    } else {
        softmaxTiling.reduceM =
            workLocalSize / (elementNumPerBlk * SOFTMAXFLASH_COMPUTE_DIM + ndinfo.k * SOFTMAX_COMPUTE_DIM);
    }

    softmaxTiling.reduceM = softmaxTiling.reduceM < ndinfo.m ? softmaxTiling.reduceM : ndinfo.m;

    if (isBasicBlock && (softmaxTiling.reduceM > SOFTMAX_BASIC_TILE_NUM) &&
        (softmaxTiling.srcM % SOFTMAX_BASIC_TILE_NUM == 0)) {
        softmaxTiling.reduceM = softmaxTiling.reduceM / SOFTMAX_BASIC_TILE_NUM * SOFTMAX_BASIC_TILE_NUM;
        while (softmaxTiling.srcM % softmaxTiling.reduceM != 0) {
            softmaxTiling.reduceM -= SOFTMAX_BASIC_TILE_NUM;
        }
    }

    softmaxTiling.reduceK = elementNumPerBlk;
    softmaxTiling.reduceSize = softmaxTiling.reduceM * elementNumPerBlk;

    softmaxTiling.splitM = softmaxTiling.reduceM;
    softmaxTiling.splitK = ndinfo.k;
    softmaxTiling.splitSize = softmaxTiling.reduceM * ndinfo.k;
    ASCENDC_ASSERT((softmaxTiling.reduceM > 0),
                   { KERNEL_LOG(KERNEL_ERROR, "softmaxflash need min tmpbuffer is not enough."); });
    softmaxTiling.rangeM = ndinfo.m / softmaxTiling.reduceM;
    softmaxTiling.tailM = ndinfo.m % softmaxTiling.reduceM;

    softmaxTiling.tailSplitSize = softmaxTiling.tailM * ndinfo.k;
    softmaxTiling.tailReduceSize = softmaxTiling.tailM * elementNumPerBlk;
    return true;
}

__aicore__ inline bool SoftMaxGradTilingFunc(const uint32_t workLocalSize, const LastAxisShapeND& ndinfo,
    SoftMaxTiling& softmaxTiling, const uint32_t elementNumPerBlk, bool isFront = false, bool isBasicBlock = false,
    bool isDataFormatNZ = false)
{
    softmaxTiling.srcM = ndinfo.m;
    softmaxTiling.srcK = ndinfo.k;
    softmaxTiling.srcSize = ndinfo.m * ndinfo.k;

    softmaxTiling.outMaxM = ndinfo.m;
    softmaxTiling.outMaxK = elementNumPerBlk;
    softmaxTiling.outMaxSize = ndinfo.m * elementNumPerBlk;

    if (elementNumPerBlk != ONE_BYTE_BIT_SIZE) { // half
        softmaxTiling.reduceM = workLocalSize /
            (elementNumPerBlk * SOFTMAX_COMPUTE_DIM + ndinfo.k * SOFTMAXGRAD_COMPUTE_DIM + FLOAT_REPEAT_SIZE);
    } else {
        if (isFront && !isDataFormatNZ) {
            softmaxTiling.reduceM = workLocalSize / (elementNumPerBlk + ndinfo.k + FLOAT_REPEAT_SIZE);
        } else {
            softmaxTiling.reduceM =
                workLocalSize / (ndinfo.k + elementNumPerBlk * SOFTMAX_COMPUTE_DIM + FLOAT_REPEAT_SIZE);
        }
    }
    if (softmaxTiling.reduceM < ndinfo.m && softmaxTiling.reduceM > SOFTMAX_BASIC_TILE_NUM) {
        softmaxTiling.reduceM = softmaxTiling.reduceM / SOFTMAX_BASIC_TILE_NUM * SOFTMAX_BASIC_TILE_NUM;
    }
    softmaxTiling.reduceM = softmaxTiling.reduceM < ndinfo.m ? softmaxTiling.reduceM : ndinfo.m;

    if (isBasicBlock && isFront && (softmaxTiling.reduceM > SOFTMAX_BASIC_TILE_NUM) &&
        (softmaxTiling.srcM % SOFTMAX_BASIC_TILE_NUM == 0)) {
        softmaxTiling.reduceM = softmaxTiling.reduceM / SOFTMAX_BASIC_TILE_NUM * SOFTMAX_BASIC_TILE_NUM;
        while (softmaxTiling.srcM % softmaxTiling.reduceM != 0) {
            softmaxTiling.reduceM -= SOFTMAX_BASIC_TILE_NUM;
        }
        // max repeat only support 255
        while (softmaxTiling.reduceM * ndinfo.k >= FLOAT_REPEAT_SIZE * DEFAULT_BLOCK_SIZE) {
            softmaxTiling.reduceM = softmaxTiling.reduceM / B16_BYTE_SIZE;
        }
    }

    softmaxTiling.reduceK = elementNumPerBlk;
    softmaxTiling.reduceSize = softmaxTiling.reduceM * elementNumPerBlk;

    softmaxTiling.splitM = softmaxTiling.reduceM;
    softmaxTiling.splitK = ndinfo.k;
    softmaxTiling.splitSize = softmaxTiling.reduceM * ndinfo.k;
    ASCENDC_ASSERT((softmaxTiling.reduceM > 0),
                   { KERNEL_LOG(KERNEL_ERROR, "softmaxgrad need min tmpbuffer is not enough."); });
    softmaxTiling.rangeM = ndinfo.m / softmaxTiling.reduceM;
    softmaxTiling.tailM = ndinfo.m % softmaxTiling.reduceM;

    softmaxTiling.tailSplitSize = softmaxTiling.tailM * ndinfo.k;
    softmaxTiling.tailReduceSize = softmaxTiling.tailM * elementNumPerBlk;
    return true;
}

}; // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_SOFTMAX_TILING_FUNC_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_TILING_FUNC_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_TILING_FUNC_H__
#endif
