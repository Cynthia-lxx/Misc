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
 * \file is_class.h
 * \brief
 */
#ifndef IMPL_STD_ASCENDC_STD_IS_CLASS_IMPL__H
#define IMPL_STD_ASCENDC_STD_IS_CLASS_IMPL__H

#include "integral_constant.h"
#include "is_union.h"

namespace AscendC {
namespace Std {

namespace IsClassImpl {

template <typename Tp>
ASCENDC_HOST_AICORE inline bool_constant<!is_union_v<Tp>> Test(int32_t Tp::*);

template <typename Tp>
ASCENDC_HOST_AICORE inline false_type Test(uint32_t);

} // namespace is_class_impl

template <typename Tp>
struct is_class : decltype(IsClassImpl::Test<Tp>(nullptr)) {};

template <typename Tp>
constexpr bool is_class_v = is_class<Tp>::value;

}
}

#endif // IMPL_STD_ASCENDC_STD_IS_CLASS_IMPL__H
