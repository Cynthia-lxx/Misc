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
* \file copy_cube_in_base.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_BASE_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_BASE_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_BASE_H

#include "../../../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, const auto &MM_CFG, class INPUT_TYPE>
class CopyCubeInBase
{
    MATMUL_USE_MODULE_ON(CubeInBuffer, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(CopyCubeInParams, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE_ON(MatmulTensorInfo, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);

    using TransT = typename INPUT_TYPE::TRANS_T;
    using SrcT = typename Conditional<IsSameType<TransT, fp8_e8m0_t>::value, fp8_e8m0_t, typename INPUT_TYPE::T>::type;
public:

    __aicore__ inline void Init()
    {
        MATMUL_MODULE(CubeInBuffer)->Init(
            MATMUL_MODULE(CopyCubeInParams)->GetBufferSize(), MATMUL_MODULE(CopyCubeInParams)->GetDepth());
    }

    __aicore__ inline void SetInput(const LocalTensor<SrcT>& localMatrix, bool isTranspose)
    {
        MATMUL_MODULE(MatmulTensorInfo)->SetLocalTensor(localMatrix, isTranspose);
        MATMUL_MODULE(CubeInBuffer)->Reset();
    }

    __aicore__ inline void SetInput(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose = false)
    {
        MATMUL_MODULE(MatmulTensorInfo)->SetGlobalTensor(globalMatrix, isTranspose);
        MATMUL_MODULE(CubeInBuffer)->Reset();
    }

    __aicore__ inline void ClearLoadData(const LocalTensor<TransT>& tensor = LocalTensor<TransT>{},
        int32_t curRow = 0, int32_t curCol = 0)
    {}

    __aicore__ inline void Destroy()
    {
        MATMUL_MODULE(CubeInBuffer)->Destroy();
    }

    __aicore__ inline void Reset()
    {
        MATMUL_MODULE(CubeInBuffer)->Reset();
    }

    __aicore__ inline int32_t GetIterIndex(int32_t curRow, int32_t curCol)
    {
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
            return curCol % MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa();
        } else {
            return curRow % MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb();
        }
    }
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_BASE_H__
#endif // _COPY_CUBE_IN_BASE_H_