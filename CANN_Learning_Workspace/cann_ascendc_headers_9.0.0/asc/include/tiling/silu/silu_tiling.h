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
 * \file silu_tiling.h
 * \brief
 */
#ifndef LIB_ACTIVATION_SILU_TILING_H
#define LIB_ACTIVATION_SILU_TILING_H
#include "graph/tensor.h"
#include "register/tilingdata_base.h"
namespace AscendC {
/*
 * @ingroup GetSiluTmpSize
 * @brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 *  The developer selects a proper space size based on this range as the tiling parameter.
 * @param [in] srcShape : input src Tensor shape
 * @param [in] typeSize : src tensor dtype size
 * @param [in] isReuseSource: whether to reuse the input space of the source operand
 * @param [out] maxValue: max temporary local space size
 * @param [out] minValue: min temporary local space size
 */
void GetSiluTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource,
    uint32_t& maxValue, uint32_t& minValue);
} // namespace AscendC
#endif