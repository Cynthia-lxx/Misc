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
 * \file ascend_dequant_tiling.h
 * \brief
 */
#ifndef LIB_QUANTIZATION_ASCEND_DEQUANT_TILING_H
#define LIB_QUANTIZATION_ASCEND_DEQUANT_TILING_H
#include <cstdint>

#include "graph/tensor.h"
namespace AscendC {

/*!
 * \brief calculate max and min tmp buffer size for AscendDequant interface.
 * \param [in] srcShape: input shape
 * \param [in] typeSize: data type size: sizeof(TYPE)
 * \param [out] maxValue: max size of tmp buffer
 * \param [out] minValue: min size of tmp buffer
 */
void GetAscendDequantMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, uint32_t& maxValue,
    uint32_t& minValue);

/*!
 * \brief The calculation of the AscendDequant interface requires the developer to reserve or apply for temporary space.
 * The relationship between the maximum temporary space (maxTmpBuffer) and the space occupied by the input (inputSize x
 * typeSize) is as follows: maxTmpBuffer = maxLiveNodeCount * inputSize * typeSize + extraBuf
 * This interface is used to obtain maxLiveNodeCount and extraBuf.
 * \param [in] srcShape, input shape information
 * \param [out] maxLiveNodeCount, the multiple of the maximum temporary space to the input occupied space
 * \param [out] extraBuf, the size of the extra temporary space
 */
void GetAscendDequantTmpBufferFactorSize(const ge::Shape& srcShape, uint32_t& maxLiveNodeCount, uint32_t& extraBuf);
} // namespace AscendC
#endif // LIB_QUANTIZATION_ASCEND_DEQUANT_TILING_H