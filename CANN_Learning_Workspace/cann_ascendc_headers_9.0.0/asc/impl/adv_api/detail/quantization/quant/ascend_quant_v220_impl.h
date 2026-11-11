/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/* !
 * \file ascend_quant_v220_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/quantization/quant/ascend_quant_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_quant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_V220_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_QUANT_ASCEND_QUANT_V220_IMPL_H
#define IMPL_QUANTIZATION_QUANT_ASCEND_QUANT_V220_IMPL_H
#include "ascend_quant_pre_impl.h"
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
// api impl
template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
__aicore__ inline void AscendQuantImpl(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const float scale, const float offset, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(AscendQuant, (T, isReuseSource, config),
        (dstTensor, srcTensor, sharedTmpBuffer, scale, offset, calCount));
    AscendQuantCalc<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, scale, offset, calCount);
}

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
__aicore__ inline void AscendQuantImpl(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor,
    const T offset, const uint32_t scaleCount, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(AscendQuant, (T, isReuseSource, config),
        (dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offset, scaleCount, calCount));
    AscendQuantCalc<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, scaleTensor,
        offset, scaleCount, calCount);
}

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
__aicore__ inline void AscendQuantImpl(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor,
    const LocalTensor<T>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount,
    const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(AscendQuant, (T, isReuseSource, config),
        (dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, scaleCount, offsetCount, calCount));
    AscendQuantCalc<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, scaleTensor,
        offsetTensor, scaleCount, offsetCount, calCount);
}

} //  namespace AscendC
#endif // IMPL_QUANTIZATION_QUANT_ASCEND_QUANT_V220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_V220_IMPL_H__
#endif
