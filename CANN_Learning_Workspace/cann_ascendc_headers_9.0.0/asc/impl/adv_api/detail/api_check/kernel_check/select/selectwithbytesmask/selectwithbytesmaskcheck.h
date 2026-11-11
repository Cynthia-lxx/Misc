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
 * \file selectwithbytesmaskcheck.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/select/selectwithbytesmask/selectwithbytesmaskcheck.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECTWITHBYTESMASKCHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_SELECT_SELECTWITHBYTESMAKS_SELECTWITHBYTESMAKS_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_SELECT_SELECTWITHBYTESMAKS_SELECTWITHBYTESMAKS_CHECK_H_

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "selectwithbytesmaskcheck_common.h"
#else
#include "selectwithbytesmaskcheck_aicore.h"
#endif

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, typename U, bool isReuseMask, bool reverse = false>
__aicore__ inline void CheckFuncSelectWithBytesMask(__gm__ const char *apiName, const LocalTensor<T> &dst, const LocalTensor<T> &src0,
    T src1, const LocalTensor<U> &mask, const LocalTensor<uint8_t> &sharedTmpBuffer,
    const SelectWithBytesMaskShapeInfo &info)
{
    CheckFuncClassSelectWithBytesMask<T, U, isReuseMask, true> checkFun(apiName);
    checkFun.VerifyingParameters(dst, src0, src1, mask, sharedTmpBuffer, info);
}

}
}

#endif // IMPL_API_CHECK_KERNEL_CHECK_SELECT_SELECTWITHBYTESMAKS_SELECTWITHBYTESMAKS_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECTWITHBYTESMASKCHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECTWITHBYTESMASKCHECK_H__
#endif
