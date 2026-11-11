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
 * \file conv_bp_input_mte2_sub_func.h
 * \brief
 */

#ifndef IMPL_CONV3D_BP_INPUT_MTE2_SUB_FUNC_H
#define IMPL_CONV3D_BP_INPUT_MTE2_SUB_FUNC_H

#include "conv3d_bp_kernel_split.h"

namespace ConvBackpropInputFunc {
constexpr int32_t MAX_16BITS_STRIDE = 65535;

// Set fmatrix parameters
template <class Intf>
__aicore__ inline void CalcSetFmatrixParams(Intf *self, uint32_t fmapH, uint32_t fmapW)
{
    // W
    self->ctx.load3d_.l1W = fmapW;
    // H
    self->ctx.load3d_.l1H = fmapH;
    // Set pad, pad list [left, right, top, down], default is 0, range [0, 255]

    if (self->ctx.curHoIdx_ < 0) {
        self->ctx.load3d_.padList[2] = abs(self->ctx.curHoIdx_);
    } else {
        self->ctx.load3d_.padList[2] = 0;
    }
}

template <class Intf>
static __aicore__ inline uint32_t CalcHDstDataSkipLine(Intf *self)
{
    uint32_t strideH = self->ctx.tiling_->strideH;
    uint32_t hDstDataSkipLine = 0;
    uint32_t actualDstDataStartIdx = (self->ctx.curHoIdx_ < 0) ? 0 : self->ctx.curHoIdx_;
    if (strideH > 1 && actualDstDataStartIdx % strideH) {
        hDstDataSkipLine = ((actualDstDataStartIdx + (strideH - 1)) / strideH) * strideH;
        hDstDataSkipLine = hDstDataSkipLine - actualDstDataStartIdx;
    }
    return hDstDataSkipLine;
}

template <class Intf, class src0_T>
__aicore__ inline void CalcLoadToA1DataCopyParams(Intf *self, AscendC::DataCopyParams &dataCopyParams,
    uint32_t &loadToA1Cout1Loop, uint32_t &loadToA1HLoop, uint64_t &srcDataStride, uint32_t &dstDataStride,
    uint32_t &padOffset, uint32_t &curHoSize, uint32_t curCout1Idx) 
{
    uint32_t curCout1Size = 0;
    if constexpr (Intf::conv3dConfig.loadB2Condition == ConvBackpropApi::B2Condition::HKWK_EQ_ONE) {
        curCout1Size = self->ctx.curLoadKal1_ >> self->ctx.tiling_->c0Bits;
    } else {
        curCout1Size = self->ctx.curLoadKal1_ / self->ctx.HkWkC0_;
    }
    curCout1Size = (curCout1Size < self->ctx.singleShapeCout1_ - curCout1Idx) ?
                    curCout1Size : self->ctx.singleShapeCout1_ - curCout1Idx;
    uint32_t hDstDataSkipLine = CalcHDstDataSkipLine(self);
    uint32_t strideH = self->ctx.tiling_->strideH;
    uint32_t woExpand = self->ctx.tiling_->wo;
    uint32_t strideW = self->ctx.tiling_->strideW;
    if (curHoSize <= hDstDataSkipLine) {
        dataCopyParams.blockCount = 0;
    } else if (strideW > 1) {
        // One datacopy, copy wo c0, each c0 is at the destination address interval stride W-1, a total of copies
        dataCopyParams.blockLen = 1;
        dataCopyParams.blockCount = self->ctx.tiling_->wo;
        dataCopyParams.dstStride = (strideW - 1);
        loadToA1Cout1Loop = curCout1Size;
        loadToA1HLoop = (curHoSize - hDstDataSkipLine + (strideH - 1)) / strideH;
        srcDataStride = (self->ctx.tiling_->wo * self->ctx.tiling_->c0);
        woExpand = ((woExpand - 1) * strideW) + 1;
        dstDataStride = woExpand * self->ctx.tiling_->c0;
        padOffset = hDstDataSkipLine * dstDataStride;
    } else {
        loadToA1HLoop = 1;
        if (strideH > 1) {  // Datacoy once, copy curHo*Wo c0, copy CurCout 1 time
            uint32_t curInputHoSize = (curHoSize - hDstDataSkipLine + (strideH - 1)) / strideH;
            dataCopyParams.srcStride = 0;
            // Currently does not support (strideH - 1) * self->ctx.tiling_->wo scenarios exceeding uint16_max
            dataCopyParams.dstStride = (strideH - 1) * self->ctx.tiling_->wo;
            dataCopyParams.blockLen = self->ctx.tiling_->wo;
            dataCopyParams.blockCount = curInputHoSize;
            uint32_t wDataStride = self->ctx.tiling_->wo * self->ctx.tiling_->c0;
            // Prevent Ho from being too large and uint32 to overflow
            srcDataStride = self->ctx.hwO_ * self->ctx.tiling_->c0;
            dstDataStride = wDataStride;
            loadToA1Cout1Loop = curCout1Size;
            // Due to the limitations of strideH and Wo, there will be no overflow here

            padOffset = hDstDataSkipLine * wDataStride;
        } else {
            // The interval between the tail and the head of the original data
            uint64_t dataCoptSrcStride = static_cast<uint64_t>(self->ctx.tiling_->ho - curHoSize) * self->ctx.tiling_->wo;
            dataCopyParams.dstStride = 0;  // The interval between the tail and the head of the destination data
            dataCopyParams.blockLen = curHoSize * self->ctx.tiling_->wo;
            if (dataCoptSrcStride <= UINT16_MAX) {
                dataCopyParams.srcStride = static_cast<uint16_t>(dataCoptSrcStride);
                dataCopyParams.blockCount = curCout1Size;
                loadToA1Cout1Loop = 1;
            } else {
                // due to srcStride should not over uint16_max, so we have to move data one by one
                dataCopyParams.srcStride = 0;
                dataCopyParams.blockCount = 1;
                loadToA1Cout1Loop = curCout1Size;
                dstDataStride = self->ctx.tiling_->wo * self->ctx.tiling_->c0;
            }
        }
    }
    CalcSetFmatrixParams(self, curHoSize, woExpand);
}

template <class Intf, class src0_T>
__aicore__ inline void LoadToA1(Intf *self, uint32_t kIdx, uint32_t curDoutIdx, bool pingPongFlag, bool loadFlag)
{
    if (loadFlag) {
        AscendC::LocalTensor<typename Intf::SrcT> useA1Buf;
        if (pingPongFlag) {
            useA1Buf = self->ctx.a1Ping_.template AllocTensor<typename Intf::SrcT>();
        } else {
            useA1Buf = self->ctx.a1Pong_.template AllocTensor<typename Intf::SrcT>();
        }

        if constexpr (!Intf::conv3dConfig.enableKernelSplit) {
            if (unlikely(self->ctx.tiling_->strideW * self->ctx.tiling_->strideH > 1)) {
                // block num is in 32B units, 5 is used to replace the division operation
                uint32_t len = useA1Buf.GetSize() * sizeof(typename Intf::SrcT);
                InitConstValue(useA1Buf, {1, static_cast<uint16_t>(len >> 5), 0, 0U});
                AscendC::PipeBarrier<PIPE_MTE2>();
            }
        }

        uint32_t curCout1Idx = 0;
        if constexpr (Intf::conv3dConfig.enableKernelSplit) {
            curCout1Idx = kIdx * self->ctx.tiling_->baseK / self->ctx.splitHkWkC0_;
        } else {
            curCout1Idx = kIdx * self->ctx.tiling_->baseK / self->ctx.HkWkC0_;
        }

        AscendC::DataCopyParams dataCopyParams;
        uint32_t loadToA1Cout1Loop = 0;
        uint32_t loadToA1HLoop = 0;
        uint64_t srcDataStride = 0;
        uint32_t dstDataStride = 0;
        uint32_t padDataOffset = 0;
        uint32_t curHoSize = self->ctx.curHoSize_;

        if constexpr (Intf::conv3dConfig.enableKernelSplit) {
            CalcLoadToA1ParamsForKernelSplit<Intf, typename Intf::SrcT>(self, dataCopyParams, loadToA1Cout1Loop,
                loadToA1HLoop, srcDataStride, dstDataStride, padDataOffset, curHoSize, curCout1Idx);
            CalcSetFmatrixParams(self, curHoSize, self->ctx.tiling_->wo - 1);
        } else {
            CalcLoadToA1DataCopyParams<Intf, typename Intf::SrcT>(self, dataCopyParams, loadToA1Cout1Loop,
                loadToA1HLoop, srcDataStride, dstDataStride, padDataOffset, curHoSize, curCout1Idx);
        }
        if (dataCopyParams.blockCount > 0) {
            // The absolute address of GM has been calculated outside the API. When absolute coordinates are used here, the starting coordinate needs to be removed
            int64_t curHoStartOffset = self->ctx.curHoStartIdx_ < 0 ? 0 : self->ctx.curHoStartIdx_;
            int64_t curHoIdx = self->ctx.curHoIdx_ < 0 ? 0 : (self->ctx.curHoIdx_ - curHoStartOffset);
            // Convert back to the relative Ho coordinates before amplification (taking single-core HoStartIdx as the origin)
            int64_t curOriHoIdx = curHoIdx;

            if (self->ctx.tiling_->strideH > 1) {
                int64_t skipHoSize = curHoStartOffset % self->ctx.tiling_->strideH;
                skipHoSize = skipHoSize > 0 ? (self->ctx.tiling_->strideH - skipHoSize) : skipHoSize;
                curHoIdx = self->ctx.curHoIdx_ < 0 ? 0 : (self->ctx.curHoIdx_ - curHoStartOffset);
                // Convert back to the relative Ho coordinates before amplification (taking single-core HoStartIdx as the origin)
                curOriHoIdx = (curHoIdx - skipHoSize + self->ctx.tiling_->strideH - 1) / self->ctx.tiling_->strideH;
            }

            int64_t hoStride = self->ctx.tiling_->wo * self->ctx.tiling_->c0;
            int64_t coutStride = self->ctx.tiling_->ho * hoStride;
            int64_t out2A1SrcAddrOffsetBase =
                (curDoutIdx * self->ctx.tiling_->cout1 + curCout1Idx) * coutStride + curOriHoIdx * hoStride;
            int64_t dstOffsetC = padDataOffset;
            int64_t dstDataStrideC = curHoSize * dstDataStride;

            for (int64_t j = 0; j < loadToA1Cout1Loop; j++) {
                int64_t out2A1SrcAddrOffset = out2A1SrcAddrOffsetBase;
                int64_t dstOffset = dstOffsetC;
                for (int64_t i = 0; i < loadToA1HLoop; i++) {
                    DataCopy(useA1Buf[dstOffset], self->ctx.outBackPropGlobal_[out2A1SrcAddrOffset], dataCopyParams);
                    out2A1SrcAddrOffset += srcDataStride;
                    dstOffset += (dstDataStride * self->ctx.tiling_->strideH);
                }
                dstOffsetC += dstDataStrideC;
                out2A1SrcAddrOffsetBase += coutStride;
            }
        }

        if (pingPongFlag) {
            self->ctx.a1Ping_.EnQue(useA1Buf);
        } else {
            self->ctx.a1Pong_.EnQue(useA1Buf);
        }
    }
}

template <class Intf, class src1_T>
__aicore__ inline void LoadToB1Fp32(Intf *self, const uint32_t kIdx, const uint32_t curDkIdx,
                                    AscendC::LocalTensor<typename Intf::SrcT> &useB1Buf, bool b1PingPongFlag)
{
    // Here is the C0in of the original input, related to dataType: 16 for bf16, 8 for fp32
    uint32_t curCin1Idx = self->ctx.curNL1Idx_ * self->ctx.curCin1Size_;

    // C0 here has nothing to do with dataType, but is related to NZ type
    uint32_t curCout1Idx = kIdx * self->ctx.tiling_->baseK / (self->ctx.HkWk_ * AscendC::BLOCK_CUBE);
    // Record the absolute Cout coordinates loaded into L1 each time, which is used to calculate the offset of load3dv2 in the M direction, involving a 1:2 data loading problem
    if (b1PingPongFlag) {
        self->ctx.curPingCoutIdx_ = curCout1Idx;
    } else {
        self->ctx.curPongCoutIdx_ = curCout1Idx;
    }

    // kernel shape: (dk * cin1 * hk * wk, cout1, cout0, cin0)
    // fp32 scene Cout may not be aligned to 16 and needs to be aligned to 16
    uint64_t out2B1SrcAddrOffset = (static_cast<uint64_t>(curDkIdx) * self->ctx.tiling_->cin1G + curCin1Idx) *
                                    self->ctx.HkWkC0_ * self->ctx.alignedCout_ +   // Related to dataType
                                    static_cast<uint64_t>(curCout1Idx) * AscendC::BLOCK_CUBE * self->ctx.tiling_->c0;  // Related to NZ Fractal

    // When the K direction is not 16-aligned, the original GM data needs to be padded to 16-aligned, and may contain padding data
    // When moving data, you need to consider padding data
    uint32_t curCin1Size = self->ctx.curCin1Size_ < (self->ctx.singleShapeCin1_ - curCin1Idx) ?
                           self->ctx.curCin1Size_ : (self->ctx.singleShapeCin1_ - curCin1Idx);
    uint32_t curCout1Size = DivCeil(self->ctx.curLoadKbl1_ / self->ctx.HkWkC0_, 2) * AscendC::BLOCK_CUBE;
    // Since L1B is fully loaded with HkWKC0, the tiling side has guaranteed HkWKC0 * curCin1Size * dataByte <= L1B, assuming that L1B is up to 512KB
    // Then HkWK * curCin1Size <= 16384, will not exceed uint16_max
    uint16_t blockCount = curCin1Size * self->ctx.HkWk_;

    AscendC::DataCopyParams dataCopyParams;
    dataCopyParams.blockLen = (curCout1Size < (self->ctx.alignedCout1_ - curCout1Idx) * AscendC::BLOCK_CUBE) ?
                               curCout1Size : ((self->ctx.alignedCout1_ - curCout1Idx) * AscendC::BLOCK_CUBE);
    dataCopyParams.dstStride = 0;
    uint64_t srcStride = self->ctx.alignedCout_ - dataCopyParams.blockLen;
    if (srcStride <= MAX_16BITS_STRIDE) {
        dataCopyParams.blockCount = blockCount;
        dataCopyParams.srcStride = static_cast<uint16_t>(srcStride);
        DataCopy(useB1Buf, self->ctx.weightGlobal_[out2B1SrcAddrOffset], dataCopyParams);
    } else {
        dataCopyParams.blockCount = 1;
        uint64_t srcOffsetInterval = self->ctx.alignedCout_ * self->ctx.tiling_->c0;
        uint64_t dstOffsetInterval = dataCopyParams.blockLen * self->ctx.tiling_->c0;
        uint64_t dstOffset = 0;
        for (uint32_t idx = 0; idx < blockCount; ++idx) {
            DataCopy(useB1Buf[dstOffset], self->ctx.weightGlobal_[out2B1SrcAddrOffset], dataCopyParams);
            out2B1SrcAddrOffset += srcOffsetInterval;
            dstOffset += dstOffsetInterval;
        }
    }
}

template <class Intf, class src1_T>
__aicore__ inline void LoadToB1BF16(Intf *self, const uint32_t kIdx, const uint32_t curDkIdx, AscendC::LocalTensor<typename Intf::SrcT> &useB1Buf)
{
    uint32_t curCin1Idx = self->ctx.curNL1Idx_ * self->ctx.curCin1Size_;
    uint32_t curCout1Idx = 0;
    uint32_t curCout1Size = 0;
    if constexpr (Intf::conv3dConfig.loadB2Condition == ConvBackpropApi::B2Condition::HKWK_EQ_ONE) {
        curCout1Idx = kIdx * self->ctx.tiling_->baseK >> self->ctx.tiling_->c0Bits;
        curCout1Size = self->ctx.curLoadKbl1_ >> self->ctx.tiling_->c0Bits;
    } else {
        curCout1Idx = kIdx * self->ctx.tiling_->baseK / self->ctx.HkWkC0_;
        curCout1Size = self->ctx.curLoadKbl1_ / self->ctx.HkWkC0_;
    }

    // kernel shape: (dk * cin1 * hk * wk, cout1, cout0, cin0)
    // group kernel shape: (group * dk * cin1G * hk * wk, cout1G, cout0, cin0)
    uint64_t out2B1SrcAddrOffset = (static_cast<uint64_t>(curDkIdx) * self->ctx.tiling_->cin1G + curCin1Idx) *
                                   self->ctx.HkWkC0_ * self->ctx.tiling_->cout1G * self->ctx.tiling_->c0 +
                                   static_cast<uint64_t>(curCout1Idx) * self->ctx.tiling_->c0 * self->ctx.tiling_->c0;
    AscendC::DataCopyParams dataCopyParams;
    dataCopyParams.blockLen = (curCout1Size < self->ctx.singleShapeCout1_ - curCout1Idx ?
                               curCout1Size : self->ctx.singleShapeCout1_ - curCout1Idx) * self->ctx.tiling_->c0;
    dataCopyParams.dstStride = 0;
    uint32_t curCin1Size = self->ctx.curCin1Size_ < (self->ctx.singleShapeCin1_ - curCin1Idx) ?
                           self->ctx.curCin1Size_ : (self->ctx.singleShapeCin1_ - curCin1Idx);
    uint16_t blockCount = curCin1Size * self->ctx.HkWk_;
    uint64_t srcStride = static_cast<uint64_t>(self->ctx.tiling_->cout1G) * self->ctx.tiling_->c0 -
                         dataCopyParams.blockLen;

    if (srcStride <= MAX_16BITS_STRIDE) {
        dataCopyParams.blockCount = blockCount;
        dataCopyParams.srcStride = static_cast<uint16_t>(srcStride);
        DataCopy(useB1Buf, self->ctx.weightGlobal_[out2B1SrcAddrOffset], dataCopyParams);
    } else {
        dataCopyParams.blockCount = 1;
        uint64_t srcOffsetInterval = static_cast<uint64_t>(self->ctx.tiling_->cout1G) * self->ctx.tiling_->c0 *
                                     self->ctx.tiling_->c0;
        uint32_t dstOffsetInterval = static_cast<uint32_t>(dataCopyParams.blockLen) * self->ctx.tiling_->c0;
        uint64_t dstOffset = 0;
        for (uint32_t idx = 0; idx < blockCount; ++idx) {
            DataCopy(useB1Buf[dstOffset], self->ctx.weightGlobal_[out2B1SrcAddrOffset], dataCopyParams);
            out2B1SrcAddrOffset += srcOffsetInterval;
            dstOffset += dstOffsetInterval;
        }
    }
}

template <class Intf, class src1_T>
__aicore__ inline void LoadToB1(Intf *self, uint32_t kIdx, uint32_t curDkIdx, bool pingPongFlag, bool loadFlag)
{
    if (unlikely(self->ctx.isB1FullLoadFlag_ && !self->ctx.isLoadB1_)) {
        return;
    }

    if (loadFlag) {
        AscendC::LocalTensor<typename Intf::SrcT> useB1Buf;
        if (pingPongFlag) {
            useB1Buf = self->ctx.b1Ping_.template AllocTensor<typename Intf::SrcT>();
        } else {
            useB1Buf = self->ctx.b1Pong_.template AllocTensor<typename Intf::SrcT>();
        }

        if constexpr (Intf::conv3dConfig.enableKernelSplit) {
            LoadToB1ForKernelSplit<Intf>(self, kIdx, curDkIdx, useB1Buf);
        } else {
            if constexpr ((std::is_same<typename Intf::SrcT, bfloat16_t>::value) || (std::is_same<typename Intf::SrcT, half>::value)) {
                LoadToB1BF16<Intf, src1_T>(self, kIdx, curDkIdx, useB1Buf); // FP16 also reuses this function
            } else if constexpr (std::is_same<typename Intf::SrcT, float>::value) {
                LoadToB1Fp32<Intf, src1_T>(self, kIdx, curDkIdx, useB1Buf, pingPongFlag);
            }
        }

        if (pingPongFlag) {
            self->ctx.b1Ping_.EnQue(useB1Buf);
        } else {
            self->ctx.b1Pong_.EnQue(useB1Buf);
        }
    }
}
}

#endif
