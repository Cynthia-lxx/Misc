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
 * \file topk_tiling.h
 * \brief
 */

#ifndef LIB_SORT_TOPK_TILING_H
#define LIB_SORT_TOPK_TILING_H
#include "topk_tilingdata.h"
#include "tiling/platform/platform_ascendc.h"
#include "graph/types.h"
#include "topk_utils_constants.h"
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {

/*
 * @ingroup GetTopKMaxMinTmpSize
 * @brief Get TopK api calculate need max and min temporary local space size.
 * @param [in] ascendcPlatform: Information about the hardware platform.
 * @param [in] inner: Inner axis length of input data.
 * @param [in] outter: Outer axis length of input data.
 * @param [in] isReuseSource: Whether temporary variables can reuse the input memory.
 * @param [in] isInitIndex: Whether to transfer the index of the input data.
 * @param [in] mode: Normal mode or small mode.
 * @param [in] isLargest: Descending or ascending order.
 * @param [in] dataTypeSize: Input data dtype size.
 * @param [out] maxValue: TopK api calculate need max temporary local space size.
 * @param [out] minValue: TopK api calculate need min temporary local space size.
 * @return true: Succeeded in obtaining the maximum and minimum temporary space sizes.
 * @return false: Failed to obtain maximum and minimum temporary space sizes.
 */
bool GetTopKMaxMinTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const int32_t inner,
    const int32_t outter, const bool isReuseSource, const bool isInitIndex, enum TopKMode mode, const bool isLargest,
    const uint32_t dataTypeSize, uint32_t& maxValue, uint32_t& minValue);

/*
 * @ingroup GetTopKMaxMinTmpSize
 * @brief Get the max and min temporary local space size used for TopK api calculation.
 * @param [in] inner: Inner axis length of input data.
 * @param [in] outter: Outer axis length of input data.
 * @param [in] k: K of TopK.
 * @param [in] isReuseSource: Whether temporary variables can reuse the input memory.
 * @param [in] isInitIndex: Whether to transfer the index of the input data.
 * @param [in] mode: Normal mode or small mode.
 * @param [in] isLargest: Max or min k values.
 * @param [in] dataType: Input data dtype.
 * @param [in] config: Configurations of TopK.
 * @param [out] maxValue: TopK api calculate need max temporary local space size.
 * @param [out] minValue: TopK api calculate need min temporary local space size.
 * @return true: Succeeded in obtaining the maximum and minimum temporary space sizes.
 * @return false: Failed to obtain maximum and minimum temporary space sizes.
 */
bool GetTopKMaxMinTmpSize(const int32_t inner, const int32_t outter, const int32_t k, const bool isReuseSource,
    const bool isInitIndex, enum TopKMode mode, const bool isLargest, ge::DataType dataType, const TopKConfig& config,
    uint32_t& maxValue, uint32_t& minValue);

/*
 * @ingroup TopKTilingFunc
 * @brief Get the tiling information required by the Topk interface.
 * @param [in] ascendcPlatform: Information about the hardware platform.
 * @param [in] inner: Inner axis length of input data.
 * @param [in] outter: Outer axis length of input data.
 * @param [in] k: Obtain the first k maximum or minimum values and their corresponding indexes.
 * @param [in] dataTypeSize: Input data dtype size.
 * @param [in] isInitIndex: Whether to transfer the index of the input data.
 * @param [in] mode: Normal mode or small mode.
 * @param [in] isLargest: Descending or ascending order.
 * @param [out] topKTiling: Output the tiling information required by the Topk interface.
 * @return true: The values of the TopK tiling parameters are obtained successfully.
 * @return false: Failed to obtain tiling data.
 */
bool TopKTilingFunc(const platform_ascendc::PlatformAscendC& ascendcPlatform, const int32_t inner, const int32_t outter,
    const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex, enum TopKMode mode, const bool isLargest,
    optiling::TopkTiling& topKTiling);
bool TopKTilingFunc(const platform_ascendc::PlatformAscendC& ascendcPlatform, const int32_t inner, const int32_t outter,
    const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex, enum TopKMode mode, const bool isLargest,
    AscendC::tiling::TopkTiling& topKTiling);

}  // namespace AscendC
#endif  // LIB_SORT_TOPK_TILING_H