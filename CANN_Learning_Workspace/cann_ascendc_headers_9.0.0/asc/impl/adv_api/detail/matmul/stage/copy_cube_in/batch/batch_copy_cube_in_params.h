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
 * \file batch_copy_cube_in_params.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/batch/batch_copy_cube_in_params.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BATCH_BATCH_COPY_CUBE_IN_PARAMS_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BATCH_BATCH_COPY_CUBE_IN_PARAMS_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BATCH_BATCH_COPY_CUBE_IN_PARAMS_H

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, const auto &MM_CFG, class INPUT_TYPE>
class BatchCopyCubeInParams {
    using SrcT = typename INPUT_TYPE::T;
    using TransT = typename INPUT_TYPE::TRANS_T;
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE_ON(CopyCubeInParams, INPUT_TYPE::TAG);
    MATMUL_USE_MODULE(BatchLoop);
public:
    __aicore__ inline uint32_t GetBatchNum()
    {
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
            return MATMUL_MODULE(BatchLoop)->GetBatchA();
        } else {
            return MATMUL_MODULE(BatchLoop)->GetBatchB();
        }
    }

    __aicore__ inline uint32_t GetBatchMainBlock()
    {
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
            return MATMUL_MODULE(BatchLoop)->GetMainBatchBlockA();
        } else {
            return MATMUL_MODULE(BatchLoop)->GetMainBatchBlockB();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetBatchOrgWidth()
    {
        // Get Head length of BSH or SBH layout
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
            return GetBatchOrgWidthA<IS_TRANS>();
        } else{
            return GetBatchOrgWidthB<IS_TRANS>();
        }
    }

    __aicore__ inline bool IsTranspose()
    {
        if (INPUT_TYPE::TAG == InputTypeTag::A) {
            return MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA();
        } else {
            return MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB();
        }
    }

    template <bool IS_TRANS = false, bool NEED_BASIC = true>
    __aicore__ inline int32_t GetSingleHeight() const
    {
        if constexpr (NEED_BASIC && IsBasic(MM_CFG)) {
            // false: not support intraBlock, true: is basic constantized scenario
            return MATMUL_MODULE(CopyCubeInParams)->template GetSingleHeight<IS_TRANS, false, true>();
        } else {
            return MATMUL_MODULE(CopyCubeInParams)->template GetSingleHeight<IS_TRANS, false, false>();
        }
    }

    template <bool IS_TRANS = false, bool NEED_BASIC = true>
    __aicore__ inline int32_t GetSingleWidth() const
    {
        if constexpr (NEED_BASIC && IsBasic(MM_CFG)) {
            // false: not support intraBlock, true: is basic constantized scenario
            return MATMUL_MODULE(CopyCubeInParams)->template GetSingleWidth<IS_TRANS, false, true>();
        } else {
            return MATMUL_MODULE(CopyCubeInParams)->template GetSingleWidth<IS_TRANS, false, false>();
        }
    }

    template <bool IS_TRANS = false, bool IS_KROW = false, bool NEED_BASIC = true>
    __aicore__ inline int64_t GetSingleSizeAlign() const
    {
        if constexpr (IS_KROW && IsSupportB8<TransT>()) {
            return CeilAlign(GetSingleHeight<IS_TRANS, NEED_BASIC>(), c0Size_) *
                   CeilAlign(GetSingleWidth<IS_TRANS, NEED_BASIC>(), c0Size_);
        } else {
            return CeilAlign(GetSingleHeight<IS_TRANS, NEED_BASIC>(), BLOCK_CUBE) *
                   CeilAlign(GetSingleWidth<IS_TRANS, NEED_BASIC>(), c0Size_);
        }
    }

private:
    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetBatchOrgWidthA()
    {
        // Get Head length of BSH or SBH layout
        if constexpr (INPUT_TYPE::layout == LayoutMode::BSNGD) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetALayoutInfoD() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetALayoutInfoN() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetALayoutInfoG();
        } else if constexpr (INPUT_TYPE::layout == LayoutMode::SBNGD) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetALayoutInfoD() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetALayoutInfoN() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetALayoutInfoG() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetALayoutInfoB();
        } else {
            // Some operators does not set LayoutInfoS/D parameters for NORMAL/BNGS1S2 layout
            return MATMUL_MODULE(CopyCubeInParams)->template GetSingleWidth<IS_TRANS>();
        }
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int32_t GetBatchOrgWidthB()
    {
        // Get Head length of BSH or SBH layout
        if constexpr (INPUT_TYPE::layout == LayoutMode::BSNGD) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBLayoutInfoD() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBLayoutInfoN() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBLayoutInfoG();
        } else if constexpr (INPUT_TYPE::layout == LayoutMode::SBNGD) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBLayoutInfoD() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBLayoutInfoN() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBLayoutInfoG() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBLayoutInfoB();
        } else {
            // Some operators does not set LayoutInfoS/D parameters for NORMAL/BNGS1S2 layout
            return MATMUL_MODULE(CopyCubeInParams)->template GetSingleWidth<IS_TRANS>();
        }
    }
    constexpr static int32_t c0Size_ = AuxGetC0Size<TransT>();
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_BATCH_BATCH_COPY_CUBE_IN_PARAMS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BATCH_BATCH_COPY_CUBE_IN_PARAMS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BATCH_BATCH_COPY_CUBE_IN_PARAMS_H__
#endif