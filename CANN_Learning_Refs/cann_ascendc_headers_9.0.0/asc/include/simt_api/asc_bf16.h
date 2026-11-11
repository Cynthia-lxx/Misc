/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef INCLUDE_SIMT_API_ASC_BF16_H
#define INCLUDE_SIMT_API_ASC_BF16_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_BF16_H__
#endif

#include "simt_api/asc_fp16.h"
#include "simt_api/device_types.h"

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)

#define ASCRT_INF_BF16 __ushort_as_bfloat16((unsigned short)0x7F80U)
#define ASCRT_MAX_NORMAL_BF16 __ushort_as_bfloat16((unsigned short)0x7F7FU)
#define ASCRT_MIN_DENORM_BF16 __ushort_as_bfloat16((unsigned short)0x0001U)
#define ASCRT_NAN_BF16 __ushort_as_bfloat16((unsigned short)0x7FFFU)
#define ASCRT_NEG_ZERO_BF16 __ushort_as_bfloat16((unsigned short)0x8000U)
#define ASCRT_ONE_BF16 __ushort_as_bfloat16((unsigned short)0x3F80U)
#define ASCRT_ZERO_BF16 __ushort_as_bfloat16((unsigned short)0x0000U)

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __hisnan(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __hisinf(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __habs(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __hfma(bfloat16_t x, bfloat16_t y, bfloat16_t z);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __hmax(bfloat16_t x, bfloat16_t y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __hmin(bfloat16_t x, bfloat16_t y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hcos(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2cos(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hsin(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2sin(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t htanh(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2tanh(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hexp(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2exp(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hexp2(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2exp2(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hexp10(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2exp10(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hlog(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2log(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hlog2(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2log2(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hlog10(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2log10(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hsqrt(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2sqrt(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hrsqrt(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2rsqrt(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hrcp(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2rcp(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hfloor(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2floor(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hrint(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2rint(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hceil(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2ceil(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t htrunc(bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2trunc(bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rn(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rn_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rz(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rz_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rd(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rd_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_ru(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_ru_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rna(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rna_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rn_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rz(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rz_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rd(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rd_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_ru(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_ru_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rna(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rna_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_rn(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_rd(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_ru(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_rna(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rn(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rn_sat(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rz(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rz_sat(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rd(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rd_sat(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_ru(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_ru_sat(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rna(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rna_sat(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __bfloat162float(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_rn(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_rz(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_rd(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_ru(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_rna(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_rn(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_rz(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_rd(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_ru(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_rna(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_rn(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_rz(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_rd(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_ru(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_rna(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_rn(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_rz(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_rd(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_ru(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_rna(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_rn(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_rz(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_rd(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_ru(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_rna(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_rn(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_rz(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_rd(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_ru(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_rna(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_rn(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_rz(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_rd(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_ru(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_rna(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_rn(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_rz(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_rd(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_ru(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_rna(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_rn(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_rz(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_rd(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_ru(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_rna(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float2bfloat162_rn(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __floats2bfloat162_rn(const float x, const float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rn(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __bfloat162bfloat162(const bfloat16_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __halves2bfloat162(const bfloat16_t x, const bfloat16_t y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __high2bfloat16(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __high2bfloat162(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __high2float(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __highs2bfloat162(const bfloat16x2_t x, const bfloat16x2_t y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __low2bfloat16(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __low2bfloat162(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __low2float(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __lowhigh2highlow(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __lows2bfloat162(const bfloat16x2_t x, const bfloat16x2_t y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 __bfloat1622float2(const bfloat16x2_t x);

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_add(__ubuf__ bfloat16_t *address, bfloat16_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_add(__ubuf__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_add(__gm__ bfloat16_t *address, bfloat16_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_add(__gm__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_sub(__ubuf__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_sub(__gm__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_exch(__ubuf__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_exch(__gm__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_max(__ubuf__ bfloat16_t *address, bfloat16_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_max(__ubuf__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_max(__gm__ bfloat16_t *address, bfloat16_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_max(__gm__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_min(__ubuf__ bfloat16_t *address, bfloat16_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_min(__ubuf__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_min(__gm__ bfloat16_t *address, bfloat16_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_min(__gm__ bfloat16x2_t *address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_cas(__ubuf__ bfloat16x2_t *address, bfloat16x2_t compare, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_cas(__gm__ bfloat16x2_t *address, bfloat16x2_t compare, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_ldcg(__gm__ bfloat16_t* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_ldcg(__gm__ bfloat16x2_t* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_ldca(__gm__ bfloat16_t* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_ldca(__gm__ bfloat16x2_t* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ bfloat16_t* address, bfloat16_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ bfloat16x2_t* address, bfloat16x2_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ bfloat16_t* address, bfloat16_t val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ bfloat16x2_t* address, bfloat16x2_t val);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t make_bfloat162(bfloat16_t x, bfloat16_t y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ushort_as_bfloat16(const unsigned short int x);

#include "impl/simt_api/asc_bf16_impl.h"

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_BF16_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_BF16_H__
#endif

#endif  // INCLUDE_SIMT_API_ASC_BF16_H