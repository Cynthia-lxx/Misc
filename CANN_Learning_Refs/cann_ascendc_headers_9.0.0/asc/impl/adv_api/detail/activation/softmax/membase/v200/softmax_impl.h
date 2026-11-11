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
 * \file softmax_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/membase/v200/softmax_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_V200_SOFTMAX_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_V200_SOFTMAX_IMPL_H

#include "softmax_common_impl.h"
#include "../common/softmax_impl/softmax_basic_block_impl.h"
#include "../common/softmax_impl/softmax_generic_nz_impl.h"
#include "../common/softmax_impl/softmax_generic_nd_impl.h"
#include "../common/softmax_common_impl.h"

namespace AscendC {

template <typename T1, typename T2, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxNDImpl(const LocalTensor<T1>& dst, const LocalTensor<T1>& sumTensor,
    const LocalTensor<T1>& maxTensor, const LocalTensor<T1>& src, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    SetMaskNorm();
    ResetMask();
    ReduceLastND reduceParam = { tiling.splitM, originalSrcShape.k, tiling.splitM,
        tiling.splitK, tiling.reduceM,     tiling.reduceK };
    PipeBarrier<PIPE_V>();
    if constexpr (isBasicBlock) {
        SoftMaxBasicBlock(dst, sumTensor, maxTensor, src, workLocal, tiling);
    } else {
        SoftMaxNDExtImpl<T1>(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling, reduceParam);
    }
}

template <typename T1, typename T2, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxNDImpl(const LocalTensor<half>& dst, const LocalTensor<float>& sumTensor,
    const LocalTensor<float>& maxTensor, const LocalTensor<half>& src, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    SetMaskNorm();
    ResetMask();
    PipeBarrier<PIPE_V>();
    if constexpr (isBasicBlock) {
        SoftMaxBasicBlock(dst, sumTensor, maxTensor, src, workLocal, tiling);
    } else {
        ReduceLastND reduceParam = { tiling.splitM, originalSrcShape.k, tiling.splitM,
            tiling.splitK, tiling.reduceM,     tiling.reduceK };
        SoftMaxNDExtImpl(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling, reduceParam);
    }
}

__aicore__ inline void SingleSoftMaxImpl(const LocalTensor<half>& dst, const LocalTensor<half>& src,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const uint32_t& offset, const uint32_t& splitSize,
    const ReduceLastND& reduceParam)
{
    const LocalTensor<float>& tmpBuffer0 = workLocal;
    const LocalTensor<float>& tmpBuffer1 = workLocal[tiling.splitSize];
    const LocalTensor<float>& tmpBuffer2 = workLocal[tiling.splitSize + tiling.reduceSize]; // need splitM * 64

    Cast(tmpBuffer0, src[offset], RoundMode::CAST_NONE, splitSize);
    PipeBarrier<PIPE_V>();
    NewReduceMaxLastNDImpl(tmpBuffer1, tmpBuffer0, tmpBuffer2, reduceParam);
    PipeBarrier<PIPE_V>();

    GenericSubNDImpl(tmpBuffer0, tmpBuffer0, tmpBuffer1, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);

    PipeBarrier<PIPE_V>();
    Exp(tmpBuffer0, tmpBuffer0, splitSize);
    PipeBarrier<PIPE_V>();
    NewReduceSumLastNDImpl(tmpBuffer1, tmpBuffer0, tmpBuffer2, reduceParam);
    PipeBarrier<PIPE_V>();

    TransDivToMulImpl(tmpBuffer0, tmpBuffer1, tmpBuffer2, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
    PipeBarrier<PIPE_V>();
    Cast(dst[offset], tmpBuffer0, FLOAT2HALF_ROUND_MODE, splitSize);
}

__aicore__ inline void SingleSoftMaxImpl(const LocalTensor<float>& dst, const LocalTensor<float>& src,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const uint32_t& offset, const uint32_t& splitSize,
    const ReduceLastND& reduceParam)
{
    const LocalTensor<float>& tmpBuffer0 = workLocal;
    const LocalTensor<float>& tmpBuffer1 = workLocal[tiling.reduceSize]; // need splitM * 64

    NewReduceMaxLastNDImpl(tmpBuffer0, src[offset], tmpBuffer1, reduceParam);
    PipeBarrier<PIPE_V>();
    GenericSubNDImpl(dst[offset], src[offset], tmpBuffer0, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
    PipeBarrier<PIPE_V>();
    Exp(dst[offset], dst[offset], splitSize);
    PipeBarrier<PIPE_V>();
    NewReduceSumLastNDImpl(tmpBuffer0, dst[offset], tmpBuffer1, reduceParam);
    PipeBarrier<PIPE_V>();
    TransDivToMulImpl(dst[offset], tmpBuffer0, tmpBuffer1, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
}

template <typename T, bool isReuseSource = false, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxNDImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    SetMaskNorm();
    ResetMask();
    uint32_t offset = 0;
    uint32_t splitSize = tiling.splitSize;
    ReduceLastND reduceParam = { tiling.splitM, originalSrcShape.k, tiling.splitM,
        tiling.splitK, tiling.reduceM,     tiling.reduceK };
    PipeBarrier<PIPE_V>();
    for (uint32_t i = 0; i <= tiling.rangeM; i++) {
        SingleSoftMaxImpl(dst, src, workLocal, tiling, offset, splitSize, reduceParam);
        offset += tiling.splitSize;
        if (i == (tiling.rangeM - 1)) {
            if (tiling.tailM == 0) {
                break;
            }
            offset = tiling.rangeM * tiling.splitSize;
            splitSize = tiling.tailSplitSize;
            reduceParam.originalSrcM = tiling.tailM;
            reduceParam.srcM = tiling.tailM;
            reduceParam.dstM = tiling.tailM;
            PipeBarrier<PIPE_V>();
        }
    }
}
}
#endif // IMPL_ACTIVATION_SOFTMAX_V200_SOFTMAX_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_IMPL_H__
#endif
