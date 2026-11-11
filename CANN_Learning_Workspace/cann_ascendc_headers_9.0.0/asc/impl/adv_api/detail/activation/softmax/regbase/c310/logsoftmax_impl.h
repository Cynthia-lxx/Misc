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
 * \file logsoftmax_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/regbase/c310/logsoftmax_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/logsoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGSOFTMAX_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_C310_LOGSOFTMAX_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_C310_LOGSOFTMAX_IMPL_H

#include "softmax_impl.h"

namespace AscendC {
template <typename T, bool isReuseSource = false>
__aicore__ inline void LogSoftMaxNZImpl(const LocalTensor<T>& dst, const LocalTensor<T>& sum,
    const LocalTensor<T>& max, const LocalTensor<T>& src, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    static_assert(SupportType<T, half, float>(), "LogSoftMax api only support half/float on current device");

    if (tiling.srcK != originalSrcShape.k) {
        SoftMaxGenericNZWithTailImpl<T, T, true>(dst, sum, max, src, workLocal, originalSrcShape, tiling);
    } else {
        SoftMaxGenericNZImpl<T, T, true>(dst, sum, max, src, workLocal, originalSrcShape, tiling);
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LogSoftMaxNDImpl(const LocalTensor<T>& dst, const LocalTensor<T>& sumTensor,
    const LocalTensor<T>& maxTensor, const LocalTensor<T>& src, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    static_assert(SupportType<T, half, float>(), "LogSoftMax api only support half/float on current device");
    constexpr uint32_t floatBlockStride = GetDataBlockSizeInBytes() / sizeof(float);
    constexpr uint32_t halfBlockStride = GetDataBlockSizeInBytes() / sizeof(half);
    constexpr uint32_t floatStride = GetVecLen() / sizeof(float);
    if (tiling.srcK == floatBlockStride && IsSameType<T, float>::value) {
        SingleSoftMaxGenericNDForBlkImpl<T, T, false, true>(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
    } else if (tiling.srcK == halfBlockStride) {
        SingleSoftMaxGenericNDAlignedWithBlkImpl<T, T, false, true>(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
    } else if (originalSrcShape.k <= floatStride) {
        SingleSoftMaxGenericNDImpl<T, T, false, true>(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
    } else if (originalSrcShape.k % floatStride != 0) {
        SoftMaxGenericNDWithTailImpl<T, T, false, true>(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
    } else {
        SoftMaxGenericNDImpl<T, T, false, true>(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
    }
}
}  // namespace AscendC
#endif  // IMPL_ACTIVATION_SOFTMAX_C310_LOGSOFTMAX_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGSOFTMAX_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGSOFTMAX_IMPL_H__
#endif
