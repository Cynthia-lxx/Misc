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
 * \file softmax.h
 * \brief SoftMax and AdjustSoftMaxRes api of AscendC
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_H__
#endif

#ifndef LIB_SOFTMAX_SOFTMAX_H
#define LIB_SOFTMAX_SOFTMAX_H

#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "softmax_utils.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || \
    __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/activation/softmax/softmax_common.h"
#include "../../../impl/adv_api/detail/activation/softmax/softmax_base_impl.h"
#pragma begin_pipe(V)

namespace AscendC {
/*!
 * \ingroup SoftMax
 * \brief compute process: max = rowmax(x), sum = rowsum(exp(x-max)), y = exp(x-max)/sum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [out] sumTensor: output sum
 * \param [out] maxTensor: output max
 * \param [in] srcTensor: input x
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if srcTensor shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to
 *                           improve performance , but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor,
    const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
    TRACE_START(TraceId::SoftMax);
    SoftMaxImpl<T, T, isReuseSource, isBasicBlock, isDataFormatNZ, config>(dstTensor, sumTensor, maxTensor, srcTensor, tiling,
        softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftMax);
}

/*!
 * \ingroup SoftMax
 * \brief compute process: max = rowmax(x), sum = rowsum(exp(x-max)), y = exp(x-max)/sum
 * \param [out] dstTensor: output y with dtype of half
 * \param [out] sumTensor: output sum with dtype of float
 * \param [out] maxTensor: output max with dtype of float
 * \param [in] srcTensor: input x with dtype of half
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if srcTensor shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to
 *                           improve performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMax(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor,
    const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
    TRACE_START(TraceId::SoftMax);
    SoftMaxImpl<half, float, isReuseSource, isBasicBlock, isDataFormatNZ, config>(dstTensor, sumTensor, maxTensor, srcTensor, tiling,
        softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftMax);
}

/*!
 * \ingroup SoftMax
 * \brief compute process: max = rowmax(x), sum = rowsum(exp(x-max)), y = exp(x-max)/sum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [in] srcTensor: input x
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: reserved param
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
    TRACE_START(TraceId::SoftMax);
    SoftMaxImpl<T, isReuseSource, isBasicBlock, config>(dstTensor, srcTensor, tiling, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftMax);
}

/*!
 * \ingroup SoftMax
 * \brief compute process: max = rowmax(x), sum = rowsum(exp(x-max)), y = exp(x-max)/sum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [in] srcTensor: input x
 * \param [in] sharedTmpBuffer: input local temporary Tensor,you can get the range by tilingfunc of
 *                               GetSoftMaxMinTmpSize/GetSoftMaxMaxTmpSize
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: reserved param
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
    TRACE_START(TraceId::SoftMax);
    SoftMaxImpl<T, isReuseSource, isBasicBlock, config>(dstTensor, srcTensor, sharedTmpBuffer, tiling,
        softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftMax);
}

/*!
 * \ingroup SoftMax
 * \brief compute process: max = rowmax(x), sum = rowsum(exp(x-max)), y = exp(x-max)/sum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [out] sumTensor: output sum
 * \param [out] maxTensor: output max
 * \param [in] srcTensor: input x
 * \param [in] sharedTmpBuffer: input local temporary Tensor,you can get the range by tilingfunc of
 *                              GetSoftMaxMinTmpSize/GetSoftMaxMaxTmpSize
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if srcTensor shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to
 *                           improve performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor,
    const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
    TRACE_START(TraceId::SoftMax);
    SoftMaxImpl<T, T, isReuseSource, isBasicBlock, isDataFormatNZ,config>(dstTensor, sumTensor, maxTensor, srcTensor, sharedTmpBuffer,
        tiling, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftMax);
}

/*!
 * \ingroup SoftMax
 * \brief compute process: max = rowmax(x), sum = rowsum(exp(x-max)), y = exp(x-max)/sum
 * \param [out] dstTensor: output y with dtype of half
 * \param [out] sumTensor: output sum with dtype of float
 * \param [out] maxTensor: output max with dtype of float
 * \param [in] srcTensor: input x with dtype of half
 * \param [in] sharedTmpBuffer: input local temporary Tensor,you can get the range by tilingfunc of
 *                              GetSoftMaxMinTmpSize/GetSoftMaxMaxTmpSize
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if srcTensor shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to
 *                           improve performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMax(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor,
    const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
    TRACE_START(TraceId::SoftMax);
    SoftMaxImpl<half, float, isReuseSource, isBasicBlock, isDataFormatNZ, config>(dstTensor, sumTensor, maxTensor, srcTensor,
        sharedTmpBuffer, tiling, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftMax);
}

/*!
 * \ingroup AdjustSoftMaxRes
 * \brief check whether inmax result has from value, if exist, reset the softmax result as to value
 * \note support data type: half and float
 * \param [out/in] softMaxRes: input need Check src LocalTensor
 * \param [in] maxTensor: softmax rowmax value of last axis
 * \param [in] from: is the value need check in maxTensor
 * \param [in] to: is the value need reset in softMaxRes
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] isDataFormatNZ: if the data format of input src is NZ
 * \return if true means inmax result has the from value
 */
template <typename T1, typename T2, bool isDataFormatNZ = false, uint8_t stepSizeMode = 0>
__aicore__ inline bool AdjustSoftMaxRes(const LocalTensor<T1>& softMaxRes, const LocalTensor<T2>& maxTensor,
    const uint32_t from, const T1 to, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    if ASCEND_IS_AIC {
        return false;
    }
    return AdjustSoftMaxResImpl<T1, T2, isDataFormatNZ, stepSizeMode>(softMaxRes, maxTensor, from, to, softmaxShapeInfo);
}
} // namespace AscendC

#pragma end_pipe
#endif
#endif // LIB_SOFTMAX_SOFTMAX_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_H__
#endif