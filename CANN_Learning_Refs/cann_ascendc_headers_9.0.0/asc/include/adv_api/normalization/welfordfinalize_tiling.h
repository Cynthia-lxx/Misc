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
 * \file welfordfinalize_tiling.h
 * \brief
 */
#ifndef LIB_NORMALIZATION_WELFORDFINALIZE_TILING_H
#define LIB_NORMALIZATION_WELFORDFINALIZE_TILING_H
#include "graph/tensor.h"
#include "tiling/platform/platform_ascendc.h"
namespace AscendC {
/*!
* \brief calculate max and min tmp buffer size for WelfordFinalize interface.
* \param [in] srcShape: input shape
* \param [in] typeSize: data type size
* \param [in] isReuseSource: indicate whether to reuse source tensor. Reserved parameter.
* \param [out] maxValue: max size required for tmp buffer
* \param [out] minValue: min size required for tmp buffer
*/
void GetWelfordFinalizeMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource,
    uint32_t& maxValue, uint32_t& minValue);
}
#endif // LIB_NORMALIZATION_WELFORDFINALIZE_TILING_H