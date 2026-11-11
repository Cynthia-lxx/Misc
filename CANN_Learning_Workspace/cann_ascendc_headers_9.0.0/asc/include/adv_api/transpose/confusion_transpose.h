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
 * \file confusion_transpose.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONFUSION_TRANSPOSE_H__
#endif

#ifndef LIB_TRANSPOSE_CONFUSION_TRANSPOSE_H
#define LIB_TRANSPOSE_CONFUSION_TRANSPOSE_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../../impl/adv_api/detail/transpose/confusion_transpose/confusion_transpose_common_impl.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
    __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
namespace AscendC {
#pragma begin_pipe(V)

/* **************************************************************************************************
 * Transpose                                              *
 * ************************************************************************************************* */
/*
 * @ingroup Transpose
 * @arrange and reshape the data from src to dst.
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] sharedTmpBuffer tmp buffer LocalTensor
 * @param [in] transposeType
 * @param [in] tiling Transpose tiling
 */
template <typename T>
__aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LocalTensor<uint8_t> &sharedTmpBuffer, TransposeType transposeType, ConfusionTransposeTiling& tiling)
{
    ConfusionTransposeImpl<T>(dst, src, sharedTmpBuffer, transposeType, tiling);
}

/* **************************************************************************************************
 * Transpose                                              *
 * ************************************************************************************************* */
/*
 * @ingroup Transpose
 * @arrange and reshape the data from src to dst.
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] transposeType
 * @param [in] tiling Transpose tiling
 */
template <typename T>
__aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    TransposeType transposeType, ConfusionTransposeTiling& tiling)
{
    LocalTensor<uint8_t> tmpBuffer;
    bool res = PopStackBuffer<uint8_t, TPosition::LCM>(tmpBuffer);
    ASCENDC_ASSERT(res, { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    ConfusionTransposeImpl<T>(dst, src, tmpBuffer, transposeType, tiling);
}
#pragma end_pipe
} // namespace AscendC
#endif
#endif // LIB_TRANSPOSE_CONFUSION_TRANSPOSE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONFUSION_TRANSPOSE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONFUSION_TRANSPOSE_H__
#endif