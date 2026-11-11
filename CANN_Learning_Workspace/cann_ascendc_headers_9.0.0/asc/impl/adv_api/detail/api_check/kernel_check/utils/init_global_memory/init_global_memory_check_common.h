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
 * \file init_global_memory_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/utils/init_global_memory/init_global_memory_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/utils/init_global_memory.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_INIT_GLOBAL_MEMORY_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_UTILS_INIT_GLOBAL_MEMORY_INIT_GLOBAL_MEMORY_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_UTILS_INIT_GLOBAL_MEMORY_INIT_GLOBAL_MEMORY_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/single_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckInitGlobalMemoryParamsClass {
public:
    template <typename T>
    __aicore__ inline void CheckInitGlobalMemoryParams(GlobalTensor<T> &gmWorkspaceAddr, const uint64_t size, const T value) {
        VerifyingParameters<T>(gmWorkspaceAddr, size, value);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T>(gmWorkspaceAddr, size, value);
        }
    }

private:
    template <typename T>
    __aicore__ inline void VerifyingParameters(GlobalTensor<T> &gmWorkspaceAddr, const uint64_t size, const T value) {
        ASCENDC_ASSERT((gmWorkspaceAddr.GetSize() > 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[Fill] Failed to check tensor size of gmWorkspaceAddr, current tensor size is %lu, "
            "should be greater than 0.", gmWorkspaceAddr.GetSize()); });

        ASCENDC_ASSERT(((size <= gmWorkspaceAddr.GetSize()) || HighLevelAPIParametersPrint), { KERNEL_LOG(KERNEL_ERROR,
            "[Fill] The value of size is %lu, should not be greater than gmWorkspaceAddr size %lu",
            size, gmWorkspaceAddr.GetSize()); });
    }

    template <typename T>
    __aicore__ inline void PrintParameters(GlobalTensor<T> &gmWorkspaceAddr, const uint64_t size, const T value) {
        KERNEL_LOG(KERNEL_INFO, "[Fill] The size of gmWorkspaceAddr is %lu.", gmWorkspaceAddr.GetSize());
    }
};

template <typename T>
class CheckFuncClassInitGlobalMemory : public DataTypeCheckFuncBasicClass, public CheckInitGlobalMemoryParamsClass {
public:
    __aicore__ inline CheckFuncClassInitGlobalMemory() {};
    __aicore__ inline CheckFuncClassInitGlobalMemory(__gm__ const char *apiName) :
    DataTypeCheckFuncBasicClass(apiName) {};

public:
    __aicore__ inline void VerifyingParameters(GlobalTensor<T> &gmWorkspaceAddr, const uint64_t size, const T value) {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float, uint16_t, int16_t, uint32_t, int32_t>(
            "template parameter (T) is not half/float/uint16_t/int16_t/uint32_t/int32_t");

        CheckInitGlobalMemoryParamsClass::CheckInitGlobalMemoryParams<T>(gmWorkspaceAddr, size, value);
    };
};

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_UTILS_INIT_GLOBAL_MEMORY_INIT_GLOBAL_MEMORY_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_INIT_GLOBAL_MEMORY_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_INIT_GLOBAL_MEMORY_CHECK_COMMON_H__
#endif
