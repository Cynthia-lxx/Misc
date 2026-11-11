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
 * \file bitwise_not_c310_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/bitwise_not/bitwise_not_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/bitwise_not.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_NOT_BITWISE_NOT_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_BITWISE_NOT_BITWISE_NOT_C310_IMPL_H
#define IMPL_MATH_BITWISE_NOT_BITWISE_NOT_C310_IMPL_H
#include "kernel_basic_intf.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/bitwise_not/bitwise_not_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
struct BitwiseNotConfig {
    bool isReuseSource;
};
constexpr BitwiseNotConfig DEFAULT_BITWISE_NOT_CONFIG = {false};
template <typename T, typename RegT, const Reg::RegTrait& Trait = Reg::RegTraitNumOne>
__simd_vf__ inline void BitwiseNotCompute(__ubuf__ T* dst, __ubuf__ T* src, uint32_t count,
                                         uint16_t repeatTime, uint32_t oneRepElm, uint32_t offset)
{
    Reg::MaskReg mask;
    RegT srcVreg;
    RegT dstVreg;

    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, Trait>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        Reg::Not(dstVreg, srcVreg, mask);
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
        mask = Reg::UpdateMask<T, Trait>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm + offset);
        Reg::Not(dstVreg, srcVreg, mask);
        Reg::StoreAlign(dst + i * oneRepElm + offset, dstVreg, mask);
    }
    mask = Reg::UpdateMask<T, Trait>(count);
    Reg::LoadAlign(srcVreg, src + repeatTime * oneRepElm * 2);
    Reg::Not(dstVreg, srcVreg, mask);
    Reg::StoreAlign(dst + repeatTime * oneRepElm * 2, dstVreg, mask);
}

template <const BitwiseNotConfig& config, typename T>
__aicore__ inline void BitwiseNotImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
    if ASCEND_IS_AIC {
        return;
    }

    static_assert(SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(),
                  "current data type is not supported on current device!");
    CHECK_FUNC_HIGHLEVEL_API(BitwiseNot, (T, config.isReuseSource), (dst, src, count));

    __ubuf__ T* dstTensor = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* srcTensor = (__ubuf__ T*)src.GetPhyAddr();

    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T) * 2);
        uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm) / 2);
        uint32_t offset = repeatTime * oneRepElm;
        BitwiseNotCompute<T, Reg::RegTensor<T, Reg::RegTraitNumTwo>, Reg::RegTraitNumTwo>(
            dstTensor, srcTensor, count, repeatTime, oneRepElm, offset);
    } else {
        constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
        uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm) / 2);
        uint32_t offset = repeatTime * oneRepElm;
        BitwiseNotCompute<T, Reg::RegTensor<T>>(dstTensor, srcTensor, count, repeatTime, oneRepElm,
                                                              offset);
    }
}
} // namespace AscendC
#endif // IMPL_MATH_BITWISE_NOT_BITWISE_NOT_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_NOT_BITWISE_NOT_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_NOT_BITWISE_NOT_C310_IMPL_H__
#endif
