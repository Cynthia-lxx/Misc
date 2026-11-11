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
 * \file max.h
 * \brief
 */
#ifndef IMPL_STD_MAX_H
#define IMPL_STD_MAX_H
#include "../type_traits/is_same.h"

namespace AscendC {
namespace Std {
template <typename T, typename U>
ASCENDC_HOST_AICORE inline constexpr auto max(const T& src0, const U& src1)
{
    return (src0 > src1) ? src0 : src1;
}

}
}
#endif