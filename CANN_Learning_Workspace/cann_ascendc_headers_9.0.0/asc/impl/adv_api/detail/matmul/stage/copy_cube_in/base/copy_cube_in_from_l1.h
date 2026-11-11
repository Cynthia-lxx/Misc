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
 * \file copy_cube_in_from_l1.h
 * \brief
 */


#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_from_l1.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_FROM_L1_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_FROM_L1_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_FROM_L1_H

#include "../copy_tile_to_cube/copy_tile_to_cube.h"
#include "copy_cube_in_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    CopyCubeIn is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    CopyCubeIn is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class INPUT_TYPE, const auto& MM_CFG>
class CopyCubeIn<IMPL, INPUT_TYPE, MM_CFG, enable_if_t<
GetCopyCubeInType<INPUT_TYPE, MM_CFG>() == CopyCubeInType::FROM_L1>>
{
    MATMUL_USE_MODULE_ON(MatmulTensorInfo, INPUT_TYPE::TAG);
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename Conditional<IsSameType<TransT, fp8_e8m0_t>::value, fp8_e8m0_t, typename INPUT_TYPE::T>::type;
public:
    __aicore__ inline CopyCubeIn() = default;
    __aicore__ inline ~CopyCubeIn() = default;

    __aicore__ inline void Init() {}

    __aicore__ inline void Reset() {}

    __aicore__ inline void SetInput(const LocalTensor<SrcT>& localMatrix, bool isTranspose)
    {
        MATMUL_MODULE(MatmulTensorInfo)->SetLocalTensor(localMatrix, isTranspose);
    }

    __aicore__ inline void SetInput(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose)
    {
        MATMUL_MODULE(MatmulTensorInfo)->SetGlobalTensor(globalMatrix, isTranspose);
    }

    template <typename ScheduleContext = int>
    __aicore__ inline LocalTensor<TransT> LoadData(
        int32_t curRow, int32_t curCol, int32_t tileHeight, int32_t tileWidth, const ScheduleContext& context = {})
    {
        LocalTensor<TransT> l1;
        l1.SetAddr(MATMUL_MODULE(MatmulTensorInfo)->GetLocalTensor().address_);
        return l1;
    }

    __aicore__ inline void AllocTensor(int32_t iterIndex = 0) {}

    __aicore__ inline void BatchLoad(const uint32_t matrixStride, const int32_t outerIdx,
        const int32_t splitIdx, const int32_t splitSize) {}

    __aicore__ inline void ClearLoadData(const LocalTensor<TransT>& tensor = LocalTensor<TransT>{},
        int32_t curRow = 0, int32_t curCol = 0) {}

    __aicore__ inline void Destroy() {}

    __aicore__ inline void BatchDestroy() {}
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _COPY_CUBE_IN_FROM_L1_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_FROM_L1_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_FROM_L1_H__
#endif