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
 * \file log_tiling_intf.h
 * \brief
 */
#ifndef LIB_MATH_LOG_TILING_INTF
#define LIB_MATH_LOG_TILING_INTF

#include "log_tiling.h"
namespace AscendC {
[[deprecated(__FILE__ " is deprecated, please use log_tiling.h instead!")]]
    typedef void LogTilingDeprecatedHeader;
using LibLogTilingInterface = LogTilingDeprecatedHeader;
} // namespace AscendC

#endif // LIB_MATH_LOG_TILING_INTF