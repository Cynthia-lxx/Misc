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
 * \file kernel_operator_vec_createvecindex_intf.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CREATEVECINDEX_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_CREATEVECINDEX_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_VEC_CREATEVECINDEX_INTERFACE_H

#include "kernel_macros.h"
#include "kernel_tensor.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

namespace AscendC {

template <typename T>
__aicore__ inline __in_pipe__(S) __out_pipe__(V) void CreateVecIndex(LocalTensor<T> &dst, const T &firstValue,
    uint64_t mask, uint8_t repeatTime, uint16_t dstBlkStride, uint8_t dstRepStride);

template <typename T>
__aicore__ inline __in_pipe__(S) __out_pipe__(V) void CreateVecIndex(LocalTensor<T> &dst, const T &firstValue,
    uint64_t mask[], uint8_t repeatTime, uint16_t dstBlkStride, uint8_t dstRepStride);

template <typename T>
__aicore__ inline __in_pipe__(S) __out_pipe__(V) void CreateVecIndex(LocalTensor<T> dst, const T &firstValue,
    uint32_t count);
} // namespace AscendC

#include "../../impl/basic_api/kernel_operator_vec_createvecindex_intf_impl.h"
#endif // ASCENDC_MODULE_OPERATOR_VEC_CREATEVECINDEX_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CREATEVECINDEX_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CREATEVECINDEX_INTF_H__
#endif
