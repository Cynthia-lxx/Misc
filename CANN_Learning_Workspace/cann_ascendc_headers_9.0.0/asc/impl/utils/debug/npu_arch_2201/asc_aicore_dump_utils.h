/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/*!
 * \file asc_aicore_dump_utils.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_NPU_ARCH_2201_ASC_AICORE_DUMP_UTILS_H
#define IMPL_UTILS_DEBUG_NPU_ARCH_2201_ASC_AICORE_DUMP_UTILS_H

#include "impl/utils/debug/asc_debug_utils.h"
namespace __asc_aicore {

__aicore__ inline void get_matrix_copy_param(uint32_t dumpSize, uint16_t& n, uint16_t& m, uint16_t& dstStrideDstD, uint16_t& srcStride)
{
    constexpr int32_t blockCube = 16;
    constexpr int32_t defaultOneBlockSize = 256;
    constexpr int32_t srcBurstLenSizeEle = 16;
    
    uint16_t align = (dumpSize % defaultOneBlockSize == 0) ? 0 : 1;
    uint16_t countBlks = align + dumpSize / defaultOneBlockSize;
    uint16_t burstLen = static_cast<uint16_t>(srcBurstLenSizeEle * srcBurstLenSizeEle * sizeof(float) / ASC_ONE_DATABLOCK_SIZE);
    n = countBlks * blockCube;
    m = (burstLen * ASC_ONE_DATABLOCK_SIZE / sizeof(float)) / blockCube;
    uint16_t howo = (burstLen * ASC_ONE_DATABLOCK_SIZE / sizeof(float)) / blockCube;
    srcStride = div_ceil(howo, blockCube) * blockCube;
    dstStrideDstD = burstLen;
}

template<typename T>
__aicore__ inline void mem_copy_cbuf_to_gm_impl(__gm__ T* dst, __cc__ T* src, const uint32_t& len)
{
#if defined(__DAV_CUBE__) 
    uint16_t nSize, mSize, dstStrideDstD, srcStride;
    get_matrix_copy_param(len, nSize, mSize, dstStrideDstD, srcStride);
    copy_matrix_cc_to_gm((__gm__ float*)dst, (__cc__ float *)src, 0, nSize, mSize, dstStrideDstD, srcStride, 0,
                         QuantMode_t::NoQuant, static_cast<uint8_t>(false), false, false);
#endif
}

template<typename T>
__aicore__ inline void mem_copy_l1buf_to_gm_impl(__gm__ T* dst, __cbuf__ T* src, const uint32_t& len)
{
#if defined(__DAV_CUBE__) 
    copy_cbuf_to_gm(reinterpret_cast<__gm__ void*>(dst), reinterpret_cast<__cbuf__ void*>(src), 0, 1, len, 0, 0);
#endif
}

template<typename T>
__aicore__ inline void mem_copy_ub_to_gm_impl(__gm__ T* dst, __ubuf__ T* src, const uint32_t& len)
{
#if defined(__DAV_VEC__) 
    copy_ubuf_to_gm(reinterpret_cast<__gm__ void*>(dst), reinterpret_cast<__ubuf__ void*>(src), 0, 1, len, 0, 0);
#endif
}

} // namespace __asc_aicore

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_DUMP_UTILS__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_DUMP_UTILS__
#endif

#endif // IMPL_UTILS_DEBUG_NPU_ARCH_2002_ASC_AICORE_PRINTF_UTILS_H