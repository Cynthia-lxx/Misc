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
 * \file dropout.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_H__
#endif

#ifndef LIB_DROPOUT_DROPOUT_H
#define LIB_DROPOUT_DROPOUT_H

#include "kernel_tensor.h"
#include "../../../impl/adv_api/detail/filter/dropout/dropout_impl.h"

namespace AscendC {
#pragma begin_pipe(V)
/* !
 * \brief filtering srcLocal based on maskLocal to obtain dstLocal
 * \param [out] dstLocal, output LocalTensor
 * \param [in] srcLocal, input LocalTensor
 * \param [in] maskLocal, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] keepProb, Weight coefficient, indicating the probability that data in srcLocal is retained
 * \param [in] highPrecision, whether to enable the high-precision interface to improve the calculation accuracy
 * \param [in] info, firstAxis, srcLastAxis and maskLastAxis
 */
template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
__aicore__ inline void DropOut(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<uint8_t>& maskLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const float keepProb,
    const DropOutShapeInfo& info)
{
    if ASCEND_IS_AIC {
        return;
    }
    DropOutImpl<T, isInitBitMode, dropOutMode>(dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
}

/* !
 * \ingroup DropOut
 * \param [out] dstLocal, output LocalTensor
 * \param [in] srcLocal, input LocalTensor
 * \param [in] maskLocal, input LocalTensor
 * \param [in] keepProb, Weight coefficient, indicating the probability that data in srcLocal is retained
 * \param [in] highPrecision, whether to enable the high-precision interface to improve the calculation accuracy
 * \param [in] info, firstAxis, srcLastAxis and maskLastAxis
 */
template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
__aicore__ inline void DropOut(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<uint8_t>& maskLocal, const float keepProb, const DropOutShapeInfo& info)
{
    if ASCEND_IS_AIC {
        return;
    }
    DropOutImpl<T, isInitBitMode, dropOutMode>(dstLocal, srcLocal, maskLocal, keepProb, info);
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_DROPOUT_DROPOUT_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_H__
#endif