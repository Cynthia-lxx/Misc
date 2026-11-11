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
 * \file round_tiling.h
 * \brief
 */
#ifndef LIB_MATH_ROUND_TILING_H
#define LIB_MATH_ROUND_TILING_H

#include "graph/tensor.h"
#include "tiling/platform/platform_ascendc.h"
namespace AscendC {
/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] ascendcPlatform, platform Information
 * \param [in] srcShape, input shape information
 * \param [in] typeSize, size of the input data type, in bytes
 * \param [in] isReuseSource, whether to reuse the input space of the source operand
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetRoundMaxMinTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const ge::Shape& srcShape,
    const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief The calculation of the Round interface requires the developer to reserve or apply for temporary space. The
 * relationship between the maximum temporary space (maxTmpBuffer) and the space occupied by the input (inputSize x
 * typeSize) is as follows: maxTmpBuffer = maxLiveNodeCount * inputSize * typeSize + extraBuf
 * This interface is used to obtain maxLiveNodeCount and extraBuf.
 *
 * \param [in] ascendcPlatform, platform Information
 * \param [in] typeSize, size of the input data type, in bytes
 * \param [out] maxLiveNodeCount, the multiple of the maximum temporary space to the input occupied space
 * \param [out] extraBuf, the size of the extra temporary space
 */
void GetRoundTmpBufferFactorSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t typeSize,
    uint32_t& maxLiveNodeCount, uint32_t& extraBuf);
}
#endif // LIB_MATH_ROUND_TILING_INTF