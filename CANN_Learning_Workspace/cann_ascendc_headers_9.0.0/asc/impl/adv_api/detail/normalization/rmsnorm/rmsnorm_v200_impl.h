/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/normalization/rmsnorm/rmsnorm_v200_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_RMSNORM_RMSNORM_V200_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_RMSNORM_RMSNORM_V200_IMPL_H
#define IMPL_NORMALIZATION_RMSNORM_RMSNORM_V200_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"

namespace AscendC {
namespace RmsNormAPI {
// use TransData + adds to do broadcast
__aicore__ inline void RmsNormBasicBlockBrc(const LocalTensor<float>& dst,
    const LocalTensor<float>& inputAddr, const LocalTensor<float>& reduceAddr, const uint32_t hLength,
    const uint32_t bsLength)
{
    constexpr uint32_t BASIC_BLK_HLENGTH = 64;
    constexpr uint32_t BASIC_BLK_BSLENGTH = 8;
    constexpr uint32_t FLOAT_PER_BLOCK = 8;
    uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE];
    uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE];
    constexpr uint32_t step = 2;
    constexpr uint32_t range = NCHW_CONV_ADDR_LIST_SIZE / step;
    for (uint32_t i = 0; i < range; ++i) {
        dstList[i * step] = (uint64_t)dst[i * hLength].GetPhyAddr();
        dstList[i * step + 1] = (uint64_t)dst[i * hLength + FLOAT_PER_BLOCK].GetPhyAddr();
        srcList[i * step] = (uint64_t)reduceAddr.GetPhyAddr();
        srcList[i * step + 1] = (uint64_t)reduceAddr.GetPhyAddr();
    }
    TransDataTo5HDParams params(false, false, bsLength / FLOAT_PER_BLOCK,
                                hLength * BASIC_BLK_BSLENGTH / FLOAT_PER_BLOCK, 1);
    // for v200, there is no brc instr, using transdataTo5HD to broadcast element to block
    TransDataTo5HD<float>(dstList, srcList, params);
    PipeBarrier<PIPE_V>();

    SetVectorMask<float>(0, bsLength * BASIC_BLK_HLENGTH);
    const uint8_t repStride = hLength / FLOAT_PER_BLOCK;
    UnaryRepeatParams unaryParams(1, 0, repStride, repStride);
    for (uint32_t i = 0; i < hLength / BASIC_BLK_HLENGTH; i++) {
        const uint32_t offset = i * BASIC_BLK_HLENGTH;
        Adds<float, false>(dst[offset], dst, 0, MASK_PLACEHOLDER, 1, unaryParams);
    }
    PipeBarrier<PIPE_V>();
}
} // namespace RmsNormAPI
} // namespace AscendC
#endif // IMPL_NORMALIZATION_RMSNORM_RMSNORM_V200_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_RMSNORM_RMSNORM_V200_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_RMSNORM_RMSNORM_V200_IMPL_H__
#endif
