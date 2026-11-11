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
* \file multiple_tensor_check.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/basic_check/multiple_tensor_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MULTIPLE_TENSOR_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_MULTIPLE_TENSOR_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_MULTIPLE_TENSOR_CHECK_H_

#include "basic_check_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {

class MultipleTensorCheckFuncBasicClass {
public:
    __aicore__ inline MultipleTensorCheckFuncBasicClass() {};
    __aicore__ inline MultipleTensorCheckFuncBasicClass(__gm__ const char *apiName) {
        this->apiName = apiName;
    };

public:
    template <typename T, typename U>
    __aicore__ inline void TensorReuseVerifyingParameters(T tensorTuple, U tensorStringTuple)
    {
        static_assert((Std::is_tuple_v<T> && Std::is_tuple_v<U>),
            "Input template T or U or W is not tuple!");
        static_assert((Std::tuple_size_v<decltype(tensorTuple)> == Std::tuple_size_v<decltype(tensorStringTuple)>),
            "TPositionVerifyingParameters func, tensorTuple.Size != tensorStringTuple.Size !");
        MultipleTensorReuseLoopCheck(tensorTuple, tensorStringTuple, tuple_sequence<T>{});
    }

private:
    template <typename T, typename U, size_t... Is>
    __aicore__ inline void MultipleTensorReuseLoopCheck(T tensorTuple, U tensorStringTuple, Std::index_sequence<Is...>)
    {
        (SingleTensorReuseLoopCheck(Std::get<Is>(tensorTuple), Std::get<Is>(tensorStringTuple),
            tensorTuple, tensorStringTuple, Is, tuple_sequence<T>{}), ...);
    }

    template <typename T, typename U, typename W, size_t... Is>
    __aicore__ inline void SingleTensorReuseLoopCheck(const LocalTensor<W> &checkTensor, __gm__ const char* tensorString,
        T tensorTuple, U tensorStringTuple, const uint32_t index, Std::index_sequence<Is...>)
    {
        (TensorReuseLoopCheck(checkTensor, tensorString,
            Std::get<Is>(tensorTuple), Std::get<Is>(tensorStringTuple), index, Is), ...);

        if constexpr (HighLevelAPIParametersPrint) {
            PrintTensorInfo(checkTensor, tensorString);
        }
    }

    template <typename T, typename U>
    __aicore__ inline void TensorReuseLoopCheck(const LocalTensor<T> &checkTensor,
        __gm__ const char* checkTensorString, const LocalTensor<U> &tensor, __gm__ const char* tensorString,
        const uint32_t indexCheck, const uint32_t indexTensor)
    {
        if (indexCheck == indexTensor) {
            return;
        }
        uint64_t checkPhyAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(checkTensor.GetPhyAddr()));
        uint64_t tensorPhyAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(tensor.GetPhyAddr()));
        uint64_t checkTensorSize = checkTensor.GetSize() * sizeof(T);
        uint64_t tensorSize = tensor.GetSize() * sizeof(U);

        if constexpr (IsSameType<T, int4b_t>::value) {
            checkTensorSize /= INT4_TWO;
        }

        if constexpr (IsSameType<U, int4b_t>::value) {
            tensorSize /= INT4_TWO;
        }
        bool ans = (checkPhyAddr == tensorPhyAddr) && (checkTensorSize == tensorSize);

        ASCENDC_ASSERT((!ans) || HighLevelAPIParametersPrint, {KERNEL_LOG(KERNEL_ERROR,
            "[%s] The input Tensor (%s) and (%s) cannot be reused, their start address is %lu, tensor size is %lu.",
            apiName, checkTensorString, tensorString, checkPhyAddr, checkTensorSize); });
    }

    template <typename T>
    __aicore__ inline void PrintTensorInfo(const LocalTensor<T> &checkTensor, __gm__ const char* checkTensorString)
    {
        uint64_t checkTensorSize = checkTensor.GetSize() * sizeof(T);
        if constexpr (IsSameType<T, int4b_t>::value) {
            checkTensorSize /= INT4_TWO;
        }
        uint64_t checkPhyAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(checkTensor.GetPhyAddr()));
        KERNEL_LOG(KERNEL_INFO, "[%s] CheckTensor (%s) physical address is (%u, %u)!",
            apiName, checkTensorString, checkPhyAddr, checkPhyAddr + checkTensorSize);
    }

private:
    __gm__ const char *apiName = nullptr;
};

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_MULTIPLE_TENSOR_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MULTIPLE_TENSOR_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MULTIPLE_TENSOR_CHECK_H__
#endif
