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
* \file batch_copy_cube_in_intf.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/batch/batch_copy_cube_in_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BATCH_BATCH_COPY_CUBE_IN_INTF_H__
#endif

#ifndef IMPL_MATMUL_STAGE_BATCH_COPY_CUBE_IN_COPY_CUBE_IN_INTF_H
#define IMPL_MATMUL_STAGE_BATCH_COPY_CUBE_IN_COPY_CUBE_IN_INTF_H

namespace AscendC {
namespace Impl {
namespace Detail {


template <typename IMPL, const auto &MM_CFG, class INPUT_TYPE, typename = void>
class BatchCopyCubeIn
{
    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename INPUT_TYPE::T;
public:
    __aicore__ inline BatchCopyCubeIn() = default;
    __aicore__ inline ~BatchCopyCubeIn() = default;
    /**
     * @description: Init of BatchCopyCubeIn
     * @return: void
     */
    __aicore__ inline void Init() {}

    /**
     * @description: Set input global address
     * @param: address: Global address input through SetTensorA or SetTensorB
     * @param: srcGlobalAddr: true if input tensor is transposed
     * @return: void
     */
    __aicore__ inline void SetInput(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose) {}

    __aicore__ inline LocalTensor<TransT> AllocTensor(int32_t iterIndex = 0) {}

    __aicore__ inline void BatchLoad(LocalTensor<TransT>& dstTensor, const uint32_t matrixStride,
                                     const int32_t outerIdx, const int32_t splitIdx, const int32_t splitSize) {}

    /**
     * @description: Load input data to L1
     * @param: curRow: The row index of the matrixA/B to be loaded at current iterate
     * @param: curCol: The column index of the matrixA/B to be loaded at current iterate
     * @param: tileHeight: The height of the matrixA/B tiles to be loaded at current iterate
     * @param: tileWidth: The width of the matrixA/B tiles to be loaded at current iterate
     * @return: Tensor on L1
     */
    template <typename ScheduleContext = int>
    __aicore__ inline LocalTensor<TransT> LoadData(int curRow, int curCol, int tileHeight, int tileWidth,
                                                   const ScheduleContext& context = 0)
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

    __aicore__ inline void BatchDestroy() {}
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _BATCH_COPY_CUBE_IN_INTF_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BATCH_BATCH_COPY_CUBE_IN_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BATCH_BATCH_COPY_CUBE_IN_INTF_H__
#endif