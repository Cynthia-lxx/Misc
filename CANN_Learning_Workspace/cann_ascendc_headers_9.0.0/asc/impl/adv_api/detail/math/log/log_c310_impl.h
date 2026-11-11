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
 * \file log_c310_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/log/log_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/log.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LOG_LOG_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_LOG_LOG_C310_IMPL_H
#define IMPL_MATH_LOG_LOG_C310_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../common/common.h"

namespace AscendC {
template <typename T> __simd_vf__ inline void LogImpl(__ubuf__ T *dst, __ubuf__ T *src, const uint32_t calCount)
{
    static_assert((std::is_same_v<T, half> || std::is_same_v<T, float>),
        "current data type is not supported on current device!");
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    uint16_t repeatTimes = CeilDivision(calCount, sregLower);
    Reg::RegTensor<T> vreg0;
    Reg::RegTensor<T> vreg1;
    uint32_t sreg = calCount;
    Reg::MaskReg preg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        preg = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(vreg0, src + i * sregLower);
        Reg::Log(vreg1, vreg0, preg);
        Reg::StoreAlign(dst + i * sregLower, vreg1, preg);
    }
}

template <typename T>
__simd_vf__ inline void LogXImpl(__ubuf__ T *dst, __ubuf__ T *src, const uint32_t calCount, const float LnXRec)
{
    static_assert((std::is_same_v<T, half> || std::is_same_v<T, float>),
        "current data type is not supported on current device!");
    constexpr uint32_t sregLower = (uint32_t)(GetVecLen() / sizeof(float));
    uint16_t repeatTimes = CeilDivision(calCount, sregLower);
    static constexpr Reg::CastTrait castTraitB16ToB32 = {
        Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
    static constexpr Reg::CastTrait castTraitB32ToB16 = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
    if constexpr (std::is_same_v<T, float>) {
        Reg::RegTensor<float> vreg0;
        Reg::RegTensor<float> vreg1;
        uint32_t sreg = calCount;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            mask = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(vreg0, src + i * sregLower);
            Reg::Log(vreg1, vreg0, mask);
            Reg::Muls(vreg1, vreg1, LnXRec, mask);
            Reg::StoreAlign(dst + i * sregLower, vreg1, mask);
        }
    } else if constexpr (std::is_same_v<T, half>) {
        Reg::RegTensor<T> vreg0;
        Reg::RegTensor<T> dst0;
        Reg::RegTensor<T> dst1;
        Reg::RegTensor<float> vreg1;
        uint32_t sreg = calCount;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            mask = Reg::UpdateMask<float>(sreg);
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(vreg0, src + i * sregLower);
            Reg::Cast<float, half, castTraitB16ToB32>(vreg1, vreg0, mask);
            Reg::Log(vreg1, vreg1, mask);
            Reg::Muls(vreg1, vreg1, LnXRec, mask);
            Reg::Cast<half, float, castTraitB32ToB16>(dst0, vreg1, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + i * sregLower, dst0, mask);
        }
    }
}
} // namespace AscendC
#endif // IMPL_MATH_LOG_LOG_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LOG_LOG_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LOG_LOG_C310_IMPL_H__
#endif
