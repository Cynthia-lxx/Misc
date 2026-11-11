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
 * \file ascend_antiquant_tiling.h
 * \brief
 */
#ifndef LIB_QUANTIZATION_ASCEND_ANTIQUANT_TILING_H
#define LIB_QUANTIZATION_ASCEND_ANTIQUANT_TILING_H
#include <cstdint>
#include "graph/tensor.h"
#include "graph/types.h"
namespace AscendC {
/*!
 * \brief This interface is used to obtain the maximum temporary space reserved or applied.
 * \param [in] srcShape, input shape information
 * \param [in] scaleShape, scale shape information
 * \param [in] isTranspose, enable transpose of input
 * \param [in] inputDataType, input data type
 * \param [in] outputDataType, output data type
 * return: maximum temporary space required
 */
uint32_t GetAscendAntiQuantMaxTmpSize(const ge::Shape& srcShape, const ge::Shape& scaleShape, bool isTranspose,
    ge::DataType inputDataType, ge::DataType outputDataType);

/*!
 * \brief This interface is used to obtain the minimum temporary space reserved or applied.
 * \param [in] srcShape, input shape information
 * \param [in] scaleShape, scale shape information
 * \param [in] isTranspose, enable transpose of input
 * \param [in] inputDataType, input data type
 * \param [in] outputDataType, output data type
 * return: minimum temporary space required
 */
uint32_t GetAscendAntiQuantMinTmpSize(const ge::Shape& srcShape, const ge::Shape& scaleShape, bool isTranspose,
    ge::DataType inputDataType, ge::DataType outputDataType);

/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 * \param [in] srcShape, input shape information
 * \param [in] scaleShape, scale shape information
 * \param [in] isTranspose, enable transpose of input
 * \param [in] inputDataType, input data type
 * \param [in] outputDataType, output data type
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetAscendAntiQuantMaxMinTmpSize(const ge::Shape& srcShape, const ge::Shape& scaleShape, bool isTranspose,
    ge::DataType inputDataType, ge::DataType outputDataType, uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief The calculation of the AscendAntiQuant interface requires the developer to reserve or apply for temporary space.
 * The relationship between the maximum temporary space (maxTmpBuffer) and the space occupied by the input (inputSize x
 * typeSize) is as follows: maxTmpBuffer = maxLiveNodeCount * inputSize * typeSize + extraBuf
 * This interface is used to obtain maxLiveNodeCount and extraBuf.
 * \param [in] srcShape, input shape information
 * \param [in] scaleShape, scale shape information
 * \param [in] isTranspose, enable transpose of input
 * \param [in] inputDataType, input data type
 * \param [in] outputDataType, output data type
 * \param [out] maxLiveNodeCount, the multiple of the maximum temporary space to the input occupied space
 * \param [out] extraBuf, the size of the extra temporary space
 */
void GetAscendAntiQuantTmpBufferFactorSize(const ge::Shape& srcShape, const ge::Shape& scaleShape, bool isTranspose,
    ge::DataType inputDataType, ge::DataType outputDataType, uint32_t& maxLiveNodeCount, uint32_t& extraBuf);
} // namespace AscendC
#endif // LIB_QUANTIZATION_ASCEND_ANTIQUANT_TILING_H