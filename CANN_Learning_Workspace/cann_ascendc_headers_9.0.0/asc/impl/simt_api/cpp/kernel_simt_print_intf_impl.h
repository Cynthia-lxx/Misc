/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef IMPL_SIMT_API_CPP_KERNEL_SIMT_PRINT_INTERFACE_IMPL_H
#define IMPL_SIMT_API_CPP_KERNEL_SIMT_PRINT_INTERFACE_IMPL_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_PRINTF_CPP__
#endif

#include "impl/utils/debug/asc_printf_simt_impl.h"

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_PRINTF_CPP__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_PRINTF_CPP__
#endif

namespace AscendC {
namespace Simt {

#ifdef ASCENDC_CPU_DEBUG
#include <cstdio>
#include <utility>

using ::printf;

template <typename... Args>
inline auto PRINTF(Args&&... args) -> decltype(printf(std::forward<Args>(args)...))
{
#ifdef ASCENDC_DUMP
    return printf(std::forward<Args>(args)...);
#else
    return 0;
#endif
}
#else

#ifndef __CHECK_FEATURE_AT_PRECOMPILE

template <class... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void PRINTF(const __gm__ char* fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
    __asc_simt_vf::simt_printf_impl(__asc_simt_vf::DumpType::DUMP_SIMT_PRINTF, fmt, args...);
#endif
}

template <class... Args>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void printf(const __gm__ char* fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
    __asc_simt_vf::simt_printf_impl(__asc_simt_vf::DumpType::DUMP_SIMT_PRINTF, fmt, args...);
#endif
}

#endif

#endif

}  // namespace Simt
}  // namespace AscendC

#endif  // IMPL_SIMT_API_CPP_KERNEL_SIMT_PRINT_INTERFACE_IMPL_H
