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
 * \file context.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/context/context.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_CONTEXT_H__
#endif

#ifndef IMPL_MATMUL_CONTEXT_CONTEXT_H
#define IMPL_MATMUL_CONTEXT_CONTEXT_H

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    MatmulContext is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MatmulContext is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, const auto& MM_CFG, typename = void>
class MatmulContext
{
public:
    __aicore__ inline MatmulContext() = default;
    __aicore__ inline ~MatmulContext() = default;
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC

#endif //IMPL_MATMUL_CONTEXT_CONTEXT_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_CONTEXT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_CONTEXT_H__
#endif