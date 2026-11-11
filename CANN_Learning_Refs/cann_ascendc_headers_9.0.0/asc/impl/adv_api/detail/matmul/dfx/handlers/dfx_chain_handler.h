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
 * \file dfx_chain_handler.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/dfx/handlers/dfx_chain_handler.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_DFX_H__ANDLERS_DFX_CHAIN_HANDLER_H
#endif

#ifndef MATMUL_DFX_CHAIN_HANDLER_H
#define MATMUL_DFX_CHAIN_HANDLER_H

namespace AscendC {
namespace Impl {
namespace Detail {

struct DfxFuncInfo;

template <typename ...HANDLERS> 
struct DfxChainHandler {
    template <typename... Args>
    __aicore__ inline static void PreCall(const DfxFuncInfo& info, Args&&... args) {
        (HANDLERS::PreCall(info, std::forward<Args>(args)...), ...);
    }

    template <typename RT>
    __aicore__ inline static void PostCall(const DfxFuncInfo& info, const RT& ret) {
        (HANDLERS::PostCall(info, ret), ...);
    }

    __aicore__ inline static void PostCall(const DfxFuncInfo& info) {
        (HANDLERS::PostCall(info), ...);
    }
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _DFX_CHAIN_HANDLER_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_DFX_H__ANDLERS_DFX_CHAIN_HANDLER_H)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_DFX_H__ANDLERS_DFX_CHAIN_HANDLER_H
#endif