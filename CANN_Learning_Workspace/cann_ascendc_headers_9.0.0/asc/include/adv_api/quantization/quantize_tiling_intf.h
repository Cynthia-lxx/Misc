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
 * \file quantize_tiling_intf.h
 * \brief
 */
#ifndef LIB_QUANTIZATION_QUANTIZE_TILING_INTF_H
#define LIB_QUANTIZATION_QUANTIZE_TILING_INTF_H

#include "quantize_tiling.h"
namespace AscendC {
[[deprecated(__FILE__ " is deprecated, please use quantize_tiling.h instead!")]]
typedef void QuantizeTilingDeprecatedHeader;
using LibQuantizeTilingInterface = QuantizeTilingDeprecatedHeader;
} // namespace AscendC
#endif // LIB_QUANTIZATION_ASCEND_QUANT_TILING_INTF_H