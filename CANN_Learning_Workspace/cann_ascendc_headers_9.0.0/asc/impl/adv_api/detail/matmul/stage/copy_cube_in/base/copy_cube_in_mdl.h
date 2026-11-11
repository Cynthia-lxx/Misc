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
 * \file copy_cube_in_mdl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_in/base/copy_cube_in_mdl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_MDL_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_MDL_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_MDL_H

#include "../copy_tile_to_cube/copy_tile_to_cube.h"
#include "copy_cube_in_intf.h"
#include "copy_cube_in_mdl_base.h"

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
    !MatmulFeatureTrait<MM_CFG>::IsNeedUB() && GetCopyCubeInType<INPUT_TYPE, MM_CFG>() == CopyCubeInType::MDL &&
    !IsFullLoad<INPUT_TYPE, MM_CFG>()>>
: public CopyCubeInMDLBase<IMPL, MM_CFG, INPUT_TYPE>
{
public:
    using BASE_MODULE = AscendC::Impl::Detail::CopyCubeInMDLBase<IMPL, MM_CFG, INPUT_TYPE>;
    __aicore__ inline CopyCubeIn() = default;
    __aicore__ inline ~CopyCubeIn() = default;
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _COPY_CUBE_IN_MDL_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_MDL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_BASE_COPY_CUBE_IN_MDL_H__
#endif