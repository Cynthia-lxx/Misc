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
* \file copy_cube_in_atop.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_atop.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_ATOP_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_ATOP_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_ATOP_H

#include "../copy_tile_to_cube/copy_tile_to_cube.h"
#include "copy_cube_in_intf.h"
#include "copy_cube_in_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    CopyCubeIn is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    CopyCubeIn is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class INPUT_TYPE, const auto &MM_CFG, typename Void,
    template <class IN_TYPE, const auto &COPY_CFG> class CopyIn>
class CopyCubeIn<IMPL, INPUT_TYPE, MM_CFG, Void, CopyIn>
    : public CopyCubeInBase<IMPL, MM_CFG, INPUT_TYPE>
{
    MATMUL_USE_MODULE_ON(CubeInBuffer, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(CopyCubeInParams, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(DataCopyUtils, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(MatmulTensorInfo, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::T;

public:
    using BASE_MODULE = AscendC::Impl::Detail::CopyCubeInBase<IMPL, MM_CFG, INPUT_TYPE>;
    __aicore__ inline CopyCubeIn() = default;
    __aicore__ inline ~CopyCubeIn() = default;

    template <typename ScheduleContext = int>
    __aicore__ inline LocalTensor<TransT> LoadData(int32_t curRow, int32_t curCol, int32_t tileHeight,
        int32_t tileWidth, const ScheduleContext &context = 0)
    {
        LocalTensor<TransT> l1;
        int32_t posL1 = BASE_MODULE::GetIterIndex(curRow, curCol);
        int32_t bufferPos = MATMUL_MODULE(CopyCubeInParams)->GetBufferPos();
        if (MATMUL_MODULE(CubeInBuffer)->Hit(posL1, bufferPos)) {
            l1 = MATMUL_MODULE(CubeInBuffer)->GetBuffer(posL1, bufferPos);
        } else {
            l1 = MATMUL_MODULE(CubeInBuffer)->AllocTensor(bufferPos);
            CopyTensor(l1, curRow, curCol, tileHeight, tileWidth);
            MATMUL_MODULE(CubeInBuffer)->EnQue(l1);
            MATMUL_MODULE(CubeInBuffer)->DeQue();
        }
        return l1;
    }

    template <typename ScheduleContext = int>
    __aicore__ inline LocalTensor<TransT> AsyncLoadData(int32_t curRow, int32_t curCol, int32_t tileHeight,
        int32_t tileWidth, const ScheduleContext &context = 0)
    {
        if constexpr (PhyPosIsL1(INPUT_TYPE::pos) || INPUT_TYPE::layout != LayoutMode::NONE) {
            ASCENDC_ASSERT((false), {
                KERNEL_LOG(KERNEL_ERROR, "Matching error. MDL AsyncLoadData doesn't support BMM && Src L1");
            });
        }

        LocalTensor<TransT> localTensor;
        int32_t posL1 = BASE_MODULE::GetIterIndex(curRow, curCol);
        int32_t bufferPos = MATMUL_MODULE(CopyCubeInParams)->GetBufferPos();
        if (MATMUL_MODULE(CubeInBuffer)->Hit(posL1, bufferPos)) {
            return MATMUL_MODULE(CubeInBuffer)->GetBuffer(posL1, bufferPos);
        } else {
            localTensor = MATMUL_MODULE(CubeInBuffer)->AllocTensor(bufferPos);
            CopyTensor(localTensor, curRow, curCol, tileHeight, tileWidth);
            MATMUL_MODULE(CubeInBuffer)->EnQue(localTensor);
            return localTensor;
        }
    }

    __aicore__ inline void ClearLoadData(const LocalTensor<TransT> &tensor = LocalTensor<TransT>{}, int32_t curRow = 0,
        int32_t curCol = 0)
    {
        auto bufferPos = MATMUL_MODULE(CopyCubeInParams)->GetBufferPos();
        MATMUL_MODULE(CubeInBuffer)->FreeTensor(bufferPos);
    }

    __aicore__ inline void AwaitLoadData()
    {
        MATMUL_MODULE(CubeInBuffer)->DeQue();
    }

private:
    __aicore__ inline void CopyTensor(const LocalTensor<TransT> &l1, int32_t curRow, int32_t curCol, int32_t tileHeight,
        int32_t tileWidth)
    {
        CopyIn<INPUT_TYPE, MM_CFG> copyIn;
        if constexpr (!INPUT_TYPE::isTrans) {
            copyIn(l1, MATMUL_MODULE(MatmulTensorInfo)->GetGlobalTensor(),
                curRow, curCol, tileHeight, tileWidth,
                MATMUL_MODULE(CopyCubeInParams)->template GetBaseHeight<false>(),
                MATMUL_MODULE(CopyCubeInParams)->template GetBaseWidth<false>(),
                MATMUL_MODULE(CopyCubeInParams)->template GetOrgHeight<false>(),
                MATMUL_MODULE(CopyCubeInParams)->template GetOrgWidth<false>(),
                MATMUL_MODULE(CopyCubeInParams)->template IsKRowDirec());
        } else {
            copyIn(l1, MATMUL_MODULE(MatmulTensorInfo)->GetGlobalTensor(),
                curCol, curRow, tileWidth, tileHeight,
                MATMUL_MODULE(CopyCubeInParams)->template GetBaseHeight<true>(),
                MATMUL_MODULE(CopyCubeInParams)->template GetBaseWidth<true>(),
                MATMUL_MODULE(CopyCubeInParams)->template GetOrgHeight<true>(),
                MATMUL_MODULE(CopyCubeInParams)->template GetOrgWidth<true>(),
                MATMUL_MODULE(CopyCubeInParams)->template IsKRowDirec());
        }
    }
};
} // namespace Detail
} // namespace Impl
} // namespace Gemm
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_ATOP_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_ATOP_H__
#endif