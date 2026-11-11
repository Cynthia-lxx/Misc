/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/sort/sort/sort_check_aicore.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/sort.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_CHECK_AICORE_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_SORT_SORT_SORT_CHECK_AICORE_H_
#define IMPL_API_CHECK_KERNEL_CHECK_SORT_SORT_SORT_CHECK_AICORE_H_

namespace AscendC {
namespace HighLevelApiCheck {
template <typename T, bool isFullSort>
class CheckFuncClassSort {
public:
    __aicore__ inline CheckFuncClassSort()
    {}
    __aicore__ inline CheckFuncClassSort(__gm__ const char* apiName)
    {}

public:
    __aicore__ inline void VerifyingParameters(const LocalTensor<T>& dst, const LocalTensor<T>& concat,
                                               const LocalTensor<uint32_t>& index, const LocalTensor<T>& tmp,
                                               const int32_t repeatTime)
    {}
};

}  // namespace HighLevelApiCheck
}  // namespace AscendC

#endif  // IMPL_API_CHECK_KERNEL_CHECK_SORT_SORT_SORT_CHECK_AICORE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_CHECK_AICORE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_CHECK_AICORE_H__
#endif