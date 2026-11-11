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
 * \file kernel_simt_cast_sat_impl.h
 * \brief
 */
#ifndef ASCENDC_MODULE_SIMT_CAST_SAT_IMPL_H
#define ASCENDC_MODULE_SIMT_CAST_SAT_IMPL_H

namespace AscendC {
namespace Simt {

#if !defined(ASCENDC_CPU_DEBUG)
template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastRintSat(U x)
{
    T y;
    if constexpr (std::is_same<T, int32_t>::value) {
        y = __cvt_int32_t<ROUND::R, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, uint32_t>::value) {
        y = __cvt_uint32_t<ROUND::R, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<int64_t, float>>::value) {
        y = __cvt_int64_t<ROUND::R, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<uint64_t, float>>::value) {
        y = __cvt_uint64_t<ROUND::R, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::R, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::R, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::R, RoundingSaturation::RS_ENABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastFloorSat(U x)
{
    T y;
    if constexpr (std::is_same<T, int32_t>::value) {
        y = __cvt_int32_t<ROUND::F, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, uint32_t>::value) {
        y = __cvt_uint32_t<ROUND::F, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<int64_t, float>>::value) {
        y = __cvt_int64_t<ROUND::F, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<uint64_t, float>>::value) {
        y = __cvt_uint64_t<ROUND::F, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::F, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::F, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::F, RoundingSaturation::RS_ENABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastCeilSat(U x)
{
    T y;
    if constexpr (std::is_same<T, int32_t>::value) {
        y = __cvt_int32_t<ROUND::C, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, uint32_t>::value) {
        y = __cvt_uint32_t<ROUND::C, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<int64_t, float>>::value) {
        y = __cvt_int64_t<ROUND::C, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<uint64_t, float>>::value) {
        y = __cvt_uint64_t<ROUND::C, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::C, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::C, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::C, RoundingSaturation::RS_ENABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastRoundSat(U x)
{
    T y;
    if constexpr (std::is_same<T, int32_t>::value) {
        y = __cvt_int32_t<ROUND::A, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, uint32_t>::value) {
        y = __cvt_uint32_t<ROUND::A, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<int64_t, float>>::value) {
        y = __cvt_int64_t<ROUND::A, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<uint64_t, float>>::value) {
        y = __cvt_uint64_t<ROUND::A, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::A, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::A, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::A, RoundingSaturation::RS_ENABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastTruncSat(U x)
{
    T y;
    if constexpr (std::is_same<T, int32_t>::value) {
        y = __cvt_int32_t<ROUND::Z, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, uint32_t>::value) {
        y = __cvt_uint32_t<ROUND::Z, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<int64_t, float>>::value) {
        y = __cvt_int64_t<ROUND::Z, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<Tuple<T, U>, Tuple<uint64_t, float>>::value) {
        y = __cvt_uint64_t<ROUND::Z, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::Z, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::Z, RoundingSaturation::RS_ENABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::Z, RoundingSaturation::RS_ENABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastRintNoSat(U x)
{
    T y;
    if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::R, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::R, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::R, RoundingSaturation::RS_DISABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastFloorNoSat(U x)
{
    T y;
    if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::F, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::F, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::F, RoundingSaturation::RS_DISABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastCeilNoSat(U x)
{
    T y;
    if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::C, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::C, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::C, RoundingSaturation::RS_DISABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastRoundNoSat(U x)
{
    T y;
    if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::A, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::A, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::A, RoundingSaturation::RS_DISABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastTruncNoSat(U x)
{
    T y;
    if constexpr (std::is_same<T, half>::value) {
        y = __cvt_half<ROUND::Z, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, float>::value) {
        y = __cvt_float<ROUND::Z, RoundingSaturation::RS_DISABLE_VALUE>(x);
    } else if constexpr (std::is_same<T, bfloat16_t>::value) {
        y = __cvt_bfloat16_t<ROUND::Z, RoundingSaturation::RS_DISABLE_VALUE>(x);
    }
    return y;
}

template <typename T, typename U, RoundMode roundMode>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastSat(U x)
{
    T y;
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            y = CastRintSat<T, U>(x);
            break;
        case RoundMode::CAST_FLOOR:
            y = CastFloorSat<T, U>(x);
            break;
        case RoundMode::CAST_CEIL:
            y = CastCeilSat<T, U>(x);
            break;
        case RoundMode::CAST_ROUND:
            y = CastRoundSat<T, U>(x);
            break;
        case RoundMode::CAST_TRUNC:
            y = CastTruncSat<T, U>(x);
            break;
        case RoundMode::CAST_ODD:
            if constexpr (std::is_same<Tuple<T, U>, Tuple<half, float>>::value) {
                y = __cvt_half<ROUND::O, RoundingSaturation::RS_ENABLE_VALUE>(x);
            }
            break;
        default:
            break;
    }
    return y;
}

template <typename T, typename U, RoundMode roundMode>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CastNoSat(U x)
{
    T y;
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            y = CastRintNoSat<T, U>(x);
            break;
        case RoundMode::CAST_FLOOR:
            y = CastFloorNoSat<T, U>(x);
            break;
        case RoundMode::CAST_CEIL:
            y = CastCeilNoSat<T, U>(x);
            break;
        case RoundMode::CAST_ROUND:
            y = CastRoundNoSat<T, U>(x);
            break;
        case RoundMode::CAST_TRUNC:
            y = CastTruncNoSat<T, U>(x);
            break;
        case RoundMode::CAST_ODD:
            if constexpr (std::is_same<Tuple<T, U>, Tuple<half, float>>::value) {
                y = __cvt_half<ROUND::O, RoundingSaturation::RS_DISABLE_VALUE>(x);
            }
            break;
        default:
            break;
    }
    return y;
}
#endif

}  // namespace Simt
}  // namespace AscendC
#endif  // ASCENDC_MODULE_SIMT_CAST_IMPL_H