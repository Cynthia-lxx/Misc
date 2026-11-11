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
 * \file layernormgrad_c310_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/normalization/layernormgrad/regbase/c310/layernormgrad_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_IMPL_H
#define IMPL_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_IMPL_H

#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "kernel_pop_stack_buffer.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../../../api_check/kernel_check/normalization/layernormgrad/layernormgrad_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../../../api_check/kernel_api_check.h"
#include "layernormgrad_c310_utils.h"
constexpr uint32_t LAYERNORM_GRAD_B32_BYTE_SIZE = 4;
constexpr uint32_t LAYERNORM_GRAD_B16_BYTE_SIZE = 2;

namespace AscendC {

template <typename T>
__simd_callee__ inline void ComputePdVar(Reg::RegTensor<float>& pdVarReg, Reg::RegTensor<float>& inputVarianceReg,
    Reg::RegTensor<float>& inputMeanReg, Reg::RegTensor<float>& inputGammaReg, __ubuf__ float* x1Tmp,
    __ubuf__ float* x2Tmp, __ubuf__ T* inputDy, __ubuf__ T* inputX, float epsilon,
    LayerNormGradParams& param, Reg::MaskReg& preg, Reg::MaskReg& pregOne, uint32_t offset,
    uint32_t tmpOffset)
{
    Reg::RegTensor<float> inputDyReg, inputXReg;
    // load and convert to float if necessary
    Internal::LayernormGrad::LoadDataWithT<T>(inputDy, inputDyReg, preg, offset);
    Internal::LayernormGrad::LoadDataWithT<T>(inputX, inputXReg, preg, offset);

    Reg::RegTensor<float> x1Reg, x2Reg;
    // 1. x1Tensor = inputDy * inputGamma
    Mul(x1Reg, inputDyReg, inputGammaReg, preg);
    // 2. x2Tensor = inputX - inputMean
    Sub(x2Reg, inputXReg, inputMeanReg, preg);

    // 3. pd_var = np.sum(((-0.5) * x1Tensor * x2Tensor * np.power((inputVariance + EPSILON), (-1.5))))
    Reg::RegTensor<float> tmpReg1, tmpReg2;
    constexpr float multiplier = -0.5;
    // 3.1. inputVariance + EPSILON
    Adds(tmpReg1, inputVarianceReg, epsilon, pregOne);
    // 3.2. res = np.power((inputVariance + EPSILON), (-1.5))
    Mul(tmpReg2, tmpReg1, tmpReg1, pregOne);
    Mul(tmpReg1, tmpReg2, tmpReg1, pregOne);
    Sqrt(tmpReg1, tmpReg1, pregOne);
    Duplicate(tmpReg2, 1.0f, pregOne);
    Div(tmpReg1, tmpReg2, tmpReg1, pregOne);

    // 3.3. broadcast to CalCount
    Duplicate(tmpReg2, tmpReg1, preg);

    // 3.4. res1 = (-0.5) * Get * (x2Reg) * res
    Mul(tmpReg2, x2Reg, tmpReg2, preg);
    Mul(tmpReg2, x1Reg, tmpReg2, preg);
    Muls(tmpReg2, tmpReg2, multiplier, preg);

    // 3.5. pd_var = np.sum(res1)
    // sum performed in the main VF
    Add(tmpReg1, pdVarReg, tmpReg2, preg);
    Select(pdVarReg, tmpReg1, pdVarReg, preg);

    // store tmp result
    Internal::LayernormGrad::StoreDataWithT<float>(x1Tmp, x1Reg, preg, tmpOffset);
    Internal::LayernormGrad::StoreDataWithT<float>(x2Tmp, x2Reg, preg, tmpOffset);
}

template <typename T>
__simd_callee__ inline void ComputePdMean(Reg::RegTensor<float>& pdMeanReg, Reg::RegTensor<float>& res2Reg,
    Reg::RegTensor<float>& inputVarianceReg, __ubuf__ float* x1Tmp, __ubuf__ float* x2Tmp,
    __ubuf__ T* resForGamma, float epsilon, LayerNormGradParams& param, Reg::MaskReg& preg,
    Reg::MaskReg& pregOne, uint32_t offset, uint32_t tmpOffset)
{
    Reg::RegTensor<float> tmpReg1, tmpReg2, x1Reg, x2Reg;
    Reg::RegTensor<float> resForGammaReg;

    // load and convert to float if necessary
    Internal::LayernormGrad::LoadDataWithT<float>(x1Tmp, x1Reg, preg, tmpOffset);
    Internal::LayernormGrad::LoadDataWithT<float>(x2Tmp, x2Reg, preg, tmpOffset);

    // 4. pd_mean = np.sum(((-1.0) * x1Tensor * np.power((inputVariance + EPSILON), (-0.5)))) +
    // pd_var * (1.0 / H) * np.sum(((-2.0) * (x2Tensor)))
    constexpr float multiplier = -1.0;
    constexpr float multiplier2 = -2.0;
    // 4.1. (inputVariance + EPSILON)
    Adds(tmpReg1, inputVarianceReg, epsilon, pregOne);

    // 4.2. res = np.power((inputVariance + EPSILON), (-0.5))
    Sqrt(tmpReg1, tmpReg1, pregOne);
    Duplicate(tmpReg2, 1.0f, pregOne);
    Div(tmpReg1, tmpReg2, tmpReg1, pregOne);

    // 4.3. duplicate
    // broadcast res from size one to size calCount
    Duplicate(tmpReg2, tmpReg1, preg);

    // res_for_gamma = x2Tensor * np.power((inputVariance + EPSILON), (-0.5))
    Mul(resForGammaReg, x2Reg, tmpReg2, preg);

    // 4.4. res1 = (-1.0) * x1Tensor * np.power((inputVariance + EPSILON), (-0.5))
    // 4.4.1 res1 = x1Tensor * np.power((inputVariance + EPSILON), (-0.5))
    Mul(x1Reg, x1Reg, tmpReg2, preg);
    // save the x1Reg to x1Tmp for pdX calculation
    Internal::LayernormGrad::StoreDataWithT<float>(x1Tmp, x1Reg, preg, tmpOffset);
    Muls(tmpReg2, x1Reg, multiplier, preg);

    // 4.5. pd_mean = np.sum(res1)
    // sum performed in main VF
    Add(tmpReg1, pdMeanReg, tmpReg2, preg);
    Select(pdMeanReg, tmpReg1, pdMeanReg, preg);

    // 4.6. res2 = np.sum(((-2.0) * (x2Tensor)))
    // sum performed in main VF
    Muls(tmpReg2, x2Reg, multiplier2, preg);
    Add(tmpReg1, res2Reg, tmpReg2, preg);
    Select(res2Reg, tmpReg1, res2Reg, preg);

    // rest of the operation is performed in the main VF
    // store tmp result and resForGamma
    Internal::LayernormGrad::StoreDataWithT<T>(resForGamma, resForGammaReg, preg, offset);
}

template <typename T>
__simd_callee__ inline void ComputePdX(Reg::RegTensor<float>& pdVarReg, Reg::RegTensor<float>& pdMeanReg,
    __ubuf__ float* x1Tmp, __ubuf__ float* x2Tmp, __ubuf__ T* outputPdX, LayerNormGradParams& param,
    Reg::MaskReg& preg, Reg::MaskReg& pregOne, uint32_t offset, uint32_t tmpOffset)
{
    Reg::RegTensor<float> tmpReg, x1Reg, x2Reg;
    Reg::RegTensor<float> outputPdXReg;
    // load x1 and x2 values
    Internal::LayernormGrad::LoadDataWithT<float>(x1Tmp, x1Reg, preg, tmpOffset);
    Internal::LayernormGrad::LoadDataWithT<float>(x2Tmp, x2Reg, preg, tmpOffset);

    // 5. pd_x = x1Tensor * np.power((inputVariance + EPSILON), (-0.5)) +
    //           pd_var*(2.0 / H)*(x2Tensor) +
    //           pd_mean*(1.0 / H)
    // 5.1. res0 = x1Tensor * np.power((inputVariance + EPSILON), (-0.5)
    // already stored in x1
    // 5.2. res1 = pd_var*(2.0 / H)*(x2Tensor)
    // pd_var*(2.0 / H) done in the main VF
    Duplicate(tmpReg, pdVarReg, preg);
    Mul(x2Reg, tmpReg, x2Reg, preg);

    // 5.3. res2 = pd_mean*(1.0 / H)
    // done in the main VF
    Duplicate(tmpReg, pdMeanReg, preg);

    // 5.4. pd_x = res0 + res1 + res2
    // 5.4.1. Add res1 and res2
    Add(x2Reg, x2Reg, tmpReg, preg);
    // 5.4.2. Add res0
    Add(outputPdXReg, x2Reg, x1Reg, preg);

    // store final result
    Internal::LayernormGrad::StoreDataWithT<T>(outputPdX, outputPdXReg, preg, offset);
}

template <typename T>
__simd_callee__ inline void ComputePdVarLoop(Reg::RegTensor<float>& pdVarReg,
    Reg::RegTensor<float>& inputVarianceReg, Reg::RegTensor<float>& inputMeanReg,
    __ubuf__ T* inputGamma, __ubuf__ float* x1Tmp, __ubuf__ float* x2Tmp,
    __ubuf__ T* inputDy, __ubuf__ T* inputX, float epsilon, LayerNormGradParams& param, uint16_t repeatTimes,
    uint16_t oneRepeatTimes, uint32_t baseOffset, Reg::MaskReg& pregOne)
{
    uint32_t calCount = param.hLength;
    Reg::MaskReg preg;
    Reg::RegTensor<float> inputGammaReg;
    // PdVar calculation, accumulatively reduce sum
    for (size_t i = 0; i < repeatTimes; ++i) {
        preg = Reg::UpdateMask<float>(calCount);
        Internal::LayernormGrad::LoadDataWithT<T>(inputGamma, inputGammaReg, preg, i * oneRepeatTimes);
        ComputePdVar<T>(pdVarReg, inputVarianceReg, inputMeanReg, inputGammaReg, x1Tmp, x2Tmp, inputDy, inputX, epsilon,
            param, preg, pregOne, i * oneRepeatTimes + baseOffset, i * oneRepeatTimes);
    }
    calCount = param.hLength;
    preg = Reg::UpdateMask<float>(calCount);
    ReduceSum(pdVarReg, pdVarReg, preg);
}

template <typename T>
__simd_callee__ inline void ComputePdMeanLoop(Reg::RegTensor<float>& pdMeanReg, Reg::RegTensor<float>& res2Reg,
    Reg::RegTensor<float>& pdVarReg, Reg::RegTensor<float>& inputVarianceReg, __ubuf__ float* x1Tmp,
    __ubuf__ float* x2Tmp, __ubuf__ T* resForGamma, float epsilon, LayerNormGradParams& param,
    uint16_t repeatTimes, uint16_t oneRepeatTimes, uint32_t baseOffset, Reg::MaskReg& pregOne)
{
    uint32_t calCount = param.hLength;
    Reg::MaskReg preg;
    // PdMean calculation, accumulatively reduce sum
    for (size_t i = 0; i < repeatTimes; ++i) {
        preg = Reg::UpdateMask<float>(calCount);
        ComputePdMean<T>(pdMeanReg, res2Reg, inputVarianceReg, x1Tmp, x2Tmp, resForGamma, epsilon, param, preg, pregOne,
            i * oneRepeatTimes + baseOffset, i * oneRepeatTimes);
    }
    calCount = param.hLength;
    preg = Reg::UpdateMask<float>(calCount);
    // np.sum(res1)
    ReduceSum(pdMeanReg, pdMeanReg, preg);
    // np.sum(((-2.0) * (x2Tensor)))
    ReduceSum(res2Reg, res2Reg, preg);

    // pd_var * (1.0 / H) * np.sum(((-2.0) * (x2Tensor)))
    Muls(res2Reg, res2Reg, param.oneOverH, pregOne);
    Mul(res2Reg, pdVarReg, res2Reg, pregOne);
    // pd_mean = pd_mean + pd_var * (1.0 / H) * np.sum(((-2.0) * (x2Tensor)))
    Add(pdMeanReg, pdMeanReg, res2Reg, pregOne);
}

template <typename T>
__simd_callee__ inline void ComputePdXLoop(Reg::RegTensor<float>& pdVarReg, Reg::RegTensor<float>& pdMeanReg,
    __ubuf__ float* x1Tmp, __ubuf__ float* x2Tmp, __ubuf__ T* outputPdX, LayerNormGradParams& param,
    uint16_t repeatTimes, uint16_t oneRepeatTimes, uint32_t baseOffset, Reg::MaskReg& pregOne)
{
    // pd_var*(2.0 / H)
    Muls(pdVarReg, pdVarReg, param.twoOverH, pregOne);
    // pd_mean*(1.0 / H)
    Muls(pdMeanReg, pdMeanReg, param.oneOverH, pregOne);

    // PdX calculation
    uint32_t calCount = param.hLength;
    Reg::MaskReg preg;
    for (size_t i = 0; i < repeatTimes; ++i) {
        preg = Reg::UpdateMask<float>(calCount);
        ComputePdX<T>(pdVarReg, pdMeanReg, x1Tmp, x2Tmp, outputPdX, param, preg, pregOne,
            i * oneRepeatTimes + baseOffset, i * oneRepeatTimes);
    }
}

template <typename T, bool isReuseSource>
__simd_vf__ inline void LayerNormGradVF(__ubuf__ float* x1Tmp, __ubuf__ float* x2Tmp, __ubuf__ T* inputDy,
    __ubuf__ T* inputX, __ubuf__ T* inputVariance, __ubuf__ T* inputMean, __ubuf__ T* inputGamma,
    __ubuf__ T* outputPdX, __ubuf__ T* resForGamma, float epsilon, LayerNormGradParams param)
{
    Reg::MaskReg preg = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    Reg::RegTensor<float> inputVarianceReg, inputMeanReg;
    Reg::RegTensor<float> pdVarReg, pdMeanReg, res2Reg;

    constexpr uint16_t oneRepeatTimes = (uint32_t)(GetVecLen() / sizeof(float));
    uint16_t repeatTimes = CeilDivision(param.hLength, oneRepeatTimes);
    uint32_t totalRepeatTimes = param.bLength * param.sLength;

    for (size_t j = 0; j < totalRepeatTimes; ++j) {
        Reg::Duplicate(pdVarReg, 0.0f, pregFull);
        Reg::Duplicate(pdMeanReg, 0.0f, pregFull);
        Reg::Duplicate(res2Reg, 0.0f, pregFull);

        Internal::LayernormGrad::FillDataWithT<T>(inputVariance, inputVarianceReg, pregFull, j);
        Internal::LayernormGrad::FillDataWithT<T>(inputMean, inputMeanReg, pregFull, j);

        uint32_t baseOffset = param.hLength * j;

        ComputePdVarLoop<T>(pdVarReg, inputVarianceReg, inputMeanReg, inputGamma, x1Tmp, x2Tmp, inputDy, inputX,
            epsilon, param, repeatTimes, oneRepeatTimes, baseOffset, pregOne);

        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

        ComputePdMeanLoop<T>(pdMeanReg, res2Reg, pdVarReg, inputVarianceReg, x1Tmp, x2Tmp, resForGamma, epsilon, param,
            repeatTimes, oneRepeatTimes, baseOffset, pregOne);

        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

        ComputePdXLoop<T>(
            pdVarReg, pdMeanReg, x1Tmp, x2Tmp, outputPdX, param, repeatTimes, oneRepeatTimes, baseOffset, pregOne);
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormGradImpl(const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma,
    const LocalTensor<T>& inputDy, const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance,
    const LocalTensor<T>& inputMean, const LocalTensor<T>& inputGamma, LocalTensor<uint8_t>& tmpBuffer, T epsilon,
    LayerNormGradTiling& tiling, const LayerNormGradShapeInfo& shapeInfo = {})
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    CHECK_FUNC_HIGHLEVEL_API(LayerNormGrad, (T, isReuseSource),
        (outputPdX, resForGamma, inputDy, inputX, inputVariance, inputMean, inputGamma, tmpBuffer, epsilon, tiling,
            shapeInfo));

    ASCENDC_ASSERT((tmpBuffer.GetSize() >= tiling.stackBufferSize),
        { KERNEL_LOG(KERNEL_ERROR, "Stack buffer size not enough."); });
    ASCENDC_ASSERT(
        shapeInfo.dataFormat == DataFormat::ND, { KERNEL_LOG(KERNEL_ERROR, "Only support format ND now!"); });

    LocalTensor<float> sharedTmpBuffer = tmpBuffer.ReinterpretCast<float>();
    LayerNormGradParams param(tiling.bLength, tiling.sLength, tiling.hLength,
        *(reinterpret_cast<float*>(&tiling.lastDimValueBack)),
        *(reinterpret_cast<float*>(&tiling.lastDimValueBackMulTwo)));

    __ubuf__ T* outputPdXDst = (__ubuf__ T*)outputPdX.GetPhyAddr();         // output gradient
    __ubuf__ T* resForGammaDst = (__ubuf__ T*)resForGamma.GetPhyAddr();     // gradient w.r.t. gamma
    __ubuf__ T* inputDySrc = (__ubuf__ T*)inputDy.GetPhyAddr();             // input gradient, (B,S,H)
    __ubuf__ T* inputXSrc = (__ubuf__ T*)inputX.GetPhyAddr();               // input activations, (B,S,H)
    __ubuf__ T* inputVarianceSrc = (__ubuf__ T*)inputVariance.GetPhyAddr(); // variance, (B,S,1)
    __ubuf__ T* inputMeanSrc = (__ubuf__ T*)inputMean.GetPhyAddr();         // mean, (B,S,1)
    __ubuf__ T* inputGammaSrc = (__ubuf__ T*)inputGamma.GetPhyAddr();       // gamma, (B,S,1)
    __ubuf__ float* sharedTmpBufferSrc = (__ubuf__ float*)sharedTmpBuffer.GetPhyAddr(); // temporary buffer
    __ubuf__ float *x1Tmp, *x2Tmp;

    uint32_t bufferOffset = (param.hLength + 7) / 8 * 8;
    if constexpr (isReuseSource && IsSameType<T, float>::value) {
        x1Tmp = reinterpret_cast<__ubuf__ float*>(inputDySrc);
        x2Tmp = reinterpret_cast<__ubuf__ float*>(inputXSrc);
    } else {
        x1Tmp = sharedTmpBufferSrc;
        x2Tmp = sharedTmpBufferSrc + bufferOffset;
    }

    float eps = static_cast<float>(epsilon);

    LayerNormGradVF<T, isReuseSource>(x1Tmp, x2Tmp, inputDySrc, inputXSrc, inputVarianceSrc, inputMeanSrc,
        inputGammaSrc, outputPdXDst, resForGammaDst, eps, param);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormGradImpl(const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma,
    const LocalTensor<T>& inputDy, const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance,
    const LocalTensor<T>& inputMean, const LocalTensor<T>& inputGamma, T epsilon, LayerNormGradTiling& tiling,
    const LayerNormGradShapeInfo& shapeInfo = {})
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    LayerNormGradImpl<T, isReuseSource>(outputPdX, resForGamma, inputDy, inputX, inputVariance, inputMean, inputGamma,
        sharedTmpBuffer, epsilon, tiling, shapeInfo);
}
} // namespace AscendC
#endif // IMPL_NORMALIZATION_LAYER_NORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_IMPL_H__
#endif
