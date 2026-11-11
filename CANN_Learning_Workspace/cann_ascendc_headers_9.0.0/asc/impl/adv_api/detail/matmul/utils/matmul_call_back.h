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
 * \file matmul_call_back.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/utils/matmul_call_back.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_CALL_BACK_H__
#endif

#ifndef LIB_MATMUL_UTILS_MATMUL_CALL_BACK_H
#define LIB_MATMUL_UTILS_MATMUL_CALL_BACK_H

namespace AscendC {

template <void (*DataCopyOut)(const __gm__ void* gm, const LocalTensor<int8_t> &co1Local,
        const void *dataCopyOutParams, const uint64_t tilingPtr, const uint64_t dataPtr) = nullptr,
        void (*CopyA1)(const LocalTensor<int8_t> &aMatrix, const __gm__ void *gm, int row, int col, int useM, int useK,
        const uint64_t tilingPtr, const uint64_t dataPtr) = nullptr,
        void (*CopyB1)(const LocalTensor<int8_t> &bMatrix, const __gm__ void *gm, int row, int col, int useK, int useN,
        const uint64_t tilingPtr, const uint64_t dataPtr) = nullptr>
struct MatmulCallBackFunc {
    constexpr static void (*DataCopyOutPtr)(const __gm__ void* gm, const LocalTensor<int8_t> &co1Local,
        const void *dataCopyOutParams, const uint64_t tilingPtr, const uint64_t dataPtr) = DataCopyOut;
    constexpr static void (*CopyA1Ptr)(const LocalTensor<int8_t> &aMatrix, const __gm__ void *gm, int row, int col,
        int useM, int useK, const uint64_t tilingPtr, const uint64_t dataPtr) = CopyA1;
    constexpr static void (*CopyB1Ptr)(const LocalTensor<int8_t> &bMatrix, const __gm__ void *gm, int row, int col,
        int useK, int useN, const uint64_t tilingPtr, const uint64_t dataPtr) = CopyB1;
};

} // namespace AscendC
#endif // _MATMUL_CALL_BACK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_CALL_BACK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_CALL_BACK_H__
#endif