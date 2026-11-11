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
#pragma message("impl/adv_api/detail/normalization/rmsnorm/rmsnorm_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_RMSNORM_RMSNORM_V220_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_RMSNORM_RMSNORM_V220_IMPL_H
#define IMPL_NORMALIZATION_RMSNORM_RMSNORM_V220_IMPL_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"

namespace AscendC {
namespace RmsNormAPI {
// src0 is input: [b,s,h], src1 is reduce results: [h,], repeatTime is b*s
// basic block brc: vbrcb + adds(0)
__aicore__ inline void RmsNormBasicBlockBrc(const LocalTensor<float>& dst, const LocalTensor<float>& inputAddr,
    const LocalTensor<float>& reduceAddr, const uint32_t hLength, const uint32_t bsLength)
{
    constexpr uint32_t BASIC_BLK_HLENGTH = 64;
    constexpr uint32_t BASIC_BLK_BSLENGTH = 8;
    constexpr uint32_t FLOAT_PER_BLOCK = 8;
    const uint16_t dstBlkStride = hLength / FLOAT_PER_BLOCK;
    const uint16_t dstRepStride = dstBlkStride * BASIC_BLK_BSLENGTH;
    const uint32_t repTime = bsLength / BASIC_BLK_BSLENGTH;
    // each repeat process 64 block, so repTime is bsLength/8
    SetMaskNorm();
    ResetMask();
    BrcbRepeatParams brcParams(dstBlkStride, dstRepStride);
    Brcb(dst, reduceAddr, repTime, brcParams);
    PipeBarrier<PIPE_V>();

    SetMaskCount();
    SetVectorMask<float>(0, bsLength * BASIC_BLK_HLENGTH);
    const uint8_t repStride = hLength / FLOAT_PER_BLOCK;
    const int32_t loop = hLength / BASIC_BLK_HLENGTH;
    UnaryRepeatParams unaryParams(1, 0, repStride, repStride);
    for (int32_t i = 0; i < loop; i++) {
        const uint32_t offset = i * BASIC_BLK_HLENGTH;
        Adds<float, false>(dst[offset], dst, 0, MASK_PLACEHOLDER, 1, unaryParams);
    }
    PipeBarrier<PIPE_V>();
}
} // namespace RmsNormAPI
} // namespace AscendC
#endif // IMPL_NORMALIZATION_RMSNORM_RMSNORM_V220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_RMSNORM_RMSNORM_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_RMSNORM_RMSNORM_V220_IMPL_H__
#endif
