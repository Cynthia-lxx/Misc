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
#pragma message("impl/adv_api/detail/activation/softmax/softmax_common/softmax_common_simple.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_SIMPLE_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_SIMPLE_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_SIMPLE_H

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
#include "../regbase/v300/simple_softmax_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "../membase/v220/simple_softmax_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "../membase/v200/simple_softmax_impl.h"
#endif

namespace AscendC {

template <typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMaxBaseImpl(const LocalTensor<T1>& dst, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<T1>& src, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    SetMaskNorm();
    ResetMask();
    ShapeInfo srcShape = src.GetShapeInfo();
    LastAxisShapeND srcNDinfo;
    LastAxisShapeND originalSrcShape;
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        srcNDinfo = GetLastAxisShapeND(srcShape);
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    } else {
        srcNDinfo = { softmaxShapeInfo.srcM, softmaxShapeInfo.srcK };
        originalSrcShape = { softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK };
    }
    if constexpr (isDataFormatNZ) {
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxTilingFunc(workLocal.GetSize(), { srcNDinfo.m, srcNDinfo.k, originalSrcShape.m, srcNDinfo.k },
                newTiling, sizeof(T1), sizeof(T2), false, isDataFormatNZ);
            SimpleSoftMaxNZImpl<T1, T2>(dst, inSumTensor, inMaxTensor, src, workLocal, newTiling);
        } else {
            SimpleSoftMaxNZImpl<T1, T2>(dst, inSumTensor, inMaxTensor, src, workLocal, tiling);
        }
    } else {
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxTilingFunc(workLocal.GetSize(), { srcNDinfo.m, srcNDinfo.k, originalSrcShape.m, srcNDinfo.k },
                newTiling, sizeof(T1), sizeof(T2), isBasicBlock);
            SimpleSoftMaxNDImpl<T1, isBasicBlock, config>(dst, inSumTensor, inMaxTensor, src, workLocal, newTiling);
        } else {
            SimpleSoftMaxNDImpl<T1, isBasicBlock, config>(dst, inSumTensor, inMaxTensor, src, workLocal, tiling);
        }
    }
}

}
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_SIMPLE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_SIMPLE_H__
#endif
