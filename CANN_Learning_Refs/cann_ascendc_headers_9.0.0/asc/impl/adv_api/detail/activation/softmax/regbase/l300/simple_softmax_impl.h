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
 * \file simple_softmax_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/regbase/l300/simple_softmax_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLE_SOFTMAX_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_L300_SIMPLE_SOFTMAX_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_L300_SIMPLE_SOFTMAX_IMPL_H

#include "softmax_common_impl.h"

namespace AscendC {
namespace Internal {
template <typename T1, typename T2>
__simd_vf__ inline void SimpleSoftMaxGenericNZImpl(__ubuf__ T1* dstUb, __ubuf__ T2* sumUb,
    __ubuf__ T2* maxUb, __ubuf__ T1* srcUb, const uint16_t mRepeatTimes,
    const uint16_t kRepeatTimes, const uint16_t outNum, const uint16_t dataBlock)
{
    Reg::MaskReg maskCnt;
    Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;
    Reg::RegTensor<float> maxVreg1;
    Reg::RegTensor<float> maxVreg2;
    Reg::RegTensor<float> sumVreg1;
    Reg::RegTensor<float> sumVreg2;

    for (uint16_t j = 0; j < kRepeatTimes; ++j) {
        uint32_t sreg = outNum;
        for (uint16_t i = 0; i < mRepeatTimes; ++i) {
            maskCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadIfNeedCast<T2>(maxVreg, maxUb + i * FLOAT_REPEAT_SIZE, maskFull);
            LoadIfNeedCast<T2>(sumVreg, sumUb + i * FLOAT_REPEAT_SIZE, maskFull);
            if constexpr (SupportType<T2, float>()) {
                Reg::Interleave(maxVreg1, maxVreg2, maxVreg, maxVreg);
                Reg::Interleave(sumVreg1, sumVreg2, sumVreg, sumVreg);
                LoadIfNeedCast<T1>(srcVreg, srcUb + 2 * i * FLOAT_REPEAT_SIZE + j * dataBlock, maskFull);
                Reg::Sub(dstVreg, srcVreg, maxVreg1, maskCnt);
                Reg::Exp(tmpVreg, dstVreg, maskCnt);
                Reg::Div(dstVreg, tmpVreg, sumVreg1, maskCnt);
                StoreIfNeedCast<T1>(dstUb + 2 * i * FLOAT_REPEAT_SIZE + j * dataBlock, dstVreg, maskCnt);
                maskCnt = Reg::UpdateMask<uint32_t>(sreg);
                LoadIfNeedCast<T1>(srcVreg, srcUb + (2 * i + 1) * FLOAT_REPEAT_SIZE + j * dataBlock, maskFull);
                Reg::Sub(dstVreg, srcVreg, maxVreg2, maskCnt);
                Reg::Exp(tmpVreg, dstVreg, maskCnt);
                Reg::Div(dstVreg, tmpVreg, sumVreg2, maskCnt);
                StoreIfNeedCast<T1>(dstUb + (2 * i + 1) * FLOAT_REPEAT_SIZE + j * dataBlock, dstVreg, maskCnt);
            } else {
                LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, maskFull);
                Reg::Sub(dstVreg, srcVreg, maxVreg, maskCnt);
                Reg::Exp(tmpVreg, dstVreg, maskCnt);
                Reg::Div(dstVreg, tmpVreg, sumVreg, maskCnt);
                StoreIfNeedCast<T1>(dstUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, dstVreg, maskCnt);
            }
        }
    }
}

template <typename T1, typename T2>
__simd_vf__ inline void SimpleSoftMaxGenericNDImpl(__ubuf__ T1* dstUb, __ubuf__ T2* sumUb,
    __ubuf__ T2* maxUb, __ubuf__ T1* srcUb, const uint16_t srcM, const uint16_t srcK,
    const uint16_t repeatTimes, const uint16_t blockStride)
{
    Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;

    for (uint16_t i = 0; i < srcM; ++i) {
        LoadIfNeedCast<T2>(maxVreg, maxUb + i * blockStride, maskFull);
        LoadIfNeedCast<T2>(sumVreg, sumUb + i * blockStride, maskFull);
        Reg::Duplicate(maxVreg, maxVreg, maskFull);
        Reg::Duplicate(sumVreg, sumVreg, maskFull);
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, maskFull);
            Reg::Sub(dstVreg, srcVreg, maxVreg, maskFull);
            Reg::Exp(tmpVreg, dstVreg, maskFull);
            Reg::Div(dstVreg, tmpVreg, sumVreg, maskFull);
            StoreIfNeedCast<T1>(dstUb + i * srcK + j * FLOAT_REPEAT_SIZE, dstVreg, maskFull);
        }
    }
}

template <typename T1, typename T2>
__simd_vf__ inline void SimpleSoftMaxGenericNDWithTailImpl(__ubuf__ T1* dstUb, __ubuf__ T2* sumUb,
    __ubuf__ T2* maxUb, __ubuf__ T1* srcUb, const uint16_t srcM, const uint16_t srcK,
    const uint16_t repeatTimes, const uint16_t blockStride)
{
    Reg::MaskReg maskCnt;
    Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;

    for (uint16_t i = 0; i < srcM; ++i) {
        LoadIfNeedCast<T2>(maxVreg, maxUb + i * blockStride, maskFull);
        LoadIfNeedCast<T2>(sumVreg, sumUb + i * blockStride, maskFull);
        Reg::Duplicate(maxVreg, maxVreg, maskFull);
        Reg::Duplicate(sumVreg, sumVreg, maskFull);
        uint32_t sreg = srcK;
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            maskCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, maskFull);
            Reg::Sub(dstVreg, srcVreg, maxVreg, maskCnt);
            Reg::Exp(tmpVreg, dstVreg, maskCnt);
            Reg::Div(dstVreg, tmpVreg, sumVreg, maskCnt);
            StoreIfNeedCast<T1>(dstUb + i * srcK + j * FLOAT_REPEAT_SIZE, dstVreg, maskCnt);
        }
    }
}
} // namespace Internal

template <typename T1, typename T2>
__aicore__ inline void SimpleSoftMaxNZImpl(const LocalTensor<T1>& dst, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<T1>& src, const LocalTensor<float> workLocal,
    const SoftMaxTiling& tiling, const LastAxisShapeND& originalSrcShape)
{
    static_assert((SupportType<Tuple<T1, T2>, Tuple<half, float>, Tuple<half, half>,
                Tuple<float, float>>()), "Failed to check dtype in SimpleSoftMax, current api "
                "support dtype combination is T1 : half, T2 : float; T1 : half, T2 : half; "
                "T1 : float, T2 : float");
    uint16_t srcM = tiling.srcM;
    uint16_t srcK = tiling.srcK;
    uint16_t oriM = originalSrcShape.m;
    constexpr uint16_t nzKUnitLen = IsSameType<T2, half>::value ? SOFTMAX_SHAPE_NZ_BASIC_COUNT 
                                                                : SOFTMAX_SHAPE_NZ_BASIC_COUNT / 2;
    uint16_t dataBlock = srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint16_t mRepeatTimes = static_cast<uint16_t>(CeilDivision(srcM * nzKUnitLen, FLOAT_REPEAT_SIZE));
    uint16_t kRepeatTimes = srcK / SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint32_t sreg = oriM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;

    __ubuf__ T1* dstUb = (__ubuf__ T1*)dst.GetPhyAddr();
    __ubuf__ T2* sumUb = (__ubuf__ T2*)inSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)inMaxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)src.GetPhyAddr();

    Internal::SimpleSoftMaxGenericNZImpl<T1, T2>(dstUb, sumUb, maxUb, srcUb, mRepeatTimes,
                                                kRepeatTimes, sreg, dataBlock);
}

template <typename T1, typename T2, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMaxNDImpl(const LocalTensor<T1>& dst, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<T1>& src, const LocalTensor<float> workLocal,
    const SoftMaxTiling& tiling)
{
    static_assert((SupportType<Tuple<T1, T2>, Tuple<half, float>, Tuple<half, half>,
                Tuple<float, float>>()), "Failed to check dtype in SimpleSoftMax, current api "
                "support dtype combination is T1 : half, T2 : float; T1 : half, T2 : half; "
                "T1 : float, T2 : float");
    uint16_t srcM = tiling.srcM;
    uint16_t srcK = tiling.srcK;
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(srcK, FLOAT_REPEAT_SIZE));
    constexpr uint16_t blockStride = GetDataBlockSizeInBytes() / sizeof(T2);

    __ubuf__ T1* dstUb = (__ubuf__ T1*)dst.GetPhyAddr();
    __ubuf__ T2* sumUb = (__ubuf__ T2*)inSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)inMaxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)src.GetPhyAddr();

    if constexpr (isBasicBlock) {
        Internal::SimpleSoftMaxGenericNDImpl<T1, T2>(
            dstUb, sumUb, maxUb, srcUb, srcM, srcK, repeatTimes, blockStride);
    } else {
        if constexpr (config.oriSrcM == 0 || config.oriSrcK == 0) {
            if (tiling.srcK % FLOAT_REPEAT_SIZE != 0) {
                Internal::SimpleSoftMaxGenericNDWithTailImpl<T1, T2>(
                    dstUb, sumUb, maxUb, srcUb, srcM, srcK, repeatTimes, blockStride);
            } else {
                Internal::SimpleSoftMaxGenericNDImpl<T1, T2>(
                    dstUb, sumUb, maxUb, srcUb, srcM, srcK, repeatTimes, blockStride);
            }
        } else if constexpr (config.oriSrcK % FLOAT_REPEAT_SIZE != 0) {
            Internal::SimpleSoftMaxGenericNDWithTailImpl<T1, T2>(
                dstUb, sumUb, maxUb, srcUb, srcM, srcK, repeatTimes, blockStride);
        } else {
            Internal::SimpleSoftMaxGenericNDImpl<T1, T2>(
                dstUb, sumUb, maxUb, srcUb, srcM, srcK, repeatTimes, blockStride);
        }
    }
}

template <typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, 
    bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMaxBaseImpl(const LocalTensor<T1>& dst, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<T1>& src, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    ShapeInfo srcShape = src.GetShapeInfo();
    LastAxisShapeND srcNDinfo;
    LastAxisShapeND originalSrcShape;
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        srcNDinfo = GetLastAxisShapeND(srcShape);
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    } else {
        srcNDinfo = { softmaxShapeInfo.srcM, softmaxShapeInfo.srcK };
        originalSrcShape = { softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK };
    }
    if constexpr (isDataFormatNZ) {
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxTilingFunc(workLocal.GetSize(), { srcNDinfo.m, srcNDinfo.k, originalSrcShape.m, srcNDinfo.k },
                newTiling, sizeof(T1), sizeof(T2), false, isDataFormatNZ);
            SimpleSoftMaxNZImpl<T1, T2>(dst, inSumTensor, inMaxTensor, src, workLocal, newTiling, originalSrcShape);
        } else {
            SimpleSoftMaxNZImpl<T1, T2>(dst, inSumTensor, inMaxTensor, src, workLocal, tiling, originalSrcShape);
        }
    } else {
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxTilingFunc(workLocal.GetSize(), { srcNDinfo.m, srcNDinfo.k, originalSrcShape.m, srcNDinfo.k },
                newTiling, sizeof(T1), sizeof(T2), isBasicBlock);
            SimpleSoftMaxNDImpl<T1, T2, isBasicBlock, config>(dst, inSumTensor, inMaxTensor, src, workLocal, newTiling);
        } else {
            SimpleSoftMaxNDImpl<T1, T2, isBasicBlock, config>(dst, inSumTensor, inMaxTensor, src, workLocal, tiling);
        }
    }
}

}
#endif // IMPL_ACTIVATION_SOFTMAX_L300_SIMPLE_SOFTMAX_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLE_SOFTMAX_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLE_SOFTMAX_IMPL_H__
#endif
