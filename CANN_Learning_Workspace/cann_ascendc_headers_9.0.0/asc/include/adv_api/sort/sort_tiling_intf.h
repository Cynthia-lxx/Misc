/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/
#ifndef LIB_SORT_SORT_TILING_INTF_H
#define LIB_SORT_SORT_TILING_INTF_H

#include <cstdint>

#include "graph/tensor.h"
#include "graph/types.h"
#include "tiling/platform/platform_ascendc.h"
#include "sort_utils_constants.h"

namespace AscendC {
constexpr uint32_t REGION_PROPOSAL_DATA_SIZE_V200 = 8;
constexpr uint32_t REGION_PROPOSAL_DATA_SIZE_HALF_V220 = 4;
constexpr uint32_t REGION_PROPOSAL_DATA_SIZE_FLOAT_V220 = 2;

/* **************************************** GetConcatTmpSize ****************************************** */
/*
 * @ingroup GetConcatTmpSize
 * @brief get concat tmp buffer size
 * @param [in] ascendcPlatform ascendc platform infomation
 * @param [in] elemCount element count number
 * @param [in] dataTypeSize data size number
 */
uint32_t GetConcatTmpSize(const platform_ascendc::PlatformAscendC &ascendcPlatform, const uint32_t elemCount,
    const uint32_t dataTypeSize);

/* **************************************** GetSortTmpSize ****************************************** */
/*
 * @ingroup GetSortTmpSize
 * @brief get sort tmp buffer size
 * @param [in] ascendcPlatform ascendc platform infomation
 * @param [in] elemCount element count number
 * @param [in] dataTypeSize data size number
 */
uint32_t GetSortTmpSize(const platform_ascendc::PlatformAscendC &ascendcPlatform, const uint32_t elemCount,
    const uint32_t dataTypeSize);

#ifndef ASCC_STRUCT_SORTCONFIG
#define ASCC_STRUCT_SORTCONFIG
struct SortConfig {
    SortType type = SortType::RADIX_SORT;
    bool isDescend = false;
    bool hasSrcIndex = false;
    bool hasDstIndex = false;
};
#endif

void GetSortMaxMinTmpSize(const ge::Shape& srcShape, ge::DataType valueType, ge::DataType indexType, bool isReuseSource,
    const SortConfig& config, uint32_t& maxValue, uint32_t& minValue);

} // namespace AscendC
#endif // LIB_SORT_SORT_TILING_INTF_H