/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef IMPL_SIMT_API_CPP_KERNEL_SIMT_CAST_INTERFACE_IMPL_H
#define IMPL_SIMT_API_CPP_KERNEL_SIMT_CAST_INTERFACE_IMPL_H

#include "impl/simt_api/cpp/dav_c310/kernel_simt_cast_impl.h"
#include "impl/simt_api/cpp/dav_c310/kernel_simt_cast_sat_impl.h"

namespace AscendC {
namespace Simt {

#if defined(ASCENDC_CPU_DEBUG)
template <typename T, typename U, RoundMode roundMode, SatMode satMode>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Cast(U x)
{
    if constexpr (roundMode == RoundMode::CAST_EVEN || roundMode == RoundMode::CAST_ZERO ||
                  roundMode == RoundMode::CAST_FLOOR || roundMode == RoundMode::CAST_CEIL) {
        static_assert(SupportTypeSimtInternel<Tuple<U, T>, Tuple<float, int>, Tuple<int, float>, Tuple<float, int64_t>,
                                  Tuple<int64_t, float>, Tuple<float, half>, Tuple<float, bfloat16_t>>,
                      "Input type (U, T) only supports"
                      "[(float, int), (int, float), (float, int64), (int64, float), (float, half), (float, bfloat16)]");
    } else if constexpr (roundMode == RoundMode::CAST_NONE) {
        static_assert(SupportTypeSimtInternel<Tuple<U, T>, Tuple<half, float>, Tuple<bfloat16_t, float>>,
                      "Input type (U, T) only supports [(half, float), (bfloat16, float)]");
    } else {
        static_assert(roundMode == RoundMode::CAST_EVEN || roundMode == RoundMode::CAST_ZERO ||
                          roundMode == RoundMode::CAST_FLOOR || roundMode == RoundMode::CAST_CEIL ||
                          roundMode == RoundMode::CAST_NONE,
                      "Cast: An invalid RoundMode!");
    }
    return CastImpl<T, U, roundMode, satMode>(x);
}
#else
template <typename T, typename U, RoundMode roundMode, SatMode satMode>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Cast(U x)
{
    if constexpr (roundMode == RoundMode::CAST_RINT || roundMode == RoundMode::CAST_FLOOR ||
                  roundMode == RoundMode::CAST_CEIL || roundMode == RoundMode::CAST_ROUND ||
                  roundMode == RoundMode::CAST_TRUNC) {
        static_assert(
            SupportTypeSimtInternel<Tuple<U, T>, Tuple<half, int>, Tuple<half, uint32_t>, Tuple<float, int>, Tuple<float, uint32_t>,
                        Tuple<float, int64_t>, Tuple<float, uint64_t>, Tuple<bfloat16_t, int>,
                        Tuple<bfloat16_t, uint32_t>, Tuple<int, half>, Tuple<int, float>, Tuple<int, bfloat16_t>,
                        Tuple<uint32_t, half>, Tuple<uint32_t, float>, Tuple<uint32_t, bfloat16_t>,
                        Tuple<int64_t, float>, Tuple<uint64_t, float>, Tuple<half, float>, Tuple<half, bfloat16_t>,
                        Tuple<float, half>, Tuple<float, bfloat16_t>, Tuple<bfloat16_t, half>,
                        Tuple<bfloat16_t, float>>,
            "Input type (U, T) only supports"
            "[(half, int), (half, uint32), (float, int), (float, uint32), (float, int64), (float, uint64),"
            " (bfloat16, int), (bfloat16, uint32), (int, half), (int, float), (int, bfloat16), (uint32, half),"
            " (uint32, float), (uint32, bfloat16), (int64, float), (uint64, float), (half, float), (half, bfloat16),"
            " (float, half), (float, bfloat16), (bfloat16, half), (bfloat16, float)]");
    } else if constexpr (roundMode == RoundMode::CAST_ODD) {
        static_assert(SupportTypeSimtInternel<Tuple<U, T>, Tuple<float, half>>, "Input type (U, T) only supports (float, half)");
    } else if constexpr (roundMode == RoundMode::CAST_EVEN || roundMode == RoundMode::CAST_ZERO) {
        static_assert(SupportTypeSimtInternel<Tuple<U, T>, Tuple<float, int>, Tuple<int, float>, Tuple<float, int64_t>,
                                  Tuple<int64_t, float>, Tuple<float, half>, Tuple<float, bfloat16_t>>,
                      "Input type (U, T) only supports"
                      "[(float, int), (int, float), (float, int64), (int64, float), (float, half), (float, bfloat16)]");
    } else if constexpr (roundMode == RoundMode::CAST_NONE) {
        static_assert(SupportTypeSimtInternel<Tuple<U, T>, Tuple<half, float>, Tuple<bfloat16_t, float>>,
                      "Input type (U, T) only supports [(half, float), (bfloat16, float)]");
    } else {
        static_assert(roundMode == RoundMode::CAST_RINT || roundMode == RoundMode::CAST_FLOOR ||
                          roundMode == RoundMode::CAST_CEIL || roundMode == RoundMode::CAST_ROUND ||
                          roundMode == RoundMode::CAST_TRUNC || roundMode == RoundMode::CAST_ODD ||
                          roundMode == RoundMode::CAST_EVEN || roundMode == RoundMode::CAST_ZERO ||
                          roundMode == RoundMode::CAST_NONE,
                      "Cast: An invalid RoundMode!");
    }
    return CastImpl<T, U, roundMode, satMode>(x);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Round(T x)
{
    static_assert(SupportTypeSimtInternel<T, float, half, bfloat16_t>, "Input type only supports float, half, bfloat16.");
    return RoundImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Rint(T x)
{
    static_assert(SupportTypeSimtInternel<T, float, half, bfloat16_t>, "Input type only supports float, half, bfloat16.");
    return RintImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Floor(T x)
{
    static_assert(SupportTypeSimtInternel<T, float, half, bfloat16_t>, "Input type only supports float, half, bfloat16.");
    return FloorImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Ceil(T x)
{
    static_assert(SupportTypeSimtInternel<T, float, half, bfloat16_t>, "Input type only supports float, half, bfloat16.");
    return CeilImpl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Trunc(T x)
{
    static_assert(SupportTypeSimtInternel<T, float, half, bfloat16_t>, "Input type only supports float, half, bfloat16.");
    return TruncImpl(x);
}

}  // namespace Simt
}  // namespace AscendC
#endif  // IMPL_SIMT_API_CPP_KERNEL_SIMT_CAST_INTERFACE_IMPL_H
