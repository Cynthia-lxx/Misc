/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/* !
 * \file copy_cube_out_atop.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_out/copy_cube_out_atop.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_ATOP_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_ATOP_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_ATOP_H

#include "../../utils/matmul_module.h"
#include "quant/quant_processor_utils.h"
#include "copy_cube_out_intf.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
* CopyCubeOut is considered entirely experimental.
* We retain the freedom to make incompatible changes, but do not guarantee the stability.
* CopyCubeOut is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, const auto &MM_CFG, McgShfMode FIXPIPE_MODE,
    template <class INPUT_TYPE, class OUTPUT_TYPE, typename T = void> class CopyOut>
class CopyCubeOut<IMPL, A_TYPE, B_TYPE, C_TYPE, MM_CFG, FIXPIPE_MODE,
    enable_if_t<!AscendC::Impl::Detail::IsQuantSenario<typename C_TYPE::T, typename A_TYPE::T>()>,
    CopyOut> {
    using DstT = typename C_TYPE::T;
    using SrcT = typename GetMmDstType<typename A_TYPE::T>::Type;

    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulSubBlockInfo);

public:
    __aicore__ inline CopyCubeOut() = default;
    __aicore__ inline ~CopyCubeOut() = default;

    template <bool enSequentialWrite = false, typename ScheduleContext = int>
    __aicore__ inline void Copy(const GlobalTensor<DstT> &gm, const LocalTensor<SrcT> &co1Local, int curRow, int curCol,
        int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth,
        const ScheduleContext &context = 0)
    {
        CopyOut<A_TYPE, C_TYPE> copyOut;
        copyOut(gm, co1Local, curRow, curCol, baseHeight, baseWidth,
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(),
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN(),
            MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<false>(),
            MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<false>(),
            MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKc<false>());
    }

    template <bool enSequentialWrite = false, typename ScheduleContext = int>
    __aicore__ inline void Copy(const LocalTensor<DstT> &co2Local, const LocalTensor<SrcT> &co1Local, int curRow, int curCol,
        int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth,
        const ScheduleContext &context = 0)
    {
        CopyOut<A_TYPE, C_TYPE> copyOut;
        copyOut(co2Local, co1Local, curRow, curCol, baseHeight, baseWidth,
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(),
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN(),
            MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<false>(),
            MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<false>(),
            MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKc<false>(),
            MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx());
    }
};

template <typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, const auto &MM_CFG, McgShfMode FIXPIPE_MODE,
    template <class INPUT_TYPE, class OUTPUT_TYPE, typename T = void> class CopyOut>
class CopyCubeOut<IMPL, A_TYPE, B_TYPE, C_TYPE, MM_CFG, FIXPIPE_MODE,
    enable_if_t<AscendC::Impl::Detail::IsQuantSenario<typename C_TYPE::T, typename A_TYPE::T>()>,
    CopyOut> {
    using DstT = typename C_TYPE::T;
    using SrcT = typename GetMmDstType<typename A_TYPE::T>::Type;

    MATMUL_USE_MODULE(MatmulQuantProcessor);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulSubBlockInfo);

public:
    __aicore__ inline CopyCubeOut() = default;
    __aicore__ inline ~CopyCubeOut() = default;

    template <bool enSequentialWrite = false, typename ScheduleContext = int>
    __aicore__ inline void Copy(const GlobalTensor<DstT> &gm, const LocalTensor<SrcT> &co1Local, int curRow, int curCol,
        int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth,
        const ScheduleContext &context = 0)
    {
        CopyOut<A_TYPE, C_TYPE> copyOut;
        if (MATMUL_MODULE(MatmulQuantProcessor)->IsPerChannelSenario()) {
            LocalTensor<uint64_t> quantTensor;
            MATMUL_MODULE(MatmulQuantProcessor)->CopyQuantTensor(quantTensor, curCol, baseWidth);
            copyOut(gm, co1Local, curRow, curCol, baseHeight, baseWidth,
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(),
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN(),
                MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<false>(),
                MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<false>(),
                MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKc<false>(), quantTensor);
            MATMUL_MODULE(MatmulQuantProcessor)->FreeQuantTensor(quantTensor);
        } else {
            copyOut(gm, co1Local, curRow, curCol, baseHeight, baseWidth,
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(),
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN(),
                MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<false>(),
                MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<false>(),
                MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKc<false>(),
                MATMUL_MODULE(MatmulQuantProcessor)->GetQuantScalarValue());
        }
    }

    template <bool enSequentialWrite = false, typename ScheduleContext = int>
    __aicore__ inline void Copy(const LocalTensor<DstT>& co2Local, const LocalTensor<SrcT>& co1Local, int curRow,
                                int curCol, int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight,
                                int32_t baseBlockWidth, const ScheduleContext& context = 0)
    {
        CopyOut<A_TYPE, C_TYPE> copyOut;
        copyOut(co2Local, co1Local, curRow, curCol, baseHeight, baseWidth,
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(),
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN(),
                MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<false>(),
                MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<false>(),
                MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKc<false>(),
                MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx());
    }
};
} // namespace Detail
} // namespace Impl
} // namespace Gemm
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_ATOP_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_ATOP_H__
#endif