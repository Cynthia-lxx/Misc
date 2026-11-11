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
* \file pad_c310_impl.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/pad/pad/pad_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/pad.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_PAD_PAD_C310_IMPL_H__
#endif

#ifndef IMPL_PAD_PAD_PAD_C310_IMPL_H
#define IMPL_PAD_PAD_PAD_C310_IMPL_H

#include "kernel_basic_intf.h"

namespace AscendC {
namespace PadInternal {

template <typename T>
__simd_callee__ inline void SetLeftPadMask(Reg::MaskReg& mask, uint16_t leftPad)
{
    uint32_t scalar = leftPad;
    mask = Reg::UpdateMask<T>(scalar);
}

template <typename T>
__simd_callee__ inline void SetRightPadMask(Reg::MaskReg& mask, uint32_t srcOriWidth, uint16_t rightPad)
{
    Reg::MaskReg maskAll = Reg::CreateMask<T, Reg::MaskPattern::ALL>();

    uint32_t srcOriWidthForMask = srcOriWidth;
    Reg::MaskReg maskSrcOri = Reg::UpdateMask<T>(srcOriWidthForMask);
    Reg::MaskReg maskNotSrcOri = Reg::CreateMask<T, Reg::MaskPattern::ALLF>();
    Reg::MaskNot(maskNotSrcOri, maskSrcOri, maskAll);

    uint32_t scalar = srcOriWidth + rightPad;
    Reg::MaskReg maskWithPad = Reg::UpdateMask<T>(scalar);
    Reg::MaskAnd(mask, maskWithPad, maskNotSrcOri, maskAll);
}

template <typename T>
__simd_vf__ inline void UnAlignedPad(__ubuf__ T* dstUb, __ubuf__ T* srcUb,
    PadParams padParams, PadTiling padTiling, uint32_t lastRegBlockPerRowElementCnt)
{
    Reg::RegTensor<T> regT;

    uint32_t regBlockElementCnt = CUBE_MAX_SIZE / sizeof(T);
    uint32_t regBlockCntPerRow = padTiling.srcWidth / regBlockElementCnt;

    Reg::MaskReg leftPadMask = Reg::CreateMask<T, Reg::MaskPattern::ALLF>();
    SetLeftPadMask<T>(leftPadMask, padParams.leftPad);
    Reg::MaskReg rightPadMask = Reg::CreateMask<T, Reg::MaskPattern::ALLF>();
    SetRightPadMask<T>(rightPadMask, padTiling.srcOriWidth - regBlockCntPerRow * regBlockElementCnt, padParams.rightPad);

    uint32_t lastRegBlockPerRowCopyOutCnt = lastRegBlockPerRowElementCnt + padParams.rightPad;

    __ubuf__ T* srcStartPerBlock = srcUb;
    __ubuf__ T* dstStartPerBlock = dstUb;
    for (uint32_t i = 0; i < padTiling.srcHeight; i++) {
        Reg::UnalignReg unalignRegCpyIn;
        Reg::UnalignReg unalignRegCpyOut;

        Reg::Duplicate<T, Reg::MaskMergeMode::MERGING>(regT, static_cast<T>(padParams.padValue), leftPadMask);
        Reg::StoreUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstStartPerBlock, regT, unalignRegCpyOut, padParams.leftPad);

        for (uint32_t j = 0; j < regBlockCntPerRow; ++j) {
            Reg::LoadUnAlignPre(unalignRegCpyIn, srcStartPerBlock);
            Reg::LoadUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
                regT, unalignRegCpyIn, srcStartPerBlock, regBlockElementCnt);

            Reg::StoreUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
                dstStartPerBlock, regT, unalignRegCpyOut, regBlockElementCnt);
        }

        Reg::LoadUnAlignPre(unalignRegCpyIn, srcStartPerBlock);
        Reg::LoadUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
            regT, unalignRegCpyIn, srcStartPerBlock, lastRegBlockPerRowElementCnt);

        Reg::Duplicate<T, Reg::MaskMergeMode::MERGING>(regT, static_cast<T>(padParams.padValue), rightPadMask);

        Reg::StoreUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstStartPerBlock, regT, unalignRegCpyOut, lastRegBlockPerRowCopyOutCnt);
        Reg::StoreUnAlignPost(dstStartPerBlock, unalignRegCpyOut, 0);
    }
}

template <typename T>
__simd_vf__ inline void AlignedPad(__ubuf__ T* dstUb, __ubuf__ T* srcUb,
    PadParams padParams, PadTiling tiling, uint32_t lastRegBlockPerRowElementCnt)
{
    Reg::RegTensor<T> regT;

    uint32_t regBlockElementCnt = CUBE_MAX_SIZE / sizeof(T);
    uint32_t regBlockCntPerRow = tiling.srcWidth / regBlockElementCnt;
    if (lastRegBlockPerRowElementCnt == 0) {
        regBlockCntPerRow = regBlockCntPerRow -1;
        lastRegBlockPerRowElementCnt = regBlockElementCnt;
    }

    uint32_t regBlockElementCntForMask = regBlockElementCnt;
    uint32_t lastRegBlockPerRowElementCntForMask = lastRegBlockPerRowElementCnt;
    Reg::MaskReg regBlockPerRowCopyOutMask = Reg::UpdateMask<T>(regBlockElementCntForMask);
    Reg::MaskReg lastRegBlockPerRowCopyOutMask = Reg::UpdateMask<T>(lastRegBlockPerRowElementCntForMask);

    Reg::MaskReg rightPadMask = Reg::CreateMask<T, Reg::MaskPattern::ALLF>();
    SetRightPadMask<T>(rightPadMask, tiling.srcOriWidth - regBlockCntPerRow * regBlockElementCnt, padParams.rightPad);

    __ubuf__ T* srcStartPerBlock = srcUb;
    __ubuf__ T* dstStartPerBlock = dstUb;

    for (uint32_t i = 0; i < tiling.srcHeight; i++) {
        for (uint32_t j = 0; j < regBlockCntPerRow; j++) {
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
                regT, srcStartPerBlock, regBlockElementCnt);
            Reg::StoreAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
                dstStartPerBlock, regT, regBlockElementCnt, regBlockPerRowCopyOutMask);
        }

        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
            regT, srcStartPerBlock, lastRegBlockPerRowElementCnt);
        Reg::Duplicate<T, Reg::MaskMergeMode::MERGING>(regT, static_cast<T>(padParams.padValue), rightPadMask);
        Reg::StoreAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstStartPerBlock, regT, lastRegBlockPerRowElementCnt, lastRegBlockPerRowCopyOutMask);
    }
}

template <typename T>
__simd_vf__ inline void UnPad(__ubuf__ T* dstUb, __ubuf__ T* srcUb, UnPadParams padParams, UnPadTiling tiling,
                              uint32_t lastRegBlockPerRowElementCnt)
{
    Reg::RegTensor<T> regT;
    Reg::UnalignReg unalignRegCpyOut;

    uint32_t regBlockElementCnt = CUBE_MAX_SIZE / sizeof(T);
    uint32_t regBlockCntPerRow = tiling.srcWidth / regBlockElementCnt;
    if (lastRegBlockPerRowElementCnt == 0) {
        regBlockCntPerRow = regBlockCntPerRow -1;
        lastRegBlockPerRowElementCnt = regBlockElementCnt;
    }

    __ubuf__ T* srcStartPerBlock = srcUb;
    __ubuf__ T* dstStartPerBlock = dstUb;
    for (uint32_t i = 0; i < tiling.srcHeight; i++) {
        for (uint32_t j = 0; j < regBlockCntPerRow; ++j) {
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
                regT, srcStartPerBlock, regBlockElementCnt);

            Reg::StoreUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
                dstStartPerBlock, regT, unalignRegCpyOut, regBlockElementCnt);
        }

        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
            regT, srcStartPerBlock, lastRegBlockPerRowElementCnt);
        Reg::StoreUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstStartPerBlock, regT, unalignRegCpyOut, lastRegBlockPerRowElementCnt - padParams.rightPad);
        Reg::StoreUnAlignPost(dstStartPerBlock, unalignRegCpyOut, 0);
    }
}

}

template <typename T>
__aicore__ inline void PadCompute(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    PadParams& padParams, const LocalTensor<uint8_t>& sharedTmpBuffer, PadTiling& tiling)
{
    static_assert(SupportType<T, int16_t, uint16_t, half, int32_t, uint32_t, float>(),
        "Pad only support int16_t/uint16_t/half/int32_t/uint32_t/float data type on current device!");
    // 32B aligned
    uint32_t regBlockElementCnt = CUBE_MAX_SIZE / sizeof(T);
    uint32_t lastRegBlockPerRowElementCnt = tiling.srcWidth % regBlockElementCnt;
    if (tiling.srcWidth * sizeof(T) % ONE_BLK_SIZE == 0) {
        PadInternal::AlignedPad<T>((__ubuf__ T *)dstTensor.GetPhyAddr(),
            (__ubuf__ T *)srcTensor.GetPhyAddr(), padParams, tiling, lastRegBlockPerRowElementCnt);
    } else {
        PadInternal::UnAlignedPad<T>((__ubuf__ T *)dstTensor.GetPhyAddr(),
            (__ubuf__ T *)srcTensor.GetPhyAddr(), padParams, tiling, lastRegBlockPerRowElementCnt);
    }
}

/* **************************************************************************************************
 * UnPad                                             *
 * ************************************************************************************************* */
/*
 * @ingroup UnPad
 * @brief unpad from src to dst, applicable to vector data
 * @param [out] dstTensor output LocalTensor
 * @param [in] srcTensor input LocalTensor
 * @param [in] sharedTmpBuffer tmp buffer LocalTensor
 * @param [in] unPadParams.leftPad number of left unpad
 * @param [in] unPadParams.rightPad number of right unpad
 */
template <typename T>
__aicore__ inline void UnPadCompute(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    UnPadParams& unPadParams, LocalTensor<uint8_t>& sharedTmpBuffer, UnPadTiling& tiling)
{
    static_assert(SupportType<T, int16_t, uint16_t, half, int32_t, uint32_t, float>(),
        "UnPad only support int16_t/uint16_t/half/int32_t/uint32_t/float data type on current device!");

    uint32_t regBlockElementCnt = CUBE_MAX_SIZE / sizeof(T);
    uint32_t lastRegBlockPerRowElementCnt = tiling.srcWidth % regBlockElementCnt;
    PadInternal::UnPad<T>((__ubuf__ T *)dstTensor.GetPhyAddr(),
        (__ubuf__ T *)srcTensor.GetPhyAddr(), unPadParams, tiling, lastRegBlockPerRowElementCnt);
}
} // namespace AscendC
#endif // IMPL_PAD_PAD_PAD_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_PAD_PAD_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_PAD_PAD_C310_IMPL_H__
#endif
