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
 * \file groupnorm_c310_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/normalization/groupnorm/groupnorm_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_GROUPNORM_GROUPNORM_C310_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_GROUPNORM_GROUPNORM_C310_IMPL_H
#define IMPL_NORMALIZATION_GROUPNORM_GROUPNORM_C310_IMPL_H

#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "kernel_tensor.h"
#include "kernel_pop_stack_buffer.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/normalization/groupnorm/groupnorm_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace GroupNormInternal {
constexpr Reg::CastTrait layoutZMrgZ = {Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN,
                                             Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait LayoutZMrgZRndRSatNS = {Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT,
                                                      Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

enum class ShapeScope {
    ONE = 1,  // less than 256B
    TWO = 2,  // less than 256 * 256B
    THREE = 3,  // less than 256 * 256 * 256B
};

template <typename T>
__simd_callee__ inline void LoadDataWithT(__ubuf__ T* src, Reg::RegTensor<float>& dstReg, Reg::MaskReg& mask,
                                          uint16_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> srcOrigin;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin, src + offset);
        Reg::Cast<float, T, layoutZMrgZ>(dstReg, srcOrigin, mask);
    } else {
        Reg::LoadAlign(dstReg, src + offset);
    }
}

template <typename T>
__simd_callee__ inline void SaveDataWithT(__ubuf__ T* dst, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask,
                                          uint16_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> regT;
        Reg::Cast<T, float, LayoutZMrgZRndRSatNS>(regT, srcReg, mask);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + offset, regT, mask);
    } else {
        Reg::StoreAlign(dst + offset, srcReg, mask);
    }
}

template <bool IsMean = false, typename T, typename U>
__simd_callee__ inline void ReduceSumOrMean(__ubuf__ T* dstLocal, __ubuf__ U* srcLocal, uint32_t count, uint16_t repeat,
                                            float factor = 0)
{
    constexpr uint16_t srcRepOffset = GetVecLen() / sizeof(T);
    Reg::MaskReg mask;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> dstReg;
    Reg::UnalignReg ureg;
    for (uint16_t i = 0; i < repeat; ++i) {
        mask = Reg::UpdateMask<T>(count);
        LoadDataWithT(srcLocal, srcReg, mask, srcRepOffset * i);
        Reg::ReduceSum(dstReg, srcReg, mask);
        if constexpr (IsMean) {
            Muls(dstReg, dstReg, factor, mask);
        }
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(dstLocal + i, dstReg, mask);
    }
}

template <ShapeScope Scope, typename T, typename U>
__simd_callee__ inline void ReduceMeanCount(__ubuf__ T* dstLocal, __ubuf__ U* srcLocal, __ubuf__ T* workLocal,
                                            uint32_t count, float factor)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    if constexpr (Scope == ShapeScope::ONE) {
        ReduceSumOrMean<true>(dstLocal, srcLocal, count, 1, factor);
    } else if constexpr (Scope == ShapeScope::TWO) {
        uint16_t count2 = static_cast<uint16_t>(CeilDivision(count, oneRepSize));
        ReduceSumOrMean(workLocal, srcLocal, count, count2);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceSumOrMean<true>(dstLocal, workLocal, count2, 1, factor);
    } else {
        uint16_t count2 = static_cast<uint16_t>(CeilDivision(count, oneRepSize));
        uint16_t count3 = CeilDivision(count2, oneRepSize);
        ReduceSumOrMean(workLocal, srcLocal, count, count2);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceSumOrMean(workLocal, workLocal, count2, count3);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceSumOrMean<true>(dstLocal, workLocal, count3, 1, factor);
    }
}

template <ShapeScope Scope, bool IsHwSizeAlignOneRepeat, typename T>
__simd_callee__ inline void CalcVariance(__ubuf__ float* outputVarianceTmp, __ubuf__ T* inputX,
                                         __ubuf__ float* outputMeanTmp, __ubuf__ float* tmpVarLocal,
                                         __ubuf__ float* tmpLocal, uint16_t index, uint16_t dhwAlignSize, uint16_t d,
                                         uint16_t hw, uint16_t hwAlignSize, uint16_t hwRepeat, float factor)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(float);
    Reg::MaskReg fullMask = Reg::CreateMask<float>();

    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> dstReg;

    __ubuf__ T* curInputX = inputX + dhwAlignSize * index;
    Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanReg, outputMeanTmp + index);
    for (uint16_t di = 0; di < d; di++) {
        if constexpr (IsHwSizeAlignOneRepeat) {
            for (uint16_t j = 0; j < hwRepeat; j++) {
                LoadDataWithT(curInputX, srcReg, fullMask, di * hwAlignSize + j * oneRepSize);
                Sub(dstReg, srcReg, meanReg, fullMask);
                Mul(dstReg, dstReg, dstReg, fullMask);
                Reg::StoreAlign(tmpLocal + di * hwAlignSize + j * oneRepSize, dstReg, fullMask);
            }
        } else {
            uint32_t hwCount = hw;
            uint32_t hwAlignCount = hwAlignSize;
            for (uint16_t j = 0; j < hwRepeat; j++) {
                Reg::MaskReg hwMask = Reg::UpdateMask<float>(hwCount);
                Reg::MaskReg hwAlignMask = Reg::UpdateMask<float>(hwAlignCount);
                LoadDataWithT(curInputX, srcReg, hwMask, di * hwAlignSize + j * oneRepSize);
                Sub(dstReg, srcReg, meanReg, hwMask);
                Mul(dstReg, dstReg, dstReg, hwMask);
                Reg::StoreAlign(tmpLocal + di * hwAlignSize + j * oneRepSize, dstReg, hwAlignMask);
            }
        }
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    ReduceMeanCount<Scope>(outputVarianceTmp + index, tmpLocal, tmpVarLocal, dhwAlignSize, factor);
}

template <typename T>
__simd_callee__ inline void CalcTmpOutput(__ubuf__ float* outputTmp, __ubuf__ float* outputVarianceTmp,
                                          __ubuf__ T* outputVariance, __ubuf__ float* outputMeanTmp,
                                          __ubuf__ T* outputMean, uint16_t ngRepeat, uint16_t meanVarSize,
                                          float epsilon)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(float);
    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> varReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<float> dstReg;
    uint32_t count = meanVarSize;
    for (uint16_t index = 0; index < ngRepeat; index++) {
        Reg::MaskReg mask = Reg::UpdateMask<float>(count);
        Reg::LoadAlign(varReg, outputVarianceTmp + index * oneRepSize);
        if constexpr (IsSameType<T, half>::value) {
            SaveDataWithT(outputVariance, varReg, mask, index * oneRepSize);
            Reg::LoadAlign(meanReg, outputMeanTmp + index * oneRepSize);
            SaveDataWithT(outputMean, meanReg, mask, index * oneRepSize);
        }
        Adds(varReg, varReg, epsilon, mask);
        Sqrt(varReg, varReg, mask);
        Duplicate(tmpReg, 1.0f, mask);
        Div(dstReg, tmpReg, varReg, mask);
        Reg::StoreAlign(outputTmp + index * oneRepSize, dstReg, mask);
    }
}

template <typename T>
__simd_callee__ inline void CalcOutput(__ubuf__ T* output, __ubuf__ T* inputX, __ubuf__ float* outputMeanTmp,
                                       __ubuf__ float* outputTmp, __ubuf__ T* gamma, __ubuf__ T* beta, uint16_t n,
                                       uint16_t g, uint16_t d, uint16_t hwAlignSize, uint16_t hwRepeat)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(float);
    Reg::MaskReg fullMask = Reg::CreateMask<float>();

    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> betaReg;
    Reg::RegTensor<float> gammaReg;
    for (uint16_t ni = 0; ni < n; ni++) {
        for (uint16_t gi = 0; gi < g; gi++) {
            uint16_t ngi = ni * g + gi;
            Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanReg, outputMeanTmp + ngi);
            Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(tmpReg, outputTmp + ngi);
            for (uint16_t di = 0; di < d; di++) {
                if constexpr (IsSameType<T, half>::value) {
                    Reg::RegTensor<T> betaOri;
                    Reg::RegTensor<T> gammaOri;
                    Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B16>(betaOri, beta + gi * d + di);
                    Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B16>(gammaOri, gamma + gi * d + di);
                    Cast<float, T, layoutZMrgZ>(betaReg, betaOri, fullMask);
                    Cast<float, T, layoutZMrgZ>(gammaReg, gammaOri, fullMask);
                } else {
                    Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(betaReg, beta + gi * d + di);
                    Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(gammaReg, gamma + gi * d + di);
                }
                uint32_t count = hwAlignSize;
                for (uint16_t j = 0; j < hwRepeat; j++) {
                    Reg::MaskReg mask = Reg::UpdateMask<float>(count);
                    LoadDataWithT(inputX, srcReg, fullMask, ngi * hwAlignSize * d + di * hwAlignSize + j * oneRepSize);
                    Sub(dstReg, srcReg, meanReg, mask);
                    Mul(dstReg, dstReg, tmpReg, mask);
                    FusedMulDstAdd(dstReg, gammaReg, betaReg, mask);
                    SaveDataWithT(output, dstReg, mask, ngi * hwAlignSize * d + di * hwAlignSize + j * oneRepSize);
                }
            }
        }
    }
}

template <ShapeScope Scope, bool IsHwSizeAlignOneRepeat, typename T>
__simd_vf__ inline void GroupNormRegbaseImpl(__ubuf__ T* output, __ubuf__ T* outputMean, __ubuf__ T* outputVariance,
                                             __ubuf__ T* inputX, __ubuf__ T* gamma, __ubuf__ T* beta,
                                             __ubuf__ float* tmpLocal, float epsilon, uint16_t n, uint16_t g,
                                             uint16_t d, uint16_t hw, float factor)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(float);
    uint16_t meanVarSize = n * g;
    uint16_t meanVarSizeAlign = AlignUp(meanVarSize * sizeof(float), GetDataBlockSizeInBytes()) / sizeof(float);
    uint16_t hwAlignSize = AlignUp(hw * sizeof(T), GetDataBlockSizeInBytes()) / sizeof(T);
    uint16_t dhwAlignSize = hwAlignSize * d;
    uint16_t hwRepeat = static_cast<uint16_t>(CeilDivision(hwAlignSize, oneRepSize));
    uint16_t ngRepeat = static_cast<uint16_t>(CeilDivision(meanVarSize, oneRepSize));
    __ubuf__ float* tmpVarLocal = tmpLocal + dhwAlignSize;  // tmp space when cal var reduce
    __ubuf__ float* outputMeanTmp = tmpLocal + 2 * dhwAlignSize;  // half use the space
    __ubuf__ float* outputVarianceTmp = outputMeanTmp + meanVarSizeAlign;  // half use the space
    __ubuf__ float* outputTmp = outputVarianceTmp;  // use var tmp output
    if constexpr (sizeof(T) == sizeof(float)) {
        outputMeanTmp = outputMean;
        outputVarianceTmp = outputVariance;
    }
    // calc mean, float save to outmean, half save to tmp mean space
    for (uint16_t index = 0; index < meanVarSize; index++) {
        ReduceMeanCount<Scope>(outputMeanTmp + index, inputX + dhwAlignSize * index, tmpLocal, dhwAlignSize, factor);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t index = 0; index < meanVarSize; index++) {
        CalcVariance<Scope, IsHwSizeAlignOneRepeat>(outputVarianceTmp, inputX, outputMeanTmp, tmpVarLocal, tmpLocal,
                                                    index, dhwAlignSize, d, hw, hwAlignSize, hwRepeat, factor);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    // outputTmp = (inputX - mean) / np.sqrt(var + eps)
    CalcTmpOutput(outputTmp, outputVarianceTmp, outputVariance, outputMeanTmp, outputMean, ngRepeat, meanVarSize,
                  epsilon);
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    // result = outputTmp * gamma + beta
    CalcOutput(output, inputX, outputMeanTmp, outputTmp, gamma, beta, n, g, d, hwAlignSize, hwRepeat);
}
}  // namespace GroupNormInternal

template <typename T, bool isReuseSource = false>
__aicore__ inline void GroupNormImpl(const LocalTensor<T>& output, const LocalTensor<T>& outputMean,
                                     const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX,
                                     const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
                                     const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon,
                                     GroupNormTiling& tiling)
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    if constexpr (isReuseSource) {
        static_assert(SupportType<T, float>(), "isReuseSource is only supported for float on current device!");
    }
    CHECK_FUNC_HIGHLEVEL_API(
        GroupNorm, (T, isReuseSource),
        (output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling));
    ASCENDC_ASSERT((tiling.oneTmpSize > 0), { KERNEL_LOG(KERNEL_ERROR, "tiling.oneTmpSize must > 0!"); });

    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<float> tmpLocal = sharedTmpBuffer.ReinterpretCast<float>();
    ASCENDC_ASSERT((tmpLocal.GetSize() > 0), { KERNEL_LOG(KERNEL_ERROR, "sharedTmpBuffer Size must > 0!"); });

    __ubuf__ T* outputUb = (__ubuf__ T*)output.GetPhyAddr();
    __ubuf__ T* meanUb = (__ubuf__ T*)outputMean.GetPhyAddr();
    __ubuf__ T* varianceUb = (__ubuf__ T*)outputVariance.GetPhyAddr();
    __ubuf__ T* inputXUb = (__ubuf__ T*)inputX.GetPhyAddr();
    __ubuf__ T* gammaUb = (__ubuf__ T*)gamma.GetPhyAddr();
    __ubuf__ T* betaUb = (__ubuf__ T*)beta.GetPhyAddr();
    __ubuf__ float* tmpUb = (__ubuf__ float*)tmpLocal.GetPhyAddr();

    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(float);
    bool isHwSizeAlignOneRepeat = (tiling.hw % oneRepSize == 0);

    auto callGroupNorm = [&](auto N) __attribute__((cce_aicore)) {
        constexpr auto value = N();
        if (isHwSizeAlignOneRepeat) {
            GroupNormInternal::GroupNormRegbaseImpl<value, true>(
                outputUb, meanUb, varianceUb, inputXUb, gammaUb, betaUb, tmpUb, static_cast<float>(epsilon),
                static_cast<uint16_t>(tiling.n), static_cast<uint16_t>(tiling.g), static_cast<uint16_t>(tiling.d),
                static_cast<uint16_t>(tiling.hw), tiling.factor);
        } else {
            GroupNormInternal::GroupNormRegbaseImpl<value, false>(
                outputUb, meanUb, varianceUb, inputXUb, gammaUb, betaUb, tmpUb, static_cast<float>(epsilon),
                static_cast<uint16_t>(tiling.n), static_cast<uint16_t>(tiling.g), static_cast<uint16_t>(tiling.d),
                static_cast<uint16_t>(tiling.hw), tiling.factor);
        }
    };
    if (tiling.dhwAlignSize <= oneRepSize) {
        callGroupNorm([] { return GroupNormInternal::ShapeScope::ONE; });
    } else if (tiling.dhwAlignSize <= oneRepSize * oneRepSize) {
        callGroupNorm([] { return GroupNormInternal::ShapeScope::TWO; });
    } else {
        callGroupNorm([] { return GroupNormInternal::ShapeScope::THREE; });
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void GroupNormImpl(const LocalTensor<T>& output, const LocalTensor<T>& outputMean,
                                     const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX,
                                     const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const T epsilon,
                                     GroupNormTiling& tiling)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    GroupNormImpl<T, isReuseSource>(output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon,
                                    tiling);
}
}  // namespace AscendC
#endif  // IMPL_NORMALIZATION_GROUPNORM_GROUPNORM_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_GROUPNORM_GROUPNORM_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_GROUPNORM_GROUPNORM_C310_IMPL_H__
#endif
