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
 * \file broadcast_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/pad/broadcast/broadcast_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/broadcast.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BROADCAST_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_PAD_BROADCAST_BROADCAST_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_PAD_BROADCAST_BROADCAST_CHECK_H_

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "broadcast_check_common.h"
#else
#include "broadcast_check_aicore.h"
#endif

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void CheckFuncBroadcast(__gm__ const char *apiName, const LocalTensor<T> &dstLocal, const LocalTensor<T> &srcLocal,
    const uint32_t dstShape[dim], const uint32_t srcShape[dim], LocalTensor<uint8_t> &sharedTmpBuffer)
{
    CheckFuncClassBroadcast<T, dim, axis, isReuseSource> checkFun(apiName);
    checkFun.VerifyingParameters(dstLocal, srcLocal, dstShape, srcShape, sharedTmpBuffer);
}

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_PAD_BROADCAST_BROADCAST_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BROADCAST_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BROADCAST_CHECK_H__
#endif
