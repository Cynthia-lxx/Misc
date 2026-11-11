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
 * \file softmax_flashv2_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/membase/v200/softmax_flashv2_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflashv2.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_V200_SOFTMAX_FLASHV2_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_V200_SOFTMAX_FLASHV2_IMPL_H

#include "softmax_impl.h"
#include "../common/softmax_flashv2_impl/softmax_flashv2_basic_block_impl.h"
#include "../common/softmax_flashv2_impl/softmax_flashv2_update_impl.h"
#include "../common/softmax_flashv2_impl/softmax_flashv2_no_update_impl.h"
#include "../common/softmax_flashv2_impl/softmax_flashv2_nz_impl.h"
#include "../common/softmax_flashv2_impl/softmax_flashv2_common_impl.h"
namespace AscendC {

template <typename T1, typename T2, bool isUpdate = false, bool isBasicBlock = false, bool isOutputReduceMax = false>
__aicore__ inline void SoftmaxFlashV2M1PostProcess(const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& outReduceMax,
    const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor, const LocalTensor<T1>& srcTensor,
    const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor, const LocalTensor<T2>& inMaxTensor,
    const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "softmaxflashv2 is not supported on current device!"); });
}
}
#endif // IMPL_ACTIVATION_SOFTMAX_V200_SOFTMAX_FLASHV2_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_IMPL_H__
#endif
