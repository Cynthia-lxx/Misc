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
 * \file sort_common_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/sort/sort_common_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/topk.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_SORT_COMMON_UTILS_H__
#endif

#ifndef IMPL_SORT_SORT_COMMON_UTILS_H
#define IMPL_SORT_SORT_COMMON_UTILS_H

#include <cstdint>
#include "kernel_basic_intf.h"

namespace AscendC {
namespace Internal {
template <uint32_t size = sizeof(uint8_t)> struct ExtractTypeBySize {
    using T = uint8_t;
};

template <> struct ExtractTypeBySize<sizeof(uint16_t)> {
    using T = uint16_t;
};

template <> struct ExtractTypeBySize<sizeof(uint32_t)> {
    using T = uint32_t;
};

template <> struct ExtractTypeBySize<sizeof(uint64_t)> {
    using T = uint64_t;
};
} // namespace Internal

namespace Reg {
namespace Internal {

template <typename T>
__aicore__ constexpr bool IsNeedTwiddleType()
{
    return SupportType<T, int8_t, int16_t, int32_t, int64_t, float, half, bfloat16_t>();
}

template <typename T>
__aicore__ constexpr bool IsNeedTwiddleFpType()
{
    return SupportType<T, half, bfloat16_t, float>();
}

template <typename T, typename U, bool isDescend>
__simd_vf__ inline void TwiddleInData(__ubuf__ U *src, __ubuf__ U *dst, uint32_t count)
// Twiddle add data in ascending order.
// float and signed int data bit should be reorder into ascending.
// The following sort will be in ascending level, reverse decending data.
{
    uint16_t repeatTime = DivCeil(count, GetVecLen() / sizeof(T));
    constexpr uint32_t stride = GetVecLen() / sizeof(U);
    RegTensor<U> tmpReg, signBitReg, allFReg, dstReg;
    MaskReg cmpMask = CreateMask<U>();
    Duplicate(signBitReg, 1ul << (sizeof(T) * 8 - 1), cmpMask);
    Duplicate(allFReg, -1ul, cmpMask);
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<U>(count);
        DataCopy(dstReg, src + stride * i);
        if constexpr (IsNeedTwiddleType<T>()) {
            if constexpr (IsNeedTwiddleFpType<T>()) {
                And(tmpReg, dstReg, signBitReg, maskReg);
                CompareScalar<U, CMPMODE::NE>(cmpMask, tmpReg, 0, maskReg);
                Select(tmpReg, allFReg, signBitReg, cmpMask);
                Xor(dstReg, dstReg, tmpReg, maskReg);
            } else {
                // process signed int data.
                Xor(dstReg, dstReg, signBitReg, maskReg);
            }
        }
        if constexpr (isDescend) {
            Not(dstReg, dstReg, maskReg);
        }
        DataCopy(dst + stride * i, dstReg, maskReg);
    }
}

template <typename T, typename U, bool isDescend>
__simd_vf__ inline void TwiddleOutData(__ubuf__ U *src, __ubuf__ U *dst, uint32_t count)
// Revert twiddled data back.
{
    uint16_t repeatTime = DivCeil(count, GetVecLen() / sizeof(T));
    constexpr uint32_t stride = GetVecLen() / sizeof(U);
    RegTensor<U> tmpReg, signBitReg, allFReg, dstReg;
    MaskReg cmpMask = CreateMask<U>();
    Duplicate(signBitReg, 1ul << (sizeof(T) * 8 - 1), cmpMask);
    Duplicate(allFReg, -1ul, cmpMask);
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<U>(count);
        DataCopy<U>(dstReg, (__ubuf__ U *)src + stride * i);
        if constexpr (isDescend) {
            Not(dstReg, dstReg, maskReg);
        }
        if constexpr (IsNeedTwiddleType<T>()) {
            if constexpr (IsNeedTwiddleFpType<T>()) {
                And(tmpReg, dstReg, signBitReg, maskReg);
                CompareScalar<U, CMPMODE::EQ>(cmpMask, tmpReg, 0, maskReg);
                Select(tmpReg, allFReg, signBitReg, cmpMask);
                Xor(dstReg, dstReg, tmpReg, maskReg);
            } else {
                // process signed int data.
                Xor(dstReg, dstReg, signBitReg, maskReg);
            }
        }
        DataCopy(dst + stride * i, dstReg, maskReg);
    }
}

} // namespace Internal
} // namespace Reg

} // namespace AscendC
#endif // IMPL_SORT_SORT_COMMON_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_SORT_COMMON_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_SORT_COMMON_UTILS_H__
#endif
