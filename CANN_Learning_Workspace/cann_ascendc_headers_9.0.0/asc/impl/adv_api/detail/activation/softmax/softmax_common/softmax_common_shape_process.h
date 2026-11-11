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
 * \file softmax_common_shape_process.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/softmax_common/softmax_common_shape_process.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_SHAPE_PROCESS_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_SHAPE_PROCESS_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_SHAPE_PROCESS_H
#include "softmax_common_utils.h"

namespace AscendC {

__aicore__ inline LastAxisShapeND GetLastAxisShapeND(const ShapeInfo& shapeInfo)
{
    uint32_t calculateSize = 1;
    LastAxisShapeND ndinfo;
    for (uint32_t i = 0; i < shapeInfo.shapeDim; i++) {
        calculateSize *= shapeInfo.shape[i];
    }

    ASCENDC_ASSERT((shapeInfo.shapeDim > 0),
                { KERNEL_LOG(KERNEL_ERROR, "shapeInfo.shapeDim must > 0"); });
    ndinfo.k = shapeInfo.shape[shapeInfo.shapeDim - 1];
    ASCENDC_ASSERT((ndinfo.k > 0),
                { KERNEL_LOG(KERNEL_ERROR, "ndinfo.k must > 0"); });
    ndinfo.m = calculateSize / ndinfo.k;
    return ndinfo;
}

__aicore__ inline LastAxisShapeND GetLastAxisOriginShapeND(const ShapeInfo& srcShapeInfo)
{
    uint32_t calculateSize = 1;
    LastAxisShapeND ndinfo;
    for (uint32_t i = 0; i < srcShapeInfo.originalShapeDim; i++) {
        calculateSize *= srcShapeInfo.originalShape[i];
    }

    ASCENDC_ASSERT((srcShapeInfo.originalShapeDim > 0),
                   { KERNEL_LOG(KERNEL_ERROR, "shapeInfo.originalShapeDim must large than zero"); });
    ndinfo.k = srcShapeInfo.originalShape[srcShapeInfo.originalShapeDim - 1];
    ASCENDC_ASSERT((ndinfo.k > 0),
                { KERNEL_LOG(KERNEL_ERROR, "ndinfo.k must > 0"); });
    ndinfo.m = calculateSize / ndinfo.k;
    return ndinfo;
}
__aicore__ inline constexpr uint32_t CalculateNDSplitM(const uint32_t workLocalSize, const uint32_t dataTypeSize,
    const uint32_t reduceK, const LastAxisShapeND& ndinfo, bool isBasicBlock = false)
{
    uint32_t splitM = 0;
    if (dataTypeSize == B16_BYTE_SIZE) {
        splitM = workLocalSize / (reduceK + ndinfo.k + FLOAT_REPEAT_SIZE);
    } else {
        splitM = workLocalSize / (reduceK + FLOAT_REPEAT_SIZE);
    }

    splitM = splitM < ndinfo.m ? splitM : ndinfo.m;

    if (isBasicBlock && (splitM > SOFTMAX_BASIC_TILE_NUM) && (ndinfo.m % SOFTMAX_BASIC_TILE_NUM == 0)) {
        splitM = splitM / SOFTMAX_BASIC_TILE_NUM * SOFTMAX_BASIC_TILE_NUM;
        while (ndinfo.m % splitM != 0) {
            splitM -= SOFTMAX_BASIC_TILE_NUM;
        }
        // max repeat only support 255
        while (splitM * ndinfo.k >= FLOAT_REPEAT_SIZE * DEFAULT_BLOCK_SIZE) {
            splitM = splitM / HALF_FACTOR;
        }
    }
    return splitM;
}

}; // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_SHAPE_PROCESS_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_SHAPE_PROCESS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_SHAPE_PROCESS_H__
#endif
