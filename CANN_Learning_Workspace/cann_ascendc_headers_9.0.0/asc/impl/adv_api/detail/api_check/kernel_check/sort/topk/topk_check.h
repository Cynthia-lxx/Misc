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
 * \file topk_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/sort/topk/topk_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/topk.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TOPK_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_SORT_TOPK_TOPK_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_SORT_TOPK_TOPK_CHECK_H_

#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/sort/topk_utils.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "topk_check_common.h"
#else
#include "topk_check_aicore.h"
#endif

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
__aicore__ inline void CheckFuncTopK(__gm__ const char *apiName, const LocalTensor<T> &dstValueLocal,
    const LocalTensor<int32_t> &dstIndexLocal,
    const LocalTensor<T> &srcLocal, const LocalTensor<int32_t> &srcIndexLocal, const LocalTensor<bool> &finishLocal,
    const LocalTensor<uint8_t> &tmpLocal, const int32_t k, const TopkTiling &tilling, const TopKInfo &topKInfo,
    const bool isLargest = true)
{
    CheckFuncClassTopKTmpBuf<T, isInitIndex, isHasfinish, isReuseSrc, topkMode> checkFun(apiName);
    checkFun.VerifyingParameters(dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal,
        finishLocal, tmpLocal, k, tilling, topKInfo, isLargest);
}

template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
__aicore__ inline void CheckFuncTopK(__gm__ const char *apiName, const LocalTensor<T> &dstValueLocal,
    const LocalTensor<int32_t> &dstIndexLocal,
    const LocalTensor<T> &srcLocal, const LocalTensor<int32_t> &srcIndexLocal, const LocalTensor<bool> &finishLocal,
    const int32_t k, const TopkTiling &tilling, const TopKInfo &topKInfo,
    const bool isLargest = true)
{
    CheckFuncClassTopK<T, isInitIndex, isHasfinish, isReuseSrc, topkMode> checkFun(apiName);
    checkFun.VerifyingParameters(dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal,
        finishLocal, k, tilling, topKInfo, isLargest);
}

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_SORT_TOPK_TOPK_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TOPK_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TOPK_CHECK_H__
#endif
