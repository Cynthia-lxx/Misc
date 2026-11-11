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
* \file data_copy_wrapper_dn.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/copy_tile_to_cube/data_copy_wrapper_dn.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_DN_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_DN_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_DN_H

#include "data_copy_wrapper_intf.h"
#include "data_copy_wrapper_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE>
class DataCopyWrapper<IMPL, MM_CFG, INPUT_TYPE,
    enable_if_t<!MatmulFeatureTrait<MM_CFG>::IsNeedUB() && INPUT_TYPE::format == CubeFormat::COLUMN_MAJOR &&
    !(INPUT_TYPE::TAG == InputTypeTag::scaleA || INPUT_TYPE::TAG == InputTypeTag::scaleB)>> {
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::T;

public:
    __aicore__ inline DataCopyWrapper() = default;
    __aicore__ inline ~DataCopyWrapper() = default;

    __aicore__ inline void CopyDN2NZ(const LocalTensor<TransT>& dst, const GlobalTensor<SrcT>& src,
        const int32_t row, const int32_t col, const int32_t height, const int32_t width, const int32_t gCol,
        const int32_t dnNum = 1, const int32_t srcDnMatrixStride = 0, const int32_t dstNzMatrixStride = 0,
        const bool kAlignToC0Size = false)
    {
        ASCENDC_ASSERT((row >= 0), { KERNEL_LOG(KERNEL_ERROR, "row is %d, which should be no less than 0.", row); });
        ASCENDC_ASSERT((col >= 0), { KERNEL_LOG(KERNEL_ERROR, "col is %d, which should be no less than 0.", col); });
        ASCENDC_ASSERT((height > 0),
            { KERNEL_LOG(KERNEL_ERROR, "height is %d, which should be no less than 0.", height); });
        ASCENDC_ASSERT((width > 0),
            { KERNEL_LOG(KERNEL_ERROR, "width is %d, which should be no less than 0.", width); });
        ASCENDC_ASSERT((gCol >= width), {
            KERNEL_LOG(KERNEL_ERROR,
                "DN2NZ width larger than origin matrix width, gCol is %d, which should be no less than width %d.", gCol,
                width);
        });

        ASCENDC_ASSERT(!IsStaticPaddingEnable(MM_CFG),
            { KERNEL_LOG(KERNEL_ERROR, "Unsupported Static Padding for DN2NZ."); });
        int64_t srcOffset = (static_cast<int64_t>(row) * static_cast<int64_t>(gCol)  + static_cast<int64_t>(col));
        Dn2NzParams dn2nzParams;
        dn2nzParams.dnNum = dnNum;
        dn2nzParams.nValue = width;
        dn2nzParams.dValue = height;
        dn2nzParams.srcDnMatrixStride = srcDnMatrixStride;
        dn2nzParams.srcDValue = gCol;
        // when k is row(height) axis, int8 type gm->l1 dn2nz should be aligned to 32(c0Size)
        // while float/half type should be aligned to 16
        if (kAlignToC0Size) {
            dn2nzParams.dstNzC0Stride = Ceil(width, c0Size_) * c0Size_;
        } else {
            dn2nzParams.dstNzC0Stride = Ceil(width, BLOCK_CUBE) * BLOCK_CUBE;
        }
        dn2nzParams.dstNzNStride = 1;
        dn2nzParams.dstNzMatrixStride = dstNzMatrixStride;

        DataCopy(dst, src[srcOffset], dn2nzParams);
    }

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<SrcT>();
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_DN_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_DN_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_DN_H__
#endif