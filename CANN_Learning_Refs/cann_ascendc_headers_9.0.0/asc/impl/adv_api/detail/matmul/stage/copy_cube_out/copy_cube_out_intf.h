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
 * \file copy_cube_out_intf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_out/copy_cube_out_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_INTF_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_INTF_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_INTF_H

#include "../../feature_trait/matmul_chip_cap.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
* CopyCubeOut is considered entirely experimental.
* We retain the freedom to make incompatible changes, but do not guarantee the stability.
* CopyCubeOut is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, const auto &MM_CFG, McgShfMode FIXPIPE_MODE = McgShfMode::SINGLE_DST_MODE, typename = void,
    template <class INPUT_TYPE, class OUTPUT_TYPE, typename T = void> class...>
class CopyCubeOut
{
    using DstT = typename C_TYPE::T;
    using SrcT = typename GetMmDstType<typename A_TYPE::T>::Type;
public:

    /**
     * @description: Copy data from L0C to LocalTensor
     * @param: co2Local: The Copy dst address
     * @param: co1Local: The L0C address while the matmul result store
     * @param: curRow: The current  handled block of the matrixA index
     * @param: curCol: The current  handled block of the matrixA index
     * @param: baseHeight: The current handled block of the matrixA tiles to be loaded this time
     * @param: baseHeight: The current handled block of the matrixB tiles to be loaded this time
     * @param: baseBlockHeight: The current block number of the matrixA tiles
     * @param: baseBlockWidth: The current handled block number of the matrixB tiles
     * @param: enSequentialWrite: The data's write type on dst address, continue or flat write
     * @param: ScheduleContext: generic type will be used later to store matmul context
     * @return: void
     */
    template <bool enSequentialWrite = false, typename ScheduleContext = int>
    __aicore__ inline void Copy(const GlobalTensor<DstT>& gm, const LocalTensor<SrcT>& co1Local, int32_t curRow,
                                   int32_t curCol, int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight,
                                   int32_t baseBlockWidth, const ScheduleContext& context = 0)
    {}

    /**
     * @description: Copy data from L0C to LocalTensor
     * @param: co2Local: The Copy dst address
     * @param: co1Local: The L0C address while the matmul result store
     * @param: curRow: The current  handled block of the matrixA index
     * @param: curCol: The current  handled block of the matrixA index
     * @param: baseHeight: The current handled block of the matrixA tiles to be loaded this time
     * @param: baseHeight: The current handled block of the matrixB tiles to be loaded this time
     * @param: baseBlockHeight: The current block number of the matrixA tiles
     * @param: baseBlockWidth: The current handled block number of the matrixB tiles
     * @param: enSequentialWrite: The data's write type on dst address, continue or flat write
     * @return: void
     */
    template <bool enSequentialWrite = false, typename ScheduleContext = int>
    __aicore__ inline void Copy(const LocalTensor<DstT>& co2Local, const LocalTensor<SrcT>& co1Local, int32_t curRow,
                                   int32_t curCol, int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight,
                                   int32_t baseBlockWidth, const ScheduleContext& context = 0)
    {}
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_INTF_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_INTF_H__
#endif