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
 * \file adjust_softmax_res_check_aicore.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/activation/softmax/adjust_softmax_res/adjust_softmax_res_check_aicore.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ADJUST_SOFTMAX_RES_CHECK_AICORE_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_ADJUST_SOFTMAX_RES_ADJUST_SOFTMAX_RES_CHECK_AICORE_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_ADJUST_SOFTMAX_RES_ADJUST_SOFTMAX_RES_CHECK_AICORE_H_

#include "include/adv_api/activation/softmax_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T1, typename T2, bool isDataFormatNZ = false, uint8_t stepSizeMode = 0>
class CheckFuncClassAdjustSoftMaxRes {
public:
    __aicore__ inline CheckFuncClassAdjustSoftMaxRes() {};
    __aicore__ inline CheckFuncClassAdjustSoftMaxRes(__gm__ const char *apiName) {};

public:
    __aicore__ inline void VerifyingParameters(const LocalTensor<T1>& softMaxRes, const LocalTensor<T2>& maxTensor,
        const uint32_t from, const T1 to, const SoftMaxShapeInfo& softmaxShapeInfo) {};
};

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_ADJUST_SOFTMAX_RES_ADJUST_SOFTMAX_RES_CHECK_AICORE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ADJUST_SOFTMAX_RES_CHECK_AICORE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ADJUST_SOFTMAX_RES_CHECK_AICORE_H__
#endif
