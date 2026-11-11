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
 * \file quantize.h
 * \brief Quantize is an Quantization function which refers to
 *        techniques for performing computations and storing tensors at lower bitwidths than floating point precision.
 * Mathematical formulas: Quantize(x, scale, offset) = (x* scale) + offset
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZE_H__
#endif

#ifndef LIB_QUANTIZATION_QUANTIZE_H
#define LIB_QUANTIZATION_QUANTIZE_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
#include "kernel_tensor.h"
#include "include/adv_api/quantization/quantize_utils.h"
#include "../../../impl/adv_api/detail/common/check.h"
#include "../../../impl/adv_api/detail/quantization/quantize/quantize_impl.h"
namespace AscendC {
#pragma begin_pipe(V)

/*!
 * \ingroup Quantize
 * \brief Quantize interface
 * \tparam config: template parameter, see detail at QuantizeConfig
 * \tparam DstT: dstTensor data type
 * \tparam SrcT: srcTensor data type
 * \tparam ScaleT: offset data type, can be a scalar or LocalTensor
 * \tparam OffsetT: offset data type, can be a scalar or LocalTensor
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] scale: quantization parameter, tensor or scalar
 * \param [in] offset: quantization parameter, tensor or scalar
 * \param [in] params: quantization parameter, see detail at QuantizeParams
 */
template <const QuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void Quantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor,
    const ScaleT& scale, const OffsetT& offset, const QuantizeParams& params)
{
    QuantizeImpl<config, DstT, SrcT, ScaleT, OffsetT>(dstTensor, srcTensor, scale, offset, params);
}

/*!
 * \ingroup Quantize
 * \brief Quantize interface
 * \tparam config: template parameter, see detail at QuantizeConfig
 * \tparam DstT: dstTensor data type
 * \tparam SrcT: srcTensor data type
 * \tparam ScaleT: offset data type, can be a scalar or LocalTensor
 * \tparam OffsetT: offset data type, can be a scalar or LocalTensor
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             quantize_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] scale: quantization parameter, tensor or scalar
 * \param [in] offset: quantization parameter, tensor or scalar
 * \param [in] params: quantization parameter, see detail at QuantizeParams
 */
template <const QuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void Quantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const ScaleT& scale, const OffsetT& offset, const QuantizeParams& params)
{
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN, VECOUT, VECCALC");
    QuantizeImpl<config, DstT, SrcT, ScaleT, OffsetT>(dstTensor, srcTensor, scale, offset, params);
}

#pragma end_pipe
}  // namespace AscendC
#endif  // defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
#endif  // LIB_QUANTIZATION_QUANTIZE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZE_H__
#endif