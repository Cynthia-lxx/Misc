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
 * \file layernorm_grad_tiling.h
 * \brief
 */
#ifndef LIB_NORMALIZATION_LAYERNORM_GRAD_TILING_H
#define LIB_NORMALIZATION_LAYERNORM_GRAD_TILING_H

#include "graph/tensor.h"
#include "layernorm_grad_tilingdata.h"
#include "kernel_tiling/kernel_tiling.h"
namespace AscendC {
constexpr uint32_t LAYERNORM_GRAD_ONE_BLOCK_SIZE_OF_FLOAT = 8;
constexpr uint32_t LAYERNORM_GRAD_B32_BYTE_SIZE = 4;
constexpr uint32_t LAYERNORM_GRAD_B16_BYTE_SIZE = 2;
constexpr uint32_t LAYERNORM_GRAD_THREE_TIMES = 3;
constexpr uint32_t LAYERNORM_GRAD_TWO_TIMES = 2;
constexpr uint32_t LAYERNORM_GRAD_REUSE_FLOAT_BUF_NUM = 4;
constexpr uint32_t LAYERNORM_GRAD_HALF_BUF_NUM = 9;
constexpr uint32_t LAYERNORM_GRAD_FLOAT_BUF_NUM = 6;
constexpr uint32_t LAYERNORM_GRAD_DAVID_BUF_NUM = 2;
constexpr uint32_t LAYERNORM_GRAD_B32_DATA_NUM_PER_BLOCK = 8;
constexpr uint32_t LAYERNORM_GRAD_B16_DATA_NUM_PER_BLOCK = 16;

/*!
 * \brief calculate max and min tmp buffer size for LayerNormGrad interface.
 * \param [in] srcShape: input shape
 * \param [in] typeSize: data type size
 * \param [in] isReuseSource: indicate whether to reuse source tensor. Reserved parameter.
 * \param [out] maxValue: max size required for tmp buffer
 * \param [out] minValue: min size required for tmp buffer
 */
void GetLayerNormGradMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource,
    uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief get tiling for LayerNormGrad interface.
 * \param [in] srcShape: input shape
 * \param [in] stackBufferSize: share temporary buffer size
 * \param [in] typeSize: data type size
 * \param [in] isReuseSource: indicate whether to reuse source tensor. Reserved parameter.
 * \param [out] tiling: LayerNormGradTiling
 */
void GetLayerNormGradNDTilingInfo(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, optiling::LayerNormGradTiling& tiling);
void GetLayerNormGradNDTilingInfo(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, AscendC::tiling::LayerNormGradTiling& tiling);
}
#endif // LIB_NORMALIZATION_LAYERNORM_GRAD_TILING_H