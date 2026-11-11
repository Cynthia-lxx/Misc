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
 * \file type_traits.h
 * \brief
 */
#ifndef AICORE_UTILS_STD_TYPE_TRAITS_H
#define AICORE_UTILS_STD_TYPE_TRAITS_H

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include "stub_def.h"
#endif

namespace AscendC {
namespace Std {

// enable_if
template <bool, typename Tp>
struct enable_if;

// conditional
template <bool Bp, typename If, typename Then>
struct conditional;

// is_convertible
template <typename From, typename To>
struct is_convertible;

// is_base_of
template <typename Base, typename Derived>
struct is_base_of;

// is_same
template <typename Tp, typename Up>
struct is_same;

// integral_constant
template <typename Tp, Tp v>
struct integral_constant;

} // namespace Std
} // namespace AscendC

#include "impl/utils/std/type_traits/add_const.h"
#include "impl/utils/std/type_traits/add_cv.h"
#include "impl/utils/std/type_traits/add_lvalue_reference.h"
#include "impl/utils/std/type_traits/add_pointer.h"
#include "impl/utils/std/type_traits/add_rvalue_reference.h"
#include "impl/utils/std/type_traits/add_volatile.h"
#include "impl/utils/std/type_traits/conditional.h"
#include "impl/utils/std/type_traits/decay.h"
#include "impl/utils/std/type_traits/enable_if.h"
#include "impl/utils/std/type_traits/integral_constant.h"
#include "impl/utils/std/type_traits/is_array.h"
#include "impl/utils/std/type_traits/is_base_of.h"
#include "impl/utils/std/type_traits/is_class.h"
#include "impl/utils/std/type_traits/is_constant.h"
#include "impl/utils/std/type_traits/is_const.h"
#include "impl/utils/std/type_traits/is_convertible.h"
#include "impl/utils/std/type_traits/is_floating_point.h"
#include "impl/utils/std/type_traits/is_function.h"
#include "impl/utils/std/type_traits/is_integral.h"
#include "impl/utils/std/type_traits/is_pointer.h"
#include "impl/utils/std/type_traits/is_referenceable.h"
#include "impl/utils/std/type_traits/is_reference.h"
#include "impl/utils/std/type_traits/is_same.h"
#include "impl/utils/std/type_traits/is_tuple.h"
#include "impl/utils/std/type_traits/is_union.h"
#include "impl/utils/std/type_traits/is_void.h"
#include "impl/utils/std/type_traits/remove_const.h"
#include "impl/utils/std/type_traits/remove_cv.h"
#include "impl/utils/std/type_traits/remove_cvref.h"
#include "impl/utils/std/type_traits/remove_extent.h"
#include "impl/utils/std/type_traits/remove_pointer.h"
#include "impl/utils/std/type_traits/remove_reference.h"
#include "impl/utils/std/type_traits/remove_volatile.h"
#include "impl/utils/std/type_traits/is_one_of.h"

#endif // AICORE_UTILS_STD_TYPE_TRAITS_H