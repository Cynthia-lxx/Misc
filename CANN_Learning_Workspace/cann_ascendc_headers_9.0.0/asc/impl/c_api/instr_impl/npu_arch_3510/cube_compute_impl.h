/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CUBE_COMPUTE_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CUBE_COMPUTE_IMPL_H

#include "instr_impl/npu_arch_3510/cube_compute_impl/asc_set_l0c2gm_config_impl.h"
#include "instr_impl/npu_arch_3510/cube_compute_impl/asc_set_l0c2gm_nz2nd_impl.h"
#include "instr_impl/npu_arch_3510/cube_compute_impl/asc_mmad_mx_impl.h"

__aicore__ inline void asc_set_l0c2gm_config(uint64_t relu_pre, uint64_t quant_pre, bool enable_unit_flag)
{
    asc_set_l0c2gm_config_impl(relu_pre, quant_pre, enable_unit_flag);
}

__aicore__ inline void asc_set_l0c2gm_nz2nd(uint64_t nd_num, uint64_t src_nd_stride, uint64_t dst_nd_stride)
{
    asc_set_l0c2gm_nz2nd_impl(nd_num, src_nd_stride, dst_nd_stride);
}

// ==========mmad_mx(e1m2/e1m2, e4m3/e5m2)==========
__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix,
    __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix,
    __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_sync_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix,
    __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix,
    __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_sync_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix,
    __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix,
    __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_sync_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix,
    __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix,
    __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_sync_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix,
    __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix,
    __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_sync_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix,
    __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix,
    __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_sync_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix,
    __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix,
    __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_sync_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix,
    __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}

__aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix,
    __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    asc_mmad_mx_sync_impl(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
        unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
}
#endif