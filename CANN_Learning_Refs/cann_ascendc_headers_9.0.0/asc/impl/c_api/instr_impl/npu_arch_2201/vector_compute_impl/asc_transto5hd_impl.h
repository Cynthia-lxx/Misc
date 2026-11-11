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
#warning "impl/c_api/instr_impl/npu_arch_2201/vector_compute_impl/asc_transto5hd_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."  
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS  
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC  
#endif    

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_2201_VECTOR_COMPUTE_ASC_TRANSTO5HD_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_2201_VECTOR_COMPUTE_ASC_TRANSTO5HD_IMPL_H

#include "instr_impl/npu_arch_2201/utils_impl/utils_impl.h"

#define asc_transto5hd_b8_impl(dst, src, repeat, dst_stride, src_stride, dst_high_half, src_high_half) scatter_vnchwconv_b8(dst, src, repeat, dst_stride, src_stride, dst_high_half, src_high_half)

#define asc_transto5hd_b16_impl(dst, src, repeat, dstStride, srcStride) scatter_vnchwconv_b16(dst, src, repeat, dstStride, srcStride)

#define asc_transto5hd_b32_impl(dst, src, repeat, dstStride, srcStride) scatter_vnchwconv_b32(dst, src, repeat, dstStride, srcStride)

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)  
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS  
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC  
#endif  

