/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_SCALAR_COMPUTE_H
#define INCLUDE_C_API_SCALAR_COMPUTE_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)

#include "instr_impl/npu_arch_2201/scalar_compute_impl.h"

#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

#include "instr_impl/npu_arch_3510/scalar_compute_impl.h"

#endif

__aicore__ inline int64_t asc_sflbits(int64_t value);

__aicore__ inline int64_t asc_ffz(uint64_t value);

__aicore__ inline int64_t asc_ffs(uint64_t value);

__aicore__ inline int64_t asc_popc(uint64_t value);

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)

__aicore__ inline int64_t asc_clz(uint64_t value_in);

__aicore__ inline int64_t asc_zero_bits_cnt(uint64_t value);

__aicore__ inline uint64_t asc_set_nthbit(uint64_t bits, int64_t idx);

__aicore__ inline uint64_t asc_clear_nthbit(uint64_t bits, int64_t idx);

__aicore__ inline int32_t asc_float2int32_rd(float value);

__aicore__ inline int32_t asc_float2int32_ru(float value);

__aicore__ inline int32_t asc_float2int32_rn(float value);

__aicore__ inline int32_t asc_float2int32_rna(float value);

#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

__aicore__ inline void asc_store_dev(__gm__ int8_t* addr, int8_t value);

__aicore__ inline void asc_store_dev(__gm__ uint8_t* addr, uint8_t value);

__aicore__ inline void asc_store_dev(__gm__ int16_t* addr, int16_t value);

__aicore__ inline void asc_store_dev(__gm__ uint16_t* addr, uint16_t value);

__aicore__ inline void asc_store_dev(__gm__ int32_t* addr, int32_t value);

__aicore__ inline void asc_store_dev(__gm__ uint32_t* addr, uint32_t value);

__aicore__ inline void asc_store_dev(__gm__ int64_t* addr, int64_t value);

__aicore__ inline void asc_store_dev(__gm__ uint64_t* addr, uint64_t value);

#endif

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif