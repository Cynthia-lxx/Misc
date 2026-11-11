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
 * \file cumsum_c310_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/cumsum/regbase/c310/cumsum_c310_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/cumsum.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CUMSUM_REGBASE_C310_CUMSUM_C310_UTILS_H__
#endif
#ifndef IMPL_MATH_CUMSUM_CUMSUM_C310_UTILS_H
#define IMPL_MATH_CUMSUM_CUMSUM_C310_UTILS_H
#include "kernel_basic_intf.h"
#include "kernel_tensor.h"

namespace AscendC {

namespace Internal {
template <typename T>
__simd_callee__ inline void LoadDataWithT(__ubuf__ T* src, Reg::RegTensor<float>& dstReg,
                                          Reg::MaskReg& dstPreg, uint32_t srcOffset)
{
    if constexpr (IsSameType<T, half>::value || IsSameType<T, bfloat16_t>::value) {
        Reg::RegTensor<T> srcOrigin;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin, src + srcOffset);
        Cast<float, T, layoutZMrgZ>(dstReg, srcOrigin, dstPreg);
    } else { // this branch: only support float
        Reg::LoadAlign(dstReg, src + srcOffset);
    }
}

template <typename T>
__simd_callee__ inline void SaveDataWithT(__ubuf__ T* dst, Reg::RegTensor<float>& srcReg,
                                          Reg::MaskReg& dstPreg, uint32_t dstOffset)
{
    if constexpr (IsSameType<T, half>::value || IsSameType<T, bfloat16_t>::value) {
        Reg::RegTensor<T> regT;
        Cast<T, float, LayoutZMrgZRndRSatNS>(regT, srcReg, dstPreg);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + dstOffset, regT, dstPreg);
    } else {
        Reg::StoreAlign(dst + dstOffset, srcReg, dstPreg);
    }
}

// process by tempBuffer
// T: fp16-> U: fp32
// T: fp32-> U: fp16
template <typename U, typename T>
__simd_vf__ inline void CumSumCopyWithCastVF(__ubuf__ T* src, __ubuf__ U* dst, const uint16_t outter,
                                             const uint16_t inner, uint16_t innerOneRepNum, uint16_t mainRepeatTime,
                                             uint16_t tailRepeatTime, uint32_t tailCount, uint16_t halfOutter,
                                             uint16_t tailOutter)
{
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> srcReg1;
    Reg::MaskReg fullMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg tailMask = Reg::UpdateMask<float>(tailCount);
    for (uint16_t j = 0; j < halfOutter; ++j) {
        for (uint16_t i = 0; i < mainRepeatTime; ++i) {
            LoadDataWithT<T>(src, srcReg, fullMask, j * inner + i * innerOneRepNum);
            SaveDataWithT<U>(dst, srcReg, fullMask, j * inner + i * innerOneRepNum);
            LoadDataWithT<T>(src, srcReg1, fullMask, (j + halfOutter) * inner + i * innerOneRepNum);
            SaveDataWithT<U>(dst, srcReg1, fullMask, (j + halfOutter) * inner + i * innerOneRepNum);
        }
        for (uint16_t i = 0; i < tailRepeatTime; ++i) {
            LoadDataWithT<T>(src, srcReg, tailMask, j * inner + (i + mainRepeatTime) * innerOneRepNum);
            SaveDataWithT<U>(dst, srcReg, tailMask, j * inner + (i + mainRepeatTime) * innerOneRepNum);
            LoadDataWithT<T>(src, srcReg1, tailMask, (j + halfOutter) * inner + (i + mainRepeatTime) * innerOneRepNum);
            SaveDataWithT<U>(dst, srcReg1, tailMask, (j + halfOutter) * inner + (i + mainRepeatTime) * innerOneRepNum);
        }
    }
    for (uint16_t j = 0; j < tailOutter; ++j) {
        for (uint16_t i = 0; i < mainRepeatTime; ++i) {
            LoadDataWithT<T>(src, srcReg, fullMask, (j + halfOutter * 2) * inner + i * innerOneRepNum);
            SaveDataWithT<U>(dst, srcReg, fullMask, (j + halfOutter * 2) * inner + i * innerOneRepNum);
        }
        for (uint16_t i = 0; i < tailRepeatTime; ++i) {
            LoadDataWithT<T>(src, srcReg, tailMask,
                             (j + halfOutter * 2) * inner + (i + mainRepeatTime) * innerOneRepNum);
            SaveDataWithT<U>(dst, srcReg, tailMask,
                             (j + halfOutter * 2) * inner + (i + mainRepeatTime) * innerOneRepNum);
        }
    }
}

template <typename U, typename T>
__simd_vf__ inline void CumSumCopyWithCast2VF(__ubuf__ T* src, __ubuf__ U* dst, const uint16_t outter,
                                              const uint16_t inner, uint16_t innerOneRepNum, uint16_t mainRepeatTime,
                                              uint16_t tailRepeatTime, uint32_t tailCount)
{
    Reg::RegTensor<float> srcReg;
    Reg::MaskReg fullMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg tailMask = Reg::UpdateMask<float>(tailCount);
    for (uint16_t j = 0; j < outter; ++j) {
        for (uint16_t i = 0; i < mainRepeatTime; ++i) {
            LoadDataWithT<T>(src, srcReg, fullMask, j * inner + i * innerOneRepNum);
            SaveDataWithT<U>(dst, srcReg, fullMask, j * inner + i * innerOneRepNum);
        }
        for (uint16_t i = 0; i < tailRepeatTime; ++i) {
            LoadDataWithT<T>(src, srcReg, tailMask, j * inner + (i + mainRepeatTime) * innerOneRepNum);
            SaveDataWithT<U>(dst, srcReg, tailMask, j * inner + (i + mainRepeatTime) * innerOneRepNum);
        }
    }
}

template <typename U, typename T>
__aicore__ inline void CumSumCopyWithCast(const LocalTensor<U>& dstTensor, const LocalTensor<T>& srcTensor,
                                          const uint32_t outter, const uint32_t inner)
{
    __ubuf__ T* src = (__ubuf__ T*)srcTensor.GetPhyAddr();
    __ubuf__ U* dst = (__ubuf__ U*)dstTensor.GetPhyAddr();
    constexpr uint16_t innerOneRepNum = (uint16_t)(GetVecLen() / sizeof(float));
    uint16_t mainRepeatTime = inner / innerOneRepNum;
    uint32_t tailCount = inner % innerOneRepNum;
    uint16_t tailRepeatTime = tailCount > 0 ? 1 : 0;
    uint16_t halfOutter = static_cast<uint16_t>(outter) / 2;
    uint16_t tailOutter = static_cast<uint16_t>(outter) % 2;
    if (outter > inner) {
        CumSumCopyWithCastVF<U, T>(src, dst, static_cast<uint16_t>(outter), static_cast<uint16_t>(inner),
                                   innerOneRepNum, mainRepeatTime, tailRepeatTime, tailCount, halfOutter, tailOutter);
    } else {
        CumSumCopyWithCast2VF<U, T>(src, dst, static_cast<uint16_t>(outter), static_cast<uint16_t>(inner),
                                    innerOneRepNum, mainRepeatTime, tailRepeatTime, tailCount);
    }
}

template <typename T>
__simd_vf__ inline void CumSumCopyOutWithBlockVF(__ubuf__ T* src, __ubuf__ T* dst, uint16_t outter, uint16_t inner,
                                                 uint16_t mainRepeatTime, uint16_t innerOneRepNum, uint32_t tailCount,
                                                 uint16_t tailRepeatTime)
{
    Reg::RegTensor<T> srcReg;
    Reg::MaskReg fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL>();
    Reg::MaskReg tailMask = Reg::UpdateMask<T>(tailCount);

    for (uint16_t i = 0; i < outter; ++i) {
        for (uint16_t j = 0; j < mainRepeatTime; ++j) {
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(srcReg, src + i * inner + j * innerOneRepNum,
                                                                           1, fullMask);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(dst + i * inner + j * innerOneRepNum, srcReg,
                                                                           1, fullMask);
        }
        for (uint16_t j = 0; j < tailRepeatTime; ++j) {
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                srcReg, src + i * inner + mainRepeatTime * innerOneRepNum, 1, tailMask);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * inner + mainRepeatTime * innerOneRepNum, srcReg, 1, tailMask);
        }
    }
}

template <typename T>
__simd_vf__ inline void CumSumCopyOutVF(__ubuf__ T* src, __ubuf__ T* dst, uint16_t outter, uint16_t inner,
                                        uint16_t mainRepeatTime, uint16_t innerOneRepNum, uint32_t tailCount,
                                        uint16_t tailRepeatTime, uint16_t halfOutter, uint16_t tailOutter)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> srcReg1;
    Reg::MaskReg fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL>();
    Reg::MaskReg tailMask = Reg::UpdateMask<T>(tailCount);

    for (uint16_t j = 0; j < halfOutter; ++j) {
        for (uint16_t i = 0; i < mainRepeatTime; ++i) {
            Reg::LoadAlign(srcReg, src + (j * 2) * inner + i * innerOneRepNum);
            Reg::StoreAlign(dst + (j * 2) * inner + i * innerOneRepNum, srcReg, fullMask);
            Reg::LoadAlign(srcReg1, src + (j * 2 + 1) * inner + i * innerOneRepNum);
            Reg::StoreAlign(dst + (j * 2 + 1) * inner + i * innerOneRepNum, srcReg1, fullMask);
        }
        for (uint16_t i = 0; i < tailRepeatTime; ++i) {
            Reg::LoadAlign(srcReg, src + (j * 2) * inner + (i + mainRepeatTime) * innerOneRepNum);
            Reg::StoreAlign(dst + (j * 2) * inner + (i + mainRepeatTime) * innerOneRepNum, srcReg, tailMask);
            Reg::LoadAlign(srcReg1, src + (j * 2 + 1) * inner + (i + mainRepeatTime) * innerOneRepNum);
            Reg::StoreAlign(dst + (j * 2 + 1) * inner + (i + mainRepeatTime) * innerOneRepNum, srcReg1, tailMask);
        }
    }

    for (uint16_t j = 0; j < tailOutter; ++j) {
        for (uint16_t i = 0; i < mainRepeatTime; ++i) {
            Reg::LoadAlign(srcReg, src + (j + 2 * halfOutter) * inner + i * innerOneRepNum);
            Reg::StoreAlign(dst + (j + 2 * halfOutter) * inner + i * innerOneRepNum, srcReg, fullMask);
        }
        for (uint16_t i = 0; i < tailRepeatTime; ++i) {
            Reg::LoadAlign(srcReg, src + (j + 2 * halfOutter) * inner + (i + mainRepeatTime) * innerOneRepNum);
            Reg::StoreAlign(dst + (j + 2 * halfOutter) * inner + (i + mainRepeatTime) * innerOneRepNum, srcReg,
                               tailMask);
        }
    }
}

template <typename T>
__aicore__ inline void CumSumCopyOut(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
                                     const uint32_t outter, const uint32_t inner)
{
    __ubuf__ T* src = (__ubuf__ T*)srcTensor.GetPhyAddr();
    __ubuf__ T* dst = (__ubuf__ T*)dstTensor.GetPhyAddr();
    constexpr uint16_t innerOneRepNum = GetVecLen() / sizeof(T);
    constexpr uint16_t elePerBlock = ONE_BLK_SIZE / sizeof(T);
    uint16_t mainRepeatTime = inner / innerOneRepNum;
    uint32_t tailCount = inner % innerOneRepNum;
    uint16_t tailRepeatTime = tailCount > 0 ? 1 : 0;
    uint16_t halfOutter = outter / 2;
    uint16_t tailOutter = outter % 2;
    CumSumCopyOutVF<T>(src, dst, outter, inner, mainRepeatTime, innerOneRepNum, tailCount, tailRepeatTime, halfOutter,
                       tailOutter);
}

template <typename D, typename T, const Reg::RegTrait& Trait, const uint16_t vlSize>
__aicore__ inline void TransposeCommonGather(__ubuf__ D* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop1,
                                             uint32_t forLoop2, uint32_t srcStride1, uint32_t srcStride2)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

// VF for TransposeCommonGather (float, float)
template <typename T, const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void TransposeCommonGatherVFFF(__ubuf__ float* dstAddr, __ubuf__ float* srcAddr, uint32_t forLoop1,
                                                  uint32_t forLoop2, uint32_t srcStride1, uint32_t srcStride2,
                                                  uint32_t tail, uint32_t count, uint16_t mainLoop, uint32_t dtypeSize,
                                                  uint32_t tailLoop)
{
    Reg::RegTensor<uint32_t, Trait> indexReg;
    Reg::RegTensor<T, Trait> srcReg;
    Reg::MaskReg fullMask = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg indexFullMask = Reg::CreateMask<float, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg mainMask = Reg::CreateMask<float, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg tailMask = Reg::UpdateMask<float, Trait>(count);
    Reg::UnalignReg ureg0;
    Arange((Reg::RegTensor<int32_t, Trait>&)indexReg, static_cast<int32_t>(0));
    Muls(indexReg, indexReg, srcStride2, indexFullMask);
    for (uint16_t j = 0; j < static_cast<uint16_t>(forLoop1); j++) {
        uint64_t hoistDstAddr = (uint64_t)dstAddr + (uint64_t)(j * forLoop2 * dtypeSize);
        for (uint16_t k = 0; k < static_cast<uint16_t>(mainLoop); k++) {
            Reg::Gather(srcReg, srcAddr + j * srcStride1 + k * vlSize * srcStride2, indexReg, mainMask);
            Reg::StoreUnAlign(((__ubuf__ float*&)hoistDstAddr), srcReg, ureg0, vlSize);
        }
        for (uint16_t k = 0; k < static_cast<uint16_t>(tailLoop); k++) {
            Reg::Gather(srcReg, srcAddr + j * srcStride1 + mainLoop * vlSize * srcStride2, indexReg, tailMask);
            Reg::StoreUnAlign(((__ubuf__ float*&)hoistDstAddr), srcReg, ureg0, tail);
        }
        Reg::StoreUnAlignPost(((__ubuf__ float*&)hoistDstAddr), ureg0, 0);
    }
}

template <typename D = float, typename T = float, const Reg::RegTrait& Trait, const uint16_t vlSize>
__aicore__ inline void TransposeCommonGather(__ubuf__ float* dstAddr, __ubuf__ float* srcAddr, uint32_t forLoop1,
                                             uint32_t forLoop2, uint32_t srcStride1, uint32_t srcStride2)
{
    uint32_t tail = forLoop2 % vlSize;
    uint32_t count = tail;
    uint16_t mainLoop = forLoop2 / vlSize;
    uint32_t dtypeSize = sizeof(float);
    uint32_t tailLoop = tail > 0 ? 1 : 0;
    TransposeCommonGatherVFFF<T, Trait, vlSize>(dstAddr, srcAddr, forLoop1, forLoop2, srcStride1, srcStride2, tail,
                                                count, mainLoop, dtypeSize, tailLoop);
}

// VF for TransposeCommonGather (float, half)
template <const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void TransposeCommonGatherVFFH(__ubuf__ float* dstAddr, __ubuf__ half* srcAddr, uint32_t forLoop1,
                                                  uint32_t forLoop2, uint32_t srcStride1, uint32_t srcStride2,
                                                  uint32_t tail, uint32_t count, uint16_t mainLoop, uint32_t dtypeSize,
                                                  uint32_t tailLoop)
{
    Reg::RegTensor<uint16_t, Trait> indexReg;
    Reg::RegTensor<half, Trait> srcReg;
    Reg::RegTensor<float, Trait> vreg;
    Reg::RegTensor<uint16_t> zeroReg;
    Reg::RegTensor<half> castReg;
    Reg::RegTensor<uint16_t> tmpReg;
    Reg::MaskReg fullMask = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg indexFullMask = Reg::CreateMask<half, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg mainMask = Reg::CreateMask<half, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg tailMask = Reg::UpdateMask<half, Trait>(count);
    Reg::Duplicate(zeroReg, static_cast<uint16_t>(0), fullMask);
    Reg::UnalignReg ureg0;
    Arange((Reg::RegTensor<int16_t, Trait>&)indexReg, static_cast<int16_t>(0));
    Muls(indexReg, indexReg, static_cast<uint16_t>(srcStride2), indexFullMask);
    for (uint16_t j = 0; j < static_cast<uint16_t>(forLoop1); j++) {
        uint64_t hoistDstAddr = (uint64_t)dstAddr + (uint64_t)(j * forLoop2 * dtypeSize);
        for (uint16_t k = 0; k < static_cast<uint16_t>(mainLoop); k++) {
            Reg::Gather(srcReg, srcAddr + j * srcStride1 + k * vlSize * srcStride2, indexReg, mainMask);
            Reg::Interleave((Reg::RegTensor<uint16_t>&)castReg, (Reg::RegTensor<uint16_t>&)tmpReg,
                                 (Reg::RegTensor<uint16_t>&)srcReg, (Reg::RegTensor<uint16_t>&)zeroReg);
            Cast<float, half, layoutZMrgZ>(vreg, castReg, mainMask);
            Reg::StoreUnAlign(((__ubuf__ float*&)hoistDstAddr), vreg, ureg0, vlSize);
        }
        for (uint16_t k = 0; k < static_cast<uint16_t>(tailLoop); k++) {
            Reg::Gather(srcReg, srcAddr + j * srcStride1 + mainLoop * vlSize * srcStride2, indexReg, tailMask);
            Reg::Interleave((Reg::RegTensor<uint16_t>&)castReg, (Reg::RegTensor<uint16_t>&)tmpReg,
                                 (Reg::RegTensor<uint16_t>&)srcReg, (Reg::RegTensor<uint16_t>&)zeroReg);
            Cast<float, half, layoutZMrgZ>(vreg, castReg, mainMask);
            Reg::StoreUnAlign(((__ubuf__ float*&)hoistDstAddr), vreg, ureg0, tail);
        }
        Reg::StoreUnAlignPost(((__ubuf__ float*&)hoistDstAddr), ureg0, 0);
    }
}

template <typename D = float, typename T = half, const Reg::RegTrait& Trait, const uint16_t vlSize>
__aicore__ inline void TransposeCommonGather(__ubuf__ float* dstAddr, __ubuf__ half* srcAddr, uint32_t forLoop1,
                                             uint32_t forLoop2, uint32_t srcStride1, uint32_t srcStride2)
{
    uint32_t tail = forLoop2 % vlSize;
    uint32_t count = tail;
    uint16_t mainLoop = forLoop2 / vlSize;
    uint32_t dtypeSize = sizeof(float);
    uint32_t tailLoop = tail > 0 ? 1 : 0;
    TransposeCommonGatherVFFH<Trait, vlSize>(dstAddr, srcAddr, forLoop1, forLoop2, srcStride1, srcStride2, tail, count,
                                             mainLoop, dtypeSize, tailLoop);
}

// VF for TransposeCommonGather (half, float)
template <const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void TransposeCommonGatherVHF(__ubuf__ half* dstAddr, __ubuf__ float* srcAddr, uint32_t forLoop1,
                                                 uint32_t forLoop2, uint32_t srcStride1, uint32_t srcStride2,
                                                 uint32_t tail, uint32_t count, uint16_t mainLoop, uint32_t dtypeSize,
                                                 uint32_t tailLoop)
{
    Reg::RegTensor<uint32_t, Trait> indexReg;
    Reg::RegTensor<float, Trait> srcReg;
    Reg::RegTensor<half, Trait> vreg;
    Reg::RegTensor<uint16_t> zeroReg;
    Reg::RegTensor<half> castReg;
    Reg::RegTensor<uint16_t> tmpReg;
    Reg::MaskReg fullMask = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg indexFullMask = Reg::CreateMask<float, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg mainMask = Reg::CreateMask<float, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg tailMask = Reg::UpdateMask<float, Trait>(count);
    Reg::Duplicate(zeroReg, static_cast<uint16_t>(0), fullMask);
    Reg::UnalignReg ureg0;
    Arange((Reg::RegTensor<int32_t, Trait>&)indexReg, static_cast<int32_t>(0));
    Muls(indexReg, indexReg, srcStride2, indexFullMask);
    for (uint16_t j = 0; j < static_cast<uint16_t>(forLoop1); j++) {
        uint64_t hoistDstAddr = (uint64_t)dstAddr + (uint64_t)(j * forLoop2 * dtypeSize);
        for (uint16_t k = 0; k < static_cast<uint16_t>(mainLoop); k++) {
            Reg::Gather(srcReg, srcAddr + j * srcStride1 + k * vlSize * srcStride2, indexReg, mainMask);
            Cast<half, float, LayoutZMrgZRndRSatNS>(vreg, srcReg, fullMask);
            Reg::DeInterleave((Reg::RegTensor<uint16_t>&)castReg, (Reg::RegTensor<uint16_t>&)tmpReg,
                                   (Reg::RegTensor<uint16_t>&)vreg, (Reg::RegTensor<uint16_t>&)zeroReg);
            Reg::StoreUnAlign(((__ubuf__ half*&)hoistDstAddr), castReg, ureg0, vlSize);
        }
        for (uint16_t k = 0; k < static_cast<uint16_t>(tailLoop); k++) {
            Reg::Gather(srcReg, srcAddr + j * srcStride1 + mainLoop * vlSize * srcStride2, indexReg, tailMask);
            Cast<half, float, LayoutZMrgZRndRSatNS>(vreg, srcReg, fullMask);
            Reg::DeInterleave((Reg::RegTensor<uint16_t>&)castReg, (Reg::RegTensor<uint16_t>&)tmpReg,
                                   (Reg::RegTensor<uint16_t>&)vreg, (Reg::RegTensor<uint16_t>&)zeroReg);
            Reg::StoreUnAlign(((__ubuf__ half*&)hoistDstAddr), castReg, ureg0, tail);
        }
        Reg::StoreUnAlignPost(((__ubuf__ half*&)hoistDstAddr), ureg0, 0);
    }
}

template <typename D = half, typename T = float, const Reg::RegTrait& Trait, const uint16_t vlSize>
__aicore__ inline void TransposeCommonGather(__ubuf__ half* dstAddr, __ubuf__ float* srcAddr, uint32_t forLoop1,
                                             uint32_t forLoop2, uint32_t srcStride1, uint32_t srcStride2)
{
    uint32_t tail = forLoop2 % vlSize;
    uint32_t count = tail;
    uint16_t mainLoop = forLoop2 / vlSize;
    uint32_t dtypeSize = sizeof(half);
    uint32_t tailLoop = tail > 0 ? 1 : 0;
    TransposeCommonGatherVHF<Trait, vlSize>(dstAddr, srcAddr, forLoop1, forLoop2, srcStride1, srcStride2, tail, count,
                                            mainLoop, dtypeSize, tailLoop);
}

/*
scene: { shape:[A, B], format:"ND"} -->{ shape:[B, A], format:"ND"};
Src: T
Dst: D
1. need cast
TransposeAB [A, B] half => [B, A] float
TransposeAB [A, B] float => [B, A] half
2. no need cast
TransposeAB [A, B] float => [B, A] float
*/
template <typename D, typename T>
__aicore__ inline void TransposeAB(const LocalTensor<D>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t outer,
                                   uint32_t inner)
{
    uint32_t srcStride1 = 1;
    uint32_t srcStride2 = inner;
    constexpr uint16_t vlSize = GetVecLen() / sizeof(float);
    TransposeCommonGather<D, T, Reg::RegTraitNumOne, vlSize>(
        (__ubuf__ D*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), inner, outer, srcStride1, srcStride2);
}

__simd_vf__ inline void CumSumFirstDimSklanskyVF(__ubuf__ float* dst, uint32_t outer, uint32_t inner,
                                                 uint32_t currRound1, uint32_t currRound2, uint16_t indexRepeatTimes,
                                                 uint16_t jRepeatTimes, uint16_t repeatTimes, uint16_t sregLower)
{
    Reg::RegTensor<float> src0Reg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> dstReg;
    Reg::MaskReg preg;

    for (uint16_t index = 0; index < indexRepeatTimes; index++) {
        // Position of the prefix sum in the previous round
        uint32_t line0 = currRound1 - 1 + index * currRound2;
        for (uint16_t j = 0; j < jRepeatTimes; j++) {
            uint32_t line1 = line0 + j + 1;
            uint32_t extent = 1;
            if (line1 > outer - 1) {
                extent = 0;
            }
            for (uint16_t k = 0; k < static_cast<uint16_t>(extent); k++) {
                uint32_t count = inner;
                for (uint16_t i = 0; i < repeatTimes; i++) {
                    preg = Reg::UpdateMask<float>(count);
                    Reg::LoadAlign(src0Reg, dst + line0 * inner + i * sregLower);
                    Reg::LoadAlign(src1Reg, dst + line1 * inner + i * sregLower);
                    Reg::Add(dstReg, src0Reg, src1Reg, preg);
                    Reg::StoreAlign(dst + line1 * inner + i * sregLower, dstReg, preg);
                }
            }
        }
    }
}

__aicore__ inline void CumSumFirstDimSklansky(const LocalTensor<float>& dstTensor, uint32_t outer, uint32_t inner)
{
    constexpr uint32_t bound = 16;
    constexpr uint32_t halfSize = 2;
    uint32_t outerAlign = 0;
    for (uint32_t i = 0; i < bound; i++) {
        if (outer <= (1U << i)) {
            outerAlign = (1U << i);
            break;
        }
    }
    uint32_t round = outerAlign / halfSize;

    uint32_t currRound = 1;
    __ubuf__ float* dst = (__ubuf__ float*)dstTensor.GetPhyAddr();
    constexpr uint16_t sregLower = (uint32_t)(GetVecLen() / sizeof(float));
    uint16_t repeatTimes = CeilDivision(inner, sregLower);

    while (round >= 1) {
        uint32_t currRound1 = 1 << (currRound - 1);
        uint32_t currRound2 = 1 << currRound;
        uint16_t indexRepeatTimes = static_cast<uint16_t>(outerAlign / currRound2);
        uint16_t jRepeatTimes = static_cast<uint16_t>(currRound1);
        CumSumFirstDimSklanskyVF(dst, outer, inner, currRound1, currRound2, indexRepeatTimes, jRepeatTimes, repeatTimes,
                                 sregLower);
        round = round / halfSize;
        currRound += 1;
    }
}

// simple implementation that cumulatively adds elements
// VF for CumSumFirstDimBasic
// simple implementation that cumulatively adds elements
// VF for CumSumFirstDimBasic
__simd_vf__ inline void CumSumFirstDimBasicVF(__ubuf__ float* dst, uint16_t outerRepeatTime, uint16_t inner,
                                              uint16_t mainRepeatTime, uint16_t innerOneRepNum, uint16_t tailTime,
                                              uint32_t tailCount, uint16_t halfMainRepeatTime,
                                              uint16_t mainTailRepeatTime, uint16_t innerTailOffset1,
                                              uint16_t innerTailOffset2)
{
    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregTail = Reg::UpdateMask<float>(tailCount);
    Reg::RegTensor<float> srcLeftReg;
    Reg::RegTensor<float> srcRightReg;
    Reg::RegTensor<float> dstLeftReg;
    Reg::RegTensor<float> dstRightReg;

    for (uint16_t j = 0; j < halfMainRepeatTime; ++j) {
        Reg::LoadAlign(dstLeftReg, dst + j * innerOneRepNum);
        Reg::LoadAlign(dstRightReg, dst + (j + halfMainRepeatTime) * innerOneRepNum);
        for (uint16_t i = 0; i < outerRepeatTime; ++i) {
            Reg::LoadAlign(srcLeftReg, dst + (i + 1) * inner + j * innerOneRepNum);
            Reg::LoadAlign(srcRightReg, dst + (i + 1) * inner + (j + halfMainRepeatTime) * innerOneRepNum);
            Reg::Add(dstLeftReg, srcLeftReg, dstLeftReg, pregFull);
            Reg::Add(dstRightReg, srcRightReg, dstRightReg, pregFull);
            Reg::StoreAlign(dst + (i + 1) * inner + j * innerOneRepNum, dstLeftReg, pregFull);
            Reg::StoreAlign(dst + (i + 1) * inner + (j + halfMainRepeatTime) * innerOneRepNum, dstRightReg,
                               pregFull);
        }
    }

    Reg::LoadAlign(dstLeftReg, dst + innerTailOffset1);
    Reg::LoadAlign(dstRightReg, dst + innerTailOffset2);
    for (uint16_t i = 0; i < outerRepeatTime; ++i) {
        for (uint16_t j = 0; j < mainTailRepeatTime; ++j) {
            Reg::LoadAlign(srcLeftReg, dst + (i + 1) * inner + innerTailOffset1);
            Reg::Add(dstLeftReg, srcLeftReg, dstLeftReg, pregFull);
            Reg::StoreAlign(dst + (i + 1) * inner + innerTailOffset1, dstLeftReg, pregFull);
        }
        for (uint16_t j = 0; j < tailTime; ++j) {
            Reg::LoadAlign(srcRightReg, dst + (i + 1) * inner + innerTailOffset2);
            Reg::Add(dstRightReg, srcRightReg, dstRightReg, pregTail);
            Reg::StoreAlign(dst + (i + 1) * inner + innerTailOffset2, dstRightReg, pregTail);
        }
    }
}

__simd_vf__ inline void CumSumFirstDimBasic2VF(__ubuf__ float* dst, uint16_t outerRepeatTime, uint16_t inner,
                                               uint16_t mainRepeatTime, uint16_t innerOneRepNum, uint16_t tailTime,
                                               uint32_t tailCount, uint16_t halfMainRepeatTime,
                                               uint16_t mainTailRepeatTime, uint16_t innerTailOffset1,
                                               uint16_t innerTailOffset2)
{
    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregTail = Reg::UpdateMask<float>(tailCount);
    Reg::RegTensor<float> srcLeftReg;
    Reg::RegTensor<float> dstLeftReg;

    for (uint16_t j = 0; j < mainRepeatTime; ++j) {
        Reg::LoadAlign(dstLeftReg, dst + j * innerOneRepNum);
        for (uint16_t i = 0; i < outerRepeatTime; ++i) {
            Reg::LoadAlign(srcLeftReg, dst + (i + 1) * inner + j * innerOneRepNum);
            Reg::Add(dstLeftReg, srcLeftReg, dstLeftReg, pregFull);
            Reg::StoreAlign(dst + (i + 1) * inner + j * innerOneRepNum, dstLeftReg, pregFull);
        }
    }

    Reg::LoadAlign(dstLeftReg, dst + innerTailOffset2);
    for (uint16_t i = 0; i < outerRepeatTime; ++i) {
        for (uint16_t j = 0; j < tailTime; ++j) {
            Reg::LoadAlign(srcLeftReg, dst + (i + 1) * inner + innerTailOffset2);
            Reg::Add(dstLeftReg, srcLeftReg, dstLeftReg, pregTail);
            Reg::StoreAlign(dst + (i + 1) * inner + innerTailOffset2, dstLeftReg, pregTail);
        }
    }
}

__aicore__ inline void CumSumFirstDimBasic(const LocalTensor<float>& dstTensor, uint32_t outer, uint32_t inner)
{
    __ubuf__ float* dst = (__ubuf__ float*)dstTensor.GetPhyAddr();
    constexpr uint16_t innerOneRepNum = (uint32_t)(GetVecLen() / sizeof(float));
    uint16_t mainRepeatTime = 0;
    if constexpr (innerOneRepNum > 0) {
        mainRepeatTime = inner / innerOneRepNum;
    }
    uint16_t outterRepeatTime = static_cast<uint16_t>(outer - 1);
    uint32_t tailCount = inner - mainRepeatTime * innerOneRepNum;
    uint16_t halfMainRepeatTime = mainRepeatTime / 2;
    uint16_t mainTailRepeatTime = mainRepeatTime % 2;
    uint16_t tailRepeatTime = tailCount > 0 ? 1 : 0;
    uint16_t castedInner = static_cast<uint16_t>(inner);
    uint16_t innerTailOffset1 = halfMainRepeatTime * innerOneRepNum * 2;
    uint16_t innerTailOffset2 = mainRepeatTime * innerOneRepNum;
    CumSumFirstDimBasic2VF(dst, outterRepeatTime, castedInner, mainRepeatTime, innerOneRepNum, tailRepeatTime,
                           tailCount, halfMainRepeatTime, mainTailRepeatTime, innerTailOffset1, innerTailOffset2);
}

__aicore__ inline TransDataTo5HDParams ExtractTransDataParam(uint8_t repeatTimes, uint32_t inner, uint16_t alignOutter,
                                                             uint32_t oneBlockElementNum, uint16_t dstRepStride,
                                                             uint32_t srcRepStride)
{
    repeatTimes = 0;
    if (oneBlockElementNum > 0) {
        repeatTimes = inner / oneBlockElementNum;
    }
    if (repeatTimes > 1) {
        return TransDataTo5HDParams(false, false, repeatTimes, alignOutter, 1);
    } else {
        return TransDataTo5HDParams(false, false, repeatTimes, dstRepStride, srcRepStride);
    }
}

template <typename T = float>
__aicore__ inline void Transpose5HDAB(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
                                      const CumSumInfo& cumSumInfo)
{
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(float);
    uint8_t repeatTimes = 1;
    uint16_t dstRepStride = 0;
    uint16_t srcRepStride = 0;
    uint16_t alignOutter =
        (cumSumInfo.outter + NCHW_CONV_ADDR_LIST_SIZE - 1) / NCHW_CONV_ADDR_LIST_SIZE * NCHW_CONV_ADDR_LIST_SIZE;
    uint64_t transDataTo5HDDstLocalList[NCHW_CONV_ADDR_LIST_SIZE];
    uint64_t transDataTo5HDSrcLocalList[NCHW_CONV_ADDR_LIST_SIZE];
    if (cumSumInfo.outter == alignOutter && alignOutter > cumSumInfo.inner) {
        repeatTimes = alignOutter / NCHW_CONV_ADDR_LIST_SIZE;
        if (repeatTimes > 1) {
            dstRepStride = 2;
            srcRepStride = cumSumInfo.inner * 2;
        }
        TransDataTo5HDParams params(false, false, repeatTimes, dstRepStride, srcRepStride);
        for (int32_t i = 0; i < cumSumInfo.inner / oneBlockElementNum; i++) {
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
                transDataTo5HDSrcLocalList[n] =
                    (uint64_t)srcTensor[i * oneBlockElementNum + n * cumSumInfo.inner].GetPhyAddr();
            }
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE / 2; n++) {
                transDataTo5HDDstLocalList[n * 2] =
                    (uint64_t)dstTensor[(i * oneBlockElementNum + n) * alignOutter].GetPhyAddr();
                transDataTo5HDDstLocalList[n * 2 + 1] =
                    (uint64_t)dstTensor[(i * oneBlockElementNum + n) * alignOutter + oneBlockElementNum].GetPhyAddr();
            }
            TransDataTo5HD<float>(transDataTo5HDDstLocalList, transDataTo5HDSrcLocalList, params);
        }
    } else {
        TransDataTo5HDParams params = ExtractTransDataParam(repeatTimes, cumSumInfo.inner, alignOutter,
                                                            oneBlockElementNum, dstRepStride, srcRepStride);
        for (int32_t i = 0; i < alignOutter / NCHW_CONV_ADDR_LIST_SIZE; i++) {
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
                transDataTo5HDSrcLocalList[n] =
                    (uint64_t)srcTensor[((i * NCHW_CONV_ADDR_LIST_SIZE
                                          + n % (cumSumInfo.outter - i * NCHW_CONV_ADDR_LIST_SIZE))
                                         * cumSumInfo.inner)]
                        .GetPhyAddr();
            }
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE / 2; n++) {
                transDataTo5HDDstLocalList[n * 2] =
                    (uint64_t)dstTensor[i * NCHW_CONV_ADDR_LIST_SIZE + n * alignOutter].GetPhyAddr();
                transDataTo5HDDstLocalList[n * 2 + 1] =
                    (uint64_t)dstTensor[i * NCHW_CONV_ADDR_LIST_SIZE + n * alignOutter + oneBlockElementNum]
                        .GetPhyAddr();
            }
            TransDataTo5HD<float>(transDataTo5HDDstLocalList, transDataTo5HDSrcLocalList, params);
        }
    }
}

template <typename T = float>
__aicore__ inline void Transpose5HDBA(const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor,
                                      const CumSumInfo& cumSumInfo)
{
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(float);
    uint8_t repeatTimes = 1;
    uint16_t dstRepStride = 0;
    uint16_t srcRepStride = 0;
    uint16_t alignOutter =
        (cumSumInfo.outter + NCHW_CONV_ADDR_LIST_SIZE - 1) / NCHW_CONV_ADDR_LIST_SIZE * NCHW_CONV_ADDR_LIST_SIZE;
    uint64_t transDataTo5HDDstLocalList[NCHW_CONV_ADDR_LIST_SIZE];
    uint64_t transDataTo5HDSrcLocalList[NCHW_CONV_ADDR_LIST_SIZE];
    if (alignOutter > cumSumInfo.inner) {
        repeatTimes = alignOutter / NCHW_CONV_ADDR_LIST_SIZE;
        if (repeatTimes > 1) {
            dstRepStride = cumSumInfo.inner * 2;
            srcRepStride = 2;
        } else {
            dstRepStride = 0;
            srcRepStride = 0;
        }
        TransDataTo5HDParams paramsBack(false, false, repeatTimes, dstRepStride, srcRepStride);
        for (int32_t i = 0; i < cumSumInfo.inner / oneBlockElementNum; i++) {
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE / 2; n++) {
                transDataTo5HDSrcLocalList[n] =
                    (uint64_t)srcTensor[i * oneBlockElementNum * alignOutter + n * alignOutter].GetPhyAddr();
                transDataTo5HDSrcLocalList[n + NCHW_CONV_ADDR_LIST_SIZE / 2] =
                    (uint64_t)srcTensor[i * oneBlockElementNum * alignOutter + n * alignOutter + oneBlockElementNum]
                        .GetPhyAddr();
                transDataTo5HDDstLocalList[n * 2] =
                    (uint64_t)dstTensor[i * oneBlockElementNum + n * cumSumInfo.inner].GetPhyAddr();
                transDataTo5HDDstLocalList[n * 2 + 1] =
                    (uint64_t)dstTensor[i * oneBlockElementNum + (n + oneBlockElementNum) * cumSumInfo.inner]
                        .GetPhyAddr();
            }
            TransDataTo5HD<float>(transDataTo5HDDstLocalList, transDataTo5HDSrcLocalList, paramsBack);
        }
    } else {
        repeatTimes = cumSumInfo.inner / oneBlockElementNum;
        if (repeatTimes > 1) {
            dstRepStride = alignOutter;
            srcRepStride = 1;
        } else {
            dstRepStride = 0;
            srcRepStride = 0;
        }
        TransDataTo5HDParams paramsBack(false, false, repeatTimes, srcRepStride, dstRepStride);
        for (int32_t i = 0; i < alignOutter / NCHW_CONV_ADDR_LIST_SIZE; i++) {
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE / 2; n++) {
                transDataTo5HDSrcLocalList[n] =
                    (uint64_t)srcTensor[i * NCHW_CONV_ADDR_LIST_SIZE + n * alignOutter].GetPhyAddr();
                transDataTo5HDSrcLocalList[n + NCHW_CONV_ADDR_LIST_SIZE / 2] =
                    (uint64_t)srcTensor[i * NCHW_CONV_ADDR_LIST_SIZE + n * alignOutter + oneBlockElementNum]
                        .GetPhyAddr();
            }
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE / 2; n++) {
                transDataTo5HDDstLocalList[n * 2] =
                    (uint64_t)dstTensor[(i * NCHW_CONV_ADDR_LIST_SIZE + n) * cumSumInfo.inner].GetPhyAddr();
                transDataTo5HDDstLocalList[n * 2 + 1] =
                    (uint64_t)dstTensor[(i * NCHW_CONV_ADDR_LIST_SIZE + (n + NCHW_CONV_ADDR_LIST_SIZE / 2))
                                        * cumSumInfo.inner]
                        .GetPhyAddr();
            }
            TransDataTo5HD<float>(transDataTo5HDDstLocalList, transDataTo5HDSrcLocalList, paramsBack);
        }
    }
}

template <typename T = half>
__aicore__ inline void Transpose5HDAB(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
                                      const CumSumInfo& cumSumInfo)
{
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(T);
    uint16_t alignOutter =
        (cumSumInfo.outter + NCHW_CONV_ADDR_LIST_SIZE - 1) / NCHW_CONV_ADDR_LIST_SIZE * NCHW_CONV_ADDR_LIST_SIZE;
    uint64_t transDataTo5HDDstLocalList[NCHW_CONV_ADDR_LIST_SIZE];
    uint64_t transDataTo5HDSrcLocalList[NCHW_CONV_ADDR_LIST_SIZE];
    uint8_t repeatTimes = 1;
    uint16_t dstRepStride = 0;
    uint16_t srcRepStride = 0;
    if (cumSumInfo.outter == alignOutter && alignOutter > cumSumInfo.inner) {
        repeatTimes = alignOutter / NCHW_CONV_ADDR_LIST_SIZE;
        if (repeatTimes > 1) {
            dstRepStride = 1;
            srcRepStride = cumSumInfo.inner;
        }
        TransDataTo5HDParams params(false, false, repeatTimes, dstRepStride, srcRepStride);
        for (int32_t i = 0; i < cumSumInfo.inner / oneBlockElementNum; i++) {
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
                transDataTo5HDSrcLocalList[n] =
                    (uint64_t)srcTensor[i * oneBlockElementNum + n * cumSumInfo.inner].GetPhyAddr();
                transDataTo5HDDstLocalList[n] =
                    (uint64_t)dstTensor[i * oneBlockElementNum * alignOutter + alignOutter * n].GetPhyAddr();
            }
            TransDataTo5HD<T>(transDataTo5HDDstLocalList, transDataTo5HDSrcLocalList, params);
        }
    } else {
        TransDataTo5HDParams params = ExtractTransDataParam(repeatTimes, cumSumInfo.inner, alignOutter,
                                                            oneBlockElementNum, dstRepStride, srcRepStride);
        for (int32_t i = 0; i < alignOutter / NCHW_CONV_ADDR_LIST_SIZE; i++) {
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
                transDataTo5HDSrcLocalList[n] =
                    (uint64_t)srcTensor[((i * NCHW_CONV_ADDR_LIST_SIZE
                                          + n % (cumSumInfo.outter - i * NCHW_CONV_ADDR_LIST_SIZE))
                                         * cumSumInfo.inner)]
                        .GetPhyAddr();
                transDataTo5HDDstLocalList[n] =
                    (uint64_t)dstTensor[i * NCHW_CONV_ADDR_LIST_SIZE + alignOutter * n].GetPhyAddr();
            }
            TransDataTo5HD<T>(transDataTo5HDDstLocalList, transDataTo5HDSrcLocalList, params);
        }
    }
}

template <typename T = half>
__aicore__ inline void Transpose5HDBA(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
                                      const CumSumInfo& cumSumInfo)
{
    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(T);
    uint16_t alignOutter =
        (cumSumInfo.outter + NCHW_CONV_ADDR_LIST_SIZE - 1) / NCHW_CONV_ADDR_LIST_SIZE * NCHW_CONV_ADDR_LIST_SIZE;
    uint64_t transDataTo5HDDstLocalList[NCHW_CONV_ADDR_LIST_SIZE];
    uint64_t transDataTo5HDSrcLocalList[NCHW_CONV_ADDR_LIST_SIZE];
    uint8_t repeatTimes = 1;
    uint16_t dstRepStride = 0;
    uint16_t srcRepStride = 0;
    if (alignOutter > cumSumInfo.inner) {
        repeatTimes = alignOutter / oneBlockElementNum;
        if (repeatTimes > 1) {
            dstRepStride = cumSumInfo.inner;
            srcRepStride = 1;
        } else {
            dstRepStride = 0;
            srcRepStride = 0;
        }
        TransDataTo5HDParams paramsBack(false, false, repeatTimes, dstRepStride, srcRepStride);
        for (int32_t i = 0; i < cumSumInfo.inner / NCHW_CONV_ADDR_LIST_SIZE; i++) {
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
                transDataTo5HDSrcLocalList[n] =
                    (uint64_t)srcTensor[(i * NCHW_CONV_ADDR_LIST_SIZE + n) * alignOutter].GetPhyAddr();
                transDataTo5HDDstLocalList[n] =
                    (uint64_t)dstTensor[i * NCHW_CONV_ADDR_LIST_SIZE + n * cumSumInfo.inner].GetPhyAddr();
            }
            TransDataTo5HD<T>(transDataTo5HDDstLocalList, transDataTo5HDSrcLocalList, paramsBack);
        }
    } else {
        repeatTimes = cumSumInfo.inner / oneBlockElementNum;
        if (repeatTimes > 1) {
            srcRepStride = 1;
            dstRepStride = alignOutter;
        } else {
            dstRepStride = 0;
            srcRepStride = 0;
        }
        TransDataTo5HDParams paramsBack(false, false, repeatTimes, srcRepStride, dstRepStride);
        for (int32_t i = 0; i < alignOutter / NCHW_CONV_ADDR_LIST_SIZE; i++) {
            for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
                transDataTo5HDSrcLocalList[n] =
                    (uint64_t)srcTensor[i * NCHW_CONV_ADDR_LIST_SIZE + alignOutter * n].GetPhyAddr();
                transDataTo5HDDstLocalList[n] =
                    (uint64_t)dstTensor[(i * NCHW_CONV_ADDR_LIST_SIZE + n) * cumSumInfo.inner].GetPhyAddr();
            }
            TransDataTo5HD<T>(transDataTo5HDDstLocalList, transDataTo5HDSrcLocalList, paramsBack);
        }
    }
}
} // namespace Internal
} // namespace AscendC

#endif // IMPL_MATH_CUMSUM_CUMSUM_C310_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CUMSUM_REGBASE_C310_CUMSUM_C310_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CUMSUM_REGBASE_C310_CUMSUM_C310_UTILS_H__
#endif
