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
 * \file dequant_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/quantization/dequant/dequant_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/dequantize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEQUANT_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_DEQUANT_DEQUANT_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_DEQUANT_DEQUANT_CHECK_H_

#include "include/adv_api/quantization/ascend_dequant_utils.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "dequant_check_common.h"
#else
#include "dequant_check_aicore.h"
#endif

namespace AscendC {
namespace HighLevelApiCheck {

template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
__aicore__ inline void CheckFuncAscendDequant(__gm__ const char *apiName, const LocalTensor<dstT>& dstTensor,
    const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params, uint32_t calCount)
{
    CheckFuncClassAscendDequant<dstT, scaleT, isPureDqParams, mode> checkFun(apiName);
    checkFun.VerifyingParameters(dstTensor, srcTensor, deqScale, sharedTmpBuffer, params, calCount);
}

template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
__aicore__ inline void CheckFuncAscendDequant(__gm__ const char *apiName, const LocalTensor<dstT>& dstTensor,
    const LocalTensor<int32_t>& srcTensor, const scaleT deqScale, const LocalTensor<uint8_t>& sharedTmpBuffer,
    DequantParams& params)
{
    CheckFuncClassAscendDequantScalar<dstT, scaleT, isPureDqParams, mode> checkFun(apiName);
    checkFun.VerifyingParameters(dstTensor, srcTensor, deqScale, sharedTmpBuffer, params);
}

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_DEQUANT_DEQUANT_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEQUANT_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEQUANT_CHECK_H__
#endif
