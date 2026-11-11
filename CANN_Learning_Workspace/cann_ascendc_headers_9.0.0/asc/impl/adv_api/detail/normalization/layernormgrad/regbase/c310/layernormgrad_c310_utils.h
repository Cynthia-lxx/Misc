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
 * \file layernormgrad_c310_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/normalization/layernormgrad/regbase/c310/layernormgrad_c310_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_UTILS_H__
#endif
#ifndef IMPL_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_UTILS_H_
#define IMPL_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_UTILS_H_

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/normalization/layernormgrad_utils.h"

namespace AscendC {

namespace Internal {

namespace LayernormGrad {

constexpr Reg::CastTrait castTraitHalfToFloat = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait castTraitFloatToHalf = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

// load a float register from float or half UB
template <typename T>
__simd_callee__ inline void LoadDataWithT(
    __ubuf__ T* src, Reg::RegTensor<float>& dstReg, Reg::MaskReg& dstPreg, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> srcTmpReg;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcTmpReg, src + offset);
        Cast<float, T, castTraitHalfToFloat>(dstReg, srcTmpReg, dstPreg);
    } else { // this branch: only support float
        Reg::LoadAlign(dstReg, src + offset);
    }
}

// fill a float register from float or half UB
template <typename T>
__simd_callee__ inline void FillDataWithT(
    __ubuf__ T* src, Reg::RegTensor<float>& dstReg, Reg::MaskReg& dstPreg, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> srcTmpReg;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B16>(srcTmpReg, src + offset);
        Cast<float, T, castTraitHalfToFloat>(dstReg, srcTmpReg, dstPreg);
    } else { // this branch: only support float
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(dstReg, src + offset);
    }
}

// store data from float register to float or half UB
template <typename T>
__simd_callee__ inline void StoreDataWithT(
    __ubuf__ T* dst, Reg::RegTensor<float>& srcReg, Reg::MaskReg& srcPreg, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> dstTmpReg;
        // cast back to half
        Reg::Cast<T, float, castTraitFloatToHalf>(dstTmpReg, srcReg, srcPreg);
        Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::LOWEST>(
            (Reg::RegTensor<uint16_t>&)dstTmpReg, (Reg::RegTensor<uint32_t>&)dstTmpReg);
        Reg::MaskPack(srcPreg, srcPreg);
        Reg::StoreAlign(dst + offset, dstTmpReg, srcPreg);
    } else { // this branch: only support float
        Reg::StoreAlign(dst + offset, srcReg, srcPreg);
    }
}
} // namespace LayernormGrad
} // namespace Internal

struct LayerNormGradParams {
    __aicore__ LayerNormGradParams(
        uint32_t b, uint32_t s, uint32_t h, float lastDimValueBack, float lastDimValueBackMulTwo)
    {
        bLength = b;
        sLength = s;
        hLength = h;

        oneOverH = lastDimValueBack;
        twoOverH = lastDimValueBackMulTwo;
    }

    uint32_t bLength;
    uint32_t sLength;
    uint32_t hLength;

    float oneOverH;
    float twoOverH;
};

} // namespace AscendC
#endif // IMPL_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_UTILS_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORMGRAD_REGBASE_C310_LAYERNORMGRAD_C310_UTILS_H__
#endif
