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
 * \file logsoftmax_tiling.h
 * \brief
 */

#ifndef LIB_ACTIVATION_LOGSOFTMAX_TILING_H
#define LIB_ACTIVATION_LOGSOFTMAX_TILING_H
#include "graph/tensor.h"
#include "logsoftmax_tilingdata.h"
#include "kernel_tiling/kernel_tiling.h"
namespace AscendC {
/*
 * @ingroup GetLogSoftMaxMaxTmpSize
 * @brief get logsoftmax api calculate need max temporary local space size
 * @param [in] srcShape : input src Tensor shape
 * @param [in] dataTypeSize : input dstMax Tensor and expSum Tensor DType size
 * @param [in] isReuseSource : whether to reuse the src Tensor
 * @return min temporary local space size
 */
uint32_t GetLogSoftMaxMaxTmpSize(const ge::Shape srcShape, const uint32_t dataTypeSize, const bool isReuseSource);
/*
 * @ingroup GetLogSoftMaxMinTmpSize
 * @brief get logsoftmax api calculate need min temporary local space size
 * @param [in] srcShape : input src Tensor shape
 * @param [in] dataTypeSize : input dstMax Tensor and expSum Tensor DType size
 * @param [in] isReuseSource : whether to reuse the src Tensor
 * @return min temporary local space size
 */
uint32_t GetLogSoftMaxMinTmpSize(const ge::Shape srcShape, const uint32_t dataTypeSize, const bool isReuseSource);
/*
 * @ingroup LogSoftMaxTilingFunc
 * @brief calculate LogSoftMax api need tiling
 * @param [in] srcShape : input src Tensor shape
 * @param [in] dataTypeSize : input dstMax Tensor and expSum Tensor DType size
 * @param [in] localWorkSpaceSize : the temporary local space size for LogSoftMax api, unit is Byte
 * @param [out] logsoftmaxTiling : LogSoftMax api tiling
 */
void LogSoftMaxTilingFunc(const ge::Shape srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    optiling::LogSoftMaxTiling& softmaxTiling);

void LogSoftMaxTilingFunc(const ge::Shape srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    AscendC::tiling::LogSoftMaxTiling& softmaxTiling);
}
#endif // LIB_ACTIVATION_LOGSOFTMAX_TILING_H