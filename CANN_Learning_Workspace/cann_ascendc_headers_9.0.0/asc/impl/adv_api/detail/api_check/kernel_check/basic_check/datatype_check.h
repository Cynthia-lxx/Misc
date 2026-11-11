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
 * \file datatype_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/basic_check/datatype_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DATATYPE_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_DATATYPE_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_DATATYPE_CHECK_H_

#include "basic_check_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, typename... Args>
struct IsTypeInPack : Std::false_type {};

template <typename T, typename First, typename... Args>
struct IsTypeInPack<T, First, Args...> :
Std::conditional_t<Std::is_same_v<T, First>, Std::true_type, IsTypeInPack<T, Args...>> {};

template <typename T, typename... Args>
constexpr bool IsTypeInPackV = IsTypeInPack<T, Args...>::value;

template <typename T, typename Tuple>
struct IndexInTuple;

template <typename T, typename... Types>
struct IndexInTuple<T, Std::tuple<T, Types...>> {
    static constexpr size_t value = 0;
};

template <typename T, typename U, typename... Types>
struct IndexInTuple<T, Std::tuple<U, Types...>> {
    static constexpr size_t value = 1 + IndexInTuple<T, Std::tuple<Types...>>::value;
};

template <typename T, typename Tuple1, typename Tuple2>
__aicore__ inline __gm__ const char* GetTypeName(const Tuple1& typeData, const Tuple2& typeString) {
    constexpr size_t index = IndexInTuple<T, Tuple1>::value;
    return Std::get<index>(typeString);
}

class DataTypeCheckFuncBasicClass {
public:
    __aicore__ inline DataTypeCheckFuncBasicClass() {};
    __aicore__ inline DataTypeCheckFuncBasicClass(__gm__ const char *apiName) {
        this->apiName = apiName;
    };

public:
    template <typename T, typename U, typename... Args>
    __aicore__ inline void DataTypeVerifyingParameters(__gm__ const char* errLog)
    {   
        VerifyingParameters<T, U, Args...>(errLog);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T>();
        }
    }

private:
    template <typename T, typename U, typename... Args>
    __aicore__ inline void VerifyingParameters(__gm__ const char* errLog) {
        if (!IsTypeInPackV<T, U, Args...>) {
            ASCENDC_ASSERT((HighLevelAPIParametersPrint),
                { KERNEL_LOG(KERNEL_ERROR, "[%s] The data type of the template parameter is incorrect, %s!",
                apiName, errLog); });
        }
    }

    template <typename T>
    __aicore__ inline void PrintParameters()
    {
        if constexpr (Std::is_tuple_v<T>) {
            PrintTupleParameters<T>(tuple_sequence<T>{});
        } else {
            PrintSingleParameter<T>();
        }
    }

    template <typename T, size_t... Is>
    __aicore__ inline void PrintTupleParameters(Std::index_sequence<Is...>)
    {
        (PrintSingleParameter<typename Std::tuple_element<Is, T>::type>(), ...);
    }

    template <typename T>
    __aicore__ inline void PrintSingleParameter()
    {
        Std::tuple<int4b_t, int8_t, uint8_t, half, int16_t, uint16_t, int32_t, uint32_t, uint64_t, int64_t,
            float, bfloat16_t, double> typeData;

        auto typeString = MakeString2Tuple("int4b_t", "int8_t", "uint8_t", "half", "int16_t", "uint16_t",
            "int32_t", "uint32_t", "uint64_t", "int64_t", "float", "bfloat16_t", "double");

        KERNEL_LOG(KERNEL_INFO, "[%s] The data type in the template parameter is %s!", apiName,
            GetTypeName<T>(typeData, typeString));
    }

private:
    __gm__ const char *apiName = nullptr;
};

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_DATATYPE_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DATATYPE_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DATATYPE_CHECK_H__
#endif
