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
* \file copy_tile_to_cube_mx.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/copy_tile_to_cube/copy_tile_to_cube_mx.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_TILE_TO_CUBE_MX_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_TILE_TO_CUBE_MX_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_TILE_TO_CUBE_MX_H
 
#include "copy_tile_to_cube_intf.h"
 
namespace AscendC {
namespace Impl {
namespace Detail {

/*
    CopyTileToCubeWrapper for Scale A/B is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    CopyTileToCubeWrapper is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE>
class CopyTileToCubeWrapper<IMPL, MM_CFG, INPUT_TYPE,
    enable_if_t<!MatmulFeatureTrait<MM_CFG>::IsNeedUB() &&
    (INPUT_TYPE::TAG == InputTypeTag::scaleA || INPUT_TYPE::TAG == InputTypeTag::scaleB)>> {

    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::TRANS_T;
    MATMUL_USE_MODULE_ON(CopyCubeInParams, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(MatmulTensorInfo, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(DataCopyWrapper, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulUserDefineInfo);

public:
    __aicore__ inline CopyTileToCubeWrapper() = default;
    __aicore__ inline ~CopyTileToCubeWrapper() = default;

    template<bool IS_INTRA_BLOCK = false>
    __aicore__ inline void CopyTileToCube(const LocalTensor<TransT>& dst, int32_t curRow, int32_t curCol,
        int32_t tileHeight, int32_t tileWidth)
    {
        constexpr int32_t widthFactor = IsSupportB4<TransT>() &&
            INPUT_TYPE::scaleFormat == CubeFormat::ND ? INT4_TWO : 1;
        if (IsTranspose<IS_INTRA_BLOCK>()) {
            GlobalTensor<SrcT> src;
            src.SetGlobalBuffer(MATMUL_MODULE(MatmulTensorInfo)-> template GetGlobalTensor<IS_INTRA_BLOCK>().address_);
            CopyTileToCubeFromGM<true, IS_INTRA_BLOCK>(dst, src, curCol, curRow,
                tileWidth, tileHeight / widthFactor, widthFactor);
        } else {
            GlobalTensor<SrcT> src;
            src.SetGlobalBuffer(MATMUL_MODULE(MatmulTensorInfo)-> template GetGlobalTensor<IS_INTRA_BLOCK>().address_);
            CopyTileToCubeFromGM<false, IS_INTRA_BLOCK>(dst, src, curRow, curCol,
                tileHeight,  tileWidth / widthFactor, widthFactor);
        }
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<SrcT>();

    template<bool IS_INTRA_BLOCK = false>
    __aicore__ inline bool IsTranspose()
    {
        if constexpr(INPUT_TYPE::TAG == InputTypeTag::scaleA) {
            return MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleA();
        } else {
            return MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleB();
        }
    }

    template<bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
    __aicore__ inline void CopyTileToCubeFromGM(const LocalTensor<TransT>& dst,
        const GlobalTensor<TransT>& src, int32_t curRow, int32_t curCol,
        int32_t tileHeight, int32_t tileWidth, int32_t widthFactor)
    {
        auto baseHeight = MATMUL_MODULE(CopyCubeInParams)->template GetBaseHeight<IS_TRANS>();
        auto baseWidth = MATMUL_MODULE(CopyCubeInParams)->template GetBaseWidth<IS_TRANS>();
        auto orgHeight = MATMUL_MODULE(CopyCubeInParams)->template GetOrgHeight<IS_TRANS, IS_INTRA_BLOCK>();
        auto orgWidth = MATMUL_MODULE(CopyCubeInParams)->template GetOrgWidth<IS_TRANS, IS_INTRA_BLOCK>() / widthFactor;
        auto iskRowDirec = MATMUL_MODULE(CopyCubeInParams)->template IsKRowDirec<IS_INTRA_BLOCK>();
        if constexpr(INPUT_TYPE::scaleFormat == CubeFormat::ND) {
            if constexpr(INPUT_TYPE::TAG == InputTypeTag::scaleA) {
                if (MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleA()) {
                    MATMUL_MODULE(DataCopyWrapper)->CopyScaleMXND2NZ(dst, src, curRow * baseHeight,
                        curCol * baseWidth, tileHeight, tileWidth, orgWidth);
                    return;
                }
            } else if constexpr(INPUT_TYPE::TAG == InputTypeTag::scaleB) {
                if (!MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleB()) {
                    MATMUL_MODULE(DataCopyWrapper)->CopyScaleMXND2NZ(dst, src, curRow * baseHeight,
                        curCol * baseWidth, tileHeight, tileWidth, orgWidth);
                    return;
                }
            }
            MATMUL_MODULE(DataCopyWrapper)->CopyScaleDN2NZ(dst, src, curRow * baseHeight,
                curCol * baseWidth, tileHeight, tileWidth, orgWidth);
        } else if constexpr (INPUT_TYPE::scaleFormat == CubeFormat::NZ) {
            MATMUL_MODULE(DataCopyWrapper)->CopyScaleNZ2NZ(dst, src, curRow * baseHeight,
                curCol * baseWidth, tileHeight, CeilAlign(tileWidth, MX_EVEN_FACTOR), orgWidth);
        } else if constexpr (INPUT_TYPE::scaleFormat == CubeFormat::VECTOR) {
            MATMUL_MODULE(DataCopyWrapper)->CopyScaleVector2A1(
                dst, src, curCol * baseWidth, CeilAlign(tileWidth, MX_EVEN_FACTOR));
        }
    }
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_TILE_TO_CUBE_MX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_TILE_TO_CUBE_MX_H__
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_COPY_TILE_TO_CUBE_MX_H