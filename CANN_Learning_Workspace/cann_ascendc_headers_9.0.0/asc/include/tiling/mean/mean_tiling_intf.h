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
 * \file mean_tiling_intf.h
 * \brief
 */
#ifndef LIB_REDUCE_MEAN_TILING_INTF
#define LIB_REDUCE_MEAN_TILING_INTF

#include "mean_tiling.h"
namespace AscendC {
[[deprecated(__FILE__ " is deprecated, please use mean_tiling.h instead!")]] typedef void MeanTilingDeprecatedHeader;
using LibMeanTilingInterface = MeanTilingDeprecatedHeader;
} // namespace AscendC
#endif  // LIB_REDUCE_MEAN_TILING_INTF