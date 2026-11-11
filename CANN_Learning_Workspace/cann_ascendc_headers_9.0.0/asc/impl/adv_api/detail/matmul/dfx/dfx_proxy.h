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
 * \file dfx_proxy.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/dfx/dfx_proxy.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_DFX_DFX_PROXY_H__
#endif

#ifndef MATMUL_DFX_PROXY_H
#define MATMUL_DFX_PROXY_H

#include <utility>
#include "dfx_handler.h"

namespace AscendC {

template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

namespace Impl {
namespace Detail {

template <class T>
constexpr bool is_void_v = std::is_void<T>::value;

///////////////////////////////////////////////////////////////////////////////
template <typename IMPL, typename MODULE>
struct DfxProxy : MODULE {
    __aicore__ inline auto operator->() { return this; }
    __aicore__ inline operator MODULE*() { return this; }
};

///////////////////////////////////////////////////////////////////////////////
#define MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC)                               \
template <typename... Args>                                                       \
__aicore__ inline auto FUNC(Args&&... args) -> enable_if_t<!is_void_v<  \
decltype(MODULE().MODULE::FUNC(std::forward<Args>(args)...))>,                    \
decltype(MODULE().MODULE::FUNC(std::forward<Args>(args)...))>{                    \
    DfxFuncInfo info{#MODULE, #FUNC, __COUNTER__};                                \
    DfxHandler::PreCall(info, std::forward<Args>(args)...);                       \
    auto ret = M_.MODULE::FUNC(std::forward<Args>(args)...);                      \
    DfxHandler::PostCall(info, ret);                                              \
    return ret;                                                                   \
}                                                                                 \
template <typename... Args>                                                       \
__aicore__ inline auto FUNC(Args&&... args) -> enable_if_t<is_void_v<   \
decltype(MODULE().MODULE::FUNC(std::forward<Args>(args)...))>> {                  \
    DfxFuncInfo info{#MODULE, #FUNC, __COUNTER__};                                \
    DfxHandler::PreCall(info, std::forward<Args>(args)...);                       \
    M_.MODULE::FUNC(std::forward<Args>(args)...);                                 \
    DfxHandler::PostCall(info);                                                   \
}   

///////////////////////////////////////////////////////////////////////////////
#define MATMUL_COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define MATMUL_COUNT_ARGS(...)                                              \
MATMUL_COUNT_ARGS_IMPL(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define MATMUL_DEF_PROXY_FUNC_1(M_, MODULE, FUNC1)                          \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC1)

#define MATMUL_DEF_PROXY_FUNC_2(M_, MODULE, FUNC1, FUNC2)                   \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC1)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC2)

#define MATMUL_DEF_PROXY_FUNC_3(M_, MODULE, FUNC1, FUNC2, FUNC3)            \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC1)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC2)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC3)

#define MATMUL_DEF_PROXY_FUNC_4(M_, MODULE, FUNC1, FUNC2, FUNC3, FUNC4)     \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC1)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC2)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC3)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC4)

#define MATMUL_DEF_PROXY_FUNC_5(M_, MODULE, FUNC1, FUNC2, FUNC3, FUNC4, FUNC5) \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC1)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC2)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC3)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC4)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC5)

#define MATMUL_DEF_PROXY_FUNC_6(M_, MODULE, FUNC1, FUNC2, FUNC3, FUNC4, FUNC5, FUNC6) \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC1)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC2)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC3)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC4)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC5)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC6)

#define MATMUL_DEF_PROXY_FUNC_7(M_, MODULE, FUNC1, FUNC2, FUNC3, FUNC4, FUNC5, FUNC6, FUNC7) \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC1)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC2)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC3)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC4)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC5)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC6)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC7)

#define MATMUL_DEF_PROXY_FUNC_8(M_, MODULE, FUNC1, FUNC2, FUNC3, FUNC4, FUNC5, FUNC6, FUNC7, FUNC8) \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC1)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC2)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC3)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC4)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC5)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC6)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC7)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC8)

#define MATMUL_DEF_PROXY_FUNC_9(M_, MODULE, FUNC1, FUNC2, FUNC3, FUNC4, FUNC5, FUNC6, FUNC7, FUNC8, FUNC9) \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC1)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC2)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC3)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC4)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC5)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC6)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC7)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC8)                                \
MATMUL_DEF_DFX_PROXY_FUNC(M_, MODULE, FUNC9)

#define MATMUL_DEF_DFX_FUNCS_IMPL2(N, M_, MODULE, ...)                      \
MATMUL_DEF_PROXY_FUNC_##N(M_, MODULE, __VA_ARGS__)

#define MATMUL_DEF_DFX_FUNCS_IMPL(N, M_, MODULE, ...)                       \
MATMUL_DEF_DFX_FUNCS_IMPL2(N, M_, MODULE, __VA_ARGS__)

#define MATMUL_DEF_DFX_FUNCS(M_, MODULE, ...)                               \
MATMUL_DEF_DFX_FUNCS_IMPL(MATMUL_COUNT_ARGS(__VA_ARGS__), M_, MODULE, __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////
#define MATMUL_DFX_PROXY_REGISTER(MODULE, ...)                              \
template <typename IMPL>                                                    \
struct DfxProxy<IMPL, typename IMPL::MODULE> {                              \
    using MODULE = typename IMPL::MODULE;                                   \
    __aicore__ inline DfxProxy(MODULE& module) : proxy{module} {}                             \
    struct FuncProxy {                                                      \
        __aicore__ inline FuncProxy(MODULE& module) : m_{module} {}                           \
        __aicore__ inline auto& operator*() { return m_; }                                    \
        MATMUL_DEF_DFX_FUNCS(m_, MODULE, __VA_ARGS__)                       \
    private:                                                                \
        MODULE& m_;                                                         \
    };                                                                      \
    __aicore__ inline auto operator->() { return &proxy; }                                    \
    __aicore__ inline operator MODULE*() { return &(*proxy); }                                \
private:                                                                    \
    FuncProxy proxy;                                                        \
};                                                                          \
template <typename IMPL>                                                    \
struct DfxProxy<const IMPL, typename IMPL::MODULE> {                        \
    using MODULE = typename IMPL::MODULE;                                   \
    __aicore__ inline DfxProxy(const MODULE& module) : proxy{module} {}                       \
    struct FuncProxy {                                                      \
        __aicore__ inline FuncProxy(const MODULE& module) : m_{module} {}                     \
        __aicore__ inline const auto& operator*() { return m_; }                              \
        MATMUL_DEF_DFX_FUNCS(m_, MODULE, __VA_ARGS__)                       \
    private:                                                                \
        const MODULE& m_;                                                   \
    };                                                                      \
    __aicore__ inline auto operator->() { return &proxy; }                                    \
    __aicore__ inline operator MODULE*() { return &(*proxy); }                                \
private:                                                                    \
    FuncProxy proxy;                                                        \
}

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _DFX_PROXY_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_DFX_DFX_PROXY_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_DFX_DFX_PROXY_H__
#endif