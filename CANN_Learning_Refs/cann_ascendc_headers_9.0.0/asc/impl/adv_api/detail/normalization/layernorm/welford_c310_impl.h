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
 * \file welford_c310_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/normalization/layernorm/welford_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_WELFORD_C310_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_WELFORD_C310_IMPL_H
#define IMPL_NORMALIZATION_WELFORD_C310_IMPL_H

#include "kernel_tensor.h"
#include "kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/normalization/normalize.h"
#include "layernorm_c310_utils.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/normalization/welforupdate/welfordupdate_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {

// Unified helper function for repeated WelfordUpdateImplForB16VF/B32VF core logic
template <typename T, bool IsB16>
__simd_callee__ inline void WelfordUpdateImplForVFCommon(Reg::MaskReg& preg, Reg::RegTensor<float>& meanVreg,
    Reg::RegTensor<float>& varVreg, Reg::RegTensor<float>& tmpVreg,
    typename std::conditional<IsB16, Reg::RegTensor<float>&, Reg::RegTensor<T>&>::type srcVreg,
    Reg::RegTensor<float>& outMeanreg, Reg::RegTensor<float>& outVarreg, Reg::RegTensor<float>& f32vreg,
    __ubuf__ float* const outMean, __ubuf__ float* const outVar, __ubuf__ float* const inMean,
    __ubuf__ float* const inVar, uint32_t offset, float nRec, uint32_t sreg)
{
    Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(meanVreg, inMean + offset);
    Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(varVreg, inVar + offset);
    Reg::Sub(tmpVreg, srcVreg, meanVreg, preg);
    Reg::Muls(outMeanreg, tmpVreg, nRec, preg);
    Reg::Add(outMeanreg, outMeanreg, meanVreg, preg);
    Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(outMean + offset, outMeanreg, preg);

    Reg::Sub(f32vreg, srcVreg, outMeanreg, preg);
    Reg::Mul(f32vreg, tmpVreg, f32vreg, preg);
    Reg::Add(outVarreg, f32vreg, varVreg, preg);
    Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(outVar + offset, outVarreg, preg);
}

// Helper for in-place copy logic in B16/B32
__simd_callee__ inline void WelfordUpdateImplInplaceCopy(Reg::MaskReg& preg, Reg::RegTensor<float>& meanVreg,
    Reg::RegTensor<float>& varVreg, __ubuf__ float* const outMean, __ubuf__ float* const outVar,
    __ubuf__ float* const inMean, __ubuf__ float* const inVar, uint32_t abLength, uint32_t inPlaceLength,
    uint16_t repeatInplace, uint32_t sregLower, uint32_t dstOffset)
{
    for (uint16_t i = 0; i < 1; ++i) {
        uint32_t sreg = inPlaceLength;
        uint32_t rowOffset = i * abLength;
        for (uint16_t j = 0; j < repeatInplace; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            uint32_t srcOffset = rowOffset + j * sregLower;
            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(meanVreg, inMean + dstOffset + srcOffset);
            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(varVreg, inVar + dstOffset + srcOffset);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(
                outMean + dstOffset + srcOffset, meanVreg, preg);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(
                outVar + dstOffset + srcOffset, varVreg, preg);
        }
    }
}

// VF helper extracted from WelfordUpdateImplForB16
template <typename T, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__simd_vf__ inline void WelfordUpdateImplForB16VF(__ubuf__ float* const outMean, __ubuf__ float* const outVar,
    __ubuf__ T* const src, __ubuf__ float* const inMean, __ubuf__ float* const inVar,
    const WelfordUpdateParam para, const uint16_t sregLowerB32, const uint32_t sregLower, const uint32_t K)
{
    Reg::MaskReg preg;

    Reg::RegTensor<T> b16vreg, vreg1, vreg2;
    Reg::RegTensor<float> f32vreg, tmpVreg, srcVreg, meanVreg, varVreg, outMeanreg, outVarreg;
    Reg::RegTensor<uint16_t> zeroReg;

    if constexpr (config.isInplace) {
        uint32_t inPlaceLength = AlignUp(para.abLength - para.abComputeLength, 8);
        uint16_t repeatInplace = static_cast<uint16_t>(CeilDivision(inPlaceLength, Internal::LAYERNORM_B32_VF_LEN));
        uint32_t dstOffset = para.abLength - inPlaceLength;
        WelfordUpdateImplInplaceCopy(preg, meanVreg, varVreg, outMean, outVar, inMean, inVar, para.abLength,
            inPlaceLength, repeatInplace, sregLower, dstOffset);
    }

    Reg::Duplicate(zeroReg, (uint16_t)0x0000);
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(K, sregLower));
    for (uint16_t i = 0; i < 1; ++i) {
        uint32_t rowOffset = i * para.abLength;
        uint32_t sreg = static_cast<uint32_t>(K);
        for (uint16_t j = 0; j < static_cast<uint16_t>(repeat); ++j) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_NORM>(b16vreg, src + rowOffset + j * sregLower);
            Reg::Interleave<uint16_t>((Reg::RegTensor<uint16_t>&)vreg1, (Reg::RegTensor<uint16_t>&)vreg2,
                (Reg::RegTensor<uint16_t>&)b16vreg, (Reg::RegTensor<uint16_t>&)zeroReg);

            // First half (64 F32 elements)
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::Cast<float, T, layoutZMrgZ>(srcVreg, vreg1, preg);
            WelfordUpdateImplForVFCommon<T, true>(preg, meanVreg, varVreg, tmpVreg, srcVreg, outMeanreg, outVarreg,
                f32vreg, outMean, outVar, inMean, inVar, rowOffset + (2 * j) * sregLowerB32,
                static_cast<float>(para.nRec), sreg);

            // Second half
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::Cast<float, T, layoutZMrgZ>(srcVreg, vreg2, preg);
            WelfordUpdateImplForVFCommon<T, true>(preg, meanVreg, varVreg, tmpVreg, srcVreg, outMeanreg, outVarreg,
                f32vreg, outMean, outVar, inMean, inVar, rowOffset + (2 * j + 1) * sregLowerB32,
                static_cast<float>(para.nRec), sreg);
        }
    }
}

// VF helper extracted from WelfordUpdateImplForB32
template <typename T, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__simd_vf__ inline void WelfordUpdateImplForB32VF(__ubuf__ float* const outMean, __ubuf__ float* const outVar,
    __ubuf__ T* const src, __ubuf__ float* const inMean, __ubuf__ float* const inVar,
    const WelfordUpdateParam para, const uint32_t sregLower, const uint32_t K)
{
    Reg::MaskReg preg;
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<float> tmpVreg;

    Reg::RegTensor<float> meanVreg;
    Reg::RegTensor<float> varVreg;
    Reg::RegTensor<float> outMeanreg;
    Reg::RegTensor<float> outVarreg;

    if constexpr (config.isInplace) {
        uint32_t inPlaceLength = AlignUp(para.abLength - para.abComputeLength, 8);
        uint16_t repeatInplace = static_cast<uint16_t>(CeilDivision(inPlaceLength, Internal::LAYERNORM_B32_VF_LEN));
        uint32_t dstOffset = para.abLength - inPlaceLength;
        WelfordUpdateImplInplaceCopy(preg, meanVreg, varVreg, outMean, outVar, inMean, inVar, para.abLength,
            inPlaceLength, repeatInplace, sregLower, dstOffset);
    }

    uint16_t repeat = static_cast<uint16_t>(CeilDivision(K, sregLower));
    for (uint16_t i = 0; i < 1; ++i) {
        uint32_t rowOffset = i * para.abLength;
        uint32_t sreg = static_cast<uint32_t>(K);
        for (uint16_t j = 0; j < static_cast<uint16_t>(repeat); ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            uint32_t offset = rowOffset + j * sregLower;
            Reg::LoadAlign<T, Reg::LoadDist::DIST_NORM>(srcVreg, src + offset);
            WelfordUpdateImplForVFCommon<T, false>(preg, meanVreg, varVreg, tmpVreg, srcVreg, outMeanreg, outVarreg,
                f32vreg, outMean, outVar, inMean, inVar, offset, static_cast<float>(para.nRec), sreg);
        }
    }
}

template <typename T, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__aicore__ inline void WelfordUpdateImplForB16(__ubuf__ float* const outMean, __ubuf__ float* const outVar,
    __ubuf__ T* const src, __ubuf__ float* const inMean, __ubuf__ float* const inVar,
    const WelfordUpdateParam& para)
{
    const uint16_t sregLowerB32 = static_cast<uint16_t>(GetVecLen() / sizeof(float)); // 64
    const uint32_t sregLower = static_cast<uint32_t>(Internal::LAYERNORM_B16_VF_LEN);
    const uint32_t K = para.abComputeLength;

    WelfordUpdateImplForB16VF<T, config>(
        outMean, outVar, src, inMean, inVar, para, sregLowerB32, sregLower, K);
}

template <typename T, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__aicore__ inline void WelfordUpdateImplForB32(__ubuf__ float* const outMean, __ubuf__ float* const outVar,
    __ubuf__ T* const src, __ubuf__ float* const inMean, __ubuf__ float* const inVar,
    const WelfordUpdateParam& para)
{
    const uint32_t sregLower = static_cast<uint32_t>(Internal::LAYERNORM_B32_VF_LEN);
    const uint32_t K = para.abComputeLength;

    WelfordUpdateImplForB32VF<T, config>(outMean, outVar, src, inMean, inVar, para, sregLower, K);
}

template <typename T, typename U = float, bool isReuseSource = false,
    const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__aicore__ inline void WelfordUpdateImpl(const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance,
    const LocalTensor<U>& inputMean, const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX,
    const WelfordUpdateParam& para)
{
    CHECK_FUNC_HIGHLEVEL_API(WelfordUpdate, (T, U, isReuseSource, config),
        (outputMean, outputVariance, inputMean, inputVariance, inputX, para));

    static_assert(SupportType<T, half, bfloat16_t, float>(), "current data type is not supported on current device!");
    static_assert(SupportType<U, float>(), "current data type is not supported on current device!");
    __ubuf__ T* srcUb = (__ubuf__ T*)inputX.GetPhyAddr();
    __ubuf__ float* inMean = (__ubuf__ float*)inputMean.GetPhyAddr();
    __ubuf__ float* inVar = (__ubuf__ float*)inputVariance.GetPhyAddr();
    __ubuf__ float* outMean = (__ubuf__ float*)outputMean.GetPhyAddr();
    __ubuf__ float* outVar = (__ubuf__ float*)outputVariance.GetPhyAddr();

    if constexpr (SupportType<T, half, bfloat16_t>()) {
        WelfordUpdateImplForB16<T, config>(outMean, outVar, srcUb, inMean, inVar, para);
    } else { // fp32
        WelfordUpdateImplForB32<T, config>(outMean, outVar, srcUb, inMean, inVar, para);
    }
}
template <typename T, typename U = float, bool isReuseSource = false,
    const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__aicore__ inline void WelfordUpdateImpl(const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance,
    const LocalTensor<U>& inputMean, const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const WelfordUpdateParam& para)
{
    WelfordUpdateImpl<T, float, isReuseSource, config>(
        outputMean, outputVariance, inputMean, inputVariance, inputX, para);
}

} // namespace AscendC
#endif // IMPL_NORMALIZATION_WELFORD_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_WELFORD_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_WELFORD_C310_IMPL_H__
#endif
