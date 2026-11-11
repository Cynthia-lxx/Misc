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
 * \file fma_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/fma/fma_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/fma.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FMA_FMA_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_FMA_FMA_COMMON_IMPL_H
#define IMPL_MATH_FMA_FMA_COMMON_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/fma/fma_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
struct FmaConfig {
    bool isReuseSource;
};
constexpr FmaConfig DEFAULT_FMA_CONFIG = { false };

template <typename T>
__simd_vf__ inline void FmaImplVF(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, __ubuf__ T* src2,
    uint32_t count, uint16_t repeatTimes)
{
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    Reg::RegTensor<T> srcVreg0;
    Reg::RegTensor<T> srcVreg1;
    Reg::RegTensor<T> srcVreg2;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<T>(count);
        Reg::LoadAlign(srcVreg0, src0 + i * oneRepElm);
        Reg::LoadAlign(srcVreg1, src1 + i * oneRepElm);
        Reg::LoadAlign(srcVreg2, src2 + i * oneRepElm);
        Reg::FusedMulDstAdd(srcVreg0, srcVreg1, srcVreg2, mask);
        Reg::StoreAlign(dst + i * oneRepElm, srcVreg0, mask);
    }
}

template <const FmaConfig& config, typename T>
__aicore__ inline void FmaImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src0,
    const LocalTensor<T>& src1, const LocalTensor<T>& src2, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(SupportType<T, half, float>(), "Fma only support half/float data type on current device!");
    CHECK_FUNC_HIGHLEVEL_API(Fma, (T, config.isReuseSource), (dst, src0, src1, src2, sharedTmpBuffer, count));
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
    FmaImplVF<T>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src0.GetPhyAddr(),
        (__ubuf__ T*)src1.GetPhyAddr(), (__ubuf__ T*)src2.GetPhyAddr(), count, repeatTimes);
}

template <const FmaConfig& config, typename T>
__aicore__ inline void FmaImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src0,
    const LocalTensor<T>& src1, const LocalTensor<T>& src2, const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    FmaImpl<config, T>(dst, src0, src1, src2, sharedTmpBuffer, count);
}
} // namespace AscendC
#endif // IMPL_MATH_FMA_FMA_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FMA_FMA_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FMA_FMA_COMMON_IMPL_H__
#endif
