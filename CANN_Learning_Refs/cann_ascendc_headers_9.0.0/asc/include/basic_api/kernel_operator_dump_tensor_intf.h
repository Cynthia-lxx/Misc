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
 * \file kernel_operator_dump_tensor_intf.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_H

#include "kernel_macros.h"
#include "kernel_tensor.h"
#include "kernel_log.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif


namespace AscendC {
template <typename T>
__aicore__ inline void DumpTensor(const LocalTensor<T> &tensor, uint32_t desc, uint32_t dumpSize);
template <typename T>
__aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize);
template <typename T>
__aicore__ inline void DumpTensor(const LocalTensor<T>& tensor, uint32_t desc,
    uint32_t dumpSize, const ShapeInfo& shapeInfo);
template <typename T>
__aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc,
    uint32_t dumpSize, const ShapeInfo& shapeInfo);
template <typename T>
__aicore__ inline void DumpAccChkPoint(const LocalTensor<T> &tensor,
    uint32_t index, uint32_t countOff, uint32_t dumpSize);
template <typename T>
__aicore__ inline void DumpAccChkPoint(const GlobalTensor<T> &tensor,
    uint32_t index, uint32_t countOff, uint32_t dumpSize);
#ifndef ASCENDC_CPU_DEBUG
template <class... Args>
__aicore__ inline void PRINTF(__gm__ const char* fmt, Args&&... args);
template <class... Args>
__aicore__ inline void printf(__gm__ const char* fmt, Args&&... args);
#endif

// assert define
#undef assert
#ifdef ASCENDC_DUMP
#if defined(__NPU_DEVICE__) || defined(__NPU_HOST__) || defined(__ASCC_DEVICE__) || defined(__ASCC_HOST__)
#define assert(expr) ASCENDC_NPU_DEBUG_ASSERT_IMPL(expr)
#else
#define assert(...) ASCENDC_DEBUG_DEPRECATE_ASSERT_IMPL(__VA_ARGS__)
#endif
#define ascendc_assert(...) ASCENDC_DEBUG_ASSERT_IMPL(__VA_ARGS__)
#else
#define assert(...)
#define ascendc_assert(...)
#endif
}  // namespace AscendC

#include "../../impl/basic_api/kernel_operator_dump_tensor_intf_impl.h"
#endif  // END OF ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_H__
#endif
