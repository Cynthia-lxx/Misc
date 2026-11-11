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
* \file copy_cube_in_intf.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_INTF_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_INTF_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_INTF_H

namespace AscendC {
namespace Impl {
namespace Detail {

/*
    CopyCubeIn is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    CopyCubeIn is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class INPUT_TYPE, const auto &MM_CFG, typename = void,
    template <class IN_TYPE, const auto &COPY_CFG> class...>
class CopyCubeIn
{
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::T;
public:
    __aicore__ inline CopyCubeIn() = default;
    __aicore__ inline ~CopyCubeIn() = default;
    /**
     * @description: Init of CopyCubeIn
     * @return: void
     */
    __aicore__ inline void Init() {}

    /**
     * @description: Set input local Tensor
     * @param: leftMatrix: Local Tensor input through SetTensorA or SetTensorB
     * @param: isTranspose: true if input tensor is transposed
     * @return: void
     */
    __aicore__ inline void SetInput(const LocalTensor<SrcT>& localMatrix, bool isTranspose) {}

    /**
     * @description: Set input global address
     * @param: gm: Global Tensor input through SetTensorA or SetTensorB
     * @param: srcGlobalAddr: true if input tensor is transposed
     * @return: void
     */
    __aicore__ inline void SetInput(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose) {}

    /**
     * @description: Load input data to L1
     * @param: ScheduleContext: generic type will be used later to store matmul context
     * @param: curRow: The row index of the matrixA/B to be loaded at current iterate
     * @param: curCol: The column index of the matrixA/B to be loaded at current iterate
     * @param: tileHeight: The height of the matrixA/B tiles to be loaded at current iterate
     * @param: tileWidth: The width of the matrixA/B tiles to be loaded at current iterate
     * @return: Tensor on L1
     */
    template <typename ScheduleContext = int>
    __aicore__ inline LocalTensor<TransT> LoadData(int curRow, int curCol, int tileHeight, int tileWidth,
                                                   const ScheduleContext& context = {})
    {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Matching error. This is an empty implementation."); });
        return LocalTensor<TransT>{};
    }

    /**
     * @description: Release tensor on l1 at one compute end
     * @param: tensor: The tensor on l1 need to be released
     * @param: curRow: The row index of the matrixA/B at current iterate
     * @param: curCol: The column index of the matrixA/B at current iterate
     * @return: void
     */
    __aicore__ inline void ClearLoadData(const LocalTensor<TransT>& tensor = LocalTensor<TransT>{},
        int32_t curRow = 0, int32_t curCol = 0) {}

    /*
     * @description: Reset buffer status used in copy in
     * @return: void
    */
   __aicore__ inline void Reset() {}

    /**
     * @description: Destroy tensor on l1 at iterate end
     * @return: void
     */
    __aicore__ inline void Destroy() {}
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _COPY_CUBE_IN_INTF_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_INTF_H__
#endif