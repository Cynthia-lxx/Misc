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
 * \file copy_cube_in_params_mx.h
 * \brief copy cube in variable manager module
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_params_mx.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_MX_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_MX_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_MX_H
 
#include "../../../param/matmul_shape_tiling.h"
 
 namespace AscendC {
 namespace Impl {
 namespace Detail {
 template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE>
 class CopyCubeInParams<IMPL, MM_CFG, INPUT_TYPE, enable_if_t<INPUT_TYPE::TAG == InputTypeTag::scaleA>>
 {
     MATMUL_USE_MODULE(MatmulShapeInfo);
     MATMUL_USE_MODULE(MatmulShapeTiling);
     MATMUL_USE_MODULE(MLoop);
     MATMUL_USE_MODULE(KLoop);
     using TransT = typename INPUT_TYPE::TRANS_T;
 
 public:
 
     __aicore__ inline int32_t GetBufferPos()
     {
         if constexpr (DoMatmulMDL(MM_CFG) || DoMatmulSpecialMDL(MM_CFG)) {
             return MATMUL_MODULE(KLoop)->IsScaleAKL1FullLoad() ? MATMUL_MODULE(MLoop)->GetOuterScaleMIdx() :
                 MATMUL_MODULE(KLoop)->GetOuterScaleKaIdx();
         } else {
             return 0;
         }
     }
 
     // only support scaleA isTrans=false, [baseM, baseK/32]
     __aicore__ inline bool IsTranspose()
     {
         return MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleA();
     }
 
     __aicore__ inline int32_t GetBufferSize()
     {
         if constexpr (!PhyPosIsUB(INPUT_TYPE::scalePosition)) {
             return GetBaseSize();
         } else {
             return GetOrgSize();
         }
     }
 
     __aicore__ inline int GetDepth() const
     {
         return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetDepthA1();
     }
 
     __aicore__ inline int GetScaleFactor() const
     {
         return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetScaleFactorKa() *
         MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetScaleFactorM();
     }
   
     template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
     __aicore__ inline int32_t GetOrgHeight()
     {
         if constexpr (PhyPosIsUB(INPUT_TYPE::scalePosition)) {
             if constexpr (IS_INTRA_BLOCK) {
                 return CeilAlign<int32_t>(
                    GetSingleHeight<IS_TRANS, true>(), BLOCK_CUBE);
             } else {
                 return GetSingleHeight<IS_TRANS, false>();
             }
         } else {
             if constexpr (IS_TRANS) {
                 return Ceil(MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKa<IS_INTRA_BLOCK>(), MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
             } else {
                 return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<IS_INTRA_BLOCK>();
             }
         }
     }
 
     template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
     __aicore__ inline int32_t GetOrgWidth()
     {
         if constexpr (PhyPosIsUB(INPUT_TYPE::scalePosition)) {
             if constexpr (!IS_INTRA_BLOCK) {
                 return GetSingleWidth<IS_TRANS, false>();
             } else {
                 return CeilAlign<int32_t>(
                    GetSingleWidth<IS_TRANS, true>(), c0Size_);
             }
         } else {
             if constexpr (IS_TRANS) {
                 return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<IS_INTRA_BLOCK>();
             } else {
                 return Ceil(MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKa<IS_INTRA_BLOCK>(), MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
             }
         }
     }
 
     template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
     __aicore__ inline int32_t GetSingleHeight() const
     {
         // Constantized scenario
         // You can set IS_BASIC to false, if you don't need to use constantized parameters
         if constexpr (IS_TRANS) {
             return Ceil(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK, IS_BASIC>(),
             MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
         } else {
             return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreM<IS_INTRA_BLOCK, IS_BASIC>();
         }
     }
 
     template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
     __aicore__ inline int32_t GetSingleWidth() const
     {
         // Constantized scenario
         // You can set IS_BASIC to false, if you don't need to use constantized parameters
         if constexpr (IS_TRANS) {
             return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreM<IS_INTRA_BLOCK, IS_BASIC>();
         } else {
             return Ceil(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK, IS_BASIC>(),
             MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
         }
     }
 
     template <bool IS_TRANS = false>
     __aicore__ inline int32_t GetBaseHeight() const
     {
         if constexpr (IS_TRANS) {
             return Ceil(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), MX_K_FACTOR);
         } else {
             return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
         }
     }
 
     template <bool IS_TRANS = false>
     __aicore__ inline int32_t GetBaseWidth() const
     {
         if constexpr (IS_TRANS) {
             return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
         } else {
             return Ceil(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), MX_K_FACTOR);
         }
     }
 
     template <bool IS_INTRA_BLOCK = false>
     __aicore__ inline bool IsKRowDirec() const
     {
         return MATMUL_MODULE(MatmulShapeInfo)->template IsTransposeScaleA<IS_INTRA_BLOCK>();
     }
 
 private:
     constexpr static int32_t c0Size_ = AuxGetC0Size<TransT>();
 
     __aicore__ inline int32_t GetOrgHeightAlign()
     {
         return Align<int32_t>(GetOrgHeight(), BLOCK_CUBE);
     }
 
     __aicore__ inline int32_t GetOrgWidthAlign()
     {
         return Align<int32_t>(GetOrgWidth(), c0Size_);
     }
 
     __aicore__ inline int32_t GetBaseHeightAlign() const
     {
         if constexpr (IsNeedC0Align<TransT>()) {
             return Align(GetBaseHeight(), c0Size_);
         } else {
             return GetBaseHeight();
         }
     }
 
     __aicore__ inline int32_t GetBaseWidthAlign() const
     {
         return GetBaseWidth();
     }
 
     __aicore__ inline int32_t GetBaseSize()
     {
         return GetBaseHeightAlign() * GetBaseWidthAlign();
     }
 
     __aicore__ inline int32_t GetOrgSize()
     {
         return GetOrgHeightAlign() * GetOrgWidthAlign();
     }
 };
 
 template <typename IMPL, const auto& MM_CFG, class INPUT_TYPE>
 class CopyCubeInParams<IMPL, MM_CFG, INPUT_TYPE, enable_if_t<INPUT_TYPE::TAG == InputTypeTag::scaleB>>
 {
     MATMUL_USE_MODULE(MatmulShapeTiling);
     MATMUL_USE_MODULE(MatmulShapeInfo);
     MATMUL_USE_MODULE(NLoop);
     MATMUL_USE_MODULE(KLoop);
     using TransT = typename INPUT_TYPE::TRANS_T;
 
 public:
     // only support scaleB isTrans=true, [baseN, baseK/32]
     __aicore__ inline bool IsTranspose()
     {
         return MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleB();
     }
 
     __aicore__ inline int32_t GetBufferPos()
     {
         if constexpr (DoMatmulMDL(MM_CFG) || DoMatmulSpecialMDL(MM_CFG)) {
             return MATMUL_MODULE(KLoop)->IsScaleBKL1FullLoad() ? MATMUL_MODULE(NLoop)->GetOuterScaleNIdx() :
                 MATMUL_MODULE(KLoop)->GetOuterScaleKbIdx();
         } else {
             return 0;
         }
     }
 
     __aicore__ inline int32_t GetBufferSize()
     {
         if constexpr (PhyPosIsUB(INPUT_TYPE::scalePosition)) {
             return GetOrgSize();
         } else {
             return GetBaseSize();
         }
     }
 
     __aicore__ inline int GetDepth() const
     {
         return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetDepthB1();
     }
 
     __aicore__ inline int GetScaleFactor() const
     {
         return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetScaleFactorKb() *
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetScaleFactorN();
     }
 
     template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
     __aicore__ inline int32_t GetOrgWidth()
     {
         if constexpr (PhyPosIsUB(INPUT_TYPE::scalePosition)) {
             if constexpr (IS_INTRA_BLOCK) {
                 return CeilAlign<int32_t>(GetSingleWidth<IS_TRANS, true>(), c0Size_);
             } else {
                 return GetSingleWidth<IS_TRANS, false>();
             }
         } else {
             if constexpr (IS_TRANS) {
                 return Ceil(MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKb<IS_INTRA_BLOCK>(), MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
             } else {
                 return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<IS_INTRA_BLOCK>();
             }
         }
     }
 
     template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false>
     __aicore__ inline int32_t GetOrgHeight()
     {
         if constexpr (PhyPosIsUB(INPUT_TYPE::scalePosition)) {
             if constexpr (IS_INTRA_BLOCK) {
                 return CeilAlign<int32_t>(GetSingleHeight<IS_TRANS, true>(), BLOCK_CUBE);
             } else {
                 return GetSingleHeight<IS_TRANS, false>();
             }
         } else {
             if constexpr (IS_TRANS) {
                 return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<IS_INTRA_BLOCK>();
             } else {
                 return Ceil(MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKb<IS_INTRA_BLOCK>(), MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
             }
         }
     }
 
     template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
     __aicore__ inline int32_t GetSingleHeight() const
     {
         // Constantized scenario
         if constexpr (IS_TRANS) {
             return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreN<IS_INTRA_BLOCK, IS_BASIC>();
         } else {
             return Ceil(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK, IS_BASIC>(),
             MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
         }
     }
 
     template <bool IS_TRANS = false>
     __aicore__ inline int32_t GetBaseHeight() const
     {
         if constexpr (IS_TRANS) {
             return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
         } else {
             return Ceil(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), MX_K_FACTOR);
         }
     }
 
     template <bool IS_TRANS = false, bool IS_INTRA_BLOCK = false, bool IS_BASIC = false>
     __aicore__ inline int32_t GetSingleWidth() const
     {
         if constexpr (IS_TRANS) {
             return Ceil(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK, IS_BASIC>(),
             MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
         } else {
             return MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreN<IS_INTRA_BLOCK, IS_BASIC>();
         }
     }
 
     template <bool IS_TRANS = false>
     __aicore__ inline int32_t GetBaseWidth() const
     {
         if constexpr (IS_TRANS) {
             return Ceil(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), MX_K_FACTOR);
         } else {
             return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
         }
     }
 
     template <bool IS_INTRA_BLOCK = false>
     __aicore__ inline bool IsKRowDirec() const
     {
         return !MATMUL_MODULE(MatmulShapeInfo)->template IsTransposeScaleB<IS_INTRA_BLOCK>();
     }
 
 private:
     __aicore__ inline int32_t GetOrgSize()
     {
         return Align<int32_t>(GetOrgHeight(), BLOCK_CUBE) * Align<int32_t>(GetOrgWidth(), c0Size_);
     }
 
     __aicore__ inline int32_t GetBaseWidthAlign() const
     {
         if constexpr (IsNeedC0Align<TransT>() && !INPUT_TYPE::isScaleTrans) {
             return Align(GetBaseWidth(), c0Size_);
         } else {
             return GetBaseWidth();
         }
     }
 
     __aicore__ inline int32_t GetBaseHeightAlign() const
     {
         if constexpr (IsNeedC0Align<TransT>()) {
             return Align(GetBaseHeight(), c0Size_);
         } else {
             return GetBaseHeight();
         }
     }
 
     __aicore__ inline int32_t GetBaseSize()
     {
         return GetBaseHeightAlign() * GetBaseWidthAlign();
     }
 
 private:
     constexpr static int32_t c0Size_ = AuxGetC0Size<TransT>();
 };
 } // namespace Detail
 } // namespace Impl
 } // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_MX_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_MX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_PARAMS_MX_H__
#endif 