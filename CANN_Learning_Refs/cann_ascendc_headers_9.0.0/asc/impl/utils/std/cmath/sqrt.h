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
 * \file sqrt.h
 * \brief
 */
#ifndef IMPL_STD_SQRT_H
#define IMPL_STD_SQRT_H


#ifdef ASCENDC_CPU_DEBUG
#include <cmath>
#endif

#include "../type_traits/is_one_of.h"

namespace AscendC {
namespace Std {
template <typename T>
__aicore__ inline T sqrt(const T src)
{
    static_assert(is_one_of<T, float, int64_t>(), 
                "current data type is not supported on current device!");
    return ::sqrt(src);
}
}
}
#endif