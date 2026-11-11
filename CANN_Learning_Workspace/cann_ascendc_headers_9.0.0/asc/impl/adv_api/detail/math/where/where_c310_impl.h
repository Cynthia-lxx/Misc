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
 * \file where_c310_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/where/where_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/where.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_WHERE_WHERE_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_WHERE_WHERE_C310_IMPL_H
#define IMPL_MATH_WHERE_WHERE_C310_IMPL_H
#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/where/where_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace WhereInternal {
template <uint32_t size = sizeof(uint8_t)> struct ExtractDataTypeBySize {
    using T = uint8_t;
};

template <> struct ExtractDataTypeBySize<sizeof(uint16_t)> {
    using T = uint16_t;
};

template <> struct ExtractDataTypeBySize<sizeof(uint32_t)> {
    using T = uint32_t;
};

template <> struct ExtractDataTypeBySize<sizeof(uint64_t)> {
    using T = uint32_t;
};
}
template <bool src0Val, bool src1Val, typename T, typename V, const Reg::RegTrait& regTrait = Reg::RegTraitNumOne>
__simd_vf__ inline void WhereCompute(__ubuf__ T* dstUb, __ubuf__ T* src0Ub,
    __ubuf__ T* src1Ub, const T src0, const T src1, __ubuf__ V* conditionUb,
    uint32_t count, const uint16_t repeatTime)
{
    constexpr uint32_t repeatElm = regTrait.REG_NUM * GetVecLen() / sizeof(T);
    Reg::RegTensor<T, regTrait> src0Reg, src1Reg, dstReg;
    Reg::RegTensor<uint8_t> selReg;
    Reg::MaskReg maskReg, selMask;
    Reg::MaskReg maskFull = Reg::CreateMask<uint8_t>();

    if constexpr (src0Val) {
        Reg::Duplicate(src0Reg, src0);
    }
    if constexpr (src1Val) {
        Reg::Duplicate(src1Reg, src1);
    }
    for (uint16_t i = 0; i < repeatTime; ++i) {
        maskReg = Reg::UpdateMask<T, regTrait>(count);
        Reg::LoadAlign(selReg, (__ubuf__ uint8_t*)conditionUb + i * repeatElm);
        Reg::CompareScalar<uint8_t, CMPMODE::NE>(selMask, selReg, static_cast<uint8_t>(0), maskFull);
        if constexpr (sizeof(T) == 2) {
            Reg::MaskUnPack(selMask, selMask);
        } else if constexpr (sizeof(T) == 4 || sizeof(T) == 8) {
            Reg::MaskUnPack(selMask, selMask);
            Reg::MaskUnPack(selMask, selMask);
        }

        if constexpr (!src0Val) {
            Reg::LoadAlign(src0Reg, src0Ub + i * repeatElm);
        }
        if constexpr (!src1Val) {
            Reg::LoadAlign(src1Reg, src1Ub + i * repeatElm);
        }
        Reg::Select(dstReg, src0Reg, src1Reg, selMask);
        Reg::StoreAlign(dstUb + i * repeatElm, dstReg, maskReg);
    }
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void WhereImpl(const LocalTensor<T>& dst, const U& src0, const S& src1,
    const LocalTensor<V>& condition, const uint32_t count)
{
    static_assert(SupportType<T, bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t>(),
    "Where only supports bool/int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t data type on current device");
    static_assert(SupportType<V, bool>(), "Where's argument of condition only supports bool data type on current device");

    CHECK_FUNC_HIGHLEVEL_API(Where, (T, U, S, V), (dst, src0, src1, condition, count));
    using WhereType = typename WhereInternal::ExtractDataTypeBySize<sizeof(T)>::T;

    __ubuf__ V *conditionUb = (__ubuf__ V *)condition.GetPhyAddr();
    uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, GetVecLen() / sizeof(WhereType)));
    if constexpr (TypeUtils::IsLocalTensorType<U, S>()) {
        static_assert(Std::is_same<U, S>::value);
        static_assert(Std::is_same<T, typename U::PrimType>::value);
        if constexpr (sizeof(T) != 8) {
            WhereCompute<false, false, WhereType, V>((__ubuf__ WhereType*)dst.GetPhyAddr(),
            (__ubuf__ WhereType*)src0.GetPhyAddr(), (__ubuf__ WhereType*)src1.GetPhyAddr(), 0, 0, conditionUb, count, repeatTime);
        } else {
            WhereCompute<false, false, uint64_t, V, Reg::RegTraitNumTwo>((__ubuf__ uint64_t*)dst.GetPhyAddr(),
            (__ubuf__ uint64_t*)src0.GetPhyAddr(), (__ubuf__ uint64_t*)src1.GetPhyAddr(), 0, 0, conditionUb, count, repeatTime);
        }
    } else if constexpr (TypeUtils::IsLocalTensorType<U>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<T, S>::value);
        static_assert(Std::is_same<T, typename U::PrimType>::value);
        if constexpr (sizeof(T) != 8) {
            WhereCompute<false, true, WhereType, V>((__ubuf__ WhereType*)dst.GetPhyAddr(),
            (__ubuf__ WhereType*)src0.GetPhyAddr(), nullptr, 0, (WhereType&)src1, conditionUb, count, repeatTime);
        } else {
            WhereCompute<false, true, uint64_t, V, Reg::RegTraitNumTwo>((__ubuf__ uint64_t*)dst.GetPhyAddr(),
            (__ubuf__ uint64_t*)src0.GetPhyAddr(), nullptr, 0, (uint64_t&)src1, conditionUb, count, repeatTime);
        }
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<U>()) {
        static_assert(Std::is_same<T, U>::value);
        static_assert(Std::is_same<T, typename S::PrimType>::value);
        if constexpr (sizeof(T) != 8) {
            WhereCompute<true, false, WhereType, V>((__ubuf__ WhereType*)dst.GetPhyAddr(),
            nullptr, (__ubuf__ WhereType*)src1.GetPhyAddr(), (WhereType&)src0, 0, conditionUb, count, repeatTime);
        } else {
            WhereCompute<true, false, uint64_t, V, Reg::RegTraitNumTwo>((__ubuf__ uint64_t*)dst.GetPhyAddr(),
            nullptr, (__ubuf__ uint64_t*)src1.GetPhyAddr(), (uint64_t&)src0, 0, conditionUb, count, repeatTime);
        }
    } else {
        static_assert(Std::is_same<T, U>::value);
        static_assert(Std::is_same<T, S>::value);
        if constexpr (sizeof(T) != 8) {
            WhereCompute<true, true, WhereType, V>((__ubuf__ WhereType*)dst.GetPhyAddr(),
            nullptr, nullptr, (WhereType&)src0, (WhereType&)src1, conditionUb, count, repeatTime);
        } else {
            WhereCompute<true, true, uint64_t, V, Reg::RegTraitNumTwo>((__ubuf__ uint64_t*)dst.GetPhyAddr(),
            nullptr, nullptr, (uint64_t&)src0, (uint64_t&)src1, conditionUb, count, repeatTime);
        }
    }
}
} // namespace AscendC
#endif // IMPL_MATH_WHERE_WHERE_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_WHERE_WHERE_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_WHERE_WHERE_C310_IMPL_H__
#endif
