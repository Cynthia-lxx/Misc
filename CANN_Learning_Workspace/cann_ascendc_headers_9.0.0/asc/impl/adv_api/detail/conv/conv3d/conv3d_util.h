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
 * \file conv3d_util.h
 * \brief
 */

#ifndef API_CONV3D_UTIL_H
#define API_CONV3D_UTIL_H

#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "kernel_utils.h"
#include "../common/conv_forward_util.h"

namespace Conv3dApi {
const static uint64_t LOAD2D_MAX_REPEAT_TIMES = 255;
const static uint8_t RIGHT_MOVE_8 = 8;
const static uint32_t L0A_SIZE = 65536;
const static uint32_t L0B_SIZE = 65536;

static __aicore__ inline uint64_t GetCurrentKD(uint64_t tilingKL1, uint64_t cin, uint64_t khxKw)
{
    return ConvApi::CeilDIV(tilingKL1, cin * khxKw);
}
}  // namespace Conv3dApi
#endif  // __API_CONV3D_UTIL_H__
