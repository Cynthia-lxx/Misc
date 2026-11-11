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
#pragma message("impl/adv_api/detail/reduce/reduce_all/reduce_all_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_ALL_REDUCE_ALL_V220_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_ALL_REDUCE_ALL_V220_IMPL_H
#define IMPL_REDUCE_REDUCE_ALL_REDUCE_ALL_V220_IMPL_H

#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../reduce_common_util_v220_impl.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/reduce/reduce_all/reduce_all_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace Internal {
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceAllImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
                                      const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[],
                                      bool srcInnerPad)
{
    uint32_t last = srcShape[1];
    uint32_t first = srcShape[0];
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    uint32_t padLast = AlignUp(last, elePerBlk);
    static_assert(SupportType<T, float, uint8_t>(), "failed to check the data type, current api supports data type is float/uint8_t!");
    static_assert(SupportType<pattern, Pattern::Reduce::AR, Pattern::Reduce::RA>(), 
        "failed to check the reduce pattern, it only supports AR/RA pattern!");
    CHECK_FUNC_HIGHLEVEL_API(ReduceAll, (T, pattern), (dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad, padLast));
    if constexpr (SupportType<T, uint8_t>()) {
        if constexpr (IsSameType<pattern, Pattern::Reduce::AR>::value) {
            uint32_t splitK = 1 << FindClosestPowerOfTwo(last);
            if (last < elePerBlk) {
                splitK = 0;
            }
            uint32_t tail = last - splitK;
            BinaryReduceAnyAllCompute<T, isReuseSource, ApiMode::API_MODE_MIN, Min<half, false>>(
                dstTensor, srcTensor, sharedTmpBuffer, ReduceParams(first, last, padLast, splitK, tail, elePerBlk));

        } else {
            // u8 type is converted to int16, so the number of elements to be calculated is halved
            padLast >>= 1;
            last = (last + 1) >> 1;
            LocalTensor<int16_t> srcTmpBuff = srcTensor.template ReinterpretCast<int16_t>();
            LocalTensor<int16_t> dstTmpBuff = dstTensor.template ReinterpretCast<int16_t>();
            LocalTensor<int16_t> tmpBuf = sharedTmpBuffer.template ReinterpretCast<int16_t>();
            BinaryReduceByFirstAxis<int16_t, isReuseSource, And<int16_t, false>>(
                dstTmpBuff, srcTmpBuff, tmpBuf, first, last, padLast);
        }
    } else {
        LocalTensor<T> tmpTensor = sharedTmpBuffer.ReinterpretCast<T>();
        if constexpr (IsSameType<pattern, Pattern::Reduce::AR>::value) {
            BlockReduceByLastAxis<T, isReuseSource, ApiMode::API_MODE_ALL, Min<T, false>>(
                dstTensor, srcTensor, tmpTensor, first, last, padLast);
        } else {
            BinaryReduceByFirstAxis<T, isReuseSource, Min<T, false>>(
                dstTensor, srcTensor, tmpTensor, first, last, padLast);
        }
    }
    SetMaskNorm();
    ResetMask();
}
} // namespace Internal
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_ALL_REDUCE_ALL_V220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_ALL_REDUCE_ALL_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_ALL_REDUCE_ALL_V220_IMPL_H__
#endif
