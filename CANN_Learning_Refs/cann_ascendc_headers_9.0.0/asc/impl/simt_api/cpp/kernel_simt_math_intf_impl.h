/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef IMPL_SIMT_API_CPP_KERNEL_SIMT_MATH_INTERFACE_IMPL_H
#define IMPL_SIMT_API_CPP_KERNEL_SIMT_MATH_INTERFACE_IMPL_H

#include "impl/simt_api/cpp/dav_c310/kernel_simt_math_impl.h"

namespace AscendC {
namespace Simt {
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Abs(T x)
{
    static_assert(SupportTypeSimtInternel<T, int32_t, int64_t, half, float>, "Input type only supports int32_t, int64_t, half, float.");
    return AbsImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T UintDiv(T dividend, T magic, T shift)
{
    static_assert(SupportTypeSimtInternel<T, uint32_t, uint64_t>, "Input type T only supports uint32_t, uint64_t.");
    return UintDivImpl(dividend, magic, shift);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Fma(T x, T y, T z)
{
    static_assert(SupportTypeSimtInternel<T, half, float>, "Input type only supports half, float.");
    return FmaImpl(x, y, z);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Max(T x, T y)
{
    static_assert(
        SupportTypeSimtInternel<T, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, half, float>,
        "Input type only supports int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, half, float.");
    return MaxImpl(x, y);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Min(T x, T y)
{
    static_assert(
    SupportTypeSimtInternel<T, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, half, float>,
    "Input type only supports int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, half, float.");
    return MinImpl(x, y);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Fdim(T x, T y)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type T only supports float.");
    return DimImpl(x, y);
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T RemQuo(T x, T y, U *quo)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type T only supports float.");
    static_assert(SupportTypeSimtInternel<U, int32_t>, "Input type U only supports int32_t.");
    return RemQuoImpl(x, y, quo);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Mod(T x, T y)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type T only supports float.");
    return ModImpl(x, y);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Remainder(T x, T y)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type T only supports float.");
    return RemainderImpl(x, y);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CopySign(T x, T y)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type T only supports float.");
    return CopySignImpl(x, y);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T NearbyInt(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type T only supports float.");
    return NearByIntImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T NextAfter(T x, T y)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type T only supports float.");
    return NextAfterImpl(x, y);
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ScaLbn(T x, U n)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type T only supports float.");
    static_assert(SupportTypeSimtInternel<U, int32_t, int64_t>, "Input type U only supports int32_t, int64_t.");
    return ScaLbnImpl(x, n);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Brev(T x)
{
    static_assert(SupportTypeSimtInternel<T, uint32_t, uint64_t>, "Input type T only supports uint32_t, uint64_t.");
    return BrevImpl(x);
}

// count the leading zero bits
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t Clz(T x)
{
    static_assert(SupportTypeSimtInternel<T, int32_t, int64_t, uint32_t, uint64_t>,
        "Input type of Clz function only supports int32_t, uint32_t, int64_t, uint64_t.");
    return ClzImpl(x);
}

// count the number of set 1 bit
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t Popc(T x)
{
    static_assert(SupportTypeSimtInternel<T, uint32_t, uint64_t>, "Input type T only supports uint32_t, uint64_t.");
    return PopcImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T BytePerm(T x, T y, T s)
{
    static_assert(SupportTypeSimtInternel<T, uint32_t>, "Input type T only supports uint32_t.");
    return BytePermImpl(x, y, s);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t Ffs(T x)
{
    static_assert(SupportTypeSimtInternel<T, int32_t, int64_t>, "Input type T only supports int32_t, int64_t.");
    return FfsImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T MulHi(T x, T y)
{
    static_assert(SupportTypeSimtInternel<T, int32_t, uint32_t>, "Input type T only supports int32_t, uint32_t.");
    return MulHiImpl(x, y);
}
}  // namespace Simt
}  // namespace AscendC
#endif  // IMPL_SIMT_API_CPP_KERNEL_SIMT_MATH_INTERFACE_IMPL_H
