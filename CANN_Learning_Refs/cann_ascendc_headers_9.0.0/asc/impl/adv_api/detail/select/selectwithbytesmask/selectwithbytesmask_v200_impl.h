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
#pragma message("impl/adv_api/detail/select/selectwithbytesmask/selectwithbytesmask_v200_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/select/selectwithbytesmask.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECT_SELECTWITHBYTESMASK_SELECTWITHBYTESMASK_V200_IMPL_H__
#endif
#ifndef IMPL_SELECT_SELECT_WITH_BYTES_MASK_V200_IMPL_H
#define IMPL_SELECT_SELECT_WITH_BYTES_MASK_V200_IMPL_H
#include "kernel_tensor.h"
#include "kernel_utils.h"
#include "selectwithbytesmask_common_impl.h"

namespace AscendC {
__aicore__ inline uint32_t ComputeMaskExtraBufSize(const uint32_t srcSize, const uint32_t typeSize)
{
    (void)srcSize;
    (void)typeSize;
    return 0;
}

template <typename T, typename U, bool reverse = false>
__aicore__ inline void SelectWithBytesMaskProcess(const LocalTensor<T>& dst, const LocalTensor<T>& src0, T src1,
    const LocalTensor<U>& mask, const LocalTensor<U>& tmpMask, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SelectWithBytesMaskShapeInfo& info, const uint32_t tmpBufferOffset, const uint32_t loopSize)
{
    if (info.srcLastAxis == info.maskLastAxis) {
        SelectWithBytesMaskLoopImpl<T, U, reverse>(dst, src0, src1, mask, sharedTmpBuffer[tmpBufferOffset], loopSize,
            src0.GetSize(), 0, 0);
    } else {
        for (uint32_t iter = 0; iter < info.firstAxis; iter++) {
            SelectWithBytesMaskLoopImpl<T, U, reverse>(dst, src0, src1, mask, sharedTmpBuffer[tmpBufferOffset],
                loopSize, info.srcLastAxis, iter * info.srcLastAxis, iter * info.maskLastAxis);
        }
    }
}
} // namespace AscendC
#endif // IMPL_SELECT_SELECT_WITH_BYTES_MASK_V200_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECT_SELECTWITHBYTESMASK_SELECTWITHBYTESMASK_V200_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECT_SELECTWITHBYTESMASK_SELECTWITHBYTESMASK_V200_IMPL_H__
#endif
