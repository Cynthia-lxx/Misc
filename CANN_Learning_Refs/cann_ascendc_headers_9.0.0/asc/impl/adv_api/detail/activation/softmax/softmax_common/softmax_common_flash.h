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
 * \file softmax_common_flash.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/activation/softmax/softmax_common/softmax_common_flash.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflash.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_FLASH_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_FLASH_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_FLASH_H

#include <type_traits>
#include "../softmax_flash_base_impl/softmax_flash_nd_process_impl.h"

namespace AscendC {

template <typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void SoftmaxFlashCommonImpl(const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor,
    const LocalTensor<T2>& maxTensor, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor,
    const LocalTensor<T2>& inSumTensor, const LocalTensor<T2>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    LocalTensor<float> workLocal;
    PopStackBuffer<float, TPosition::LCM>(workLocal);
    uint32_t workLocalSize = workLocal.GetSize();
    LastAxisShapeND srcNDinfo;
    LastAxisShapeND originalSrcShape;
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        ShapeInfo srcShape = srcTensor.GetShapeInfo();
        srcNDinfo = GetLastAxisShapeND(srcShape);
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    } else {
        srcNDinfo = { softmaxShapeInfo.srcM, softmaxShapeInfo.srcK };
        originalSrcShape = { softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK };
    }
    if constexpr (Std::is_same<T1, half>::value && Std::is_same<T2, float>::value) {
        if (srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxFlashTilingFunc(workLocalSize, srcNDinfo, newTiling, FLOAT_NUM_PER_BLK, isUpdate, isBasicBlock);
            if (!isUpdate) {
                SoftMaxNDImpl<half, float>(dstTensor, sumTensor, maxTensor, srcTensor, workLocal, originalSrcShape,
                    newTiling);
            } else {
                SoftmaxFlashNDImpl<isBasicBlock>(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor,
                    inMaxTensor, workLocal, originalSrcShape, newTiling);
            }
        } else {
            if (!isUpdate) {
                SoftMaxNDImpl<half, float>(dstTensor, sumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
            } else {
                SoftmaxFlashNDImpl<isBasicBlock>(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor,
                    inMaxTensor, workLocal, originalSrcShape, tiling);
            }
        }
    } else if constexpr (Std::is_same<T1, T2>::value){
        const uint32_t elementNumPerBlk = ONE_BLK_SIZE / sizeof(T1);
        const uint32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T1);
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxFlashTilingFunc(workLocalSize, srcNDinfo, newTiling, elementNumPerBlk, isUpdate, isBasicBlock);
            SoftmaxFlashPostProcess<T1, isBasicBlock>(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor,
                inMaxTensor, workLocal, originalSrcShape, newTiling, isUpdate);
        } else {
            SoftmaxFlashPostProcess<T1, isBasicBlock>(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor,
                inMaxTensor, workLocal, originalSrcShape, tiling, isUpdate);
        }
    }
}

template <typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void SoftmaxFlashTmpBufCommonImpl(const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor,
    const LocalTensor<T2>& maxTensor, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor,
    const LocalTensor<T2>& inSumTensor, const LocalTensor<T2>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SoftMaxTiling& tiling, bool isUpdate, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    auto tempBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    uint32_t workLocalSize = tempBuffer.GetSize();
    LastAxisShapeND srcNDinfo;
    LastAxisShapeND originalSrcShape;
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        ShapeInfo srcShape = srcTensor.GetShapeInfo();
        srcNDinfo = GetLastAxisShapeND(srcShape);
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    } else {
        srcNDinfo = { softmaxShapeInfo.srcM, softmaxShapeInfo.srcK };
        originalSrcShape = { softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK };
    }

    if constexpr (Std::is_same<T1, half>::value && Std::is_same<T2, float>::value) { 
        if (srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxFlashTilingFunc(workLocalSize, srcNDinfo, newTiling, FLOAT_NUM_PER_BLK, isUpdate, isBasicBlock);
            if (!isUpdate) {
                SoftMaxNDImpl<half, float>(dstTensor, sumTensor, maxTensor, srcTensor, tempBuffer, originalSrcShape,
                    newTiling);
            } else {
                SoftmaxFlashNDImpl<isBasicBlock>(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor,
                    inMaxTensor, tempBuffer, originalSrcShape, newTiling);
            }
        } else {
            if (!isUpdate) {
                SoftMaxNDImpl<half, float>(dstTensor, sumTensor, maxTensor, srcTensor, tempBuffer, originalSrcShape,
                    tiling);
            } else {
                SoftmaxFlashNDImpl<isBasicBlock>(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor,
                    inMaxTensor, tempBuffer, originalSrcShape, tiling);
            }
        }
    } else if constexpr (Std::is_same<T1, T2>::value){
        const uint32_t elementNumPerBlk = ONE_BLK_SIZE / sizeof(T1);
        const uint32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T1);
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxFlashTilingFunc(workLocalSize, srcNDinfo, newTiling, elementNumPerBlk, isUpdate, isBasicBlock);
            SoftmaxFlashPostProcess<T1, isBasicBlock>(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor,
                inMaxTensor, tempBuffer, originalSrcShape, newTiling, isUpdate);
        } else {
            SoftmaxFlashPostProcess<T1, isBasicBlock>(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor,
                inMaxTensor, tempBuffer, originalSrcShape, tiling, isUpdate);
        }
    }
}

}
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_FLASH_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_FLASH_H__
#endif
