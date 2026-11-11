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
 * \file asc_copy_gm2l1_impl.h
 * \brief
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning "impl/c_api/instr_impl/npu_arch_2201/npu_arch_2201/cube_datamove_impl/asc_copy_gm2l1_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_2201_CUBE_DATAMOVE_IMPL_ASC_COPY_GM2L1_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_2201_CUBE_DATAMOVE_IMPL_ASC_COPY_GM2L1_IMPL_H

#include "instr_impl/npu_arch_2201/utils_impl/utils_impl.h"

// PAD_NONE
__aicore__ inline void asc_copy_gm2l1_impl(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                           uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, burst_len, src_stride, dst_stride, PAD_NONE);
    }
}

__aicore__ inline void asc_copy_gm2l1_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_impl(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_sync_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_impl(dst, src, size);
    asc_sync_post_process();
}

// PAD_MODE1
__aicore__ inline void asc_copy_gm2l1_pad1_impl(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                           uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, burst_len, src_stride, dst_stride, PAD_MODE1);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad1_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad1_impl(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad1_sync_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad1_impl(dst, src, size);
    asc_sync_post_process();
}

// PAD_MODE2
__aicore__ inline void asc_copy_gm2l1_pad2_impl(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                           uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, burst_len, src_stride, dst_stride, PAD_MODE2);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad2_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad2_impl(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad2_sync_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad2_impl(dst, src, size);
    asc_sync_post_process();
}

// PAD_MODE3
__aicore__ inline void asc_copy_gm2l1_pad3_impl(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                           uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, burst_len, src_stride, dst_stride, PAD_MODE3);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad3_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad3_impl(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad3_sync_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad3_impl(dst, src, size);
    asc_sync_post_process();
}

// PAD_MODE4
__aicore__ inline void asc_copy_gm2l1_pad4_impl(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                           uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, burst_len, src_stride, dst_stride, PAD_MODE4);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad4_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad4_impl(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad4_sync_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad4_impl(dst, src, size);
    asc_sync_post_process();
}

// PAD_MODE5
__aicore__ inline void asc_copy_gm2l1_pad5_impl(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                           uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, burst_len, src_stride, dst_stride, PAD_MODE5);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad5_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad5_impl(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad5_sync_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad5_impl(dst, src, size);
    asc_sync_post_process();
}

// PAD_MODE6
__aicore__ inline void asc_copy_gm2l1_pad6_impl(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                           uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, burst_len, src_stride, dst_stride, PAD_MODE6);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad6_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad6_impl(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad6_sync_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad6_impl(dst, src, size);
    asc_sync_post_process();
}

// PAD_MODE7
__aicore__ inline void asc_copy_gm2l1_pad7_impl(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                           uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, burst_len, src_stride, dst_stride, PAD_MODE7);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad7_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad7_impl(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad7_sync_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad7_impl(dst, src, size);
    asc_sync_post_process();
}

// PAD_MODE8
__aicore__ inline void asc_copy_gm2l1_pad8_impl(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                           uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, burst_len, src_stride, dst_stride, PAD_MODE8);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad8_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad8_impl(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad8_sync_impl(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad8_impl(dst, src, size);
    asc_sync_post_process();
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
