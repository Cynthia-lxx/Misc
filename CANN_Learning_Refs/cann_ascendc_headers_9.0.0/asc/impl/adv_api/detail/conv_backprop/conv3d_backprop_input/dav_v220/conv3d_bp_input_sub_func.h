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
 * \file conv_bp_input_sub_func.h
 * \brief
 */

#ifndef IMPL_CONV3D_BP_INPUT_SUB_FUNC_H
#define IMPL_CONV3D_BP_INPUT_SUB_FUNC_H

#include "conv3d_bp_kernel_split.h"

namespace ConvBackpropInputFunc {

template <class Intf>
__aicore__ inline uint32_t CalFmapH(Intf *self, uint32_t mL1Size)
{
    uint32_t hiCal;
    if (mL1Size >= self->ctx.tiling_->wi) {
        hiCal = mL1Size / self->ctx.tiling_->wi;
        if (mL1Size != hiCal * self->ctx.tiling_->wi) {
            hiCal += 2;
        }
    } else {
        hiCal = self->ctx.tiling_->wi % mL1Size == 0 ? 1 : 2;
    }
    uint32_t khDilation = (self->ctx.tiling_->hk - 1) * self->ctx.tiling_->dilationH + 1;
    return (hiCal - 1) + khDilation;
}

template <class Intf>
__aicore__ inline void InitMmadParams(Intf *self)
{
    self->ctx.mmad_.m = self->ctx.baseUseM_;
    if (unlikely(self->ctx.curML0Idx_ == self->ctx.mIter_ - 1)) {
        // 4 is used to replace the division operation
        self->ctx.mmad_.m = ((self->ctx.baseUseM_ + AscendC::BLOCK_CUBE - 1) >> 4) * AscendC::BLOCK_CUBE;
    }
    self->ctx.mmad_.k = self->ctx.tiling_->baseK;
    self->ctx.mmad_.n = self->ctx.baseUseN_;
    self->ctx.mmad_.unitFlag = 0;
    self->ctx.mmad_.kDirectionAlign = 0;
    self->ctx.mmad_.cmatrixSource = 0;
    self->ctx.mmad_.cmatrixInitVal = 1;
}

template <class Intf>
__aicore__ inline void MmadLocal(Intf *self, const AscendC::LocalTensor<typename Intf::SrcT> &l0a,
                                        const AscendC::LocalTensor<typename Intf::SrcT> &l0b,
                                        AscendC::LocalTensor<typename Intf::L0cT> &l0c)
{
    // When the MMAD calculation amount baseM*baseN is less than a certain threshold, PIPE_M synchronization needs to be added. The current platform threshold is 10*256
    constexpr int32_t mmadThreshold = 10 * 256;
    if (self->ctx.mmad_.m * self->ctx.mmad_.n <= mmadThreshold) {
        AscendC::PipeBarrier<PIPE_M>();
    }
    MmadImpl(l0c, l0a, l0b, self->ctx.mmad_);
}

// Calculate the instruction parameters of Load2A2
template <class Intf>
__aicore__ inline void InitLoadToA2Params(Intf *self)
{
    // load3dStepM
    self->ctx.load3d_.mExtension = self->ctx.tiling_->baseM;
    // load3dStepK
    self->ctx.load3d_.kExtension = self->ctx.tiling_->baseK;
    // posM
    self->ctx.load3d_.mStartPt = 0;
    // posK
    self->ctx.load3d_.kStartPt = 0;
    // Pre-amplification preprocessing and uniform conversion to stride=1 operation
    self->ctx.load3d_.strideW = 1;
    self->ctx.load3d_.strideH = 1;
    if constexpr (Intf::conv3dConfig.enableKernelSplit) {
        self->ctx.load3d_.filterW = self->ctx.splitWk_;
        self->ctx.load3d_.filterH = self->ctx.splitHk_;
        self->ctx.load3d_.filterSizeW = (self->ctx.splitWk_ >> 8) & 255;
        self->ctx.load3d_.filterSizeH = (self->ctx.splitHk_ >> 8) & 255;
    } else {
        self->ctx.load3d_.filterW = self->ctx.tiling_->wk;
        self->ctx.load3d_.filterH = self->ctx.tiling_->hk;
        self->ctx.load3d_.filterSizeW = (self->ctx.tiling_->wk >> 8) & 255;
        self->ctx.load3d_.filterSizeH = (self->ctx.tiling_->hk >> 8) & 255;
    }
    self->ctx.load3d_.dilationFilterW = self->ctx.tiling_->dilationW;
    self->ctx.load3d_.dilationFilterH = self->ctx.tiling_->dilationH;
    self->ctx.load3d_.enTranspose = 0;
    self->ctx.load3d_.fMatrixCtrl = 0;
    self->ctx.load3d_.channelSize = self->ctx.channelSize_;

    // Set pad, pad list [left, right, top, down], default is 0, range [0, 255]
    self->ctx.load3d_.padList[0] = self->ctx.tiling_->backpropPadLeft;
    self->ctx.load3d_.padList[1] = self->ctx.tiling_->backpropPadRight;
    self->ctx.load3d_.padList[3] = 255;
}


template <class Intf>
__aicore__ inline void UpdateLoadToA2ParamsM(Intf *self)
{
    // load3dStepM
    self->ctx.load3d_.mExtension = self->ctx.baseUseM_;
    if (unlikely(self->ctx.curML0Idx_ == self->ctx.mIter_ - 1)) {
        // 4 is used to replace the division operation
        self->ctx.load3d_.mExtension = ((self->ctx.baseUseM_ + AscendC::BLOCK_CUBE - 1) >> 4) * AscendC::BLOCK_CUBE;
    }
    // posM: current default stepM = 1
    if constexpr (Intf::conv3dConfig.enableKernelSplit) {
        self->ctx.load3d_.mStartPt = self->ctx.curML0Idx_ * self->ctx.tiling_->baseM % self->ctx.splitWi_;
    } else {
        self->ctx.load3d_.mStartPt = self->ctx.curML0Idx_ * self->ctx.tiling_->baseM % self->ctx.tiling_->wi;
    }
}

template <class Intf>
__aicore__ inline void UpdateCurHoSize(Intf *self)
{
    // posM: current default stepM = 1
    uint32_t curHoSize = 0;
    if constexpr (Intf::conv3dConfig.enableKernelSplit) {
        curHoSize = CalFmapHForKernelSplit<Intf>(self, self->ctx.tiling_->baseM * self->ctx.tiling_->stepM);
    } else {
        curHoSize = CalFmapH<Intf>(self, self->ctx.tiling_->baseM * self->ctx.tiling_->stepM);
    }
    uint32_t hoExpand = (self->ctx.tiling_->ho - 1) * self->ctx.tiling_->strideH + 1;
    if (self->ctx.curHoIdx_ < 0) {
        curHoSize += self->ctx.curHoIdx_;
        curHoSize = curHoSize > hoExpand ? hoExpand : curHoSize;
    } else if (self->ctx.curHoIdx_ + curHoSize >= hoExpand) {
        if (self->ctx.curHoIdx_ < hoExpand) {
            curHoSize = hoExpand - self->ctx.curHoIdx_;
        } else {
            curHoSize = 0;
        }
    }
    self->ctx.curHoSize_ = curHoSize;
}

// Calculate B2 parameters
template <class Intf>
__aicore__ inline void InitLoadToB2Params(Intf *self)
{
    self->ctx.load2d_.startIndex = 0;
    self->ctx.load2d_.repeatTimes = 0;
    self->ctx.load2d_.srcStride = 0;
    self->ctx.load2d_.dstGap = 0;
    self->ctx.load2d_.ifTranspose = 1;
    self->ctx.idxC1in_ = 0;
    self->ctx.baseB1Offset_ = 0;
}

template <class Intf>
__aicore__ inline void UpdateLoadToB2ParamsN(Intf *self)
{
    self->ctx.blockBaseN_ = self->ctx.baseUseN_ >> self->ctx.tiling_->c0Bits;
    self->ctx.load2d_.repeatTimes = self->ctx.blockBaseN_;
}

template <class Intf>
__aicore__ inline void UpdateLoadToB2ParamsK(Intf *self)
{
    if constexpr (Intf::conv3dConfig.enableKernelSplit) {
        self->ctx.load2d_.srcStride =
            self->ctx.curLoadKbl1_ / self->ctx.splitHkWkC0_ * self->ctx.splitHkWk_;
    } else if constexpr (Intf::conv3dConfig.loadB2Condition == ConvBackpropApi::B2Condition::HKWK_EQ_ONE) {
            self->ctx.load2d_.srcStride = self->ctx.curLoadKbl1_ >> self->ctx.tiling_->c0Bits;
    } else {
        int64_t curBL1DivChannelSize = self->ctx.curLoadKbl1_ >> self->ctx.tiling_->c0Bits;
        self->ctx.load2d_.srcStride =
            ConvBackpropApi::CalcFloorAlign(curBL1DivChannelSize, self->ctx.HkWk_);
    }
}

template <class Intf>
__aicore__ inline void LoadToB2V1(Intf *self, const AscendC::LocalTensor<typename Intf::SrcT> &l1B1Matrix, uint32_t kPos,
                                         AscendC::LocalTensor<typename Intf::SrcT> &l0b)
{
    // transpose reverse order
    uint32_t kRepeat = self->ctx.mmad_.k >> self->ctx.tiling_->c0Bits;
    uint32_t kBlockC0Num = kPos * (self->ctx.tiling_->baseK >> self->ctx.tiling_->c0Bits);
    constexpr uint32_t blockSize = 256; // 256 is the number of elements in the minimum handling unit of load2d
    uint32_t dstB2Stride = self->ctx.blockBaseN_ * blockSize;

    if constexpr (Intf::conv3dConfig.enableKernelSplit) {
        LoadToB2ForKernelSplit<Intf>(self, l1B1Matrix, kRepeat, kBlockC0Num, blockSize, l0b);
    } else if constexpr (Intf::conv3dConfig.loadB2Condition == ConvBackpropApi::B2Condition::HKWK_EQ_ONE) {
        uint64_t dstB2Offset = 0;
        uint32_t srcB1Offset = self->ctx.baseB1Offset_ + kBlockC0Num * blockSize;
        for (uint32_t i = 0; i < kRepeat; i++) {
            AscendC::LoadData(l0b[dstB2Offset], l1B1Matrix[srcB1Offset], self->ctx.load2d_);
            dstB2Offset += dstB2Stride;
            srcB1Offset += blockSize;
        }
    } else if constexpr (Intf::conv3dConfig.loadB2Condition == ConvBackpropApi::B2Condition::BASEK_LT_HKWK) {
        uint32_t baseC1outIdx = ConvBackpropApi::CalcDiv(kBlockC0Num, self->ctx.HkWk_);
        uint32_t curL1CoutC0 = ConvBackpropApi::CalcDiv(self->ctx.curLoadKbl1_, self->ctx.HkWk_) * self->ctx.tiling_->c0;
        uint32_t baseHkWkOffset = self->ctx.HkWk_ - 1 - kBlockC0Num + baseC1outIdx * self->ctx.HkWk_;
        uint32_t dstB2Offset = 0;
        uint32_t srcB1Offset = self->ctx.baseB1Offset_ + baseC1outIdx * blockSize + baseHkWkOffset * curL1CoutC0;

        for (uint32_t i = 0; i < kRepeat; i++) {
            AscendC::LoadData(l0b[dstB2Offset], l1B1Matrix[srcB1Offset], self->ctx.load2d_);
            dstB2Offset += dstB2Stride;
            srcB1Offset -= curL1CoutC0;
        }
    } else {
        uint32_t baseC1outIdx = ConvBackpropApi::CalcDiv(kBlockC0Num, self->ctx.HkWk_);
        uint32_t curL1Cout = ConvBackpropApi::CalcDiv(self->ctx.curLoadKbl1_, self->ctx.HkWk_);
        uint32_t baseHkWkOffset = self->ctx.HkWk_ - 1 - kBlockC0Num + baseC1outIdx * self->ctx.HkWk_;
        uint64_t dstB2Offset = 0;

        for (uint32_t i = 0; i < kRepeat; i++) {
            uint32_t idxC1out = baseC1outIdx + ConvBackpropApi::CalcDiv(i, self->ctx.HkWk_);
            uint32_t srcB1Offset = self->ctx.baseB1Offset_ + idxC1out * blockSize +
                                (baseHkWkOffset - ConvBackpropApi::CalcRemainder(i, self->ctx.HkWk_)) * self->ctx.tiling_->c0 * curL1Cout;
            AscendC::LoadData(l0b[dstB2Offset], l1B1Matrix[srcB1Offset], self->ctx.load2d_);
            dstB2Offset += dstB2Stride;
        }
    }
}

static constexpr AscendC::IsResetLoad3dConfig LOAD3DV2_CONFIG = {false, false};
static constexpr uint8_t PADLIST_B[4] = {0, 0, 0, 0};

template <class Intf>
__aicore__ inline void LoadToB2Pro(Intf *self, const AscendC::LocalTensor<typename Intf::SrcT> &l1B1Matrix,
    uint32_t kPos, uint32_t l0bKIdx, bool b1PingPongFlag, AscendC::LocalTensor<typename Intf::SrcT> &l0b)
{
    // Transpose reverse order: load3dv2 version, currently supports fp32, fp16 scenes can be easily adapted
    //In fp32 scenario, when baseK=8, it needs to be rounded up
    uint32_t kRepeat = self->ctx.mmad_.k >> self->ctx.tiling_->c0Bits;
    uint32_t kBlockC0Num = l0bKIdx * kRepeat;
    uint32_t baseHkOffset = kBlockC0Num / self->ctx.tiling_->wk % self->ctx.tiling_->hk;
    uint32_t baseWkOffset = kBlockC0Num % self->ctx.tiling_->wk;
    uint32_t baseHkWkOffset = self->ctx.HkWk_ - 1 - baseHkOffset * self->ctx.tiling_->wk - baseWkOffset;
    uint32_t curL1Cout = DivCeil(self->ctx.curLoadKbl1_ / self->ctx.HkWkC0_, 2) * AscendC::BLOCK_CUBE;
    uint16_t wSize = static_cast<uint16_t>(curL1Cout * self->ctx.HkWk_);
    // Parameter correspondence: H, W, pad, mode
    SetFmatrix(1, wSize, PADLIST_B, AscendC::FmatrixMode::FMATRIX_RIGHT);
    uint16_t kStart = static_cast<uint16_t>(self->ctx.curNL0Idx_ % self->ctx.curStepN_ * self->ctx.baseUseN_);
    uint16_t channelSize = static_cast<uint16_t>(self->ctx.curStepN_ * self->ctx.baseUseN_);
    // In the fp32 scenario, the B matrix is ​​a 16*8 small fractal, and two 16*8 small fractals become two 8*16 small fractals
    uint16_t numHkWk = kPos * self->ctx.mmad_.k / self->ctx.HkWkC0_;
    // Depending on whether it is the upper half fractal or the lower half fractal, determine whether the M direction is offset
    uint16_t mStartOffset = static_cast<uint16_t>((numHkWk & 1) * self->ctx.tiling_->c0);
    // When loading L1 data, every 2 HkWkC0 corresponds to 1 C0out(16), and only half of it will be used. At this time, the M direction offset of load3dv2 must be calculated based on the coordinates
    uint32_t curCoutIdx = b1PingPongFlag ? self->ctx.curPingCoutIdx_ : self->ctx.curPongCoutIdx_;
    uint32_t coutOffset = (kPos * self->ctx.tiling_->baseK / (self->ctx.HkWk_ * AscendC::BLOCK_CUBE) - curCoutIdx) * AscendC::BLOCK_CUBE;
    for (uint32_t i = 0; i < kRepeat; i++) {
        // The baseCoutOffset parameter is used to determine whether baseK needs to load multiple HkWkC0. At this time, Cout offset calculation is required
        uint32_t baseCoutOffset = i / self->ctx.HkWk_ * self->ctx.tiling_->c0;
        uint32_t dstB2Offset = i * self->ctx.baseUseAlignN_ * self->ctx.tiling_->c0;
        // (baseHkWkOffset - i % self->ctx.HkWk_) * BLOCK_CUBE represents the upper offset of HkWk, coutOffset represents the upper offset of Cout, and mStartOffset represents a small type that takes the upper or lower half of the data
        uint16_t mStart = static_cast<uint16_t>((baseHkWkOffset - i % self->ctx.HkWk_) * curL1Cout + baseCoutOffset + coutOffset + mStartOffset);
        AscendC::LoadData<typename Intf::SrcT, LOAD3DV2_CONFIG>(l0b[dstB2Offset], l1B1Matrix[0],
                                                    {
                                                        PADLIST_B,      // pad
                                                        1,              // L1_H
                                                        wSize,  // L1_W
                                                        channelSize,   // channelSize
                                                        static_cast<uint16_t>(self->ctx.baseUseN_),     // kExtension
                                                        AscendC::BLOCK_CUBE,     // mExtension
                                                        kStart,      // kStartPt
                                                        mStart,      // mStartPt
                                                        1,      // strideW
                                                        1,      // strideH
                                                        1,      // filterW
                                                        1,      // filterH
                                                        1,      // dilationFilterW
                                                        1,      // dilationFilterH
                                                        true,   // enableTranspose, when dst is L0B, the hardware will definitely enable the transpose capability to meet the L0B classification requirements
                                                        false,  // enableSmallK
                                                        0,      // padValue
                                                        0,      // filterSizeWIn
                                                        0,      // filterSizeHIn
                                                        1       // fMatrixCtrlIn enables set_fmatrix setting
                                                    }
                                                );
    }
}

template <class Intf>
__aicore__ inline void LoadToB2ProGemm(Intf *self, const AscendC::LocalTensor<typename Intf::SrcT> &l1B1Matrix,
    uint32_t kPos, uint32_t l0bKIdx, AscendC::LocalTensor<typename Intf::SrcT> &l0b)
{
    // Transpose in reverse order: load3dv2 version, split out kernel=1*1 template
    uint32_t kRepeat = DivCeil(self->ctx.mmad_.k, AscendC::BLOCK_CUBE);
    // Calculating 2 blocks of HkWkC0 requires loading a complete C0out=16
    uint16_t wSize = static_cast<uint16_t>(DivCeil(self->ctx.curLoadKbl1_ / self->ctx.HkWkC0_, 2) * AscendC::BLOCK_CUBE);
    // Parameter correspondence: H, W, pad, mode
    SetFmatrix(1, wSize, PADLIST_B, AscendC::FmatrixMode::FMATRIX_RIGHT);
    uint16_t kStart = static_cast<uint16_t>(self->ctx.curNL0Idx_ % self->ctx.curStepN_ * self->ctx.baseUseN_);
    uint16_t channelSize = static_cast<uint16_t>(self->ctx.curStepN_ * self->ctx.baseUseN_);
    uint32_t mStartL1Pos = l0bKIdx * self->ctx.mmad_.k;
    for (uint32_t i = 0; i < kRepeat; i++) {
        uint32_t dstB2Offset = i * self->ctx.baseUseAlignN_ * AscendC::BLOCK_CUBE;
        uint16_t mStart = static_cast<uint16_t>(i * AscendC::BLOCK_CUBE + mStartL1Pos);
        AscendC::LoadData<typename Intf::SrcT, LOAD3DV2_CONFIG>(l0b[dstB2Offset], l1B1Matrix[0],
                                                    {
                                                        PADLIST_B,      // pad
                                                        1,              // L1_H
                                                        wSize,  // L1_W
                                                        channelSize,   // channelSize
                                                        static_cast<uint16_t>(self->ctx.baseUseN_),     // kExtension
                                                        AscendC::BLOCK_CUBE,     // mExtension
                                                        kStart,      // kStartPt
                                                        mStart,      // mStartPt
                                                        1,      // strideW
                                                        1,      // strideH
                                                        1,      // filterW
                                                        1,      // filterH
                                                        1,      // dilationFilterW
                                                        1,      // dilationFilterH
                                                        true,   // enableTranspose, when dst is L0B, the hardware will definitely enable the transpose capability to meet the L0B classification requirements
                                                        false,  // enableSmallK
                                                        0,      // padValue
                                                        0,      // filterSizeWIn
                                                        0,      // filterSizeHIn
                                                        1       // fMatrixCtrlIn enables set_fmatrix setting
                                                    }
                                                );
    }
}

template <class Intf>
__aicore__ inline void LoadToB2(Intf *self, const AscendC::LocalTensor<typename Intf::SrcT> &l1B1Matrix,
    uint32_t l0bKIdx, uint32_t kPos, bool b1PingPongFlag, AscendC::LocalTensor<typename Intf::SrcT> &l0b)
{
    if constexpr((std::is_same<typename Intf::SrcT, bfloat16_t>::value) || (std::is_same<typename Intf::SrcT, half>::value)) {
        LoadToB2V1<Intf>(self, l1B1Matrix, l0bKIdx, l0b);
    } else if constexpr(std::is_same<typename Intf::SrcT, float>::value) {
        if constexpr (Intf::conv3dConfig.loadB2Condition == ConvBackpropApi::B2Condition::HKWK_EQ_ONE) {
            LoadToB2ProGemm<Intf>(self, l1B1Matrix, kPos, l0bKIdx, l0b);
        } else {
            LoadToB2Pro<Intf>(self, l1B1Matrix, kPos, l0bKIdx, b1PingPongFlag, l0b);
        }
    }
}

// Data is loaded from A1 to A2
template <class Intf>
__aicore__ inline void LoadToA2(Intf *self, const AscendC::LocalTensor<typename Intf::SrcT> &l1A1Matrix,
                                       AscendC::LocalTensor<typename Intf::SrcT> &l0a)
{
    LoadDataImpl(l0a, l1A1Matrix, self->ctx.load3d_);
}

template <class Intf>
__aicore__ inline void CopyData2Gm(Intf *self, const AscendC::GlobalTensor<typename Intf::DstT> &output,
                                          AscendC::LocalTensor<typename Intf::L0cT> &useC1Buf, QuantMode_t quantMode)
{
    uint64_t dstOffset = static_cast<uint64_t>(self->ctx.curNL0Idx_) * self->ctx.tiling_->baseN * self->ctx.hwI_ +
        (static_cast<uint64_t>(self->ctx.curML0Idx_) * self->ctx.tiling_->baseM +  // M direction offset
        static_cast<uint64_t>(self->ctx.curDinIdx_) * self->ctx.hwI_ *
        self->ctx.tiling_->cin1) * self->ctx.tiling_->c0;  // D direction offset
    bool enableChannelSplit = false;
    if constexpr (std::is_same<typename Intf::DstT, float>::value) {
        enableChannelSplit = true;
    }
    uint32_t alingedBaseUseM = ConvBackpropApi::ShiftCeilBlockCube(self->ctx.baseUseM_) * AscendC::BLOCK_CUBE;
    if (self->ctx.hwI_ <= UINT32_MAX) {
        AscendC::DataCopyCO12DstParams dataCopyParams(
            static_cast<uint16_t>(self->ctx.baseUseN_),         //nSize
            static_cast<uint16_t>(self->ctx.baseUseM_),         //mSize
            static_cast<uint32_t>(self->ctx.hwI_),
            alingedBaseUseM, //srcStride
            quantMode,
            0,
            enableChannelSplit,
            false
        );
        DataCopy(output[dstOffset], useC1Buf, dataCopyParams);
    } else {
        // Since HF32/FP32 requires channel split, the temporary command cannot support this scenario and needs to be intercepted on the tiling side. Only BF16/FP16 comes in here
        uint16_t blockCnt = self->ctx.baseUseN_ / self->ctx.tiling_->c0;
        uint64_t dstStrideOffset = self->ctx.hwI_ * self->ctx.tiling_->c0;
        uint32_t srcStrideOffset = alingedBaseUseM;
        uint32_t srcOffset = 0;
        for (uint16_t i = 0; i < blockCnt; i++) {
            AscendC::DataCopyCO12DstParams dataCopyParams(
                static_cast<uint16_t>(self->ctx.tiling_->c0),         //nSize
                static_cast<uint16_t>(self->ctx.baseUseM_),         //mSize
                static_cast<uint32_t>(self->ctx.baseUseM_),  // The interval between the destination data header and the header. In order to meet the requirement that the interface cannot be set to 0, it does not actually take effect
                0, //The interval between the original data header and the header is only copied once, so it is set to 0
                quantMode,
                0,
                enableChannelSplit,
                false
            );
            DataCopy(output[dstOffset], useC1Buf[srcOffset], dataCopyParams);
            dstOffset += dstStrideOffset;
            srcOffset += srcStrideOffset;
        }
    }
}

template <class Intf>
__aicore__ inline void CopyData2TmpWorkspace(Intf *self, const AscendC::GlobalTensor<typename Intf::DstT> &output,
    AscendC::LocalTensor<typename Intf::L0cT> &useC1Buf)
{
    QuantMode_t quantMode = QuantMode_t::F322BF16;
    if constexpr(std::is_same<typename Intf::DstT, half>::value) {
        quantMode = QuantMode_t::F322F16;
    } else if constexpr(std::is_same<typename Intf::DstT, float>::value) {
        quantMode = NoQuant;
    }
    int64_t dstOffset = AscendC::GetBlockIdx() * self->ctx.tiling_->baseM * self->ctx.tiling_->baseN;
    if constexpr((std::is_same<typename Intf::SrcT, bfloat16_t>::value) || (std::is_same<typename Intf::SrcT, half>::value)) {
        AscendC::FixpipeParams<typename Intf::L0cT> fixpipeParams(
            static_cast<uint16_t>(Ceil(self->ctx.baseUseN_, 16)),
            static_cast<uint16_t>(self->ctx.baseUseM_ * AscendC::BLOCK_CUBE * sizeof(typename Intf::L0cT) / 32),
            0,
            0);
        fixpipeParams.quantParams.quantPre = quantMode;
        Fixpipe(output[dstOffset], useC1Buf, fixpipeParams);
    }
}

template <class Intf>
__aicore__ inline void FreeL0cTensor(Intf *self, AscendC::LocalTensor<typename Intf::L0cT> &l0c)
{
    if (self->ctx.usingCacheC1Ping_) {
        self->ctx.c1Ping_.FreeTensor(l0c);
    } else {
        self->ctx.c1Pong_.FreeTensor(l0c);
    }
}

template <class Intf>
__aicore__ inline void LoadL0c2Gm(Intf *self, const AscendC::GlobalTensor<typename Intf::DstT> &output,
                                         uint8_t enAtomic = 0, bool enSequentialWrite = false)
{
    if (!self->ctx.needComputeFlag_) {
        return;
    }

    AscendC::LocalTensor<typename Intf::L0cT> useC1Buf;
    if (self->ctx.usingCacheC1Ping_) {
        useC1Buf = self->ctx.c1Ping_.template DeQue<typename Intf::L0cT>();
    } else {
        useC1Buf = self->ctx.c1Pong_.template DeQue<typename Intf::L0cT>();
    }
    if (unlikely(!self->ctx.needComputeKFlag_)) {
        FreeL0cTensor<Intf>(self, useC1Buf);
        return;
    }
    if (enAtomic == 1) {
        AscendC::SetAtomicAdd<typename Intf::DstT>();
    }
    if constexpr (Intf::Config::dType::format == ConvCommonApi::ConvFormat::NDC1HWC0) {
        if (!enSequentialWrite) {
            QuantMode_t quantMode = QuantMode_t::F322BF16;
            if constexpr(std::is_same<typename Intf::DstT, half>::value) {
                quantMode = QuantMode_t::F322F16;
            } else if constexpr(std::is_same<typename Intf::DstT, float>::value) {
                quantMode = NoQuant;
            }
            if constexpr (Intf::conv3dConfig.enableKernelSplit) {
                LoadL0c2GmForKernelSplit<Intf>(self, output, useC1Buf, quantMode);
            } else {
                CopyData2Gm<Intf>(self, output, useC1Buf, quantMode);
            }
        } else {
            return;
        }
    } else if constexpr (Intf::Config::dType::format == ConvCommonApi::ConvFormat::NCDHW) {
        if (!enSequentialWrite) {
            CopyData2TmpWorkspace<Intf>(self, output, useC1Buf);
        } else {
            return;
        }
    }
    if (enAtomic == 1) {
        AscendC::SetAtomicNone();
    }

    FreeL0cTensor<Intf>(self, useC1Buf);
    if (unlikely(self->ctx.tiling_->cl0Pbuffer > 1)) {
        self->ctx.usingCacheC1Ping_ = !self->ctx.usingCacheC1Ping_;
    }
}
}  // namespace ConvBackpropInputFunc

#endif
