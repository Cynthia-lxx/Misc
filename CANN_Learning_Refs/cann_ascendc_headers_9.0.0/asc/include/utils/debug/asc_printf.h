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
 * \file asc_printf.h
 * \brief
 */

#ifndef INCLUDE_UTILS_DEBUG_ASC_PRINTF_H
#define INCLUDE_UTILS_DEBUG_ASC_PRINTF_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_PRINTF_H__
#endif

#include "impl/utils/sys_macros.h"
#include "simt_api/device_types.h"

#ifndef __CHECK_FEATURE_AT_PRECOMPILE

namespace __asc_simt_vf {
template <class... Args>
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
static __attribute__((noinline)) __SIMT_DEVICE_FUNCTIONS_DECL__ void printf(const __gm__ char* fmt, Args&&... args);
#else
static __attribute__((noinline)) __SIMT_DEVICE_FUNCTIONS_DECL__ void printf(const char* fmt, Args&&... args);
#endif
} // namespace __asc_simt_vf

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
#include "impl/utils/debug/asc_printf_simt_impl.h"
#endif
#endif

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
#ifndef __CHECK_FEATURE_AT_PRECOMPILE
namespace __asc_aicore {
template <class... Args>
__aicore__ inline void printf_impl(__gm__ const char* fmt, Args&&... args);
template <class... Args>
__aicore__ inline void printf(__gm__ const char* fmt, Args&&... args)
{
    printf_impl(fmt, args...);
}
} // namespace __asc_aicore
#endif

#if (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510)
#include "impl/utils/debug/asc_aicore_printf_impl.h"
#endif
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_PRINTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_PRINTF_H__
#endif

#endif