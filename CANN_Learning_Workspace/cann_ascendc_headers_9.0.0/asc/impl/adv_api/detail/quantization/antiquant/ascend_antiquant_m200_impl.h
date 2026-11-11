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
 * \file ascend_antiquant_m200_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/quantization/antiquant/ascend_antiquant_m200_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_antiquant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_M200_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_M200_IMPL_H
#define IMPL_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_M200_IMPL_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "kernel_pop_stack_buffer.h"
#include "ascend_antiquant_common.h"

namespace AscendC {

template <typename SrcType, typename OutType>
__aicore__ inline void CheckApiDtypeValid()
{
    constexpr bool inputValid = (IsSameType<SrcType, int8_t>::value);
    constexpr bool outputValid = (IsSameType<OutType, half>::value);
    ASCENDC_ASSERT((inputValid && outputValid), {KERNEL_LOG(KERNEL_ERROR, "Failed to check dtype in AscendAntiQuant, "
        "current api support dtype combination is src: int8_t, dst: half.");});
}

template <typename DstType>
__aicore__ inline bool AntiQuantCheckPerformanceMode(const LocalTensor<DstType> &scale,
    const LocalTensor<uint8_t> &sharedTmpBuffer, const uint32_t K)
{
    return true;    // DstType can only be FP16, no need for cast
}

__aicore__ inline void AntiQuantFp16BrcbWithTransdata(
    const LocalTensor<half>& dst, const LocalTensor<half>& src, const uint32_t scaleN)
{
    __ubuf__ half* dstAddr = (__ubuf__  half*)dst.GetPhyAddr();
    uint64_t srcAddr = (uint64_t)(src.GetPhyAddr());
    uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE];
    uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE];
    for (uint32_t i = 0; i < NCHW_CONV_ADDR_LIST_SIZE; ++i) {
        dstList[i] = (uint64_t)(dstAddr + B16_DATA_NUM_PER_BLOCK * i);
        srcList[i] = srcAddr;
    }
    const uint32_t repTimes = scaleN / B16_DATA_NUM_PER_BLOCK;
    uint16_t dstRepStride = DEFAULT_REPEAT_STRIDE * ANTIQUANT_TWO;
    uint16_t srcRepStride = 1;
    if (repTimes == 1) {
        dstRepStride = 0;
        srcRepStride = 0;
    }
    TransDataTo5HDParams params(false, false, repTimes, dstRepStride, srcRepStride);
    // broadcast element to block using transdataTo5HD
    TransDataTo5HD<half>(dstList, srcList, params);
    PipeBarrier<PIPE_V>();
}

template <bool withOffset = true>
__aicore__ inline void AntiQuantFp16Brcb(const LocalTensor<half> &scale, const LocalTensor<half> &offset,
    AntiquantParams<half> &params, uint32_t scaleN)
{
    AntiQuantFp16BrcbWithTransdata(params.tempTensorScale, scale, scaleN);
    if constexpr (withOffset) {
        AntiQuantFp16BrcbWithTransdata(params.tempTensorOffset, offset, scaleN);
    }
}

template <typename SrcType, typename DstType>
__aicore__ inline void AscendAntiQuantBF16Transpose(const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src,
    const LocalTensor<DstType>& offset, const LocalTensor<DstType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
{
    return;    // BF16 is not supported in current platform
}

template <typename SrcType, typename DstType>
__aicore__ inline void AscendAntiQuantBF16Transpose(const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src,
    const LocalTensor<DstType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo = {})
{
    return;    // BF16 is not supported in current platform
}
} // namespace AscendC
#endif // IMPL_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_M200_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_M200_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_M200_IMPL_H__
#endif
