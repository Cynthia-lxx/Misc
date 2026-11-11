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

#ifndef INCLUDE_C_API_CUBE_DATAMOVE_CUBE_DATAMOVE_H
#define INCLUDE_C_API_CUBE_DATAMOVE_CUBE_DATAMOVE_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)

#include "instr_impl/npu_arch_2201/cube_datamove_impl.h"

#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

#include "instr_impl/npu_arch_3510/cube_datamove_impl.h"

#endif

__aicore__ inline void asc_set_l13d_rpt(asc_load3d_v2_config& config);

__aicore__ inline void asc_set_l13d_fmatrix(asc_l13d_fmatrix_config& config);

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)

// ==========asc_copy_gm2l1==========
//PAD_NONE
__aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

__aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

//PAD_MODE1
__aicore__ inline void asc_copy_gm2l1_pad1(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_copy_gm2l1_pad1(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

__aicore__ inline void asc_copy_gm2l1_pad1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

//PAD_MODE2
__aicore__ inline void asc_copy_gm2l1_pad2(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_copy_gm2l1_pad2(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

__aicore__ inline void asc_copy_gm2l1_pad2_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

//PAD_MODE3
__aicore__ inline void asc_copy_gm2l1_pad3(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_copy_gm2l1_pad3(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

__aicore__ inline void asc_copy_gm2l1_pad3_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

//PAD_MODE4
__aicore__ inline void asc_copy_gm2l1_pad4(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_copy_gm2l1_pad4(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

__aicore__ inline void asc_copy_gm2l1_pad4_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

//PAD_MODE5
__aicore__ inline void asc_copy_gm2l1_pad5(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_copy_gm2l1_pad5(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

__aicore__ inline void asc_copy_gm2l1_pad5_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

//PAD_MODE6
__aicore__ inline void asc_copy_gm2l1_pad6(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_copy_gm2l1_pad6(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

__aicore__ inline void asc_copy_gm2l1_pad6_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

//PAD_MODE7
__aicore__ inline void asc_copy_gm2l1_pad7(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_copy_gm2l1_pad7(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

__aicore__ inline void asc_copy_gm2l1_pad7_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

//PAD_MODE8
__aicore__ inline void asc_copy_gm2l1_pad8(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t burst_len, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_copy_gm2l1_pad8(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

__aicore__ inline void asc_copy_gm2l1_pad8_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

// ==========asc_copy_gm2l1_nd2nz b8(int8_t/uint8_t)==========
__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

// ==========asc_copy_gm2l1_nd2nz b16(bfloat16_t/half/int16_t)==========
__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ half* dst, __gm__ half* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ half* dst, __gm__ half* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

// ==========asc_copy_gm2l1_nd2nz b32s(float/int32_t/uint32_t)==========
__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ float* dst, __gm__ float* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ float* dst, __gm__ float* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t nd_num,
    uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
    uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride);

// ==========asc_copy_l12fb==========
__aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint16_t burst_num, uint16_t burst_len,
                                      uint16_t src_gap_size, uint16_t dst_gap_size);

__aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size);

__aicore__ inline void asc_copy_l12fb_sync(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size);

// ==========asc_copy_l12l0b, 2D, int4b_t/uint8_t/int8_t/half/bfloat16_t/uint32_t/int32_t/float==========
__aicore__ inline void asc_copy_l12l0b(__cb__ int4b_t* dst, __cbuf__ int4b_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ int4b_t* dst, __cbuf__ int4b_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b(__cb__ uint8_t* dst, __cbuf__ uint8_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ uint8_t* dst, __cbuf__ uint8_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b(__cb__ int8_t* dst, __cbuf__ int8_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ int8_t* dst, __cbuf__ int8_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b(__cb__ half* dst, __cbuf__ half* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ half* dst, __cbuf__ half* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b(__cb__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b(__cb__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b(__cb__ float* dst, __cbuf__ float* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ float* dst, __cbuf__ float* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

// ==========asc_copy_l12l0b, 3D, half/bfloat16_t/uint32_t/int32_t/float==========
__aicore__ inline void asc_copy_l12l0b(__cb__ half* dst, __cbuf__ half* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ half* dst, __cbuf__ half* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b(__cb__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b(__cb__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b(__cb__ float* dst, __cbuf__ float* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b_sync(__cb__ float* dst, __cbuf__ float* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

// ==========asc_copy_l12l0a, 2D, int4b_t/uint8_t/int8_t/half/bfloat16_t/uint32_t/int32_t/float==========
__aicore__ inline void asc_copy_l12l0a(__ca__ int4b_t* dst, __cbuf__ int4b_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ int4b_t* dst, __cbuf__ int4b_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a(__ca__ uint8_t* dst, __cbuf__ uint8_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint8_t* dst, __cbuf__ uint8_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a(__ca__ int8_t* dst, __cbuf__ int8_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ int8_t* dst, __cbuf__ int8_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a(__ca__ half* dst, __cbuf__ half* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ half* dst, __cbuf__ half* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a(__ca__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a(__ca__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a(__ca__ float* dst, __cbuf__ float* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ float* dst, __cbuf__ float* src,
    uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

// ==========asc_copy_l12l0a, 3D, int4b_t/uint8_t/int8_t/half/bfloat16_t/uint32_t/int32_t/float==========
__aicore__ inline void asc_copy_l12l0a(__ca__ int4b_t* dst, __cbuf__ int4b_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ int4b_t* dst, __cbuf__ int4b_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a(__ca__ uint8_t* dst, __cbuf__ uint8_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint8_t* dst, __cbuf__ uint8_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a(__ca__ int8_t* dst, __cbuf__ int8_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ int8_t* dst, __cbuf__ int8_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a(__ca__ half* dst, __cbuf__ half* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ half* dst, __cbuf__ half* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a(__ca__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ uint32_t* dst, __cbuf__ uint32_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a(__ca__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ int32_t* dst, __cbuf__ int32_t* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a(__ca__ float* dst, __cbuf__ float* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0a_sync(__ca__ float* dst, __cbuf__ float* src,
    uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h,
    uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h,
    bool transpose, bool f_matrix_ctrl, uint16_t channel_size);

__aicore__ inline void asc_copy_l12l0b_sparse(__cb__ int8_t* dst, __cbuf__ int8_t* src, __cbuf__ int8_t* index, uint16_t start_index, uint8_t repeat);

__aicore__ inline void asc_copy_l12l0b_sparse_sync(__cb__ int8_t* dst, __cbuf__ int8_t* src, __cbuf__ int8_t* index, uint16_t start_index, uint8_t repeat);

// ==========asc_copy_l12l0b_trans=========
__aicore__ inline void asc_copy_l12l0b_trans(__cb__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat,
                                             uint16_t src_stride, uint16_t dst_stride, bool addrmode,
                                             uint16_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id,
                                             uint8_t repeat, uint16_t src_stride, uint16_t dst_stride, bool addrmode,
                                             uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id,
                                                  uint8_t repeat, uint16_t src_stride, uint16_t dst_stride,
                                                  bool addrmode, uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans(__cb__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat,
                                             uint16_t src_stride, uint16_t dst_stride, bool addrmode,
                                             uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans(__cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id,
                                             uint8_t repeat, uint16_t src_stride, uint16_t dst_stride, bool addrmode,
                                             uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans(__cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id,
                                             uint8_t repeat, uint16_t src_stride, uint16_t dst_stride, bool addrmode,
                                             uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans(__cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id,
                                             uint8_t repeat, uint16_t src_stride, uint16_t dst_stride, bool addrmode,
                                             uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans(__cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id,
                                             uint8_t repeat, uint16_t src_stride, uint16_t dst_stride, bool addrmode,
                                             uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ half* dst, __cbuf__ half* src, uint16_t index_id,
                                                  uint8_t repeat, uint16_t src_stride, uint16_t dst_stride,
                                                  bool addrmode, uint16_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ float* dst, __cbuf__ float* src, uint16_t index_id,
                                                  uint8_t repeat, uint16_t src_stride, uint16_t dst_stride,
                                                  bool addrmode, uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id,
                                                  uint8_t repeat, uint16_t src_stride, uint16_t dst_stride,
                                                  bool addrmode, uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id,
                                                  uint8_t repeat, uint16_t src_stride, uint16_t dst_stride,
                                                  bool addrmode, uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id,
                                                  uint8_t repeat, uint16_t src_stride, uint16_t dst_stride,
                                                  bool addrmode, uint64_t dst_frac_stride);

__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id,
                                                  uint8_t repeat, uint16_t src_stride, uint16_t dst_stride,
                                                  bool addrmode, uint64_t dst_frac_stride);

__aicore__ inline void asc_set_l13d_size(uint64_t value);

__aicore__ inline void asc_set_l13d_padding(uint64_t config);

__aicore__ inline void asc_set_l13d_padding(half config);

__aicore__ inline void asc_set_l13d_padding(int16_t config);

__aicore__ inline void asc_set_l13d_padding(uint16_t config);

// ==========asc_load_image_to_cbuf(half/int8_t)==========
__aicore__ inline void asc_load_image_to_cbuf(__cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size,
    uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size,
    uint16_t left_pad_size, uint16_t right_pad_size);

__aicore__ inline void asc_load_image_to_cbuf_sync(__cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size,
    uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size,
    uint16_t left_pad_size, uint16_t right_pad_size);

__aicore__ inline void asc_load_image_to_cbuf(__cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size,
    uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size,
    uint16_t left_pad_size, uint16_t right_pad_size);

__aicore__ inline void asc_load_image_to_cbuf_sync(__cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size,
    uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size,
    uint16_t left_pad_size, uint16_t right_pad_size);

// ==========asc_fill_l0a(half/float/int16_t/int32_t/uint16_t/uint32_t/bfloat16_t)==========
__aicore__ inline void asc_fill_l0a(__ca__ half* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ half* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ half* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ half* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ float* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ float* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ float* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ float* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ int16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ int16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ int16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ int16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ int32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ int32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ int32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ int32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ uint16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ uint16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ uint32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ uint32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a(__ca__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0a_sync(__ca__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config);

// ==========asc_fill_l0b(half/float/int16_t/int32_t/uint16_t/uint32_t/bfloat16_t)==========
__aicore__ inline void asc_fill_l0b(__cb__ half* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ half* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ float* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ float* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ int16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ int16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ int32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ int32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ uint16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ uint32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config);

// ==========asc_fill_l1(half/float/int16_t/int32_t/uint16_t/uint32_t/bfloat16_t)==========
__aicore__ inline void asc_fill_l1(__cbuf__ half* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ half* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ half* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ half* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ float* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ float* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ float* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ float* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ int16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ int16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ int16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ int16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ int32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ int32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ int32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ int32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ uint16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint16_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ uint32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint32_t* dst, half value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1(__cbuf__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_fill_l1_sync(__cbuf__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config);

__aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ void* src, uint16_t conv_control, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap);

__aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ void* src, uint32_t size);

__aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ void* src, uint32_t size);

__aicore__ inline void asc_set_l0c_copy_prequant(uint64_t config);

__aicore__ inline void asc_set_l0c_copy_params(uint16_t nd_num, uint16_t src_nd_stride, uint16_t dst_nd_stride);

#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

// ==========asc_l0c2l1_l0c2l1===========
// half  float
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size,
                                            uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre,
                                            uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
                                            bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post,
                                            bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_cfg,
                                            bool c0_pad_en);

// int8_t  float
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size,
                                            uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre,
                                            uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
                                            bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post,
                                            bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_cfg,
                                            bool c0_pad_en);


// uint8_t  float
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size,
                                            uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre,
                                            uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
                                            bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post,
                                            bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_cfg,
                                            bool c0_pad_en);

// float  float
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size,
                                            uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre,
                                            uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
                                            bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post,
                                            bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_cfg,
                                            bool c0_pad_en);

// half int32_t
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size,
                                            uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre,
                                            uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
                                            bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post,
                                            bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_cfg,
                                            bool c0_pad_en);

// int8_t int32_t
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size,
                                            uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre,
                                            uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
                                            bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post,
                                            bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_cfg,
                                            bool c0_pad_en);

// uint8_t int32_t
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size,
                                            uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre,
                                            uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
                                            bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post,
                                            bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_cfg,
                                            bool c0_pad_en);

// int32_t int32_t
__aicore__ inline void asc_copy_l0c2l1(__cbuf__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size,
                                            uint32_t dst_stride, uint16_t src_stride, uint8_t clip_relu_pre,
                                            uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
                                            bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post,
                                            bool clip_relu_post, uint8_t eltwise_op, uint8_t eltwise_antq_cfg,
                                            bool c0_pad_en);

// ==========asc_copy_l12l0a_mx==========
__aicore__ inline void asc_copy_l12l0a_mx(uint64_t dst, __cbuf__ fp8_e8m0_t* src, uint16_t x_start_pos,
    uint16_t y_start_pos, uint8_t x_step, uint8_t y_step, uint16_t src_stride, uint16_t dst_stride);

__aicore__ inline void asc_set_gm2l1_loop_size(uint64_t loop1_size, uint64_t loop2_size);

__aicore__ inline void asc_set_gm2l1_loop1_stride(uint64_t loop1_src_stride, uint64_t loop1_dst_stride);

__aicore__ inline void asc_set_gm2l1_loop2_stride(uint64_t loop2_src_stride, uint64_t loop2_dst_stride);

__aicore__ inline void asc_set_gm2l1_pad(uint32_t pad_val);

#endif

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)  
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS  
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H  
#endif    