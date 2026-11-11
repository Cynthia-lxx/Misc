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
* \file data_copy_wrapper_mx.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/copy_tile_to_cube/data_copy_wrapper_mx.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_MX_H__
#endif

#ifndef IMPL_MATMUL_MODULES_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_MX_H
#define IMPL_MATMUL_MODULES_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_MX_H
 
#include "data_copy_wrapper_intf.h"
#include "data_copy_wrapper_utils.h"
 
namespace AscendC {
namespace Impl {
namespace Detail {
 
template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE>
class DataCopyWrapper<IMPL, MM_CFG, INPUT_TYPE,
                      enable_if_t<!MatmulFeatureTrait<MM_CFG>::IsNeedUB() && (INPUT_TYPE::TAG == InputTypeTag::scaleA || INPUT_TYPE::TAG == InputTypeTag::scaleB)>> {
    MATMUL_USE_MODULE_ON(CopyCubeInParams, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(LocalWorkspace);
 
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::TRANS_T;
 
public:
    __aicore__ inline DataCopyWrapper() = default;
    __aicore__ inline ~DataCopyWrapper() = default;
 
    __aicore__ inline void CopyScaleNZ2NZ(const LocalTensor<fp8_e8m0_t>& dst, const GlobalTensor<fp8_e8m0_t>& src, const int row,
        const int col, const int height, const int width, const int gRow, const bool kAlignToC0Size = false) {
        ASCENDC_ASSERT((gRow >= width), {
            KERNEL_LOG(KERNEL_ERROR,
                "NZ2NZ width larger than origin matrix width, gRow is %d, which should be no less than width %d.", gRow,
                width);
        });
        // gRow indicates globalWidth. The NZ format of scale is different from that of data.
        int64_t srcOffset = (int64_t)row * (int64_t)gRow + BLOCK_CUBE * col;
 
        // scale: 16*2 fractal
        int blockLen = BLOCK_CUBE * width * static_cast<int>(sizeof(fp8_e8m0_t)) / ONE_BLK_SIZE;
        int srcStride = BLOCK_CUBE * (gRow - width) * static_cast<int>(sizeof(fp8_e8m0_t)) / ONE_BLK_SIZE;
        if (srcStride >= UINT16_MAX) {
            for (int i = 0; i < static_cast<int>(Ceil(height, BLOCK_CUBE)); ++i) {
                DataCopy(dst[i * BLOCK_CUBE * width], src[srcOffset + i * gRow * BLOCK_CUBE],
                    { 1, static_cast<uint16_t>(blockLen), 0, 0 });
            }
        } else {
            uint16_t nburst = static_cast<uint16_t>(Ceil(height, BLOCK_CUBE));
            int dstStride = 0;
            DataCopy(dst, src[srcOffset], { nburst, static_cast<uint16_t>(blockLen), static_cast<uint16_t>(srcStride),
                static_cast<uint16_t>(dstStride) });
        }
    }
 
    __aicore__ inline void CopyScaleDN2NZ(
        const LocalTensor<TransT>& dst, const GlobalTensor<TransT>& src, const int row, const int col,
        const int height, const int width, const int gScaleCol)
    {
        ASCENDC_ASSERT((row >= 0),
                        { KERNEL_LOG(KERNEL_ERROR, "row is %d, which should be no less than 0.", row); });
        ASCENDC_ASSERT((col >= 0),
                        { KERNEL_LOG(KERNEL_ERROR, "col is %d, which should be no less than 0.", col); });
        ASCENDC_ASSERT((height > 0),
                       { KERNEL_LOG(KERNEL_ERROR, "height is %d, which should be larger than 0.", height); });
        ASCENDC_ASSERT((width > 0),
                       { KERNEL_LOG(KERNEL_ERROR, "width is %d, which should be larger than 0.", width); });
        auto srcOffset = ((int64_t)row * (int64_t)gScaleCol + (int64_t)col);
 
        Dn2NzParams dn2nzParams;
        dn2nzParams.dnNum = 1;
        dn2nzParams.dValue = height;
        dn2nzParams.nValue = Ceil(width, MM_NUM_TWO);
        dn2nzParams.srcDnMatrixStride = 0;
        dn2nzParams.srcDValue = gScaleCol / MM_NUM_TWO;
        dn2nzParams.dstNzC0Stride = Ceil(width, MM_NUM_TWO);
        dn2nzParams.dstNzNStride = 1;
        dn2nzParams.dstNzMatrixStride = 0;
 
        GlobalTensor<half> aScaleGlobalB16;
        aScaleGlobalB16.SetGlobalBuffer(((__gm__ half*)(src.GetPhyAddr())), height * width / 2);
        auto workLocal = dst.template ReinterpretCast<half>();
        DataCopy(workLocal, aScaleGlobalB16[srcOffset / MM_NUM_TWO], dn2nzParams);
    }
 
    __aicore__ inline void CopyScaleMXND2NZ(const LocalTensor<fp8_e8m0_t>& dst, const GlobalTensor<fp8_e8m0_t>& src,
        const int32_t row, const int32_t col, const int32_t height, const int32_t width, const int32_t gRow) {
        ASCENDC_ASSERT((gRow >= width), {
            KERNEL_LOG(KERNEL_ERROR,
                "MXND2NZ width larger than origin matrix width, origin matrix width is %d, which should be no less than width %d.", gRow,
                width);
        });
        int64_t srcOffset = static_cast<int64_t>(row * gRow  + col * MM_NUM_TWO);
        auto srcTensor = src[srcOffset].template ReinterpretCast<half>();
        auto dstTensor = dst.template ReinterpretCast<half>();

        Nd2NzParams nd2nzParams;
        nd2nzParams.ndNum = 1;
        nd2nzParams.nValue = Ceil(height, MM_NUM_TWO);
        nd2nzParams.dValue = width;
        nd2nzParams.srcDValue = gRow;
        nd2nzParams.dstNzC0Stride = Ceil(height, MM_NUM_TWO);
        nd2nzParams.dstNzNStride = 1;
        nd2nzParams.dstNzMatrixStride = 0;
        DataCopy(dstTensor, srcTensor, nd2nzParams);
    }

    __aicore__ inline void CopyScaleVector2A1(const LocalTensor<fp8_e8m0_t>& dst, const GlobalTensor<fp8_e8m0_t>& src,
        const int32_t col, const int32_t blockLen) {
        ASCENDC_ASSERT((col >= 0), { KERNEL_LOG(KERNEL_ERROR, "col is %d, which should be no less than 0.", col); });
        ASCENDC_ASSERT((INPUT_TYPE::format == CubeFormat::VECTOR),
                    { KERNEL_LOG(KERNEL_ERROR, "INPUT_TYPE::format should be CubeFormat::VECTOR."); });
        DataCopyParams copyParams{1, static_cast<uint16_t>(blockLen), 0, 0};
        uint8_t rightPadding = static_cast<uint8_t>(Ceil(blockLen, ONE_BLK_SIZE) * ONE_BLK_SIZE - blockLen);
        DataCopyPadParams copyPadParams{1, 0, rightPadding, 0};
        DataCopyPad(dst, src[col], copyParams, copyPadParams);
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<SrcT>();
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_MX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_MX_H__
#endif // IMPL_MATMUL_MODULES_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_MX_WRAPPER_ND_H