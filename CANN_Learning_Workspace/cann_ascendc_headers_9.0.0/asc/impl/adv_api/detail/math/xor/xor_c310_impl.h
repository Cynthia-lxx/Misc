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
* \file xor_c310_impl.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/xor/xor_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/xor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_XOR_XOR_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_XOR_XOR_C310_IMPL_H
#define IMPL_MATH_XOR_XOR_C310_IMPL_H
#include "kernel_basic_intf.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/xor/xor_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace XorAPI {
template<typename T>
__simd_vf__ inline void XorCompute(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1,
    uint32_t calCount, uint16_t repeatTimes)
{
    Reg::MaskReg mask;
    Reg::RegTensor<T> dstVreg;
    Reg::RegTensor<T> src0Vreg;
    Reg::RegTensor<T> src1Vreg;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<T>(calCount);
        Reg::LoadAlign(src0Vreg, src0 + i * oneRepSize);
        Reg::LoadAlign(src1Vreg, src1 + i * oneRepSize);
        Reg::Xor(dstVreg, src0Vreg, src1Vreg, mask);
        Reg::StoreAlign(dst + i * oneRepSize, dstVreg, mask);
    }
}
} // namespace XorAPI

template <typename T, bool isReuseSource = false>
__aicore__ inline void XorImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor,
   const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(Xor, (T, isReuseSource), (dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, calCount));
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
   IsXorParamValid(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, calCount);
#endif

    static_assert(SupportType<T, uint16_t, int16_t>(), "current data type is not supported on current device!");
    __ubuf__ T *dst = (__ubuf__ T *)dstTensor.GetPhyAddr();
    __ubuf__ T *src0 = (__ubuf__ T *)src0Tensor.GetPhyAddr();
    __ubuf__ T *src1 = (__ubuf__ T *)src1Tensor.GetPhyAddr();
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t repeatTimes = CeilDivision(calCount, oneRepSize);
    XorAPI::XorCompute<T>(dst, src0, src1, calCount, repeatTimes);
}
} // namespace AscendC
#endif // IMPL_MATH_XOR_XOR_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_XOR_XOR_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_XOR_XOR_C310_IMPL_H__
#endif
