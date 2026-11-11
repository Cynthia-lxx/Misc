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
 * \file algorithm.h
 * \brief
 */
#ifndef AICORE_UTILS_STD_ALGORITHM_H
#define AICORE_UTILS_STD_ALGORITHM_H

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include "stub_def.h"
#endif

namespace AscendC {
namespace Std {
template <typename T, typename U> ASCENDC_HOST_AICORE inline constexpr auto min(const T& src0, const U& src1);
template <typename T, typename U> ASCENDC_HOST_AICORE inline constexpr auto max(const T& src0, const U& src1);
}
}

#include "impl/utils/std/algorithm/max.h"
#include "impl/utils/std/algorithm/min.h"

#endif  // AICORE_UTILS_STD_ALGORITHM_H
