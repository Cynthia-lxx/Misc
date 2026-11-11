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
 * \file is_floating_point.h
 * \brief
 */
#ifndef IMPL_STD_ASCENDC_STD_IS_FLOATING_POINT_IMPL__H
#define IMPL_STD_ASCENDC_STD_IS_FLOATING_POINT_IMPL__H

#include "is_same.h"
#include "remove_cv.h"

namespace AscendC {
namespace Std {

template <typename T> 
struct is_floating_point {
private:
    template <typename Head, typename... Args>
    ASCENDC_HOST_AICORE inline static constexpr bool IsUnqualifiedAnyOf() {
        return (... || is_same_v<remove_cv_t<Head>, Args>);
    }

public:
    static constexpr bool value = IsUnqualifiedAnyOf<T, float, double, long double, half>();
};

template <typename Tp>
constexpr bool is_floating_point_v = is_floating_point<Tp>::value;

}
}

#endif // IMPL_STD_ASCENDC_STD_IS_FLOATING_POINT_IMPL__H
