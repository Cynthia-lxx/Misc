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
 * \file reuse_source_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/basic_check/reuse_source_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REUSE_SOURCE_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_REUSE_SOURCE_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_REUSE_SOURCE_CHECK_H_

#include "basic_check_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {

class ReuseSourceCheckFuncBasicClass {
public:
    __aicore__ inline ReuseSourceCheckFuncBasicClass() {};
    __aicore__ inline ReuseSourceCheckFuncBasicClass(__gm__ const char *apiName) {
        this->apiName = apiName;
    };

public:
    template <bool isConfigurable = true>
    __aicore__ inline void IsReuseSourceVerifyingParameters(const bool isReuseSource, __gm__ const char* paraName)
    {   
        ReuseSourceKernelCheckVerify<isConfigurable>(isReuseSource, paraName);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters(isReuseSource, paraName);
        }
    }

private:
    template <bool isConfigurable = true>
    __aicore__ inline void ReuseSourceKernelCheckVerify(const bool isReuseSource, __gm__ const char* paraName) {
        if (!isConfigurable && isReuseSource == true) {
            KERNEL_LOG(KERNEL_WARN, "[%s] The parameter of %s is true, may not be effective.", apiName, paraName);
        }
    }
    __aicore__ inline void PrintParameters(const bool isReuseSource, __gm__ const char* paraName)
    {
        if (isReuseSource) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter of %s is true!", apiName, paraName);
        } else {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter of %s is false!", apiName, paraName);
        }
    }

private:
    __gm__ const char *apiName = nullptr;
};

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_BASIC_CHECK_REUSE_SOURCE_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REUSE_SOURCE_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REUSE_SOURCE_CHECK_H__
#endif