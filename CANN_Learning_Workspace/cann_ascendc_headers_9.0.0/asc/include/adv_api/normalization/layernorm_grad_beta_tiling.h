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
 * \file layernorm_grad_beta_tiling.h
 * \brief
 */
#ifndef LIB_NORMALIZATION_LAYERNORM_GRAD_BETA_TILING_H
#define LIB_NORMALIZATION_LAYERNORM_GRAD_BETA_TILING_H
#include "graph/tensor.h"
#include "layernorm_grad_beta_tilingdata.h"
#include "kernel_tiling/kernel_tiling.h"
namespace AscendC {
struct LayerNormGradBetaTilingTmp {
    uint32_t stackBufferSize = 0;
    uint32_t bLength = 0;
    uint32_t sLength = 0;
    uint32_t hLength = 0;
    uint32_t originalHLength = 0;
    uint32_t bshLength = 0;
    uint32_t bsLength = 0;
    uint32_t oneCalSize = 0;
    uint32_t numberOfTmpBuf = 0;
    uint32_t loopRound = 0;
    uint32_t inputTailSize = 0;
    uint32_t inputTailPos = 0;
    uint32_t bsTailSize = 0;
    uint32_t bshCurLength = 0;
    uint32_t bsCurLength = 0;
    uint32_t gammaTempTensorPos = 0;
    uint32_t betaTempTensorPos = 0;
    uint32_t inputDyTmpTensorPos = 0;
    uint32_t resForGammaTmpTensorPos = 0;
    uint32_t reserved = 0;
};

constexpr uint32_t LAYERNORM_GRAD_BETA_B16_BYTE_SIZE = 2;
constexpr uint32_t LAYERNORM_GRAD_BETA_FOUR_BUF_NUM = 4;
constexpr uint32_t LAYERNORM_GRAD_BETA_TWO_BUF_NUM = 2;
constexpr uint32_t LAYERNORM_GRAD_BETA_ONE_BUF_NUM = 1;
constexpr uint32_t LAYERNORM_GRAD_BETA_INDEX_BLENGTH = 0;
constexpr uint32_t LAYERNORM_GRAD_BETA_INDEX_SLENGTH = 1;
constexpr uint32_t LAYERNORM_GRAD_BETA_INDEX_HLENGTH = 2;
constexpr uint32_t LAYERNORM_GRAD_BETA_INDEX_ORIGINALHLENGTH = 3;

/*!
 * \brief calculate max and min tmp buffer size for LayerNormGradBeta interface.
 * \param [in] srcShape: input shape
 * \param [in] typeSize: data type size
 * \param [in] isReuseSource: indicate whether to reuse source tensor. Reserved parameter.
 * \param [out] maxValue: max size required for tmp buffer
 * \param [out] minValue: min size required for tmp buffer
 */
void GetLayerNormGradBetaMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource,
    uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief get tiling for LayerNormGradBeta interface.
 * \param [in] srcShape: input shape
 * \param [in] stackBufferSize: share temporary buffer size
 * \param [in] typeSize: data type size
 * \param [in] isReuseSource: indicate whether to reuse source tensor. Reserved parameter.
 * \param [out] tiling: LayerNormGradBetaTiling
 */
void GetLayerNormGradBetaNDTilingInfo(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, optiling::LayerNormGradBetaTiling& tiling);
void GetLayerNormGradBetaNDTilingInfo(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, AscendC::tiling::LayerNormGradBetaTiling& tiling);
} // namespace AscendC
#endif // LIB_NORMALIZATION_LAYERNORM_GRAD_BETA_TILING_H