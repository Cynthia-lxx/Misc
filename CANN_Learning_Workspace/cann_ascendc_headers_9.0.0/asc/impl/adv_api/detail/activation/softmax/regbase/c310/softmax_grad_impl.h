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
 * \file softmax_grad_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/regbase/c310/softmax_grad_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxgrad.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_GRAD_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_GRAD_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_GRAD_IMPL_H

#include "softmax_common_impl.h"

namespace AscendC {
template <typename T, bool isFront = true>
__simd_vf__ inline void SoftmaxGradGenericNZWithTailVFImpl(__ubuf__ T *dstUb, __ubuf__ T *gradUb,
    __ubuf__ T *srcUb, __ubuf__ float *workUb, __ubuf__ T *workUbFront,
    const SoftMaxTiling tiling, const LastAxisShapeND originalSrcShape, uint16_t dtypeRepStride)
{
    uint16_t srcM = tiling.srcM;
    uint16_t oriM = originalSrcShape.m;
    uint16_t kOuter = originalSrcShape.k / B16_DATA_NUM_PER_BLOCK;
    uint16_t dtypeBlkStride = dtypeRepStride / DEFAULT_BLK_NUM;
    uint16_t dataPadInner = srcM * B16_DATA_NUM_PER_BLOCK;
    uint16_t mRepeatInner = (dataPadInner + FLOAT_REPEAT_SIZE - 1) / FLOAT_REPEAT_SIZE;
    uint16_t dataNumAfterVcg = srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT / DEFAULT_BLK_NUM;
    // vcg post process do 2vl load/store, it use Half repeat size.
    uint16_t VcgFoldRepeat = (dataNumAfterVcg + HALF_REPEAT_SIZE - 1) / HALF_REPEAT_SIZE;
    uint16_t e2bRep = srcM / DEFAULT_BLK_NUM;

    Reg::MaskReg pregCnt;
    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    Reg::MaskReg pregkTail = Reg::MoveMask<uint32_t>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> gradVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<T> castReg;

    for (uint16_t i = 0; i < mRepeatInner; ++i) {
        Duplicate(sumVreg, 0);
        for (uint16_t j = 0; j < kOuter; ++j) {
            LoadIfNeedCast<T>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j *dataPadInner, pregFull);
            LoadIfNeedCast<T>(gradVreg, gradUb + i * FLOAT_REPEAT_SIZE + j *dataPadInner, pregFull);
            Reg::Mul(tmpVreg, gradVreg, srcVreg, pregFull);
            Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        }
        uint32_t tailOffset = i * FLOAT_REPEAT_SIZE + kOuter * dataPadInner;
        LoadIfNeedCast<T>(srcVreg, srcUb + tailOffset, pregFull);
        LoadIfNeedCast<T>(gradVreg, gradUb + tailOffset, pregFull);
        Reg::Mul(tmpVreg, gradVreg, srcVreg, pregkTail);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        Reg::ReduceSumWithDataBlock(sumVreg, sumVreg, pregFull);
        Reg::StoreAlign(workUb + i * DEFAULT_BLK_NUM, sumVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; i++) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(
            sumVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        if constexpr (isFront) {
            StoreIfNeedCast<T>(workUbFront + i * FLOAT_REPEAT_SIZE, sumVreg, pregFull);
        } else {
            Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
                workUb + i * HALF_REPEAT_SIZE, sumVreg, sumVreg, pregFull);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    if constexpr (isFront) {
        uint32_t sreg = oriM * dtypeBlkStride;
        for (uint16_t i = 0; i < e2bRep; ++i) {
            pregCnt = Reg::UpdateMask<T>(sreg);
            LoadE2B<T>(castReg, workUbFront + i * DEFAULT_BLK_NUM);
            Reg::StoreAlign(dstUb + i * dtypeRepStride, castReg, pregCnt);
        }
    } else {
        for (uint16_t j = 0; j < kOuter; ++j) {
            uint32_t sreg = oriM * B16_DATA_NUM_PER_BLOCK;
            for (uint16_t i = 0; i < mRepeatInner; ++i) {
                pregCnt = Reg::UpdateMask<uint32_t>(sreg);
                LoadIfNeedCast<T>(srcVreg, srcUb + j * dataPadInner + i * FLOAT_REPEAT_SIZE, pregFull);
                LoadIfNeedCast<T>(gradVreg, gradUb + j * dataPadInner + i * FLOAT_REPEAT_SIZE, pregFull);
                LoadE2B<float>(sumVreg, workUb + i * DEFAULT_BLK_NUM);
                Reg::Sub(tmpVreg, gradVreg, sumVreg, pregCnt);
                Reg::Mul(tmpVreg, srcVreg, tmpVreg, pregCnt);
                StoreIfNeedCast<T>(dstUb + j * dataPadInner + i * FLOAT_REPEAT_SIZE, tmpVreg, pregCnt);
            }
        }
        uint32_t sreg = oriM * B16_DATA_NUM_PER_BLOCK;
        for (uint16_t i = 0; i < mRepeatInner; ++i) {
            uint32_t tailOffset = i * FLOAT_REPEAT_SIZE + kOuter * dataPadInner;
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            Reg::MaskAnd(pregOneBlk, pregCnt, pregkTail, pregFull);
            LoadIfNeedCast<T>(srcVreg, srcUb + tailOffset, pregFull);
            LoadIfNeedCast<T>(gradVreg, gradUb + tailOffset, pregFull);
            LoadE2B<float>(sumVreg, workUb + i * DEFAULT_BLK_NUM);
            Reg::Sub(tmpVreg, gradVreg, sumVreg, pregOneBlk);
            Reg::Mul(tmpVreg, srcVreg, tmpVreg, pregOneBlk);
            StoreIfNeedCast<T>(dstUb + tailOffset, tmpVreg, pregOneBlk);
        }
    }
}

template <typename T, bool isFront = true>
__aicore__ inline void SoftmaxGradGenericNZWithTailImpl(const LocalTensor<T>& dstTensor,
    const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, const LastAxisShapeND& originalSrcShape)
{
    __ubuf__ T *srcUb = (__ubuf__ T *)srcTensor.GetPhyAddr();
    __ubuf__ T *gradUb = (__ubuf__ T *)gradTensor.GetPhyAddr();
    __ubuf__ T *dstUb = (__ubuf__ T *)dstTensor.GetPhyAddr();
    __ubuf__ float *workUb = (__ubuf__ float *)workLocal.GetPhyAddr();
    __ubuf__ T *workUbFront = (__ubuf__ T *)workLocal.GetPhyAddr();

    uint64_t kTail = originalSrcShape.k % B16_DATA_NUM_PER_BLOCK;
    // create k inner tail mask repeat 4 time.
    uint64_t mask[2] = {0, 0};
    CreateSpecialFormatMask(mask[0], kTail, FLOAT_REPEAT_SIZE / SOFTMAX_SHAPE_NZ_BASIC_COUNT);
    SetVectorMask<uint32_t>(mask[1], mask[0]);
    uint16_t dtypeRepStride = IsSameType<T, half>::value ? HALF_REPEAT_SIZE:FLOAT_REPEAT_SIZE;

    SoftmaxGradGenericNZWithTailVFImpl<T, isFront>(dstUb, gradUb, srcUb, workUb, workUbFront,
        tiling, originalSrcShape, dtypeRepStride);
}

template <typename T, bool isFront = true>
__simd_vf__ inline void SoftmaxGradGenericNZVFImpl(__ubuf__ T *dstUb, __ubuf__ T *gradUb,
    __ubuf__ T *srcUb, __ubuf__ float *workUb, __ubuf__ T *workUbFront,
    const SoftMaxTiling tiling, const LastAxisShapeND originalSrcShape, uint16_t dtypeRepStride)
{
    uint16_t oriM = originalSrcShape.m;
    uint16_t srcM = tiling.srcM;
    uint16_t dtypeBlkStride = dtypeRepStride / DEFAULT_BLK_NUM;
    uint16_t kOuter = tiling.srcK / B16_DATA_NUM_PER_BLOCK;
    uint16_t dataLenInner = oriM * B16_DATA_NUM_PER_BLOCK;
    uint16_t dataPadInner = srcM * B16_DATA_NUM_PER_BLOCK;
    uint16_t kRepeatInner = (dataPadInner + FLOAT_REPEAT_SIZE - 1) / FLOAT_REPEAT_SIZE;
    uint16_t dataNumAfterVcg = srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT / DEFAULT_BLK_NUM;
    // vcg post process do 2vl load/store, it use Half repeat size.
    uint16_t VcgFoldRepeat = (dataNumAfterVcg + HALF_REPEAT_SIZE - 1) / HALF_REPEAT_SIZE;
    uint16_t e2bRep = tiling.srcM / DEFAULT_BLK_NUM;

    Reg::MaskReg pregCnt;
    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> gradVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<T> castReg;

    for (uint16_t i = 0; i < kRepeatInner; ++i) {
        Duplicate(sumVreg, 0);
        for (uint16_t j = 0; j < kOuter; ++j) {
            LoadIfNeedCast<T>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataPadInner, pregFull);
            LoadIfNeedCast<T>(gradVreg, gradUb + i * FLOAT_REPEAT_SIZE + j * dataPadInner, pregFull);
            Reg::Mul(tmpVreg, gradVreg, srcVreg, pregFull);
            Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        }
        Reg::ReduceSumWithDataBlock(sumVreg, sumVreg, pregFull);
        Reg::StoreAlign(workUb + i * DEFAULT_BLK_NUM, sumVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; i++) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(
            sumVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        if constexpr (isFront) {
            StoreIfNeedCast<T>(workUbFront + i * FLOAT_REPEAT_SIZE, sumVreg, pregFull);
        } else {
            Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
                workUb + i * HALF_REPEAT_SIZE, sumVreg, sumVreg, pregFull);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    if constexpr (isFront) {
        uint32_t sreg = oriM * dtypeBlkStride;
        for (uint16_t i = 0; i < e2bRep; ++i) {
            pregCnt = Reg::UpdateMask<T>(sreg);
            LoadE2B<T>(castReg, workUbFront + i * DEFAULT_BLK_NUM);
            Reg::StoreAlign(dstUb + i * dtypeRepStride, castReg, pregCnt);
        }
    } else {
        for (uint16_t j = 0; j < kOuter; ++j) {
            uint32_t sreg = dataLenInner;
            for (uint16_t i = 0; i < kRepeatInner; ++i) {
                pregCnt = Reg::UpdateMask<uint32_t>(sreg);
                LoadIfNeedCast<T>(srcVreg, srcUb + j * dataPadInner + i * FLOAT_REPEAT_SIZE, pregFull);
                LoadIfNeedCast<T>(gradVreg, gradUb + j * dataPadInner + i * FLOAT_REPEAT_SIZE, pregFull);
                LoadE2B<float>(sumVreg, workUb + i * DEFAULT_BLK_NUM);
                Reg::Sub(tmpVreg, gradVreg, sumVreg, pregCnt);
                Reg::Mul(tmpVreg, srcVreg, tmpVreg, pregCnt);
                StoreIfNeedCast<T>(dstUb + j * dataPadInner + i * FLOAT_REPEAT_SIZE, tmpVreg, pregCnt);
            }
        }
    }
}

template <typename T, bool isFront = true>
__aicore__ inline void SoftmaxGradGenericNZImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor,
    const LocalTensor<T>& srcTensor, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const LastAxisShapeND& originalSrcShape)
{
    __ubuf__ T *srcUb = (__ubuf__ T *)srcTensor.GetPhyAddr();
    __ubuf__ T *gradUb = (__ubuf__ T *)gradTensor.GetPhyAddr();
    __ubuf__ T *dstUb = (__ubuf__ T *)dstTensor.GetPhyAddr();
    __ubuf__ float *workUb = (__ubuf__ float *)workLocal.GetPhyAddr();
    __ubuf__ T *workUbFront = (__ubuf__ T *)workLocal.GetPhyAddr();

    uint16_t dtypeRepStride = IsSameType<T, half>::value ? HALF_REPEAT_SIZE:FLOAT_REPEAT_SIZE;
    SoftmaxGradGenericNZVFImpl<T, isFront>(dstUb, gradUb, srcUb, workUb, workUbFront,
        tiling, originalSrcShape, dtypeRepStride);
}

template <typename T, bool isFront = true>
__simd_vf__ inline void SoftMaxGradGenericNDVFImpl(__ubuf__ T *dstUb, __ubuf__ T *gradUb,
    __ubuf__ T *srcUb, const SoftMaxTiling tiling, const LastAxisShapeND originalSrcShape,
    uint16_t blockStride)
{
    uint16_t srcM = originalSrcShape.m;
    uint16_t srcK = tiling.srcK;
    uint16_t oriK = originalSrcShape.k;
    uint16_t repeatTimes = (srcK + FLOAT_REPEAT_SIZE - 1) / FLOAT_REPEAT_SIZE;

    Reg::MaskReg pregCnt;
    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOneBlk;
    if constexpr (IsSameType<T, half>::value) {
        pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
    } else {
        pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    }
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> gradVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    for (uint16_t i = 0; i < srcM; ++i) {
        uint32_t sreg = oriK;
        Duplicate(sumVreg, 0);
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadIfNeedCast<T>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            LoadIfNeedCast<T>(gradVreg, gradUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            Reg::Mul(tmpVreg, gradVreg, srcVreg, pregCnt);
            Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        }
        Reg::ReduceSum(tmpVreg, sumVreg, pregFull);
        if constexpr (isFront) {
            Duplicate(tmpVreg, tmpVreg, pregOneBlk);
            StoreIfNeedCast<T>(dstUb + i * blockStride, tmpVreg, pregOneBlk);
        } else {
            Duplicate(sumVreg, tmpVreg, pregFull);
            sreg = oriK;
            for (uint16_t j = 0; j < repeatTimes; ++j) {
                pregCnt = Reg::UpdateMask<uint32_t>(sreg);
                LoadIfNeedCast<T>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
                LoadIfNeedCast<T>(gradVreg, gradUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
                Reg::Sub(tmpVreg, gradVreg, sumVreg, pregCnt);
                Reg::Mul(tmpVreg, srcVreg, tmpVreg, pregCnt);
                StoreIfNeedCast<T>(dstUb + i * srcK + j * FLOAT_REPEAT_SIZE, tmpVreg, pregCnt);
            }
        }
    }
}

template <typename T, bool isFront = true>
__aicore__ inline void SoftMaxGradGenericNDImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor,
    const LocalTensor<T>& srcTensor, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const LastAxisShapeND& originalSrcShape)
{
    __ubuf__ T *srcUb = (__ubuf__ T *)srcTensor.GetPhyAddr();
    __ubuf__ T *gradUb = (__ubuf__ T *)gradTensor.GetPhyAddr();
    __ubuf__ T *dstUb = (__ubuf__ T *)dstTensor.GetPhyAddr();

    uint16_t blockStride = IsSameType<T, half>::value ? B16_DATA_NUM_PER_BLOCK:B32_DATA_NUM_PER_BLOCK;
    SoftMaxGradGenericNDVFImpl<T, isFront>(dstUb, gradUb, srcUb, tiling, originalSrcShape, blockStride);
}

template <typename T>
__aicore__ inline void SoftmaxGradNZImpl(const LocalTensor<T>& dst, const LocalTensor<T>& gradTensor,
    const LocalTensor<T>& src, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling, bool isFront = false)
{
    static_assert(SupportType<T, half, float>(), "SoftMaxGrad api only support half/float on current device");
    if (tiling.srcK != originalSrcShape.k) {
        if (isFront) {
            SoftmaxGradGenericNZWithTailImpl<T, true>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
        } else {
            SoftmaxGradGenericNZWithTailImpl<T, false>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
        }
    } else {
        if (isFront) {
            SoftmaxGradGenericNZImpl<T, true>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
        } else {
            SoftmaxGradGenericNZImpl<T, false>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
        }
    }
}

template <typename T>
__aicore__ inline void SoftmaxGradFrontNZImpl(const LocalTensor<T>& dst, const LocalTensor<T>& gradTensor,
    const LocalTensor<T>& src, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    static_assert(SupportType<T, half, float>(), "SoftMaxGradFront api only support half/float on current device");
    if (tiling.srcK != originalSrcShape.k) {
        SoftmaxGradGenericNZWithTailImpl<T, true>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
    } else {
        SoftmaxGradGenericNZImpl<T, true>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
    }
}

template <typename T>
__aicore__ inline void SoftmaxGradPostProcess(const LocalTensor<T>& dst, const LocalTensor<T>& gradTensor,
    const LocalTensor<T>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const LastAxisShapeND& originalSrcShape, bool isFront = false)
{
    static_assert(SupportType<T, half, float>(), "SoftMaxGrad api only support half/float on current device");
    if (isFront) {
        SoftMaxGradGenericNDImpl<T, true>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
    } else {
        SoftMaxGradGenericNDImpl<T, false>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
    }
}

template <typename T, bool isBasicBlock = false>
__aicore__ inline void SoftmaxGradFrontNDImpl(const LocalTensor<T>& dst, const LocalTensor<T>& gradTensor,
    const LocalTensor<T>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const LastAxisShapeND& originalSrcShape)
{
    static_assert(SupportType<T, half, float>(), "SoftMaxGradFront api only support half/float on current device");
    SoftMaxGradGenericNDImpl<T, true>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
}
}  // namespace AscendC
#endif  // IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_GRAD_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_GRAD_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_GRAD_IMPL_H__
#endif
