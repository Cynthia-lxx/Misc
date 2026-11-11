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
* \file copy_cube_in_utils.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_UTILS_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_CUBE_IN_UTILS_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_CUBE_IN_UTILS_H

#include "../../../feature_trait/matmul_feature_trait.h"

namespace AscendC {
namespace Impl {
namespace Detail {
enum class CopyCubeInType : uint8_t {
    NONE = 0,
    NORMAL = 1,
    MDL,
    BMM,
    FROM_L1,
    SPARSE_MDL,
    UBTOL1_SINGLESHAPE,
    MX_NORM,
    MX_MDL,
};

template <typename INPUT_TYPE, const auto& MM_CFG>
__aicore__ inline constexpr bool IsSameABTemplate()
{
    return DoMatmulIBShareNorm(MM_CFG) && INPUT_TYPE::ibShare;
}

template <typename INPUT_TYPE, const auto& MM_CFG>
__aicore__ inline constexpr bool IsCopyFromUB()
{
    return PhyPosIsUB(INPUT_TYPE::pos) && MatmulFeatureTrait<MM_CFG>().IsSupportUBToL1();
}

template <typename INPUT_TYPE, const auto& MM_CFG>
__aicore__ inline constexpr bool IsBMMFromL1()
{
    return PhyPosIsL1(INPUT_TYPE::pos) && (INPUT_TYPE::layout != LayoutMode::NORMAL ||
           ToMatmulConfig(MM_CFG).batchMode != BatchMode::SINGLE_LARGE_THAN_L1);
}

template <typename INPUT_TYPE, const auto& MM_CFG>
__aicore__ inline constexpr CopyCubeInType GetCopyCubeInType()
{
    if constexpr (InputPhyPosIsL1<INPUT_TYPE>()) {
        return CopyCubeInType::FROM_L1;
    } else if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape() && InputPhyPosIsUB<INPUT_TYPE>()) {
        return CopyCubeInType::UBTOL1_SINGLESHAPE;
    } else if constexpr (DoMatmulIBShareNorm(MM_CFG)) {
        return CopyCubeInType::NORMAL;
    } else if constexpr (DoMatmulNorm(MM_CFG)) {
        if constexpr (INPUT_TYPE::layout != LayoutMode::NONE &&
            ToMatmulConfig(MM_CFG).batchMode != BatchMode::SINGLE_LARGE_THAN_L1) {
            return CopyCubeInType::BMM;
        } else if constexpr (IsScaleTag<INPUT_TYPE>()) {
            return CopyCubeInType::MX_NORM;
        } else {
            return CopyCubeInType::NORMAL;
        }
    } else if constexpr (DoMatmulMDL(MM_CFG) || DoMatmulSpecialMDL(MM_CFG)) {
        if constexpr (HasSparseIndex<INPUT_TYPE>()) {
            return CopyCubeInType::SPARSE_MDL;
        } else if constexpr (IsScaleTag<INPUT_TYPE>()) {
            return CopyCubeInType::MX_MDL;
        } else {
            return CopyCubeInType::MDL;
        }
    } else if constexpr (DoMatmulBasicBlock(MM_CFG) || DoMatmulSpecialBasicBlock(MM_CFG)) {
        return CopyCubeInType::NORMAL;
    } else {
        return CopyCubeInType::NONE;
    }
}

template <typename INPUT_TYPE, const auto& MM_CFG>
__aicore__ inline constexpr bool IsFullLoad()
{
    if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
        return IsAFullLoad(MM_CFG);
    } else {
        return IsBFullLoad(MM_CFG);
    }
}

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_UTILS_H__
#endif // _COPY_CUBE_IN_UTILS_H_