/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_VECTOR_COMPUTE_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_VECTOR_COMPUTE_IMPL_H

#include "instr_impl/npu_arch_3510/utils_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_relu_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_create_iter_reg_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_sub_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_subc_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_and_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_shiftleft_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_shiftright_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_select_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2half_impl/asc_float2half_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2half_impl/asc_float2half_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2half_impl/asc_float2half_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2half_impl/asc_float2half_ro_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2half_impl/asc_float2half_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2half_impl/asc_float2half_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162half_impl/asc_bfloat162half_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162half_impl/asc_bfloat162half_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162half_impl/asc_bfloat162half_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162half_impl/asc_bfloat162half_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162half_impl/asc_bfloat162half_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_uint322int16_impl/asc_uint322int16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2bfloat16_impl/asc_half2bfloat16_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2bfloat16_impl/asc_half2bfloat16_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2bfloat16_impl/asc_half2bfloat16_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2bfloat16_impl/asc_half2bfloat16_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2bfloat16_impl/asc_half2bfloat16_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2e5m2_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2float_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_e4m32float_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int162float_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int4x22int16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int322int16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int322uint8_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_duplicate_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_duplicate_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_abs_sub_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_min_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_neg_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_ge_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_ge_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_reduce_max_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_reduce_min_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_reduce_min_datablock_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_axpy_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_abs_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_arange_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_cumulative_histogram_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_le_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_le_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_frequency_histogram_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_intlv_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_mull_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_squeeze_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_unsqueeze_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_update_mask_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162float_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2hif8_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int8_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int162uint32_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int322float_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int4x22bfloat16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_uint162uint8_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int4x2_impl/asc_half2int4x2_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int4x2_impl/asc_half2int4x2_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int4x2_impl/asc_half2int4x2_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int4x2_impl/asc_half2int4x2_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int4x2_impl/asc_half2int4x2_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_hif82float_impl/asc_hif82float_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_hif82half_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_uint82half_impl/asc_uint82half_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_uint162uint32_impl/asc_uint162uint32_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_add_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_addc_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_shiftleft_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_shiftright_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_not_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_lt_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_lt_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_madd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_pair_reduce_sum_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_max_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_min_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_gt_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_gt_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_reduce_max_datablock_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_leakyrelu_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2uint8_impl/asc_half2uint8_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2uint8_impl/asc_half2uint8_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2uint8_impl/asc_half2uint8_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2uint8_impl/asc_half2uint8_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2uint8_impl/asc_half2uint8_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int162half_impl/asc_int162half_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int162half_impl/asc_int162half_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int162half_impl/asc_int162half_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int162half_impl/asc_int162half_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int162half_impl/asc_int162half_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int4x22half_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int82int16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int162uint8_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162int32_impl/asc_bfloat162int32_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162int32_impl/asc_bfloat162int32_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162int32_impl/asc_bfloat162int32_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162int32_impl/asc_bfloat162int32_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162int32_impl/asc_bfloat162int32_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int16_impl/asc_half2int16_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int16_impl/asc_half2int16_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int16_impl/asc_half2int16_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int16_impl/asc_half2int16_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int16_impl/asc_half2int16_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int642float_impl/asc_int642float_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int642float_impl/asc_int642float_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int642float_impl/asc_int642float_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int642float_impl/asc_int642float_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int642float_impl/asc_int642float_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int82half_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int162int32_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_uint322uint8_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_hif82half_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_sqrt_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_or_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_mul_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_mul_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_eq_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_eq_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int16_impl/asc_float2int16_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int16_impl/asc_float2int16_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int16_impl/asc_float2int16_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int16_impl/asc_float2int16_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int16_impl/asc_float2int16_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int32_impl/asc_float2int32_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int32_impl/asc_float2int32_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int32_impl/asc_float2int32_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int32_impl/asc_float2int32_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int32_impl/asc_float2int32_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e2m1x2_impl/asc_bfloat162e2m1x2_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e2m1x2_impl/asc_bfloat162e2m1x2_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e2m1x2_impl/asc_bfloat162e2m1x2_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e2m1x2_impl/asc_bfloat162e2m1x2_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e2m1x2_impl/asc_bfloat162e2m1x2_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2hif8_impl/asc_float2hif8_rh_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2hif8_impl/asc_float2hif8_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_ceil_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_e2m1x22bfloat16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_floor_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_rint_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_round_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_trunc_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_uint82uint32_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_uint322uint16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_muls_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_exp_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_add_scalar_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2bfloat16_impl/asc_float2bfloat16_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2bfloat16_impl/asc_float2bfloat16_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2bfloat16_impl/asc_float2bfloat16_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2bfloat16_impl/asc_float2bfloat16_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2bfloat16_impl/asc_float2bfloat16_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int64_impl/asc_float2int64_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int64_impl/asc_float2int64_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int64_impl/asc_float2int64_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int64_impl/asc_float2int64_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2int64_impl/asc_float2int64_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int32_impl/asc_half2int32_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int32_impl/asc_half2int32_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int32_impl/asc_half2int32_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int32_impl/asc_half2int32_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_half2int32_impl/asc_half2int32_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_e5m22float_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_float2e4m3_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int322int64_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int322uint16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int642int32_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_uint82uint16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_pack_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_exp_sub_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_ln_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_div_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_max_scalar.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_ne.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_ne_scalar.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_deintlv_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_reduce_sum.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_reduce_sum_datablock.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_unpack_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_copy_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e1m2x2_impl/asc_bfloat162e1m2x2_rd_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e1m2x2_impl/asc_bfloat162e1m2x2_rn_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e1m2x2_impl/asc_bfloat162e1m2x2_rna_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e1m2x2_impl/asc_bfloat162e1m2x2_ru_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_bfloat162e1m2x2_impl/asc_bfloat162e1m2x2_rz_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_e1m2x22bfloat16_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/vconv/asc_int82int32_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_xor_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_prelu_impl.h"
#include "instr_impl/npu_arch_3510/vector_compute_impl/asc_clear_ar_spr_impl.h"

// ==========asc_create_iter_reg(b8/b16/b32)=========
__simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset)
{
    return asc_create_iter_reg_b32_impl(offset);
}

__simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset)
{
    return asc_create_iter_reg_b16_impl(offset);
}

__simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset)
{
    return asc_create_iter_reg_b8_impl(offset);
}

// ==========asc_add(u8/s8/u16/s18/u32/s32)=========
__simd_callee__ inline void asc_add(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_add_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_add_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_add_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_add_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_add_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_add_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_add_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_add_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_add_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_bool& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_add_impl(dst0, dst1, src0, src1, mask);
}

__simd_callee__ inline void asc_add(vector_bool& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_add_impl(dst0, dst1, src0, src1, mask);
}

// // ==========asc_addc(uint32_t/int32_t)==========
__simd_callee__ inline void asc_addc(vector_bool& dst0, vector_uint32_t& dst1,
    vector_uint32_t src0, vector_uint32_t src1, vector_bool src2, vector_bool mask)
{
    asc_addc_impl(dst0, dst1, src0, src1, src2, mask);
}

__simd_callee__ inline void asc_addc(vector_bool& dst0, vector_int32_t& dst1,
    vector_int32_t src0, vector_int32_t src1, vector_bool src2, vector_bool mask)
{
    asc_addc_impl(dst0, dst1, src0, src1, src2, mask);
}

// ==========asc_shiftleft(u8/s8/u16/s16/u32/s32)==========
__simd_callee__ inline void asc_shiftleft(vector_uint8_t& dst,
    vector_uint8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_shiftleft_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftleft(vector_int8_t& dst,
    vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_shiftleft_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftleft(vector_uint16_t& dst,
    vector_uint16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_shiftleft_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftleft(vector_int16_t& dst,
    vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_shiftleft_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftleft(vector_uint32_t& dst,
    vector_uint32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_shiftleft_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftleft(vector_int32_t& dst,
    vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_shiftleft_impl(dst, src0, src1, mask);
}

// ==========asc_shiftright(u8/s8/u16/s16/u32/s32)==========
__simd_callee__ inline void asc_shiftright(vector_uint8_t& dst,
    vector_uint8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_shiftright_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftright(vector_int8_t& dst,
    vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_shiftright_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftright(vector_uint16_t& dst,
    vector_uint16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_shiftright_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftright(vector_int16_t& dst,
    vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_shiftright_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftright(vector_uint32_t& dst,
    vector_uint32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_shiftright_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_shiftright(vector_int32_t& dst,
    vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_shiftright_impl(dst, src0, src1, mask);
}

// ==========asc_not(u8/s8/u16/s16/half/u32/s32/f32/bool)==========
__simd_callee__ inline void asc_not(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_not_impl(dst, src, mask);
}

__simd_callee__ inline void asc_not(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_not_impl(dst, src, mask);
}

__simd_callee__ inline void asc_not(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_not_impl(dst, src, mask);
}

__simd_callee__ inline void asc_not(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_not_impl(dst, src, mask);
}

__simd_callee__ inline void asc_not(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_not_impl(dst, src, mask);
}

__simd_callee__ inline void asc_not(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_not_impl(dst, src, mask);
}

__simd_callee__ inline void asc_not(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_not_impl(dst, src, mask);
}

__simd_callee__ inline void asc_not(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_not_impl(dst, src, mask);
}

__simd_callee__ inline void asc_not(vector_bool& dst, vector_bool src, vector_bool mask)
{
    asc_not_impl(dst, src, mask);
}

//==========asc_lt(u8/s8/half/u16/s16/float/u32/s32/bf16)==========
__simd_callee__ inline void asc_lt(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_lt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_lt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_lt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_lt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_lt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_lt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_lt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_lt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_lt(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_lt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    asc_lt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    asc_lt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    asc_lt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    asc_lt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_lt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask)
{
    asc_lt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    asc_lt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    asc_lt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask)
{
    asc_lt_scalar_impl(dst, src, value, mask);
}

// ==========asc_madd(half/float)==========
__simd_callee__ inline void asc_madd(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_madd_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_madd(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_madd_impl(dst, src0, src1, mask);
}

// ==========asc_madd(half/float)==========
__simd_callee__ inline void asc_pair_reduce_sum(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_pair_reduce_sum_impl(dst, src, mask);
}

__simd_callee__ inline void asc_pair_reduce_sum(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_pair_reduce_sum_impl(dst, src, mask);
}

// ==========asc_relu(half/int32_t/float)==========
__simd_callee__ inline void asc_relu(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_relu_impl(dst, src, mask);
}

__simd_callee__ inline void asc_relu(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_relu_impl(dst, src, mask);
}

__simd_callee__ inline void asc_relu(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_relu_impl(dst, src, mask);
}

// ==========asc_sub(uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_sub(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_sub_impl(carry, dst, src0, src1, mask);
}

__simd_callee__ inline void asc_sub(vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_sub_impl(carry, dst, src0, src1, mask);
}

// ==========asc_subc(uint32_t/int32_t)==========
__simd_callee__ inline void asc_subc(vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool carry_src, vector_bool mask)
{
    asc_subc_impl(carry, dst, src0, src1, carry_src, mask);
}

__simd_callee__ inline void asc_subc(vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool carry_src, vector_bool mask)
{
    asc_subc_impl(carry, dst, src0, src1, carry_src, mask);
}

// ==========asc_and(bool/uint8_t/int8_t/fp8_e4m3/fp8_e5m2/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_and(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_and(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_and_impl(dst, src0, src1, mask);
}

// ==========asc_shiftleft_scalar(uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t)==========
__simd_callee__ inline void asc_shiftleft_scalar(vector_uint8_t& dst, vector_uint8_t src, int16_t value, vector_bool mask)
{
    asc_shiftleft_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftleft_scalar(vector_int8_t& dst, vector_int8_t src, int16_t value, vector_bool mask)
{
    asc_shiftleft_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftleft_scalar(vector_uint16_t& dst, vector_uint16_t src, int16_t value, vector_bool mask)
{
    asc_shiftleft_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftleft_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_shiftleft_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftleft_scalar(vector_uint32_t& dst, vector_uint32_t src, int16_t value, vector_bool mask)
{
    asc_shiftleft_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftleft_scalar(vector_int32_t& dst, vector_int32_t src, int16_t value, vector_bool mask)
{
    asc_shiftleft_scalar_impl(dst, src, value, mask);
}

// ==========asc_shiftright_scalar(uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t)==========
__simd_callee__ inline void asc_shiftright_scalar(vector_uint8_t& dst, vector_uint8_t src, int16_t value, vector_bool mask)
{
    asc_shiftright_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftright_scalar(vector_int8_t& dst, vector_int8_t src, int16_t value, vector_bool mask)
{
    asc_shiftright_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftright_scalar(vector_uint16_t& dst, vector_uint16_t src, int16_t value, vector_bool mask)
{
    asc_shiftright_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftright_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_shiftright_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftright_scalar(vector_uint32_t& dst, vector_uint32_t src, int16_t value, vector_bool mask)
{
    asc_shiftright_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_shiftright_scalar(vector_int32_t& dst, vector_int32_t src, int16_t value, vector_bool mask)
{
    asc_shiftright_scalar_impl(dst, src, value, mask);
}

// ==========asc_select(bool/uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_select(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_hifloat8_t& dst, vector_hifloat8_t src0, vector_hifloat8_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_select(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_select_impl(dst, src0, src1, mask);
}

// ==========asc_float2half(rd/ru/rz/rn/rna/ro)==========
__simd_callee__ inline void asc_float2half_rd(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rd_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rd_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rd_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rd_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_ru(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_ru_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_ru_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_ru_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_ru_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rz(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rz_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rz_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rz_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rz_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rn(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rn_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rn_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rn_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rn_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rna(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rna_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rna_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_rna_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_rna_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_ro(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_ro_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_ro_sat(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_ro_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_ro_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_ro_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2half_ro_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
{
    asc_float2half_ro_sat_v2_impl(dst, src, mask);
}

// ==========asc_uint322int16==========
__simd_callee__ inline void asc_uint322int16(vector_int16_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322int16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322int16_sat(vector_int16_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322int16_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322int16_v2(vector_int16_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322int16_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322int16_sat_v2(vector_int16_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322int16_sat_v2_impl(dst, src, mask);
}

// ==========asc_half2bfloat16==========
__simd_callee__ inline void asc_half2bfloat16_rd(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2bfloat16_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2bfloat16_rn(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2bfloat16_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2bfloat16_rna(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2bfloat16_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2bfloat16_ru(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2bfloat16_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2bfloat16_rz(vector_bfloat16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2bfloat16_rz_impl(dst, src, mask);
}

// ==========asc_bfloat162half(rd/ru/rz/rn/rna)==========
__simd_callee__ inline void asc_bfloat162half_rd(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162half_rd_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162half_ru(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162half_ru_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162half_rz(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162half_rz_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162half_rn(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162half_rn_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162half_rna(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162half_rna_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162half_rna_sat_impl(dst, src, mask);
}

// ==========asc_e4m32float==========
__simd_callee__ inline void asc_e4m32float(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    asc_e4m32float_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e4m32float_v2(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    asc_e4m32float_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e4m32float_v3(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    asc_e4m32float_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e4m32float_v4(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    asc_e4m32float_v4_impl(dst, src, mask);
}

// ==========asc_int162float==========
__simd_callee__ inline void asc_int162float(vector_float& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162float_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162float_v2(vector_float& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162float_v2_impl(dst, src, mask);
}

// ==========asc_int4x22int16==========
__simd_callee__ inline void asc_int4x22int16(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22int16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int4x22int16_v2(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22int16_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int4x22int16_v3(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22int16_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int4x22int16_v4(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22int16_v4_impl(dst, src, mask);
}

// ==========asc_int322int16==========
__simd_callee__ inline void asc_int322int16(vector_int16_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322int16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322int16_sat(vector_int16_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322int16_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322int16_v2(vector_int16_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322int16_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322int16_sat_v2(vector_int16_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322int16_sat_v2_impl(dst, src, mask);
}

// ==========asc_int322uint8==========
__simd_callee__ inline void asc_int322uint8(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint8_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint8_sat(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint8_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint8_v2(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint8_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint8_sat_v2(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint8_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint8_v3(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint8_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint8_sat_v3(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint8_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint8_v4(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint8_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint8_sat_v4(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint8_sat_v4_impl(dst, src, mask);
}

// ==========asc_duplicate_scalar(uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst, int8_t value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e5m2_t& dst, fp8_e5m2_t value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_uint16_t& dst, uint16_t value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_int16_t& dst, int16_t value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_half& dst, half value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_bfloat16_t& dst, bfloat16_t value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_uint32_t& dst, uint32_t value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_int32_t& dst, int32_t value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

__simd_callee__ inline void asc_duplicate_scalar(vector_float& dst, float value, vector_bool mask)
{
    asc_duplicate_scalar_impl(dst, value, mask);
}

// ==========asc_duplicate(uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_duplicate(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

__simd_callee__ inline void asc_duplicate(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_duplicate_impl(dst, src, mask);
}

// ==========asc_abs_sub(half/float)==========
__simd_callee__ inline void asc_abs_sub(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_abs_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_abs_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_abs_sub_impl(dst, src0, src1, mask);
}

// ==========asc_min(uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_min(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_min_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_min(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_min_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_min(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_min_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_min(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_min_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_min(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_min_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_min(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_min_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_min(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_min_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_min(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_min_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_min(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_min_impl(dst, src0, src1, mask);
}

// ==========asc_neg(int8_t/int16_t/half/int32_t/float)==========
__simd_callee__ inline void asc_neg(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_neg_impl(dst, src, mask);
}

__simd_callee__ inline void asc_neg(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_neg_impl(dst, src, mask);
}

__simd_callee__ inline void asc_neg(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_neg_impl(dst, src, mask);
}

__simd_callee__ inline void asc_neg(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_neg_impl(dst, src, mask);
}

__simd_callee__ inline void asc_neg(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_neg_impl(dst, src, mask);
}

// ==========asc_ge(uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_ge_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_ge_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_ge_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_ge_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_ge_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_ge_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_ge_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_ge_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ge(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_ge_impl(dst, src0, src1, mask);
}

// ==========asc_ge_scalar(uint8_t/int8_t/uint16_t/int16_t/half/bfloat16_t/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    asc_ge_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_ge_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    asc_ge_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    asc_ge_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    asc_ge_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    asc_ge_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask)
{
    asc_ge_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    asc_ge_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ge_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask)
{
    asc_ge_scalar_impl(dst, src, value, mask);
}

// ==========asc_reduce_max(uint16_t/int16_t/half/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_reduce_max(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_reduce_max_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_reduce_max_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_reduce_max_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_reduce_max_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_reduce_max_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_reduce_max_impl(dst, src, mask);
}

// ==========asc_reduce_min(uint16_t/int16_t/half/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_reduce_min(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_reduce_min_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_reduce_min_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_reduce_min_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_reduce_min_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_reduce_min_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_reduce_min_impl(dst, src, mask);
}

// ==========asc_reduce_min_datablock(uint16_t/int16_t/half/uint32_t/int32_t/float)==========
__simd_callee__ inline void asc_reduce_min_datablock(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_reduce_min_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_reduce_min_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min_datablock(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_reduce_min_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_reduce_min_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min_datablock(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_reduce_min_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_min_datablock(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_reduce_min_datablock_impl(dst, src, mask);
}

// ==========asc_axpy(half/float)==========
__simd_callee__ inline void asc_axpy(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    asc_axpy_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_axpy(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    asc_axpy_impl(dst, src, value, mask);
}

// ==========asc_abs(int8_t/int16_t/int32_t/half/float)==========
__simd_callee__ inline void asc_abs(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_abs_impl(dst, src, mask);
}

__simd_callee__ inline void asc_abs(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_abs_impl(dst, src, mask);
}

__simd_callee__ inline void asc_abs(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_abs_impl(dst, src, mask);
}

__simd_callee__ inline void asc_abs(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_abs_impl(dst, src, mask);
}

__simd_callee__ inline void asc_abs(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_abs_impl(dst, src, mask);
}

// ==========asc_le(uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/half/float/bfloat16_t)==========
__simd_callee__ inline void asc_le(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_le_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_le(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_le_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_le(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_le_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_le(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_le_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_le(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_le_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_le(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_le_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_le(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_le_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_le(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_le_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_le(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_le_impl(dst, src0, src1, mask);
}

// ==========asc_le_scalar(uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/half/float/bfloat16_t)==========
__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    asc_le_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    asc_le_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    asc_le_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_le_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    asc_le_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    asc_le_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask)
{
    asc_le_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask)
{
    asc_le_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_le_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    asc_le_scalar_impl(dst, src, value, mask);
}

// ==========asc_squeeze(uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/half/float/bfloat16_t/hifloat8_t/fp8_e4m3fn_t/fp8_e5m2_t/fp8_e8m0_t)==========
__simd_callee__ inline void asc_squeeze(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
{
    asc_squeeze_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze_v2(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_squeeze_v2(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

__simd_callee__ inline void asc_squeeze_v2(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
{
    asc_squeeze_v2_impl(dst, src, mask); 
}

// ==========asc_intlv(uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/half/float/fp8_e8m0_t/fp8_e5m2_t/fp8_e4m3fn_t/bfloat16_t)==========
__simd_callee__ inline void asc_intlv_b8(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    asc_intlv_b8_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv_b16(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    asc_intlv_b16_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv_b32(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    asc_intlv_b32_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_uint8_t& dst0, vector_uint8_t& dst1, vector_uint8_t src0, vector_uint8_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_int8_t& dst0, vector_int8_t& dst1, vector_int8_t src0, vector_int8_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_uint16_t& dst0, vector_uint16_t& dst1, vector_uint16_t src0, vector_uint16_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_int16_t& dst0, vector_int16_t& dst1, vector_int16_t src0, vector_int16_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_half& dst0, vector_half& dst1, vector_half src0, vector_half src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_float& dst0, vector_float& dst1, vector_float src0, vector_float src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_intlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    asc_intlv_impl(dst0, dst1, src0, src1);
}

// ==========asc_unsqueeze(uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t)==========
__simd_callee__ inline void asc_unsqueeze(vector_uint8_t& dst, vector_bool mask)
{
    asc_unsqueeze_impl(dst, mask);
}

__simd_callee__ inline void asc_unsqueeze(vector_int8_t& dst, vector_bool mask)
{
    asc_unsqueeze_impl(dst, mask);
}

__simd_callee__ inline void asc_unsqueeze(vector_uint16_t& dst, vector_bool mask)
{
    asc_unsqueeze_impl(dst, mask);
}

__simd_callee__ inline void asc_unsqueeze(vector_int16_t& dst, vector_bool mask)
{
    asc_unsqueeze_impl(dst, mask);
}

__simd_callee__ inline void asc_unsqueeze(vector_uint32_t& dst, vector_bool mask)
{
    asc_unsqueeze_impl(dst, mask);
}

__simd_callee__ inline void asc_unsqueeze(vector_int32_t& dst, vector_bool mask)
{
    asc_unsqueeze_impl(dst, mask);
}

// ==========asc_arange(int8_t/int16_t/int32_t/half/float)==========
__simd_callee__ inline void asc_arange(vector_int8_t& dst, int8_t value)
{
    asc_arange_impl(dst, value);
}

__simd_callee__ inline void asc_arange(vector_int16_t& dst, int16_t value)
{
    asc_arange_impl(dst, value);
}

__simd_callee__ inline void asc_arange(vector_int32_t& dst, int32_t value)
{
    asc_arange_impl(dst, value);
}

__simd_callee__ inline void asc_arange(vector_half& dst, half value)
{
    asc_arange_impl(dst, value);
}

__simd_callee__ inline void asc_arange(vector_float& dst, float value)
{
    asc_arange_impl(dst, value);
}

__simd_callee__ inline void asc_arange_descend(vector_int8_t& dst, int8_t value)
{
    asc_arange_descend_impl(dst, value);
}

__simd_callee__ inline void asc_arange_descend(vector_int16_t& dst, int16_t value)
{
    asc_arange_descend_impl(dst, value);
}

__simd_callee__ inline void asc_arange_descend(vector_int32_t& dst, int32_t value)
{
    asc_arange_descend_impl(dst, value);
}

__simd_callee__ inline void asc_arange_descend(vector_half& dst, half value)
{
    asc_arange_descend_impl(dst, value);
}

__simd_callee__ inline void asc_arange_descend(vector_float& dst, float value)
{
    asc_arange_descend_impl(dst, value);
}

// ==========asc_cumulative_histogram/asc_frequency_histogram==========
__simd_callee__ inline void asc_cumulative_histogram_bin0(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_cumulative_histogram_bin0_impl(dst, src, mask);
}

__simd_callee__ inline void asc_cumulative_histogram_bin1(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_cumulative_histogram_bin1_impl(dst, src, mask);
}

__simd_callee__ inline void asc_frequency_histogram_bin0(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_frequency_histogram_bin0_impl(dst, src, mask);
}

__simd_callee__ inline void asc_frequency_histogram_bin1(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_frequency_histogram_bin1_impl(dst, src, mask);
}

// ==========asc_update_mask==========
__simd_callee__ inline vector_bool asc_update_mask_b8(uint32_t& value)
{
    return asc_update_mask_b8_impl(value);
}

__simd_callee__ inline vector_bool asc_update_mask_b16(uint32_t& value)
{
    return asc_update_mask_b16_impl(value);
}

__simd_callee__ inline vector_bool asc_update_mask_b32(uint32_t& value)
{
    return asc_update_mask_b32_impl(value);
}

// ==========asc_half2int8(rd/ru/rz/rn/rna)==========
__simd_callee__ inline void asc_half2int8_rd(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rd_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rd_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rd_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rd_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_ru(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_ru_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_ru_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_ru_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_ru_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rz(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rz_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rz_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rz_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rz_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rn(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rn_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rn_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rn_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rn_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rna(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rna_sat(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rna_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int8_rna_sat_v2(vector_int8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int8_rna_sat_v2_impl(dst, src, mask);
}

// ==========asc_half2hif8(rh/rna)==========
__simd_callee__ inline void asc_half2hif8_rh(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2hif8_rh_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2hif8_rh_sat(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2hif8_rh_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2hif8_rh_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2hif8_rh_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2hif8_rh_sat_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2hif8_rh_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2hif8_rna(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2hif8_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2hif8_rna_sat(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2hif8_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2hif8_rna_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2hif8_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2hif8_rna_sat_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
{
    asc_half2hif8_rna_sat_v2_impl(dst, src, mask);
}

// ==========asc_bfloat162float==========
__simd_callee__ inline void asc_bfloat162float(vector_float& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162float_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162float_v2(vector_float& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162float_v2_impl(dst, src, mask);
}

// ==========asc_int4x22bfloat16==========
__simd_callee__ inline void asc_int4x22bfloat16(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22bfloat16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int4x22bfloat16_v2(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22bfloat16_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int4x22bfloat16_v3(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22bfloat16_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int4x22bfloat16_v4(vector_bfloat16_t& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22bfloat16_v4_impl(dst, src, mask);
}

// ==========asc_int162uint32==========
__simd_callee__ inline void asc_int162uint32(vector_uint32_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162uint32_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162uint32_v2(vector_uint32_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162uint32_v2_impl(dst, src, mask);
}

// ==========asc_int322float(rd/ru/rz/rn/rna)==========
__simd_callee__ inline void asc_int322float_rd(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322float_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322float_ru(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322float_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322float_rz(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322float_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322float_rn(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322float_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322float_rna(vector_float& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322float_rna_impl(dst, src, mask);
}

// ==========asc_uint162uint8==========
__simd_callee__ inline void asc_uint162uint8(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_uint162uint8_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint162uint8_sat(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_uint162uint8_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint162uint8_v2(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_uint162uint8_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint162uint8_sat_v2(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_uint162uint8_sat_v2_impl(dst, src, mask);
}

// ==========asc_bfloat162int32_rn==========
__simd_callee__ inline void asc_bfloat162int32_rn(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rn_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rn_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rn_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rn_sat_v2_impl(dst, src, mask);
}

// ==========asc_bfloat162int32_rna==========
__simd_callee__ inline void asc_bfloat162int32_rna(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rna_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rna_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rna_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rna_sat_v2_impl(dst, src, mask);
}

// ==========asc_bfloat162int32_rd==========
__simd_callee__ inline void asc_bfloat162int32_rd(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rd_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rd_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rd_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rd_sat_v2_impl(dst, src, mask);
}

// ==========asc_bfloat162int32_ru==========
__simd_callee__ inline void asc_bfloat162int32_ru(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_ru_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_ru_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_ru_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_ru_sat_v2_impl(dst, src, mask);
}

// ==========asc_bfloat162int32_rz==========
__simd_callee__ inline void asc_bfloat162int32_rz(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rz_sat(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rz_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162int32_rz_sat_v2(vector_int32_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162int32_rz_sat_v2_impl(dst, src, mask);
}

// ==========asc_half2int16_rn==========
__simd_callee__ inline void asc_half2int16_rn(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int16_rn_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_rn_sat_impl(dst, src, mask);
}

// ==========asc_half2int16_rna==========
__simd_callee__ inline void asc_half2int16_rna(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int16_rna_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_rna_sat_impl(dst, src, mask);
}

// ==========asc_half2int16_rd==========
__simd_callee__ inline void asc_half2int16_rd(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int16_rd_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_rd_sat_impl(dst, src, mask);
}

// ==========asc_half2int16_ru==========
__simd_callee__ inline void asc_half2int16_ru(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int16_ru_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_ru_sat_impl(dst, src, mask);
}

// ==========asc_half2int16_rz==========
__simd_callee__ inline void asc_half2int16_rz(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int16_rz_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int16_rz_sat_impl(dst, src, mask);
}

// ==========asc_int642float_rn==========
__simd_callee__ inline void asc_int642float_rn(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int642float_rn_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_rn_v2_impl(dst, src, mask);
}

// ==========asc_int642float_rna==========
__simd_callee__ inline void asc_int642float_rna(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int642float_rna_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_rna_v2_impl(dst, src, mask);
}

// ==========asc_int642float_rd==========
__simd_callee__ inline void asc_int642float_rd(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int642float_rd_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_rd_v2_impl(dst, src, mask);
}

// ==========asc_int642float_ru==========
__simd_callee__ inline void asc_int642float_ru(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int642float_ru_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_ru_v2_impl(dst, src, mask);
}

// ==========asc_int642float_rz==========
__simd_callee__ inline void asc_int642float_rz(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int642float_rz_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642float_rz_v2_impl(dst, src, mask);
}

// ==========asc_int82half==========
__simd_callee__ inline void asc_int82half(vector_half& dst, vector_int8_t src, vector_bool mask)
{
    asc_int82half_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int82half_v2(vector_half& dst, vector_int8_t src, vector_bool mask)
{
    asc_int82half_v2_impl(dst, src, mask);
}

// ==========asc_int162int32==========
__simd_callee__ inline void asc_int162int32(vector_int32_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162int32_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162int32_v2(vector_int32_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162int32_v2_impl(dst, src, mask);
}

// ==========asc_uint322uint8==========
__simd_callee__ inline void asc_uint322uint8(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint8_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint8_sat(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint8_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint8_v2(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint8_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint8_sat_v2(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint8_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint8_v3(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint8_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint8_sat_v3(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint8_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint8_v4(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint8_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint8_sat_v4(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint8_sat_v4_impl(dst, src, mask);
}

// ==========asc_hif82half==========
__simd_callee__ inline void asc_hif82half(vector_half& dst, vector_hifloat8_t src, vector_bool mask)
{
    asc_hif82half_impl(dst, src, mask);
}

__simd_callee__ inline void asc_hif82half_v2(vector_half& dst, vector_hifloat8_t src, vector_bool mask)
{
    asc_hif82half_v2_impl(dst, src, mask);
}

// rd - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_rd(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rd_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rd_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rd_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rd_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rd_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rd_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rd_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rd_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rd_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rd_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rd_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rd_sat_v4_impl(dst, src, mask);
}

// ru - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_ru(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_ru_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_ru_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_ru_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_ru_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_ru_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_ru_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_ru_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_ru_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_ru_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_ru_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_ru_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_ru_sat_v4_impl(dst, src, mask);
}

// rz - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_rz(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rz_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rz_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rz_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rd_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rz_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rz_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rz_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rz_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rz_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rz_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rz_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rz_sat_v4_impl(dst, src, mask);
}

// rn - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_rn(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rn_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rn_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rn_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rn_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rn_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rn_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rn_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rn_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rn_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rn_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rn_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rn_sat_v4_impl(dst, src, mask);
}

// rna - sat:dis/en - v1 v2 v3 v4
__simd_callee__ inline void asc_half2int4x2_rna(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rna_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rna_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rna_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rna_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rna_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rna_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rna_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rna_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rna_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rna_sat_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int4x2_rna_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int4x2_rna_sat_v4_impl(dst, src, mask);
}

// ==========asc_uint82half==========
__simd_callee__ inline void asc_uint82half(vector_half& dst, vector_uint8_t src, vector_bool mask)
{
    asc_uint82half_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint82half_v2(vector_half& dst, vector_uint8_t src, vector_bool mask)
{
    asc_uint82half_v2_impl(dst, src, mask);
}

// ==========asc_uint162uint32==========
__simd_callee__ inline void asc_uint162uint32(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_uint162uint32_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint162uint32_v2(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_uint162uint32_v2_impl(dst, src, mask);
}

// ==========asc_hif82float==========
__simd_callee__ inline void asc_hif82float(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
{
    asc_hif82float_impl(dst, src, mask);
}

__simd_callee__ inline void asc_hif82float_v2(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
{
    asc_hif82float_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_hif82float_v3(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
{
    asc_hif82float_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_hif82float_v4(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
{
    asc_hif82float_v4_impl(dst, src, mask);
}

//===============asc_max===============
__simd_callee__ inline void asc_max(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_max_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_max(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_max_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_max(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_max_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_max(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_max_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_max(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_max_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_max(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_max_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_max(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_max_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_max(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_max_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_max(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_max_impl(dst, src0, src1, mask);
}

//================asc_min_scalar=================
__simd_callee__ inline void asc_min_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    asc_min_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_min_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    asc_min_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_min_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_min_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_min_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    asc_min_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_min_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    asc_min_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_min_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    asc_min_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_min_scalar(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    asc_min_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_min_scalar(vector_half& dst, vector_f16 src, half value, vector_bool mask)
{
    asc_min_scalar_impl(dst, src, value, mask);
}

//==================asc_reduce_max_datablock====================
__simd_callee__ inline void asc_reduce_max_datablock(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_reduce_max_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max_datablock(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_reduce_max_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max_datablock(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_reduce_max_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max_datablock(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_reduce_max_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_reduce_max_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_max_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_reduce_max_datablock_impl(dst, src, mask);
}

//================asc_leakyrelu===================
__simd_callee__ inline void asc_leakyrelu(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    asc_leakyrelu_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_leakyrelu(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    asc_leakyrelu_impl(dst, src, value, mask);
}

//=================asc_gt=====================
__simd_callee__ inline void asc_gt(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_gt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_gt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_gt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_gt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_gt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_gt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_gt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_gt_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_gt(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_gt_impl(dst, src0, src1, mask);
}

//=================asc_gt_scalar=====================
__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    asc_gt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    asc_gt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    asc_gt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    asc_gt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_gt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    asc_gt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    asc_gt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask)
{
    asc_gt_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_gt_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask)
{
    asc_gt_scalar_impl(dst, src, value, mask);
}

//=================aschal2uint8===============
__simd_callee__ inline void asc_half2uint8_rd(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rd_sat(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rd_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rd_sat_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rd_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rn(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rn_sat(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rn_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rn_sat_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rn_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rna(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rna_sat(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rna_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rna_sat_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rna_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_ru(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_ru_sat(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_ru_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_ru_sat_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_ru_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rz(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rz_sat(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rz_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2uint8_rz_sat_v2(vector_u8& dst, vector_f16 src, vector_bool mask)
{
    asc_half2uint8_rz_sat_v2_impl(dst, src, mask);
}

//===================asc_int162half_impl=================
__simd_callee__ inline void asc_int162half_rd(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162half_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162half_rn(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162half_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162half_rna(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162half_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162half_ru(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162half_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162half_rz(vector_half& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162half_rz_impl(dst, src, mask);
}

//================asc_int4x22half==============
__simd_callee__ inline void asc_int4x22half(vector_half& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22half_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int4x22half_v2(vector_half& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22half_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int4x22half_v3(vector_half& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22half_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int4x22half_v4(vector_half& dst, vector_int4x2_t src, vector_bool mask)
{
    asc_int4x22half_v4_impl(dst, src, mask);
}

//===============asc_int82int16==============
__simd_callee__ inline void asc_int82int16(vector_int16_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_int82int16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int82int16_v2(vector_int16_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_int82int16_v2_impl(dst, src, mask);
}

//===============asc_int162uint8===============
__simd_callee__ inline void asc_int162uint8(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162uint8_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162uint8_sat(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162uint8_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162uint8_v2(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162uint8_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int162uint8_sat_v2(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_int162uint8_sat_v2_impl(dst, src, mask);
}
// ==========asc_sqrt(half/float)==========
__simd_callee__ inline void asc_sqrt(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_sqrt_impl(dst, src, mask);
}

__simd_callee__ inline void asc_sqrt(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_sqrt_impl(dst, src, mask);
}

// ==========asc_or(int8_t/uint8_t/int16_t/uint16_t/half/int32_t/uint32_t/float/bool)==========
__simd_callee__ inline void asc_or(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_or_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_or(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_or_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_or(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_or_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_or(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_or_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_or(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_or_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_or(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_or_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_or(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_or_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_or(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_or_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_or(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
{
    asc_or_impl(dst, src0, src1, mask);
}

// ==========asc_mul(int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float)==========
__simd_callee__ inline void asc_mul(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_mul_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_mul(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_mul_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_mul(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_mul_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_mul(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_mul_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_mul(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_mul_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_mul(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_mul_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_mul(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_mul_impl(dst, src0, src1, mask);
}

// ==========asc_mul_scalar(int16_t/uint16_t/half/int32_t/uint32_t/float)==========
__simd_callee__ inline void asc_mul_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_mul_salar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_mul_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    asc_mul_salar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_mul_scalar(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    asc_mul_salar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_mul_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    asc_mul_salar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_mul_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    asc_mul_salar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_mul_scalar(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    asc_mul_salar_impl(dst, src, value, mask);
}

// ==========asc_eq(vcmp int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float)==========
__simd_callee__ inline void asc_eq(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_eq_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_eq_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_eq_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_eq_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_eq_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
{
    asc_eq_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_eq_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_eq_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_eq(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_eq_impl(dst, src0, src1, mask);
}

// ==========asc_eq_scalar(int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float)==========
__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_int8_t src0, int8_t value, vector_bool mask)
{
    asc_eq_scalar_impl(dst, src0, value, mask);
}

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_uint8_t src0, uint8_t value, vector_bool mask)
{
    asc_eq_scalar_impl(dst, src0, value, mask);
}

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_int16_t src0, int16_t value, vector_bool mask)
{
    asc_eq_scalar_impl(dst, src0, value, mask);
}

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_uint16_t src0, uint16_t value, vector_bool mask)
{
    asc_eq_scalar_impl(dst, src0, value, mask);
}

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_half src0, half value, vector_bool mask)
{
    asc_eq_scalar_impl(dst, src0, value, mask);
}

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_bfloat16_t src0, bfloat16_t value, vector_bool mask)
{
    asc_eq_scalar_impl(dst, src0, value, mask);
}

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_int32_t src0, int32_t value, vector_bool mask)
{
    asc_eq_scalar_impl(dst, src0, value, mask);
}

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_uint32_t src0, uint32_t value, vector_bool mask)
{
    asc_eq_scalar_impl(dst, src0, value, mask);
}

__simd_callee__ inline void asc_eq_scalar(vector_bool& dst, vector_float src0, float value, vector_bool mask)
{
    asc_eq_scalar_impl(dst, src0, value, mask);
}

// ==========asc_float2int16_rd/ru/rz/rn/rna==========
__simd_callee__ inline void asc_float2int16_rd(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rd_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rd_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rd_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rd_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rn(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rn_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rn_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rn_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rn_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rna(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rna_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rna_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rna_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rna_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_ru(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_ru_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_ru_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_ru_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_ru_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rz(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rz_sat(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rz_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int16_rz_sat_v2(vector_int16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int16_rz_sat_v2_impl(dst, src, mask);
}

// ==========asc_float2int32_rd/ru/rz/rn/rna==========
__simd_callee__ inline void asc_float2int32_rd(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int32_rd_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int32_ru(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int32_ru_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int32_rz(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int32_rz_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int32_rn(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int32_rn_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int32_rna(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int32_rna_sat(vector_int32_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int32_rna_sat_impl(dst, src, mask);
}

// ==========asc_float2int64_rd/ru/rz/rn/rna==========
__simd_callee__ inline void asc_float2int64_rd(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rd_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rd_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rd_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rd_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rn(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rn_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rn_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rn_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rn_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rna(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rna_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rna_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rna_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rna_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_ru(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_ru_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_ru_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_ru_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_ru_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rz(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rz_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rz_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2int64_rz_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
{
    asc_float2int64_rz_sat_v2_impl(dst, src, mask);
}

// ==========asc_bfloat162e2m1x2_rd/rn/rna/ru/rz==========
__simd_callee__ inline void asc_bfloat162e2m1x2_rd(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rd_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rd_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rn_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rn_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rna_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rna_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_ru_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_ru_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rz_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e2m1x2_rz_v4_impl(dst, src, mask);
}

// ==========asc_float2hif8_rh/rna==========
__simd_callee__ inline void asc_float2hif8_rh_sat(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rh_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rh(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rh_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rh_sat_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rh_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rh_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rh_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rh_sat_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rh_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rh_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rh_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rh_sat_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rh_sat_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rh_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rh_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rna_sat(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rna(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rna_sat_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rna_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rna_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rna_sat_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rna_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rna_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rna_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rna_sat_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rna_sat_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2hif8_rna_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
{
    asc_float2hif8_rna_v4_impl(dst, src, mask);
}

// ==========asc_uint82uint32==========
__simd_callee__ inline void asc_uint82uint32(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_uint82uint32_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint82uint32_v2(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_uint82uint32_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint82uint32_v3(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_uint82uint32_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint82uint32_v4(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_uint82uint32_v4_impl(dst, src, mask);
}

// ==========asc_uint322uint16==========
__simd_callee__ inline void asc_uint322uint16_sat(vector_uint16_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint16_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint16(vector_uint16_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint16_sat_v2(vector_uint16_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint16_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint322uint16_v2(vector_uint16_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_uint322uint16_v2_impl(dst, src, mask);
}

// ==========asc_ceil==========
__simd_callee__ inline void asc_ceil(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_ceil_impl(dst, src, mask);
}

__simd_callee__ inline void asc_ceil(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_ceil_impl(dst, src, mask);
}

__simd_callee__ inline void asc_ceil(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_ceil_impl(dst, src, mask);
}

// ==========asc_floor==========
__simd_callee__ inline void asc_floor(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_floor_impl(dst, src, mask);
}

__simd_callee__ inline void asc_floor(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_floor_impl(dst, src, mask);
}

__simd_callee__ inline void asc_floor(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_floor_impl(dst, src, mask);
}

// ==========asc_rint==========
__simd_callee__ inline void asc_rint(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_rint_impl(dst, src, mask);
}

__simd_callee__ inline void asc_rint(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_rint_impl(dst, src, mask);
}

__simd_callee__ inline void asc_rint(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_rint_impl(dst, src, mask);
}

// ==========asc_round==========
__simd_callee__ inline void asc_round(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_round_impl(dst, src, mask);
}

__simd_callee__ inline void asc_round(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_round_impl(dst, src, mask);
}

__simd_callee__ inline void asc_round(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_round_impl(dst, src, mask);
}

// ==========asc_trunc==========
__simd_callee__ inline void asc_trunc(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_trunc_impl(dst, src, mask);
}

__simd_callee__ inline void asc_trunc(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_trunc_impl(dst, src, mask);
}

__simd_callee__ inline void asc_trunc(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_trunc_impl(dst, src, mask);
}

// ==========asc_e2m1x22bfloat16==========
__simd_callee__ inline void asc_e2m1x22bfloat16(vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask)
{
    asc_e2m1x22bfloat16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e2m1x22bfloat16_v2(vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask)
{
    asc_e2m1x22bfloat16_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e2m1x22bfloat16_v3(vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask)
{
    asc_e2m1x22bfloat16_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e2m1x22bfloat16_v4(vector_bfloat16_t& dst, vector_fp4x2_e2m1_t src, vector_bool mask)
{
    asc_e2m1x22bfloat16_v4_impl(dst, src, mask);
}

// ==========asc_muls==========
__simd_callee__ inline void asc_muls(vector_half& dst, vector_float src, float value, vector_bool mask)
{
    asc_muls_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_muls_v2(vector_half& dst, vector_float src, float value, vector_bool mask)
{
    asc_muls_v2_impl(dst, src, value, mask);
}

// ==========asc_exp(half/float)==========
__simd_callee__ inline void asc_exp(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_exp_impl(dst, src, mask);
}

__simd_callee__ inline void asc_exp(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_exp_impl(dst, src, mask);
}

// ==========asc_add_scalar(int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float)==========
__simd_callee__ inline void asc_add_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    asc_add_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_add_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    asc_add_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_add_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_add_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_add_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    asc_add_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_add_scalar(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    asc_add_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_add_scalar(vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    asc_add_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_add_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    asc_add_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_add_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    asc_add_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_add_scalar(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    asc_add_scalar_impl(dst, src, value, mask);
}

// ==========asc_float2bfloat16==========
__simd_callee__ inline void asc_float2bfloat16_rd(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rd_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rd_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rd_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rd_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rd_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rn(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rn_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rn_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rn_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rn_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rna(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rna_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rna_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rna_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rna_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rna_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_ru(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_ru_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_ru_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_ru_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_ru_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_ru_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rz(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rz_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rz_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rz_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2bfloat16_rz_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
{
    asc_float2bfloat16_rz_sat_v2_impl(dst, src, mask);
}

// ==========asc_float2e5m2==========
__simd_callee__ inline void asc_float2e5m2_rn(vector_f8e5m2& dst, vector_float src, vector_bool mask)
{
    asc_float2e5m2_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e5m2_rn_v2(vector_f8e5m2& dst, vector_float src, vector_bool mask)
{
    asc_float2e5m2_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e5m2_rn_v3(vector_f8e5m2& dst, vector_float src, vector_bool mask)
{
    asc_float2e5m2_rn_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e5m2_rn_v4(vector_f8e5m2& dst, vector_float src, vector_bool mask)
{
    asc_float2e5m2_rn_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e5m2_rn_sat(vector_f8e5m2& dst, vector_float src, vector_bool mask)
{
    asc_float2e5m2_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e5m2_rn_sat_v2(vector_f8e5m2& dst, vector_float src, vector_bool mask)
{
    asc_float2e5m2_rn_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e5m2_rn_sat_v3(vector_f8e5m2& dst, vector_float src, vector_bool mask)
{
    asc_float2e5m2_rn_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e5m2_rn_sat_v4(vector_f8e5m2& dst, vector_float src, vector_bool mask)
{
    asc_float2e5m2_rn_sat_v4_impl(dst, src, mask);
}

//================asc_half2float==============
__simd_callee__ inline void asc_half2float(vector_float& dst, vector_half src, vector_bool mask)
{
    asc_half2float_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2float_v2(vector_float& dst, vector_half src, vector_bool mask)
{
    asc_half2float_v2_impl(dst, src, mask);
}

// ==========asc_half2int32==========
__simd_callee__ inline void asc_half2int32_rd(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int32_rd_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int32_rn(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int32_rn_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int32_rna(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int32_rna_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int32_ru(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int32_ru_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int32_rz(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_half2int32_rz_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
{
    asc_half2int32_rz_v2_impl(dst, src, mask);
}

// ==========asc_e5m22float==========
__simd_callee__ inline void asc_e5m22float(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    asc_e5m22float_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e5m22float_v2(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    asc_e5m22float_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e5m22float_v3(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    asc_e5m22float_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e5m22float_v4(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
{
    asc_e5m22float_v4_impl(dst, src, mask);
}

// ==========asc_int322int64==========
__simd_callee__ inline void asc_int322int64(vector_int64_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322int64_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322int64_v2(vector_int64_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322int64_v2_impl(dst, src, mask);
}

// ==========asc_int322uint16==========
__simd_callee__ inline void asc_int322uint16(vector_uint16_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint16_sat(vector_uint16_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint16_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint16_v2(vector_uint16_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint16_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int322uint16_sat_v2(vector_uint16_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_int322uint16_sat_v2_impl(dst, src, mask);
}

// ==========asc_int642int32==========
__simd_callee__ inline void asc_int642int32(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642int32_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int642int32_sat(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642int32_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int642int32_v2(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642int32_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int642int32_sat_v2(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
{
    asc_int642int32_sat_v2_impl(dst, src, mask);
}

// ==========asc_uint82uint16==========
__simd_callee__ inline void asc_uint82uint16(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_uint82uint16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_uint82uint16_v2(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_uint82uint16_v2_impl(dst, src, mask);
}

// ==========asc_pack(bool/uint16_t/int16_t/uint32_t/int32_t)==========
__simd_callee__ inline void asc_pack(vector_uint8_t& dst, vector_uint16_t src)
{
    asc_pack_impl(dst, src);
}

__simd_callee__ inline void asc_pack(vector_uint8_t& dst, vector_int16_t src)
{
    asc_pack_impl(dst, src);
}

__simd_callee__ inline void asc_pack(vector_uint16_t& dst, vector_uint32_t src)
{
    asc_pack_impl(dst, src);
}

__simd_callee__ inline void asc_pack(vector_uint16_t& dst, vector_int32_t src)
{
    asc_pack_impl(dst, src);
}

__simd_callee__ inline void asc_pack(vector_bool& dst, vector_bool src)
{
    asc_pack_impl(dst, src);
}

__simd_callee__ inline void asc_pack_v2(vector_uint8_t& dst, vector_uint16_t src)
{
    asc_pack_v2_impl(dst, src);
}

__simd_callee__ inline void asc_pack_v2(vector_uint8_t& dst, vector_int16_t src)
{
    asc_pack_v2_impl(dst, src);
}

__simd_callee__ inline void asc_pack_v2(vector_uint16_t& dst, vector_uint32_t src)
{
    asc_pack_v2_impl(dst, src);
}

__simd_callee__ inline void asc_pack_v2(vector_uint16_t& dst, vector_int32_t src)
{
    asc_pack_v2_impl(dst, src);
}

__simd_callee__ inline void asc_pack_v2(vector_bool& dst, vector_bool src)
{
    asc_pack_v2_impl(dst, src);
}

// ==========asc_exp_sub(half/float)==========
__simd_callee__ inline void asc_exp_sub(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_exp_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_exp_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_exp_sub_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_exp_sub_v2_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_exp_sub_v2_impl(dst, src0, src1, mask);
}

// ==========asc_ln(half/float)==========
__simd_callee__ inline void asc_ln(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_ln_impl(dst, src, mask);
}

__simd_callee__ inline void asc_ln(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_ln_impl(dst, src, mask);
}

// ==========asc_div(half/float)==========
__simd_callee__ inline void asc_div(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_div_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_div(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_div_impl(dst, src0, src1, mask);
}

// ==========asc_max_scalar(int8_t/int16_t/int32_t/uint8_t/uint16_t/uint32_t/half/float/bfloat16_t)==========
__simd_callee__ inline void asc_max_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    asc_max_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_max_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_max_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_max_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    asc_max_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_max_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    asc_max_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_max_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    asc_max_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_max_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    asc_max_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_max_scalar(vector_half& dst, vector_half src, half value, vector_bool mask)
{
    asc_max_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_max_scalar(vector_float& dst, vector_float src, float value, vector_bool mask)
{
    asc_max_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_max_scalar(vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    asc_max_scalar_impl(dst, src, value, mask);
}

// ==========asc_ne(uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/half/float)==========
__simd_callee__ inline void asc_ne(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_ne_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_ne_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_ne_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_ne_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_ne_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_ne_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_ne_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_ne(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_ne_impl(dst, src0, src1, mask);
}

// ==========asc_ne_scalar(uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/half/float/bfloat16_t)==========
__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
{
    asc_ne_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask)
{
    asc_ne_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
{
    asc_ne_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask)
{
    asc_ne_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
{
    asc_ne_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask)
{
    asc_ne_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask)
{
    asc_ne_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask)
{
    asc_ne_scalar_impl(dst, src, value, mask);
}

__simd_callee__ inline void asc_ne_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
{
    asc_ne_scalar_impl(dst, src, value, mask);
}

// ==========asc_deintlv(int32_t/uint32_t/int16_t/uint16_t/int8_t/uint8_t/fp8_e4m3fn/fp8_e5m2/fp8_e8m0/bfloat16_t/float/half)==========
__simd_callee__ inline void asc_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, vector_int16_t src0, vector_int16_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, vector_uint16_t src0, vector_uint16_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, vector_int8_t src0, vector_int8_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, vector_uint8_t src0, vector_uint8_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, vector_hifloat8_t src0, vector_hifloat8_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, vector_bfloat16_t src0, vector_bfloat16_t src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_float& dst0, vector_float& dst1, vector_float src0, vector_float src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv(vector_half& dst0, vector_half& dst1, vector_half src0, vector_half src1)
{
    asc_deintlv_impl(dst0, dst1, src0, src1);
}

// ==========asc_deintlv_b8/b16/b32(bool)==========
__simd_callee__ inline void asc_deintlv_b8(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    asc_deintlv_b8_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv_b16(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    asc_deintlv_b16_impl(dst0, dst1, src0, src1);
}

__simd_callee__ inline void asc_deintlv_b32(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
{
    asc_deintlv_b32_impl(dst0, dst1, src0, src1);
}

// ==========asc_reduce_sum(int16_t/int32_t/uint16_t/uint32_t/half/float)==========
__simd_callee__ inline void asc_reduce_sum(vector_int32_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_reduce_sum_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_reduce_sum_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_reduce_sum_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_reduce_sum_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_reduce_sum_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_reduce_sum_impl(dst, src, mask);
}

// ==========asc_reduce_sum_datablock(half/float/uint16_t/int16_t/uint32_t/int32_t)==========
__simd_callee__ inline void asc_reduce_sum_datablock(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_reduce_sum_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum_datablock(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_reduce_sum_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum_datablock(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_reduce_sum_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum_datablock(vector_int32_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_reduce_sum_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum_datablock(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_reduce_sum_datablock_impl(dst, src, mask);
}

__simd_callee__ inline void asc_reduce_sum_datablock(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_reduce_sum_datablock_impl(dst, src, mask);
}

// ==========asc_unpack_lower/upper(uint8_t/int8_t/uint16_t/int16_t/bool)==========
__simd_callee__ inline void asc_unpack_upper(vector_uint16_t& dst, vector_uint8_t src)
{
    asc_unpack_upper_impl(dst, src);
}

__simd_callee__ inline void asc_unpack_lower(vector_uint16_t& dst, vector_uint8_t src)
{
    asc_unpack_lower_impl(dst, src);
}

__simd_callee__ inline void asc_unpack_upper(vector_int16_t& dst, vector_int8_t src)
{
    asc_unpack_upper_impl(dst, src);
}

__simd_callee__ inline void asc_unpack_lower(vector_int16_t& dst, vector_int8_t src)
{
    asc_unpack_lower_impl(dst, src);
}

__simd_callee__ inline void asc_unpack_upper(vector_uint32_t& dst, vector_uint16_t src)
{
    asc_unpack_upper_impl(dst, src);
}

__simd_callee__ inline void asc_unpack_lower(vector_uint32_t& dst, vector_uint16_t src)
{
    asc_unpack_lower_impl(dst, src);
}

__simd_callee__ inline void asc_unpack_upper(vector_int32_t& dst, vector_int16_t src)
{
    asc_unpack_upper_impl(dst, src);
}

__simd_callee__ inline void asc_unpack_lower(vector_int32_t& dst, vector_int16_t src)
{
    asc_unpack_lower_impl(dst, src);
}

__simd_callee__ inline void asc_unpack_upper(vector_bool& dst, vector_bool src)
{
    asc_unpack_upper_impl(dst, src);
}

__simd_callee__ inline void asc_unpack_lower(vector_bool& dst, vector_bool src)
{
    asc_unpack_lower_impl(dst, src);
}

// ==========asc_copy(u8/s8/half/u16/s16/float/u32/s32/bf16)=========
__simd_callee__ inline void asc_copy(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_half& dst, vector_half src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_float& dst, vector_float src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_bool src, vector_bool mask)
{
    asc_copy_impl(dst, src, mask);
}

__simd_callee__ inline void asc_copy(vector_bool& dst, vector_bool src) 
{ 
    asc_copy_impl(dst, src); 
}

// ==========asc_float2e4m3_rn=========
__simd_callee__ inline void asc_float2e4m3_rn_sat(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    asc_float2e4m3_rn_sat_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e4m3_rn(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    asc_float2e4m3_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e4m3_rn_sat_v2(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    asc_float2e4m3_rn_sat_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e4m3_rn_v2(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    asc_float2e4m3_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e4m3_rn_sat_v3(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    asc_float2e4m3_rn_sat_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e4m3_rn_v3(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    asc_float2e4m3_rn_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e4m3_rn_sat_v4(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    asc_float2e4m3_rn_sat_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_float2e4m3_rn_v4(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
{
    asc_float2e4m3_rn_v4_impl(dst, src, mask);
}

// ==========asc_bfloat162e1m2x2_rd/rn/rna/ru/rz==========
__simd_callee__ inline void asc_bfloat162e1m2x2_rd(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rd_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd_v2(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rd_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd_v3(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rd_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rd_v4(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rd_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rn_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn_v2(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rn_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn_v3(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rn_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rn_v4(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rn_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rna_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna_v2(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rna_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna_v3(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rna_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rna_v4(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rna_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_ru_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru_v2(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_ru_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru_v3(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_ru_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_ru_v4(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_ru_v4_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rz_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz_v2(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rz_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz_v3(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rz_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_bfloat162e1m2x2_rz_v4(vector_f4e1m2x2& dst, vector_bfloat16_t src, vector_bool mask)
{
    asc_bfloat162e1m2x2_rz_v4_impl(dst, src, mask);
}

// ==========asc_e1m2x22bfloat16==========
__simd_callee__ inline void asc_e1m2x22bfloat16(vector_bfloat16_t& dst, vector_f4e1m2x2 src, vector_bool mask)
{
    asc_e1m2x22bfloat16_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e1m2x22bfloat16_v2(vector_bfloat16_t& dst, vector_f4e1m2x2 src, vector_bool mask)
{
    asc_e1m2x22bfloat16_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e1m2x22bfloat16_v3(vector_bfloat16_t& dst, vector_f4e1m2x2 src, vector_bool mask)
{
    asc_e1m2x22bfloat16_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_e1m2x22bfloat16_v4(vector_bfloat16_t& dst, vector_f4e1m2x2 src, vector_bool mask)
{
    asc_e1m2x22bfloat16_v4_impl(dst, src, mask);
}

// ==========asc_int82int32==========
__simd_callee__ inline void asc_int82int32(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_int82int32_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int82int32_v2(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_int82int32_v2_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int82int32_v3(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_int82int32_v3_impl(dst, src, mask);
}

__simd_callee__ inline void asc_int82int32_v4(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
{
    asc_int82int32_v4_impl(dst, src, mask);
}

//=================asc_xor==================
__simd_callee__ inline void asc_xor(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_xor_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_xor(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_xor_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_xor(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
{
    asc_xor_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_xor(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
{
    asc_xor_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_xor(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
{
    asc_xor_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_xor(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
{
    asc_xor_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_xor(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
{
    asc_xor_impl(dst, src0, src1, mask);
}

//===================asc_prelu=====================
__simd_callee__ inline void asc_prelu(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    asc_prelu_impl(dst, src0, src1, mask);
}

__simd_callee__ inline void asc_prelu(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    asc_prelu_impl(dst, src0, src1, mask);
}

// ==========asc_mull(uint32_t/int32_t)==========
__simd_callee__ inline void asc_mull(vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
{
    asc_mull_impl(dst0, dst1, src0, src1, mask);
}

__simd_callee__ inline void asc_mull(vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
{
    asc_mull_impl(dst0, dst1, src0, src1, mask);
}

__simd_callee__ inline void asc_clear_ar_spr()
{
    asc_clear_ar_spr_impl();
}
#endif