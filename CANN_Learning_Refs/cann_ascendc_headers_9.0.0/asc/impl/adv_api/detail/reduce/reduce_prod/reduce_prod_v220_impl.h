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
#pragma message("impl/adv_api/detail/reduce/reduce_prod/reduce_prod_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_PROD_REDUCE_PROD_V220_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_PROD_REDUCE_PROD_V220_IMPL_H_
#define IMPL_REDUCE_REDUCE_PROD_REDUCE_PROD_V220_IMPL_H_

#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../reduce_common_util_v220_impl.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/reduce/reduce_prod/reduce_prod_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace Internal {

template <typename T, bool isReuseSource>
__aicore__ inline void PreProcessReduceForAR(const LocalTensor<T>& src, const LocalTensor<T>& currBuff,
    uint32_t row, uint32_t last, uint32_t padLast, uint32_t remain, uint32_t& splitK)
{
    BinaryRepeatParams defaultParam;
    UnaryRepeatParams defaultUnaryParam;
    constexpr uint32_t bytePerBlk = 32;
    constexpr uint32_t elePerBlk = bytePerBlk / sizeof(T);
    if constexpr (isReuseSource) {
        if (last >= elePerBlk && remain != 0) {
            SetVectorMask<T, MaskMode::COUNTER>(0, remain);
            Mul<T, false>(currBuff, currBuff, currBuff[splitK], MASK_PLACEHOLDER, 1, defaultParam);
            PipeBarrier<PIPE_V>();
        }
    } else {
        if (last >= elePerBlk && remain != 0) {
            SetVectorMask<T, MaskMode::COUNTER>(0, splitK);
            Adds<T, false>(currBuff, src[row * padLast], static_cast<T>(0), MASK_PLACEHOLDER, 1, defaultUnaryParam);
            PipeBarrier<PIPE_V>();

            SetVectorMask<T, MaskMode::COUNTER>(0, remain);
            Mul<T, false>(currBuff, currBuff, src[row * padLast + splitK], MASK_PLACEHOLDER, 1, defaultParam);
            PipeBarrier<PIPE_V>();
        } else if (splitK > elePerBlk) {
            splitK >>= 1;
            SetVectorMask<T, MaskMode::COUNTER>(0, splitK);

            Mul<T, false>(currBuff, src[row * padLast], src[row * padLast + splitK], MASK_PLACEHOLDER, 1, defaultParam);
            PipeBarrier<PIPE_V>();
        } else {
            SetVectorMask<T, MaskMode::COUNTER>(0, last);
            Adds<T, false>(currBuff, src[row * padLast], static_cast<T>(0), MASK_PLACEHOLDER, 1, defaultUnaryParam);
            PipeBarrier<PIPE_V>();
        }
    }
}

template <typename T, bool isReuseSource>
__aicore__ inline void ReduceProdByLastAxis(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LocalTensor<T>& tmp, uint32_t first, uint32_t last, uint32_t padLast)
{
    constexpr uint32_t bytePerBlk = 32;
    constexpr uint32_t elePerBlk = bytePerBlk / sizeof(T);
    constexpr uint32_t bytePerRep = 256;
    constexpr uint32_t elePerRep = bytePerRep / sizeof(T);

    BinaryRepeatParams defaultParam;
    UnaryRepeatParams defaultUnaryParam;
    BrcbRepeatParams defaultBrcbParam;
    LocalTensor<T> resBeforeGather = tmp[elePerRep];
    LocalTensor<T> finalResStored = isReuseSource ? src : resBeforeGather;

    uint32_t k = FindClosestPowerOfTwo(last);
    uint32_t splitK = 1 << k;
    uint32_t remain = last - splitK;
    SetMaskCount();
    if constexpr (!isReuseSource) {
        CheckTmpBufferSize(tmp.GetSize(), 0, tmp.GetSize());
    }
    for (uint32_t j = 0; j < first; j++) {
        uint32_t splitKCopy = splitK;
        LocalTensor<T> tmpRowRes = isReuseSource ? src[j * padLast] : resBeforeGather[j * elePerBlk];
        LocalTensor<T> tmpDst = isReuseSource ? src[j * elePerBlk] : resBeforeGather[j * elePerBlk];

        PreProcessReduceForAR<T, isReuseSource>(src, tmpRowRes, j, last, padLast, remain, splitKCopy);
        
        while (splitKCopy > elePerBlk) {
            splitKCopy >>= 1;
            SetVectorMask<T, MaskMode::COUNTER>(0, splitKCopy);

            Mul<T, false>(tmpRowRes, tmpRowRes, tmpRowRes[splitKCopy], MASK_PLACEHOLDER, 1, defaultParam);
            PipeBarrier<PIPE_V>();
        }

        Brcb(tmp, tmpRowRes, 1, defaultBrcbParam);
        PipeBarrier<PIPE_V>();
        uint32_t finalTail = last < elePerBlk ? splitK : elePerBlk;
        if (splitK != last && finalTail < elePerBlk) {
            SetVectorMask<T, MaskMode::COUNTER>(0, remain * elePerBlk);
            Mul<T, false>(tmp, tmp, tmp[splitK * elePerBlk], MASK_PLACEHOLDER, 1, defaultParam);
            PipeBarrier<PIPE_V>();
        }
        while (finalTail > 1) {
            finalTail >>= 1;
            SetVectorMask<T, MaskMode::COUNTER>(0, finalTail * elePerBlk);

            Mul<T, false>(tmp, tmp, tmp[finalTail * elePerBlk], MASK_PLACEHOLDER, 1, defaultParam);
            PipeBarrier<PIPE_V>();
        }
        SetVectorMask<T, MaskMode::COUNTER>(0, elePerBlk);
        Adds<T, false>(tmpDst, tmp, static_cast<T>(0), MASK_PLACEHOLDER, 1, defaultUnaryParam);
        PipeBarrier<PIPE_V>();
    }
    LocalTensor<uint32_t> tmpInt = tmp.template ReinterpretCast<uint32_t>();
    Duplicate(tmpInt, 1u, elePerRep);
    PipeBarrier<PIPE_V>();
    GatherMaskParams gatherMaskParam = {1, static_cast<uint16_t>(first), 1, 0};
    uint64_t rsvdCnt;
    GatherMask(dst, finalResStored, tmpInt, true, elePerBlk, gatherMaskParam, rsvdCnt);
}

template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceProdImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
                                      const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[],
                                      bool srcInnerPad)
{
    uint32_t last = srcShape[1];
    uint32_t first = srcShape[0];
    constexpr uint32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    uint32_t padLast = AlignUp(last, elePerBlk);
    static_assert(SupportType<T, float>(), "failed to check the data type, current api supports data type is float!");
    static_assert(SupportType<pattern, Pattern::Reduce::AR, Pattern::Reduce::RA>(), 
        "failed to check the reduce pattern, it only supports AR/RA pattern!");
    CHECK_FUNC_HIGHLEVEL_API(ReduceProd, (T, pattern), (dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad, padLast));
    LocalTensor<T> tmpDst = sharedTmpBuffer.ReinterpretCast<T>();
    if constexpr (IsSameType<pattern, Pattern::Reduce::AR>::value) {
        ReduceProdByLastAxis<T, isReuseSource>(dstTensor, srcTensor,tmpDst, first, last, padLast);
    } else {
        BinaryReduceByFirstAxis<T, isReuseSource, Mul<T, false>>(
            dstTensor, srcTensor, tmpDst, first, last, padLast);
    }
    SetMaskNorm();
    ResetMask();
}
} // namespace Internal
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_PROD_REDUCE_PROD_V220_IMPL_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_PROD_REDUCE_PROD_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_PROD_REDUCE_PROD_V220_IMPL_H__
#endif
