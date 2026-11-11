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
* \file data_copy_wrapper_vec.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/copy_tile_to_cube/data_copy_wrapper_vec.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_VEC_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_VEC_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_VEC_H

#include "data_copy_wrapper_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE>
class DataCopyWrapper<IMPL, MM_CFG, INPUT_TYPE, enable_if_t<INPUT_TYPE::format == CubeFormat::VECTOR && 
    !(INPUT_TYPE::TAG == InputTypeTag::scaleA || INPUT_TYPE::TAG == InputTypeTag::scaleB)>> {
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::T;

public:
    __aicore__ inline DataCopyWrapper() = default;
    __aicore__ inline ~DataCopyWrapper() = default;

    __aicore__ inline void CopyVector2A1(
        const LocalTensor<TransT>& dst, const GlobalTensor<SrcT>& src, const int32_t col, const int32_t blockLen)
    {
        ASCENDC_ASSERT((col >= 0), { KERNEL_LOG(KERNEL_ERROR, "col is %d, which should be no less than 0.", col); });
        ASCENDC_ASSERT((INPUT_TYPE::format == CubeFormat::VECTOR),
                    { KERNEL_LOG(KERNEL_ERROR, "INPUT_TYPE::format should be CubeFormat::VECTOR."); });

        DataCopyParams dataCopyInfo;
        dataCopyInfo.blockCount = 1;
        dataCopyInfo.blockLen = blockLen;
        dataCopyInfo.srcStride = 0;
        dataCopyInfo.dstStride = 0;
        DataCopyEnhancedParams enhancedParams;
        enhancedParams.blockMode = BlockMode::BLOCK_MODE_VECTOR;
        DataCopy(dst, src[col], dataCopyInfo, enhancedParams);
        return;
    }

    __aicore__ inline void CopyVector2A1(const LocalTensor<TransT>& dst, const LocalTensor<SrcT>& src,
        const int32_t col, const int32_t blockLen)
    {
        ASCENDC_ASSERT((col >= 0), { KERNEL_LOG(KERNEL_ERROR, "col is %d, which should be no less than 0.", col); });
        ASCENDC_ASSERT((INPUT_TYPE::format == CubeFormat::VECTOR),
                    { KERNEL_LOG(KERNEL_ERROR, "INPUT_TYPE::format should be CubeFormat::VECTOR."); });

        DataCopyParams dataCopyInfo;
        dataCopyInfo.blockCount = 1;
        dataCopyInfo.blockLen = blockLen;
        dataCopyInfo.srcStride = 0;
        dataCopyInfo.dstStride = 0;
        DataCopy(dst, src[col], dataCopyInfo);
        return;
    }
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_VEC_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_VEC_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_VEC_H__
#endif