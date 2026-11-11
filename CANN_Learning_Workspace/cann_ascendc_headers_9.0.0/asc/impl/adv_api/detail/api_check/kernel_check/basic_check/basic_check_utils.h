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
 * \file basic_check_utils.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/basic_check/basic_check_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BASIC_CHECK_UTILS_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_BASIC_CHECK_UTILS_H_
#define IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_BASIC_CHECK_UTILS_H_

#include "kernel_basic_intf.h"
#include "include/utils/std/tuple.h"
#include "include/utils/std/algorithm.h"
#include "include/utils/std/type_traits.h"
#include "include/utils/std/utility.h"
#include "kernel_tiling/kernel_tiling.h"
#include "kernel_check_util.h"
#include "kernel_check.h"

namespace AscendC {
namespace HighLevelApiCheck {

constexpr bool HighLevelAPIParametersPrint = false;

enum class HardWareIndex {
    GM = 0,
    UB,
    L1,
    L0A,
    L0B,
    L0C,
    BIAS,
    FIXBUF,
    MAX
};

template <typename TupleType>
using tuple_sequence = Std::make_index_sequence<Std::tuple_size_v<Std::remove_cvref_t<TupleType>>>;

#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 10,9,8,7,6,5,4,3,2,1,0)
#define COUNT_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N

#define EXPAND(...) __VA_ARGS__

#define FOR_EACH(macro, ...) EXPAND(FOR_EACH_N(COUNT_ARGS(__VA_ARGS__), macro, __VA_ARGS__))

#define FOR_EACH_CONCAT(a, b) a##b
#define FOR_EACH_N(N, macro, ...) EXPAND(FOR_EACH_CONCAT(FOR_EACH_, N)(macro, __VA_ARGS__))

#define FOR_EACH_1(macro, x) macro(x)
#define FOR_EACH_2(macro, x, ...) macro(x), EXPAND(FOR_EACH_1(macro, __VA_ARGS__))
#define FOR_EACH_3(macro, x, ...) macro(x), EXPAND(FOR_EACH_2(macro, __VA_ARGS__))
#define FOR_EACH_4(macro, x, ...) macro(x), EXPAND(FOR_EACH_3(macro, __VA_ARGS__))
#define FOR_EACH_5(macro, x, ...) macro(x), EXPAND(FOR_EACH_4(macro, __VA_ARGS__))
#define FOR_EACH_6(macro, x, ...) macro(x), EXPAND(FOR_EACH_5(macro, __VA_ARGS__))
#define FOR_EACH_7(macro, x, ...) macro(x), EXPAND(FOR_EACH_6(macro, __VA_ARGS__))
#define FOR_EACH_8(macro, x, ...) macro(x), EXPAND(FOR_EACH_7(macro, __VA_ARGS__))
#define FOR_EACH_9(macro, x, ...) macro(x), EXPAND(FOR_EACH_8(macro, __VA_ARGS__))
#define FOR_EACH_10(macro, x, ...) macro(x), EXPAND(FOR_EACH_9(macro, __VA_ARGS__))

#define TO_STRING(x) (#x)
#define VA_ARGS_TO_MAKE_TUPLE(...) MakeParameters2Tuple(__VA_ARGS__), MakeString2Tuple(FOR_EACH(TO_STRING, __VA_ARGS__))

template<typename... Args>
__aicore__ inline auto MakeParameters2Tuple(Args... args)
{
    static_assert(sizeof...(Args) > 0, "The number of input elements must be greater than 0.");
    return Std::make_tuple(args...);
}

template<typename... Args>
__aicore__ inline auto MakeString2Tuple(__gm__ const char* errLog, Args... args)
{
    return Std::make_tuple(errLog, args...);
}

#define ARG_AND_STRING(x) (x), (TO_STRING(x))

#define ARGS_TO_STRING(...) #__VA_ARGS__
#define VA_ARGS_TO_MAKE_TUPLE_STRING(...) MakeParameters2Tuple(__VA_ARGS__), ARGS_TO_STRING(__VA_ARGS__)

#define ASCENDC_LOG_IF_CHECK(cond, behavior) \
    do {                                     \
        if (!(cond)) {                       \
            behavior;                        \
        }                                    \
    } while(0)

}

template <typename srcType>
__aicore__ inline int64_t ComparePosFromTuple(const srcType targetPos, const srcType posFromTuple, int64_t posFromString) 
{
    return targetPos == posFromTuple ? posFromString : 0;
}

template <typename srcType, typename T, typename U, size_t... Is>
__aicore__ inline __gm__ const char* FindStringFromTuple(const srcType pos, T posTuple, U posString, Std::index_sequence<Is...>) 
{
    static_assert((Std::is_tuple_v<T> && Std::is_tuple_v<U>), "Input template T or U is not tuple!");
    int64_t ans = (ComparePosFromTuple(pos, Std::get<Is>(posTuple), reinterpret_cast<int64_t>(Std::get<Is>(posString))) + ...);

    return reinterpret_cast<__gm__ const char*>(ans);
}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_BASIC_CHECK_UTILS_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BASIC_CHECK_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BASIC_CHECK_UTILS_H__
#endif
