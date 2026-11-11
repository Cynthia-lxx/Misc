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
 * \file lgamma_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/lgamma/lgamma_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/lgamma.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LGAMMA_LGAMMA_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_LGAMMA_LGAMMA_COMMON_IMPL_H
#define IMPL_MATH_LGAMMA_LGAMMA_COMMON_IMPL_H
#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/math/sin.h"
#include "lgamma_common_utils.h"
#include "lgamma_common_basic_impl.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/lgamma/lgamma_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
namespace AscendC {
__aicore__ inline void Lgamma1Compute(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
    const LocalTensor<float>& tmpTensor, const uint32_t splitSize)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binParams;

    LocalTensor<float> tmp1Tensor = tmpTensor;
    LocalTensor<float> tmp2Tensor = tmp1Tensor[splitSize];
    LocalTensor<float> tmp3Tensor = tmp2Tensor[splitSize];
    LocalTensor<float> tmp4Tensor = tmp3Tensor[splitSize];
    tmp1Tensor.SetSize(splitSize);
    tmp2Tensor.SetSize(splitSize);
    tmp3Tensor.SetSize(splitSize);
    tmp4Tensor.SetSize(splitSize);

    Adds<float, false>(tmp1Tensor, srcTensor, t4, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    // inv_x = 1 / x
    Duplicate<float, false>(dstTensor, f1, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Div<float, false>(dstTensor, dstTensor, tmp1Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    // tmp2Tensor = 0.5 * torch.log(2 * torch.pi * inv_x)
    Muls<float, false>(tmp2Tensor, dstTensor, PI, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(tmp2Tensor, tmp2Tensor, f2, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Ln<float, false>(tmp2Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(tmp2Tensor, tmp2Tensor, f05, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    // tmp4Tensor = x * (torch.log(x + 1 / (12 * x - 0.1 * inv_x)) - 1)
    // tmp3Tensor = -0.1 * inv_x
    Muls<float, false>(tmp3Tensor, dstTensor, N01, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    // tmp4Tensor = x * 12
    Muls<float, false>(tmp4Tensor, tmp1Tensor, t12, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    // tmp4Tensor = 12 * x - 0.1 * inv_x
    Add<float, false>(tmp4Tensor, tmp4Tensor, tmp3Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    Duplicate<float, false>(dstTensor, f1, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Div<float, false>(tmp4Tensor, dstTensor, tmp4Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    Add<float, false>(tmp4Tensor, tmp4Tensor, tmp1Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    Ln<float, false>(tmp4Tensor, tmp4Tensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmp4Tensor, tmp4Tensor, fn1, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Mul<float, false>(tmp4Tensor, tmp4Tensor, tmp1Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    // tmp4Tensor = tmp2Tensor + tmp4Tensor, lgamma1(x + 5)
    Add<float, false>(dstTensor, tmp4Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void LgammaComputePosHalf(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
    const LocalTensor<float>& tmpTensor, const uint32_t splitSize)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binParams;

    LocalTensor<float> tmp1Tensor = tmpTensor;
    LocalTensor<float> tmp2Tensor = tmpTensor[splitSize];
    LocalTensor<float> tmp3Tensor = tmpTensor[splitSize * 2];
    LocalTensor<float> tmp4Tensor = tmpTensor[splitSize * 3];

    tmp1Tensor.SetSize(splitSize);
    tmp2Tensor.SetSize(splitSize);
    tmp3Tensor.SetSize(splitSize);
    tmp4Tensor.SetSize(splitSize);

    // lgamma1(x + 4)
    Lgamma1Compute(dstTensor, srcTensor, tmpTensor, splitSize);
    PipeBarrier<PIPE_V>();

    // tmp2Tensor = torch.log(x)
    Ln<float, false>(tmp3Tensor, srcTensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    // tmp2Tensor = torch.log(x + 1)
    Adds<float, false>(tmp2Tensor, srcTensor, f1, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Ln<float, false>(tmp2Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Add<float, false>(tmp3Tensor, tmp3Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    // tmp2Tensor = torch.log(x + 2)
    Adds<float, false>(tmp2Tensor, srcTensor, f2, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Ln<float, false>(tmp2Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Add<float, false>(tmp3Tensor, tmp3Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    // tmp2Tensor = torch.log(x + 3)
    Adds<float, false>(tmp2Tensor, srcTensor, f3, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Ln<float, false>(tmp2Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Add<float, false>(tmp3Tensor, tmp3Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    // dstTensor=tmp4Tensor-tmp3Tensor
    Sub<float, false>(dstTensor, dstTensor, tmp3Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void LgammaComputeNegHalf(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
    const LocalTensor<float>& tmpTensor, const uint32_t splitSize)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binParams;

    LocalTensor<float> tmp1Tensor = tmpTensor;
    LocalTensor<float> tmp2Tensor = tmp1Tensor[splitSize];
    LocalTensor<float> tmp3Tensor = tmp2Tensor[splitSize];
    LocalTensor<float> tmp4Tensor = tmp3Tensor[splitSize];
    LocalTensor<float> tmp5Tensor = tmp4Tensor[splitSize];
    LocalTensor<float> tmp6Tensor = tmp5Tensor[splitSize];
    LocalTensor<float> tmp7Tensor = tmpTensor[splitSize * i2];
    tmp1Tensor.SetSize(splitSize);
    tmp2Tensor.SetSize(splitSize);
    tmp3Tensor.SetSize(splitSize);
    tmp4Tensor.SetSize(splitSize);
    tmp5Tensor.SetSize(splitSize);
    tmp6Tensor.SetSize(splitSize);
    tmp7Tensor.SetSize(splitSize * i4);

    // lgamma_our_p(1 - x)
    Muls<float, false>(tmp1Tensor, srcTensor, fn1, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Adds<float, false>(tmp1Tensor, tmp1Tensor, f1, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    LgammaComputePosHalf(dstTensor, tmp1Tensor, tmp7Tensor, splitSize);
    PipeBarrier<PIPE_V>();

    Muls<float, false>(dstTensor, dstTensor, fn1, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    // torch.log(torch.pi / torch.abs((torch.sin(torch.pi * (x - torch.floor(x))))))
    LGammaFloor(tmp1Tensor, srcTensor);

    // tmp1Tensor = x - torch.floor(x)
    Sub<float, false>(tmp1Tensor, srcTensor, tmp1Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    // pi * tmp1Tensor
    Muls<float, false>(tmp1Tensor, tmp1Tensor, PI, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    // isReuseSource is always false when the input data type is half
    SinCompute(tmp2Tensor, tmp1Tensor, tmp7Tensor, splitSize, false);
    PipeBarrier<PIPE_V>();

    Abs<float, false>(tmp2Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Duplicate<float, false>(tmp3Tensor, PI, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();

    Div<float, false>(tmp2Tensor, tmp3Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();

    Ln<float, false>(tmp2Tensor, tmp2Tensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Add<float, false>(dstTensor, dstTensor, tmp2Tensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();
}

// generate mask, src < scalar is 1, else is 0
__aicore__ inline void LGammaGenLTMaskHalf(const LocalTensor<uint8_t> &mask, const LocalTensor<float> &src,
    const LocalTensor<float> &tmptensor, const float scalar, const uint32_t splitSize)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binParams;

    Duplicate<float, false>(tmptensor, scalar, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();

    uint8_t repeat = DivCeil(splitSize * sizeof(float), ONE_REPEAT_BYTE_SIZE);
    Compare<float, uint8_t, false>(mask, src, tmptensor, CMPMODE::LT, MASK_PLACEHOLDER, repeat, binParams);
    PipeBarrier<PIPE_V>();
}

// generate mask, src >= scalar is 1, else is 0
__aicore__ inline void LGammaGenGEMaskHalf(const LocalTensor<uint8_t> &mask, const LocalTensor<float> &src,
    const LocalTensor<float> &tmptensor, const float scalar, const uint32_t splitSize)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binParams;

    Duplicate<float, false>(tmptensor, scalar, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();

    uint8_t repeat = DivCeil(splitSize * sizeof(float), ONE_REPEAT_BYTE_SIZE);
    Compare<float, uint8_t, false>(mask, src, tmptensor, CMPMODE::GE, MASK_PLACEHOLDER, repeat, binParams);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void LGammaSelectHalf(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
    const LocalTensor<uint8_t>& mask, const LocalTensor<float>& tmpTensor, const LocalTensor<float>& tmpScalar)
{
    const BinaryRepeatParams binParams;
    SetCmpMask<float>(tmpScalar);
    PipeBarrier<PIPE_V>();
    Select<float, uint8_t>(tmpTensor, mask, srcTensor, 1, binParams);
    PipeBarrier<PIPE_V>();
    Add<float, false>(dstTensor, tmpTensor, dstTensor, MASK_PLACEHOLDER, 1, binParams);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void LGammaSelectINF(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
    const LocalTensor<uint8_t>& mask, const LocalTensor<float>& tmpTensor, const LocalTensor<float>& tmpScalar)
{
    const BinaryRepeatParams binParams;
    Duplicate<float, false>(tmpScalar, 655040.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    SetCmpMask<float>(tmpScalar);
    PipeBarrier<PIPE_V>();
    Select<float, uint8_t>(dstTensor, mask, srcTensor, 1, binParams);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void LgammaComputeImpl(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
    LGammaParams& params, const uint32_t splitSize)
{
    // half-->float
    Duplicate<float, false>(params.tmp1, 0.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Cast<float, half, false>(params.tmp2, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
    // all 0 tensor
    Duplicate<float, false>(params.tmpScalar, 0.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();

    // compute result x >= 0
    LgammaComputePosHalf(params.tmp3, params.tmp2, params.tmp6, splitSize);
    PipeBarrier<PIPE_V>();
    // compute mask x >= 0
    LGammaGenGEMaskHalf(params.mask, params.tmp2, params.tmp5, 0.0f, splitSize);
    PipeBarrier<PIPE_V>();
    LGammaSelectHalf(params.tmp1, params.tmp3, params.mask, params.tmp5, params.tmpScalar);
    PipeBarrier<PIPE_V>();

    // compute result x < 0
    LgammaComputeNegHalf(params.tmp4, params.tmp2, params.tmp6, splitSize);
    PipeBarrier<PIPE_V>();
    // compute mask x < 0
    LGammaGenLTMaskHalf(params.tmpMask1, params.tmp2, params.tmp5, 0.0f, splitSize);
    PipeBarrier<PIPE_V>();
    LGammaSelectHalf(params.tmp1, params.tmp4, params.tmpMask1, params.tmp5, params.tmpScalar);
    PipeBarrier<PIPE_V>();

    // for nan
    SetVectorMask<float>(0, ConstCeil(params.splitSize, sizeof(uint16_t) * ONE_BYTE_BIT_SIZE));
    Not<uint16_t, false>(params.tmpMask2.ReinterpretCast<uint16_t>(), params.mask.ReinterpretCast<uint16_t>(),
                         MASK_PLACEHOLDER, 1, params.unaryParams);
    Not<uint16_t, false>(params.tmpMask3.ReinterpretCast<uint16_t>(), params.tmpMask1.ReinterpretCast<uint16_t>(),
                         MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    And<uint16_t, false>(params.tmpMask2.ReinterpretCast<uint16_t>(), params.tmpMask2.ReinterpretCast<uint16_t>(),
        params.tmpMask3.ReinterpretCast<uint16_t>(), MASK_PLACEHOLDER, 1, params.binaryParams);

    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, params.splitSize);
    LGammaSelectHalf(params.tmp1, params.tmp2, params.tmpMask2, params.tmp4, params.tmpScalar);
    PipeBarrier<PIPE_V>();

    // for inf/-inf
    Abs<float, false>(params.tmp2, params.tmp2, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    // generate |x| >= 65504 mask
    LGammaGenGEMaskHalf(params.tmpMask2, params.tmp2, params.tmp4, 65504.0f, splitSize);
    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, ConstCeil(params.splitSize, sizeof(uint16_t) * ONE_BYTE_BIT_SIZE));
    Not<uint16_t, false>(params.tmpMask3.ReinterpretCast<uint16_t>(), params.tmpMask2.ReinterpretCast<uint16_t>(),
                         MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, params.splitSize);
    LGammaSelectINF(params.tmp1, params.tmp1, params.tmpMask3, params.tmp4, params.tmpScalar);
    PipeBarrier<PIPE_V>();

    // float-->half
    Cast<half, float, false>(dstTensor, params.tmp1, RoundMode::CAST_NONE, MASK_PLACEHOLDER,
        1, {1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
}

__aicore__ inline void LgammaComputeImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, LGammaParams& params)
{
    // Gen masks with x >= 0 and < 0, which will not be overwritten in the future
    LGammaGenGEMask(params.tmpMask2, src, params, 0.0f);
    LGammaGenLTMask(params.tmpMask3, src, params, 0.0f);

    // tmp6 = |src|, will no longer use src in the future.
    // When ReuseSource is true, we will reuse src in tmpScalar and initialize it to 0 for the CmpMask
    Abs<float, false>(params.tmp6, src, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    Duplicate<float, false>(params.tmpScalar, 0.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();

    // Cal the result for x >= 0, write to tmp5, and select to dst
    LGammaPositive(params);
    Duplicate<float, false>(dst, 0.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    LGammaSelect(dst, params.tmp5, params.tmpMask2, params);

    // Cal the result for x < 0, write to tmp4, and select to dst
    LGammaNegative(params);
    LGammaSelect(dst, params.tmp4, params.tmpMask3, params);

    // for nan
    SetVectorMask<float>(0, ConstCeil(params.splitSize, sizeof(uint16_t) * ONE_BYTE_BIT_SIZE));
    Not<uint16_t, false>(params.mask.ReinterpretCast<uint16_t>(),
        params.tmpMask2.ReinterpretCast<uint16_t>(),
        MASK_PLACEHOLDER,
        1,
        params.unaryParams);
    Not<uint16_t, false>(params.tmpMask1.ReinterpretCast<uint16_t>(),
        params.tmpMask3.ReinterpretCast<uint16_t>(),
        MASK_PLACEHOLDER,
        1,
        params.unaryParams);
    PipeBarrier<PIPE_V>();
    And<uint16_t, false>(params.mask.ReinterpretCast<uint16_t>(),
        params.tmpMask1.ReinterpretCast<uint16_t>(),
        params.mask.ReinterpretCast<uint16_t>(),
        MASK_PLACEHOLDER,
        1,
        params.binaryParams);
    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, params.splitSize);
    LGammaSelect(dst, params.tmp6, params.mask, params);
}

template <bool isReuseSource = false>
__aicore__ inline void LgammaCompute(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Lgamma, (half, isReuseSource), (dstTensor, srcTensor, sharedTmpBuffer, calCount));

    uint32_t bufferSize = sharedTmpBuffer.GetSize();
    uint32_t tmpBufferSize = bufferSize / sizeof(float);
    CheckTmpBufferSize(tmpBufferSize, 0, bufferSize);

    LocalTensor<float> tmpBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    uint32_t splitSize = 0;

    splitSize = tmpBufferSize / LGAMMA_HALF_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE;  // 32 byte
    CheckTmpBufferSize(splitSize, 0, bufferSize);

    // init params
    LGammaParams params;
    LGammaInitHParams<isReuseSource>(tmpBuffer, splitSize, srcTensor, params);

    const uint32_t round = calCount / splitSize;
    const uint32_t tail = calCount % splitSize;
    SetMaskCount();
    SetVectorMask<half, MaskMode::COUNTER>(0, splitSize);
    uint32_t offset = 0;
    for (uint32_t i = 0; i < round; i++) {
        LgammaComputeImpl(dstTensor[offset], srcTensor[offset], params, splitSize);
        offset = offset + splitSize;
    }

    if (tail > 0) {
        SetVectorMask<half, MaskMode::COUNTER>(0, tail);
        params.splitSize = tail;
        LgammaComputeImpl(
            dstTensor[round * splitSize], srcTensor[round * splitSize], params, splitSize);
    }
    SetMaskNorm();
    AscendCUtils::ResetMask();
}

template <bool isReuseSource = false>
__aicore__ inline void LgammaCompute(const LocalTensor<float>& dst, const LocalTensor<float>& src,
    const LocalTensor<uint8_t>& tmp, const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Lgamma, (float, isReuseSource), (dst, src, tmp, calCount));

    LocalTensor<float> tmpBuffer = tmp.ReinterpretCast<float>();
    uint32_t tmpBufferSize = tmpBuffer.GetSize();
    uint32_t splitSize = tmpBufferSize;
    if constexpr (isReuseSource) {
        splitSize = splitSize / FLOAT_REUSE_CALC_PROC / ONE_BLK_SIZE * ONE_BLK_SIZE;
    } else {
        splitSize = splitSize / FLOAT_NOREUSE_CALC_PROC / ONE_BLK_SIZE * ONE_BLK_SIZE;
    }
    CheckTmpBufferSize(splitSize, 0, tmpBufferSize);

    // init params
    LGammaParams params;
    LGammaInitFParams<isReuseSource>(tmpBuffer, splitSize, src, params);

    const uint32_t loopCount = calCount / splitSize;
    uint32_t calcTail = calCount % splitSize;
    SetMaskCount();
    SetVectorMask<float>(0, splitSize);
    for (uint32_t i = 0U; i < loopCount; ++i) {
        LgammaComputeImpl(dst[i * splitSize], src[i * splitSize], params);
    }
    if (calcTail > 0) {
        SetVectorMask<float>(0, calcTail);
        params.splitSize = calcTail;
        LgammaComputeImpl(dst[loopCount * splitSize], src[loopCount * splitSize], params);
    }
    SetMaskNorm();
    ResetMask();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LgammaImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& tmp, const uint32_t calCount)
{
    LgammaCompute<isReuseSource>(dst, src, tmp, calCount);
}
}  // namespace AscendC
#endif
#endif  // IMPL_MATH_LGAMMA_LGAMMA_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LGAMMA_LGAMMA_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LGAMMA_LGAMMA_COMMON_IMPL_H__
#endif
