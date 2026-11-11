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
 * \file dropout_c220_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/filter/dropout/dropout_c220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/filter/dropout.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_C220_IMPL_H__
#endif

#ifndef IMPL_FILTER_DROPOUT_DROPOUT_C220_IMPL_H
#define IMPL_FILTER_DROPOUT_DROPOUT_C220_IMPL_H

#include "dropout_membase_impl.h"

namespace AscendC {
template <typename T, bool isInitBitMode = false>
__aicore__ inline void DropOutBitMode(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<uint8_t>& maskLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T divValue,
    const DropOutShapeInfo& info)
{
    if constexpr (isInitBitMode == false) {
        DropOutBitModeInit(sharedTmpBuffer);
    }

    GatherMaskParams reducev2Params;
    reducev2Params.repeatTimes = info.firstAxis;
    reducev2Params.src0RepeatStride = info.maskLastAxis / ONE_BLK_SIZE;

    LocalTensor<uint16_t> maskTmpLocal = maskLocal.ReinterpretCast<uint16_t>();

    const uint32_t mask = info.srcLastAxis / ONE_BYTE_BIT_SIZE / sizeof(uint16_t);
    uint64_t rsvdCnt = 0;

    GatherMask<uint16_t>(maskTmpLocal, maskTmpLocal, REDUCEV2_MODE_SEVEN, true, mask, reducev2Params, rsvdCnt);
    PipeBarrier<PIPE_V>();
    SetMaskCount();

    DropOutBitMode<T, true>(dstLocal, srcLocal, maskLocal, sharedTmpBuffer, divValue,
        info.firstAxis * info.srcLastAxis);
}

template <typename T>
__aicore__ inline void DropOutByteMode(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<uint8_t>& maskLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T divValue,
    const DropOutShapeInfo& info)
{
    GatherMaskParams reducev2Params;
    reducev2Params.repeatTimes = info.firstAxis;
    reducev2Params.src0RepeatStride = info.maskLastAxis / ONE_BLK_SIZE;

    LocalTensor<uint16_t> maskTmpLocal = maskLocal.ReinterpretCast<uint16_t>();

    const uint32_t mask = info.srcLastAxis / sizeof(uint16_t);
    uint64_t rsvdCnt = 0;

    GatherMask<uint16_t>(maskTmpLocal, maskTmpLocal, REDUCEV2_MODE_SEVEN, true, mask, reducev2Params, rsvdCnt);
    PipeBarrier<PIPE_V>();
    SetMaskCount();

    DropOutByteMode(dstLocal, srcLocal, maskLocal, sharedTmpBuffer, divValue, info.firstAxis * info.srcLastAxis);
}
} // namespace AscendC
#endif // IMPL_FILTER_DROPOUT_DROPOUT_C220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_C220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_C220_IMPL_H__
#endif
