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
 * \file softmax_flashv3_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/activation/softmax/softmax_flashv3/softmax_flashv3_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflashv3.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV3_SOFTMAX_FLASHV3_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV3_SOFTMAX_FLASHV3_CHECK_H_

#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/activation/softmax_utils.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "softmax_flashv3_check_common.h"
#else
#include "softmax_flashv3_check_aicore.h"
#endif

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, typename U, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ, const SoftmaxConfig& config>
__aicore__ inline void CheckFuncSoftmaxFlashV3(__gm__ const char *apiName, const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor,
    const LocalTensor<U>& expSumTensor, const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<T>& expMaxTensor, const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inexpSumTensor,
    const LocalTensor<U>& inMaxTensor, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const SoftMaxParams& params)
{
    CheckFuncClassSoftmaxFlashV3<T, U, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config> checkFun(apiName);
    checkFun.VerifyingParameters(dstTensor, meanTensor, expSumTensor, maxTensor,
        srcTensor, expMaxTensor, inMeanTensor, inexpSumTensor, inMaxTensor, workLocal, tiling, params);
}

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV3_SOFTMAX_FLASHV3_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_CHECK_H__
#endif
