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
 * \file layernorm_check_aicore.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/normalization/layernorm/layernorm_check_aicore.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_CHECK_AICORE_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORM_LAYERNORM_CHECK_AICORE_H_
#define IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORM_LAYERNORM_CHECK_AICORE_H_

#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/normalization/layernorm_utils.h"
#include "../../../../normalization/layernorm/layernorm_normal_config.h"

namespace AscendC {
namespace HighLevelApiCheck {
template <typename T, bool isReuseSource = false>
class CheckFuncClassLayerNorm {
public:
    __aicore__ inline CheckFuncClassLayerNorm() {};
    __aicore__ inline CheckFuncClassLayerNorm(__gm__ const char *name) {};

public:
    __aicore__ inline void VerifyingParameters(const LocalTensor<T>& output, const LocalTensor<T>& outputMean,
        const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma,
        const LocalTensor<T>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling) {};
};

template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
class CheckFuncClassLayerNormRstd {
public:
    __aicore__ inline CheckFuncClassLayerNormRstd() {};
    __aicore__ inline CheckFuncClassLayerNormRstd(__gm__ const char *name) {};

public:
    __aicore__ inline void VerifyingParameters(const LocalTensor<T>& output,  const LocalTensor<float>& outputMean,
        const LocalTensor<float>& outputRstd, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
        const LocalTensor<U>& beta, const float epsilon, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const LayerNormPara& para, const LayerNormSeparateTiling& tiling) {};
};

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORM_LAYERNORM_CHECK_AICORE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_CHECK_AICORE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_CHECK_AICORE_H__
#endif
