/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/transpose/transdata/transdata_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/transpose/transdata.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_TRANSDATA_TRANSDATA_IMPL_H__
#endif
#ifndef IMPL_TRANSPOSE_TRANSDATA_TRANSDATA_IMPL_H
#define IMPL_TRANSPOSE_TRANSDATA_TRANSDATA_IMPL_H

#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/transpose/transdata/transdata_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace Internal {

namespace {
constexpr int32_t n0 = 16;
constexpr int32_t c0 = 16;
constexpr int32_t hw0 = 16;
constexpr int32_t ncdhwDims = 5;
constexpr int32_t fractalZ3DDims = 7;
constexpr int32_t ndc1hwc0Dims = 6;
}

struct TransDataTmpParams {
    int32_t n;
    int32_t c;
    int32_t d;
    int32_t h;
    int32_t w;
    int32_t n1;
    int32_t c1;
    int32_t padHw;
};

constexpr int32_t DEFAULT_TRANSDATA_5HD_LIST = 16;

template <typename T>
__aicore__ inline void DC1Hwn1n0c0ToC1DHwn1n0c0HWAlign(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const TransDataTmpParams& params)
{
    // d, c1, h w n1 n0 c0 -> c1, d, hw1*hw0 n1 n0 c0
    int32_t d = params.d;
    int32_t h = params.h;
    int32_t w = params.w;
    int32_t n1 = params.n1;
    int32_t c1 = params.c1;
    int32_t padHw = params.padHw;

    uint32_t dim0 = d;
    uint32_t dim1 = c1;
    uint32_t lastDim = h * w * n1 * n0 * c0;

    // dim0, dim1, lastDim -> dim1, dim0, lastDim
    int32_t n1n0c0DimElems = n1 * n0 * c0;
    int32_t hwAlignElems = padHw * n1n0c0DimElems;
    int32_t hwPadElems = (padHw - h * w) * n1n0c0DimElems;

    uint16_t blockCount = dim1;
    uint16_t blockLen = lastDim * sizeof(T) / ONE_BLK_SIZE;
    uint16_t srcGap = 0;
    uint16_t dstGap = ((dim0 - 1) * hwAlignElems + hwPadElems) * sizeof(T) / ONE_BLK_SIZE;

    uint32_t dstSize = c1 * d * padHw * n1 * n0 * c0;
    Duplicate<T>(dst, static_cast<T>(0), dstSize);
    PipeBarrier<PIPE_V>();

    DataCopyParams dataCopyParams = { blockCount, blockLen, srcGap, dstGap };
    for (uint32_t d0 = 0; d0 < dim0; d0++) {
        DataCopy(dst[d0 * hwAlignElems], src[d0 * dim1 * lastDim], dataCopyParams);
    }
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void C1Dhwn1n0c0ToC1C0Dhwn1n0(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const TransDataTmpParams& params)
{
    // C1 DHWN1N0 C0 -> C1 C0 DHWN1N0
    int32_t d = params.d;
    int32_t n1 = params.n1;
    int32_t c1 = params.c1;
    int32_t padHw = params.padHw;

    TransDataTo5HDParams transDataParams;
    transDataParams.dstHighHalf = false;
    transDataParams.srcHighHalf = false;
    transDataParams.repeatTimes = d * padHw * n1;
    if (transDataParams.repeatTimes == 1) {
        transDataParams.srcRepStride = 0;
        transDataParams.dstRepStride = 0;
    } else {
        transDataParams.srcRepStride = DEFAULT_TRANSDATA_5HD_LIST * c0 * sizeof(T) / ONE_BLK_SIZE;
        transDataParams.dstRepStride = n0 * sizeof(T) / ONE_BLK_SIZE;
    }

    uint64_t srcOffsetArr[DEFAULT_TRANSDATA_5HD_LIST];
    uint64_t dstOffsetArr[DEFAULT_TRANSDATA_5HD_LIST];
    uint64_t srcAddr = (uint64_t)src.GetPhyAddr();
    uint64_t dstAddr = (uint64_t)dst.GetPhyAddr();
    for (uint32_t j = 0; j < c1; j++) {
        uint32_t outOffset = j * d * padHw * n1 * n0 * c0;
        for (uint8_t i = 0; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
            srcOffsetArr[i] = (uint64_t)(srcAddr + (outOffset + i * n0) * sizeof(T));
            dstOffsetArr[i] = (uint64_t)(dstAddr + (outOffset + i * d * padHw * n1 * n0) * sizeof(T));
        }
        TransDataTo5HD<T>(dstOffsetArr, srcOffsetArr, transDataParams);
    }
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void C1c0dhwN1n0ToNcdhw(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LocalTensor<T>& tmp, const TransDataTmpParams& params)
{
    // C1C0DHW N1N0 -> N CDHW
    int32_t d = params.d;
    int32_t n1 = params.n1;
    int32_t padHw = params.padHw;
    int32_t currN = params.n;
    int32_t c = params.c;

    TransDataTo5HDParams transDataParams;
    transDataParams.dstHighHalf = false;
    transDataParams.srcHighHalf = false;
    transDataParams.repeatTimes = c * d * padHw / n0;
    if (transDataParams.repeatTimes == 1) {
        transDataParams.srcRepStride = 0;
        transDataParams.dstRepStride = 0;
    } else {
        transDataParams.srcRepStride = DEFAULT_TRANSDATA_5HD_LIST * n1 * n0 * sizeof(T) / ONE_BLK_SIZE;
        transDataParams.dstRepStride = c0 * sizeof(T) / ONE_BLK_SIZE;
    }

    uint64_t srcOffsetArr[DEFAULT_TRANSDATA_5HD_LIST];
    uint64_t dstOffsetArr[DEFAULT_TRANSDATA_5HD_LIST];
    uint64_t srcAddr = (uint64_t)src.GetPhyAddr();
    uint64_t dstAddr = (uint64_t)dst.GetPhyAddr();
    uint64_t tmpAddr = (uint64_t)tmp.GetPhyAddr();
    for (uint32_t j = 0; j < n1; j++) {
        if (n0 - currN > 0) {
            for (uint8_t i = 0; i < currN; i++) {
                dstOffsetArr[i] = (uint64_t)(dstAddr + (j * d * c * padHw * n0 + i * c * d * padHw) * sizeof(T));
            }
            for (uint8_t i = currN; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
                dstOffsetArr[i] = (uint64_t)(tmpAddr + i * ONE_BLK_SIZE * sizeof(T));
            }
        } else {
            for (uint8_t i = 0; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
                dstOffsetArr[i] = (uint64_t)(dstAddr + (j * d * c * padHw * n0 + i * c * d * padHw) * sizeof(T));
            }
        }
        for (uint8_t i = 0; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
            srcOffsetArr[i] = (uint64_t)(srcAddr + (j * n0 + i * n0 * n1) * sizeof(T));
        }
        TransDataTo5HD<T>(dstOffsetArr, srcOffsetArr, transDataParams);
        currN -= n0;
    }
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void N1n0C1c0DHWToNCDHW(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const TransDataTmpParams& params)
{
    // N1N0 C1C0 D H W -> N C D H W
    int32_t n = params.n;
    int32_t c = params.c;
    int32_t d = params.d;
    int32_t c1 = params.c1;
    int32_t padHw = params.padHw;

    uint16_t blockCount = n;
    uint16_t blockLen = (c * (d * padHw)) * sizeof(T) / ONE_BLK_SIZE;
    uint16_t srcGap = ((c1 * c0 - c) * (d * padHw)) * sizeof(T) /ONE_BLK_SIZE;
    uint16_t dstGap = 0;
    DataCopyParams dataCopyParams = { blockCount, blockLen, srcGap, dstGap };
    DataCopy(dst, src, dataCopyParams);
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void TransDataFractalToNcdhw(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LocalTensor<uint8_t>& tmpBuffer, const TransDataTmpParams& params)
{
    int32_t d = params.d;
    int32_t n1 = params.n1;
    int32_t c1 = params.c1;
    int32_t padHw = params.padHw;
    int32_t n = params.n;
    int32_t c = params.c;

    LocalTensor<half> tmp = tmpBuffer.template ReinterpretCast<half>();
    LocalTensor<half> srcTmp = src.template ReinterpretCast<half>();
    if (c == c1 * c0 && n == n1 * n0) {
        LocalTensor<half> dstTmp = dst.template ReinterpretCast<half>();
        // D C1 HWN1N0C0 -> C1 D HWN1N0C0 (H*W 32B ALIGN -> HW1*HW0)
        DC1Hwn1n0c0ToC1DHwn1n0c0HWAlign<half>(dstTmp, srcTmp, params);
        // C1 DHWN1N0 C0 -> C1 C0 DHWN1N0
        C1Dhwn1n0c0ToC1C0Dhwn1n0<half>(tmp, dstTmp, params);
        // C1C0DHW N1N0 -> N CDHW
        C1c0dhwN1n0ToNcdhw<half>(dstTmp, tmp, tmp, params);
    } else {
        LocalTensor<half> transDataTmp = tmp[n1 * n0 * c1 * c0 * d * padHw];
        LocalTensor<half> dstTmp = dst.template ReinterpretCast<half>();
        // D C1 HWN1N0C0 -> C1 D HWN1N0C0 (H*W 32B ALIGN -> HW1*HW0)
        DC1Hwn1n0c0ToC1DHwn1n0c0HWAlign<half>(tmp, srcTmp, params);
        // C1 DHWN1N0 C0 -> C1 C0 DHWN1N0
        C1Dhwn1n0c0ToC1C0Dhwn1n0<half>(transDataTmp, tmp, params);
        // C1C0DHW N1N0 -> N CDHW
        C1c0dhwN1n0ToNcdhw<half>(dstTmp, transDataTmp, tmp, params);
    }
}

// Transdata NCDHW -> FRACTAL_Z_3D
template <typename T>
__aicore__ inline void TransDataImplNcdhwToFractal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& tmpBuffer,
    const TransDataTmpParams& param)
{
    constexpr int32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    const int32_t n = param.n, c = param.c, d = param.d, h = param.h, w = param.w;
    constexpr int32_t c0 = 16;
    constexpr int32_t n0 = 16;
    const int32_t c1 = DivCeil(c, c0);
    const int32_t n1 = DivCeil(n, n0);
    int32_t padHw = AlignUp(h * w, elePerBlk);
    int32_t currAxis = c * d * padHw;
    int32_t tmpDupSize = currAxis;
    if (d * h * w * n1 * n0 > tmpDupSize) {
        tmpDupSize = d * h * w * n1 * n0;
    }
    Duplicate<T>(tmpBuffer.ReinterpretCast<T>(), static_cast<T>(0), tmpDupSize);
    PipeBarrier<PIPE_V>();
    auto tmpDstTensor = tmpBuffer[tmpDupSize * sizeof(T)].ReinterpretCast<T>();
    uint64_t dstLocalList[DEFAULT_TRANSDATA_5HD_LIST];
    uint64_t srcLocalList[DEFAULT_TRANSDATA_5HD_LIST];

    uint64_t dstTensorAddr = (uint64_t)dst.GetPhyAddr();
    uint64_t srcTensorAddr = (uint64_t)src.GetPhyAddr();
    uint64_t tmpDstTensorAddr = (uint64_t)tmpDstTensor.GetPhyAddr();
    uint64_t tmpBufferAddr = (uint64_t)tmpBuffer.GetPhyAddr();
    // step1, NCDHW -> CDHW, N1, N0
    // Do n1 times Transpose to split axis N, and fill with 0 on padding data.
    TransDataTo5HDParams transDataParams;
    transDataParams.dstHighHalf = false;
    transDataParams.srcHighHalf = false;
    transDataParams.repeatTimes = currAxis / elePerBlk;
    // if repeat = 1, start offset is auto incremental by stride.
    transDataParams.dstRepStride = transDataParams.repeatTimes == 1 ? 0 : n1 * n0;
    transDataParams.srcRepStride = transDataParams.repeatTimes == 1 ? 0 : 1;

    bool isPadded = padHw != h * w;
    // dst tensor is unable to fill all padded data.
    auto tmpIfPadAddr = isPadded ? tmpDstTensorAddr : dstTensorAddr;
    for (int j = 0; j < n1; j++) {
        uint64_t currDstAddr = tmpIfPadAddr + j * n0 * sizeof(T);
        uint64_t currSrcAddr = srcTensorAddr + j * currAxis * n0 * sizeof(T);
        // handle the last axis if N is not even splited by n0.
        int remain = j == n1 - 1 ? n - j * n0 : n0;
        for (int32_t i = 0; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
            dstLocalList[i] = currDstAddr +  (i * n1 * n0) * sizeof(T);
        }
        for (int32_t i = 0; i < remain; i++) {
            srcLocalList[i] = currSrcAddr + i * currAxis * sizeof(T);
        }
        for (int32_t i = remain; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
            srcLocalList[i] = tmpBufferAddr;
        }
        TransDataTo5HD<half>(dstLocalList, srcLocalList, transDataParams);
    }
    PipeBarrier<PIPE_V>();
    // step1.5 collapse padded H,W axis for CDHW, N1N0
    DataCopyParams copyParams;
    if (isPadded) {
        currAxis = h * w * n1 * n0;
        copyParams.blockCount = c * d;
        copyParams.blockLen = currAxis / elePerBlk;
        // Merge axis by skipping padded H,W.
        copyParams.srcStride = (padHw - h * w) * n1 * n0 / elePerBlk;
        copyParams.dstStride = 0;
        DataCopy(dst, tmpDstTensor, copyParams);
        PipeBarrier<PIPE_V>();
    }

    // step2, CDHWN1N0 -> C1DHW, N1N0, C0
    currAxis = d * h * w * n1 * n0;
    transDataParams.repeatTimes = currAxis / elePerBlk;
    transDataParams.dstRepStride = transDataParams.repeatTimes == 1 ? 0 : c0;
    transDataParams.srcRepStride = transDataParams.repeatTimes == 1 ? 0 : 1;
    for (int32_t j = 0; j < c1; j++) {
        uint64_t currDstAddr = tmpDstTensorAddr + j * currAxis * c0 * sizeof(T);
        uint64_t currSrcAddr = dstTensorAddr + j * currAxis * c0 * sizeof(T);
        int remain = j == c1 - 1 ? c - j * c0 : c0;
        for (int32_t i = 0; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
            dstLocalList[i] = currDstAddr + i * c0 * sizeof(T);
        }
        for (int32_t i = 0; i < remain; i++) {
            srcLocalList[i] = currSrcAddr + i * currAxis * sizeof(T);
        }
        for (int32_t i = remain; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
            srcLocalList[i] = tmpBufferAddr;
        }
        TransDataTo5HD<half>(dstLocalList, srcLocalList, transDataParams);
    }
    PipeBarrier<PIPE_V>();
    // step3 C1DHW, N1N0, C0 -> DC1HW, N1N0, C0
    currAxis = c0 * h * w * n1 * n0;
    copyParams.blockCount = d;
    copyParams.blockLen = currAxis / elePerBlk;
    // Merge axis by skipping padding padHW -> h, w
    copyParams.srcStride = 0;
    copyParams.dstStride = (c1 - 1) * currAxis / elePerBlk;
    for (int32_t i = 0; i < c1; i++) {
        DataCopy(dst[i * currAxis], tmpDstTensor[i * d * currAxis], copyParams);
    }
    PipeBarrier<PIPE_V>();
}

// Transdata NCDHW -> NDC1HWC0
template <typename T>
__aicore__ inline void TransDataImplNcdhwTo6Hd(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& tmpBuffer,
    const TransDataTmpParams& param)
{
    constexpr int32_t c0 = 16;
    constexpr int32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    const int32_t n = param.n, c = param.c, d = param.d, h = param.h, w = param.w;
    const int32_t c1 = DivCeil(c, c0);
    const int32_t padHw = AlignUp(h * w, elePerBlk);
    int32_t currAxis = d * padHw;

    int32_t axisHwd = h * w * d;
    int32_t axisHwc0 = h * w * c0;
    int32_t axisC1hwc0 = axisHwc0 * c1;
    int32_t axisC1hwdc0 = axisC1hwc0 * d;
    int32_t axisPadHwd = padHw * d;
    int32_t axisPadHwc0 = padHw * c0;
    int32_t axisPadHwdc0 = padHw * c0 * d;
    Duplicate<T>(tmpBuffer.ReinterpretCast<T>(), static_cast<T>(0), axisPadHwd);
    PipeBarrier<PIPE_V>();

    // reserve for padded 0 on additional axis c.
    auto tmpDstTensor = tmpBuffer[axisPadHwd * sizeof(T)].ReinterpretCast<T>();

    uint64_t dstTensorAddr = (uint64_t)dst.GetPhyAddr();
    uint64_t srcTensorAddr = (uint64_t)src.GetPhyAddr();
    uint64_t tmpDstTensorAddr = (uint64_t)tmpDstTensor.GetPhyAddr();
    uint64_t tmpBufferAddr = (uint64_t)tmpBuffer.GetPhyAddr();
    uint64_t dstLocalList[DEFAULT_TRANSDATA_5HD_LIST];
    uint64_t srcLocalList[DEFAULT_TRANSDATA_5HD_LIST];
    TransDataTo5HDParams transDataParams;
    transDataParams.dstHighHalf = false;
    transDataParams.srcHighHalf = false;
    transDataParams.repeatTimes = axisPadHwd / elePerBlk;
    transDataParams.dstRepStride = transDataParams.repeatTimes == 1 ? 0 : c0;
    transDataParams.srcRepStride = transDataParams.repeatTimes == 1 ? 0 : 1;

    DataCopyParams copyParams;
    copyParams.blockCount = d;
    copyParams.blockLen = axisHwc0 / elePerBlk;
    copyParams.srcStride = (padHw - h * w) * c0 / elePerBlk;
    copyParams.dstStride = (c1 - 1) * axisHwc0 / elePerBlk;
    // iterates N times CDHW -> C1DHWC0
    for (int32_t k = 0; k < n; k++) {
        int32_t currSrcStart = k * axisPadHwd * c;
        int32_t currDstStart = k * axisC1hwdc0;
        // it's impossible to have calculation size exceed max 255 repeats due to the total memory size.
        // step1, CDHW -> C1DHWC0 with pad data
        for (int32_t j = 0; j < c1; j++) {
            uint64_t currDstAddr = tmpDstTensorAddr + j * axisPadHwdc0 * sizeof(T);
            uint64_t currSrcAddr = srcTensorAddr + (currSrcStart + j * axisPadHwdc0) * sizeof(T);
            int remain = j == c1 - 1 ? c - j * c0 : c0;
            for (int32_t i = 0; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
                dstLocalList[i] = currDstAddr + i * c0 * sizeof(T);
            }
            for (int32_t i = 0; i < remain; i++) {
                srcLocalList[i] = currSrcAddr + i * axisPadHwd * sizeof(T);
            }
            for (int32_t i = remain; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
                srcLocalList[i] = tmpBufferAddr;
            }
            TransDataTo5HD<half>(dstLocalList, srcLocalList, transDataParams);
        }
        PipeBarrier<PIPE_V>();
        // step2, C1DHWC0 -> DC1HWC0
        for (int32_t i = 0; i < c1; i++) {
            DataCopy(dst[currDstStart + i * axisHwc0], tmpDstTensor[i * axisPadHwdc0], copyParams);
        }
        PipeBarrier<PIPE_V>();
    }
}

// Transdata NDC1HWC0 -> NCDHW
template <typename T>
__aicore__ inline void TransDataImpl6HdToNcdhw(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& tmpBuffer,
    const TransDataTmpParams& param)
{
    const int32_t n = param.n, c = param.c, d = param.d, h = param.h, w = param.w;
    constexpr int32_t c0 = 16;
    constexpr int32_t elePerBlk = ONE_BLK_SIZE / sizeof(T);
    const int32_t c1 = DivCeil(c, c0);
    const int32_t padHw = AlignUp(h * w, elePerBlk);
    constexpr int32_t reservedDummy = 512;
    auto tmpDstTensor = tmpBuffer[reservedDummy].template ReinterpretCast<T>();
    uint64_t dstLocalList[DEFAULT_TRANSDATA_5HD_LIST];
    uint64_t srcLocalList[DEFAULT_TRANSDATA_5HD_LIST];

    uint64_t dstTensorAddr = (uint64_t)dst.GetPhyAddr();
    uint64_t tmpDstTensorAddr = (uint64_t)tmpDstTensor.GetPhyAddr();
    uint64_t tmpBufferAddr = (uint64_t)tmpBuffer.GetPhyAddr();

    int32_t axisHwd = h * w * d;
    int32_t axisHwc0 = h * w * c0;
    int32_t axisC1hwc0 = axisHwc0 * c1;
    int32_t axisC1hwdc0 = axisC1hwc0 * d;
    int32_t axisPadHwd = padHw * d;
    int32_t axisPadHwc0 = padHw * c0;
    int32_t axisPadHwdc0 = padHw * c0 * d;
    TransDataTo5HDParams transDataParams;
    transDataParams.dstHighHalf = false;
    transDataParams.srcHighHalf = false;
    transDataParams.repeatTimes = padHw * d / elePerBlk;
    transDataParams.srcRepStride = transDataParams.repeatTimes == 1 ? 0 : c0;
    transDataParams.dstRepStride = transDataParams.repeatTimes == 1 ? 0 : 1;

    DataCopyParams copyParams;
    copyParams.blockCount = c1;
    copyParams.blockLen = h * w * c0 / elePerBlk;
    copyParams.srcStride = 0;
    copyParams.dstStride = (d * padHw - h * w) * c0 / elePerBlk;
    // iterates N times C1DHWC0 -> CDHW
    for (int32_t k = 0; k < n; k++) {
        // step1 DC1HWC0 -> C1DHWC0
        int32_t currSrcStart = k * axisC1hwdc0;
        int32_t currDstStart = k * axisPadHwd * c;
        for (int32_t i = 0; i < d; i++) {
            DataCopy(tmpDstTensor[i * axisPadHwc0], src[currSrcStart + i * axisC1hwc0], copyParams);
        }
        PipeBarrier<PIPE_V>();
        // step2, C1DHWC0 -> C1C0DHW
        // it's impossible to have calculation size exceed max 255 repeats due to the total memory size.
        for (int32_t j = 0; j < c1; j++) {
            int32_t remain = j == c1 - 1 ? c - j * c0 : c0;
            uint64_t currDstAddr = dstTensorAddr + (currDstStart + j * axisPadHwdc0) * sizeof(T);
            uint64_t currSrcAddr = tmpDstTensorAddr + j * axisPadHwdc0 * sizeof(T);
            for (int32_t i = 0; i < remain; i++) {
                dstLocalList[i] = currDstAddr + i * axisPadHwd * sizeof(T);
            }
            for (int32_t i = remain; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
                // temp for reserve redundant data.
                dstLocalList[i] = tmpBufferAddr + i * ONE_BLK_SIZE;
            }
            for (int32_t i = 0; i < DEFAULT_TRANSDATA_5HD_LIST; i++) {
                srcLocalList[i] = currSrcAddr + i * c0 * sizeof(T);
            }
            TransDataTo5HD<half>(dstLocalList, srcLocalList, transDataParams);
        }
        PipeBarrier<PIPE_V>();
    }
}

template <typename T, typename U, typename S>
__aicore__ inline void TransDataCheck(const TransDataParams<U, S>& params)
{
    static_assert(SupportType<T, half, bfloat16_t, uint16_t, int16_t>(),
        "Current only supports half/bfloat16_t/uint16_t/int16_t types.");
    static_assert(is_layout_v<U>, "srcLayout must be a layout");
    static_assert(is_layout_v<S>, "dstLayout must be a layout");
    using SrcShapeTuple = Std::remove_cvref_t<decltype(params.srcLayout.GetShape())>;
    using DstShapeTuple = Std::remove_cvref_t<decltype(params.dstLayout.GetShape())>;
    static_assert(Std::is_tuple_v<SrcShapeTuple>, "srcLayout.GetShape() must be a shape.");
    static_assert(Std::is_tuple_v<DstShapeTuple>, "dstLayout.GetShape() must be a shape.");
}

template <const TransDataConfig& config, typename T, typename U, typename S>
__aicore__ inline void TransDataImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const TransDataParams<U, S>& params)
{
    TransDataCheck<T, U, S>(params);
    auto srcShape = params.srcLayout.GetShape();
    auto dstShape = params.dstLayout.GetShape();
    constexpr uint32_t srcShapeSize = static_cast<uint32_t>(Std::tuple_size<decltype(srcShape)>::value);
    constexpr uint32_t dstShapeSize = static_cast<uint32_t>(Std::tuple_size<decltype(dstShape)>::value);
    CHECK_FUNC_HIGHLEVEL_API(TransData, (config, T, U, S), (dstTensor, srcTensor, sharedTmpBuffer, params));
    using srcType = decltype(srcShape);
    using dstType = decltype(dstShape);
    using ncdhwType = Std::conditional_t<config.srcFormat == DataFormat::NCDHW, srcType, dstType>;
    ncdhwType ncdhwShape;
    if constexpr (config.srcFormat == DataFormat::NCDHW) {
        ncdhwShape = params.srcLayout.GetShape();
    } else {
        ncdhwShape = params.dstLayout.GetShape();
    }
    int32_t n = Std::get<0>(ncdhwShape);
    int32_t c = Std::get<1>(ncdhwShape);
    int32_t d = Std::get<2>(ncdhwShape);
    int32_t h = Std::get<3>(ncdhwShape);
    int32_t w = Std::get<4>(ncdhwShape);
    int32_t n1 = (n + n0 - 1) / n0;
    int32_t c1 = (c + c0 - 1) / c0;
    int32_t hw1 = (h * w + hw0 - 1) / hw0;
    int32_t padHw = hw1 * hw0;
    TransDataTmpParams tmpParams = { n, c, d, h, w, n1, c1, padHw };
    if constexpr (config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::FRACTAL_Z_3D) {
        static_assert(srcShapeSize == ncdhwDims, "srcLayout's shape dims must be equal to 5!");
        static_assert(dstShapeSize == fractalZ3DDims, "dstLayout's shape dims must be equal to 7!");
        TransDataImplNcdhwToFractal(dstTensor, srcTensor, sharedTmpBuffer, tmpParams);
    } else if constexpr (config.srcFormat == DataFormat::FRACTAL_Z_3D && config.dstFormat == DataFormat::NCDHW) {
        static_assert(srcShapeSize == fractalZ3DDims, "srcLayout's shape dims must be equal to 7!");
        static_assert(dstShapeSize == ncdhwDims, "dstLayout's shape dims must be equal to 5!");
        TransDataFractalToNcdhw<T>(dstTensor, srcTensor, sharedTmpBuffer, tmpParams);
    } else if constexpr (config.srcFormat == DataFormat::NCDHW && config.dstFormat == DataFormat::NDC1HWC0) {
        static_assert(srcShapeSize == ncdhwDims, "srcLayout's shape dims must be equal to 5!");
        static_assert(dstShapeSize == ndc1hwc0Dims, "dstLayout's shape dims must be equal to 6!");
        TransDataImplNcdhwTo6Hd(dstTensor, srcTensor, sharedTmpBuffer, tmpParams);
    } else if constexpr (config.srcFormat == DataFormat::NDC1HWC0 && config.dstFormat == DataFormat::NCDHW) {
        static_assert(srcShapeSize == ndc1hwc0Dims, "srcLayout's shape dims must be equal to 6!");
        static_assert(dstShapeSize == ncdhwDims, "dstLayout's shape dims must be equal to 5!");
        TransDataImpl6HdToNcdhw(dstTensor, srcTensor, sharedTmpBuffer, tmpParams);
    }
}

} // namespace Internal
} // namespace AscendC
#endif // IMPL_TRANSPOSE_TRANSDATA_TRANSDATA_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_TRANSDATA_TRANSDATA_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_TRANSDATA_TRANSDATA_IMPL_H__
#endif
