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
 * \file is_void.h
 * \brief
 */
#ifndef IMPL_STD_ASCENDC_STD_IS_VOID_IMPL__H
#define IMPL_STD_ASCENDC_STD_IS_VOID_IMPL__H

#include "is_same.h"
#include "remove_cv.h"

namespace AscendC {
namespace Std {

template <typename Tp>
struct is_void : public is_same<remove_cv_t<Tp>, void> {};

template <typename Tp>
constexpr bool is_void_v = is_void<Tp>::value;

}
}

#endif // IMPL_STD_ASCENDC_STD_IS_VOID_IMPL__H
