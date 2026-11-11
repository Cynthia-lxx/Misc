/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/* !
 * \file kernel_simt_common_intf.h
 * \brief
 */
#ifndef ASCENDC_MODULES_SIMT_COMMON_INTERFACE_H
#define ASCENDC_MODULES_SIMT_COMMON_INTERFACE_H

#include "simt_api/device_types.h"
#include "simt_api/cpp/kernel_simt_utils.h"

namespace AscendC {
namespace Simt {

enum class SatMode {
    NO_SAT = 0,
    SAT
};

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t GetWarpSize();

template <int32_t dim = 0>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t GetThreadNum();

template <int32_t dim = 0>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t GetThreadIdx();

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t GetBlockIdx();

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t GetBlockNum();

}  // namespace Simt
}  // namespace AscendC

#include "impl/simt_api/cpp/kernel_simt_common_intf_impl.h"
#endif
