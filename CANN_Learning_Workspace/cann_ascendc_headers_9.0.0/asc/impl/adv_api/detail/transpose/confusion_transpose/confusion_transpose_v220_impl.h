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
 * \file confusion_transpose_v220_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/transpose/confusion_transpose/confusion_transpose_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/transpose/transdata.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_V220_IMPL_H__
#endif

#ifndef IMPL_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_V220_IMPL_H
#define IMPL_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_V220_IMPL_H

#include "confusion_transpose_base_impl.h"

namespace AscendC {
/*
scene1：{ shape:[B, A1, A3 / 16, A2 / 16, 16, 16], format:"NZ"} -->{ shape:[B, A2, A1, A3], ori_shape:[B, A2, A1, A3],
format:"ND"} scene2：{ shape:[B, A1, A3 / 16, A2 / 16, 16, 16], format:"NZ"}-->{ shape:[B, A2, A3 / 16, A1 / 16, 16,
16], origin_shape:[B, A2, A1, A3], format:"NZ"}
*/
template <typename T>
__aicore__ inline void ConfusionTranspose0213(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t> &sharedTmpBuffer, TransposeType transposeTypeIn, ConfusionTranspose0213Tiling& tiling)
{
    ConfusionTranspose0213Compute(dstTensor, srcTensor, sharedTmpBuffer, transposeTypeIn, tiling);
}

/*
scene3：{ shape:[B, H / 16, S / 16, 16, 16], format:"NZ"}-->{ shape:[B, N, H/N/16, S / 16, 16, 16], ori_shape:[B, N, S,
H/N], format:"NZ"}
*/
template <typename T>
__aicore__ inline void ConfusionTranspose2NZ012N(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t> &sharedTmpBuffer, ConfusionTranspose2NZ012NTiling& tiling)
{
    ConfusionTranspose2NZ012NCompute(dstTensor, srcTensor, sharedTmpBuffer, tiling);
}

/*
scene4：{ shape:[B, H / 16, S / 16, 16, 16], format:"NZ"}-->{ shape:[B, N, S, H/N], ori_shape:[B, N, S, H/N],
format:"ND"}
*/
template <typename T>
__aicore__ inline void ConfusionTranspose2ND012N(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t> &sharedTmpBuffer, ConfusionTranspose2ND012NTiling& tiling)
{
    ConfusionTranspose2ND012NCompute(dstTensor, srcTensor, sharedTmpBuffer, tiling);
}

/*
scene5：{ shape:[B, N, H/N/16, S/16, 16, 16], format:"NZ"}-->{ shape:[B, S, H], ori_shape:[B, S, H], format:"ND"}
scene6：{ shape:[B, N, H/N/16, S/16, 16, 16], format:"NZ"}-->{ shape:[B, H/16, S/16, 16, 16], ori_shape:[B, S, H],
format:"NZ"}
*/
template <typename T>
__aicore__ inline void ConfusionTranspose012(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t> &sharedTmpBuffer, TransposeType transposeTypeIn, ConfusionTranspose012Tiling& tiling)
{
    ConfusionTranspose012Compute(dstTensor, srcTensor, sharedTmpBuffer, transposeTypeIn, tiling);
}

/*
scene7：{ shape:[H, W], format:"ND"} -->{ shape:[W, H], format:"ND"}
*/
template <typename T>
__aicore__ inline void ConfusionTransposeOnly(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    ConfusionTransposeOnlyTiling& tiling)
{
    ConfusionTransposeOnlyCompute(dstTensor, srcTensor, tiling);
}
} // namespace AscendC
#endif // IMPL_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_V220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_V220_IMPL_H__
#endif
