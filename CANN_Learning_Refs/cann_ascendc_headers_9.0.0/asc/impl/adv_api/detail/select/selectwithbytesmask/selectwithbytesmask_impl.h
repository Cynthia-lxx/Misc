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
#pragma message("impl/adv_api/detail/select/selectwithbytesmask/selectwithbytesmask_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/select/selectwithbytesmask.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECT_SELECTWITHBYTESMASK_SELECTWITHBYTESMASK_IMPL_H__
#endif
#ifndef IMPL_SELECT_SELECT_WITH_BYTES_MASK_IMPL_H
#define IMPL_SELECT_SELECT_WITH_BYTES_MASK_IMPL_H

#include "kernel_tensor.h"
#include "kernel_utils.h"
#include "selectwithbytesmask_common_impl.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/select/selectwithbytesmask/selectwithbytesmaskcheck.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "selectwithbytesmask_v220_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "selectwithbytesmask_v200_impl.h"
#endif

namespace AscendC {
// Selects Values from two sources and put into dst according to the mask values.
// True: Select scalar, False: select src.
template <typename T, typename U, bool isReuseMask, bool reverse = false>
__aicore__ inline __inout_pipe__(V) void SelectWithBytesMaskImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src0,
    T src1, const LocalTensor<U>& mask, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SelectWithBytesMaskShapeInfo& info)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(SelectWithBytesMask, (T, U, isReuseMask, reverse), (dst, src0, src1, mask, sharedTmpBuffer, info));
    PipeBarrier<PIPE_V>();
    constexpr uint32_t MIN_REQUIRED_BUFFER = 1024;
    constexpr uint32_t RESERVED_BUFFER = 256;
    constexpr uint32_t MAX_CALC_BYTE_PER_LOOP = 255 * ONE_REPEAT_BYTE_SIZE;
    const uint32_t firstAxis = info.firstAxis;
    const uint32_t srcLastAxis = info.srcLastAxis;
    const uint32_t maskLastAxis = info.maskLastAxis;
    const uint32_t srcSize = src0.GetSize();

    uint32_t bufferSize = sharedTmpBuffer.GetSize();
    ASCENDC_ASSERT((bufferSize >= MIN_REQUIRED_BUFFER), { KERNEL_LOG(KERNEL_ERROR,
        "Insufficient temporary space, current operation is not enough, sharedTmpBuffer size must >= 1024B!"); });
    LocalTensor<U> tmpMask = mask;
    LocalTensor<T> tmpTensor = sharedTmpBuffer.ReinterpretCast<T>();
    uint32_t tmpBufferOffset = 0;
    if constexpr (!isReuseMask) {
        if (srcLastAxis != maskLastAxis) {
            const uint32_t tmpMaskRequiredBuffer = ComputeMaskExtraBufSize(srcSize, sizeof(U));
            ASCENDC_ASSERT((bufferSize >= MIN_REQUIRED_BUFFER + tmpMaskRequiredBuffer), {
                KERNEL_LOG(KERNEL_ERROR, "Insufficient temporary space, current operation is not enough, "
                    "unaligned axis and do not reuse source must provide %d buffer",
                    MIN_REQUIRED_BUFFER + tmpMaskRequiredBuffer);
            });
            tmpMask = sharedTmpBuffer.template ReinterpretCast<U>();
            tmpMask.SetSize(tmpMaskRequiredBuffer / sizeof(U));
            bufferSize -= tmpMaskRequiredBuffer;
            tmpBufferOffset = tmpMaskRequiredBuffer;
        }
    }
    // Remove tmp buffer, which is reserved for tail part computation.
    bufferSize -= RESERVED_BUFFER;
    uint32_t loopSize = bufferSize / sizeof(half) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    if (loopSize > MAX_CALC_BYTE_PER_LOOP / sizeof(half)) {
        loopSize = MAX_CALC_BYTE_PER_LOOP / sizeof(half);
    }

    SetMaskCount();
    InitScalarSelectMask(tmpTensor, src1);

    SelectWithBytesMaskProcess<T, U, reverse>(dst, src0, src1, mask, tmpMask, sharedTmpBuffer, info, tmpBufferOffset,
        loopSize);
    SetMaskNorm();
    ResetMask();
}

template <typename T, typename U, bool isReuseMask = true>
__aicore__ inline void SelectWithBytesMask(const LocalTensor<T>& dst, const LocalTensor<T>& src0, T src1,
    const LocalTensor<U>& mask, const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
{
    SelectWithBytesMaskImpl<T, U, isReuseMask, false>(dst, src0, src1, mask, sharedTmpBuffer, info);
}

template <typename T, typename U, bool isReuseMask = true>
__aicore__ inline void SelectWithBytesMask(const LocalTensor<T>& dst, T src0, const LocalTensor<T>& src1,
    const LocalTensor<U>& mask, const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
{
    SelectWithBytesMaskImpl<T, U, isReuseMask, true>(dst, src1, src0, mask, sharedTmpBuffer, info);
}
} // namespace AscendC
#endif // IMPL_SELECT_SELECT_WITH_BYTES_MASK_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECT_SELECTWITHBYTESMASK_SELECTWITHBYTESMASK_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECT_SELECTWITHBYTESMASK_SELECTWITHBYTESMASK_IMPL_H__
#endif
