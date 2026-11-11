/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef ASCENDC_MODULE_SIMT_WARP_LEVEL_INTERFACE_H
#define ASCENDC_MODULE_SIMT_WARP_LEVEL_INTERFACE_H

#include "simt_api/device_types.h"
#include "simt_api/cpp/kernel_simt_utils.h"

namespace AscendC {
namespace Simt {
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t WarpAllSync(int32_t predicate);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t WarpAnySync(int32_t predicate);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t Uni(int32_t predicate);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t WarpBallotSync(int32_t predicate);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t WarpActiveMask();

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpShflSync(T var, int32_t srcLane, int32_t width = warpSize);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpShflUpSync(T var, uint32_t delta, int32_t width = warpSize);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpShflDownSync(T var, uint32_t delta, int32_t width = warpSize);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpShflXorSync(T var, int32_t laneMask, int32_t width = warpSize);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpReduceAddSync(T val);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpReduceMaxSync(T val);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpReduceMinSync(T val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void ThreadBarrier();

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void ThreadFence();
}  // namespace Simt
}  // namespace AscendC

#include "impl/simt_api/cpp/kernel_simt_warp_level_intf_impl.h"
#endif  // ASCENDC_MODULE_SIMT_WARP_LEVEL_INTERFACE_H
