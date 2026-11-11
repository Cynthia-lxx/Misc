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
 * \file conv_bp_filter_sub_func.h
 * \brief
 */

#ifndef IMPL_CONV3D_BP_FILTER_SUB_FUNC_H
#define IMPL_CONV3D_BP_FILTER_SUB_FUNC_H

namespace ConvBackpropFilterFunc {
const int32_t MAX_BLOCK_COUNT = 4095;
const int32_t MIN_BLOCK_COUNT = 1;
const int32_t MAX_BLOCK_LEN = 65535;
const int32_t MAX_16BITS_STRIDE = 65535;
const int32_t MAX_L1W = 32767;

template <class Intf>
static __aicore__ inline void CalOut2L1ScalarParams(Intf *self, Out2L1ScalarParams& params) {

    // to L1A
    if (params.isLoad2L1A) {
        params.out2A1SrcAddr = static_cast<uint64_t>(self->ctx.curML1Idx_) * self->ctx.tiling_->baseM * self->ctx.hwO_;
        params.isLastMAL1 =
            ConvBackpropApi::DivStepM((self->ctx.mIter_ - 1), self->ctx.tiling_->stepM) == ConvBackpropApi::DivStepM(self->ctx.curML0Idx_, self->ctx.tiling_->stepM);
    }

    // to L1B
    // Calculate the starting idx of cin on L1, remove the HkWk in cin1HkWkCin, and align the GM to 8 in the fp32 scenario
    if (params.isLoad2L1B) {
        uint64_t localN = ConvBackpropApi::ShiftDivChannelSize<Intf>(self->ctx.tiling_->baseN, self->ctx.tiling_->channelSize);
        uint64_t b1SrCin = ConvBackpropApi::DivHkWk(self->ctx.curNL1Idx_ * localN, self->ctx.hwK_);
        uint64_t singleShapeHi = self->ctx.singleShapeHo_ * self->ctx.tiling_->strideH + self->ctx.strideKernelDilationH;
        if constexpr (Intf::Config::xType::format == ConvCommonApi::ConvFormat::NCDHW) { // Transdata merge x input
            params.out2B1SrcAddr = b1SrCin * singleShapeHi * self->ctx.tiling_->wi;
        } else {
            params.out2B1SrcAddr = b1SrCin * self->ctx.hwI_;
        }
        uint32_t bL1N = self->ctx.curStepN_ * localN;
        uint32_t bL1cin1CopyLen = ConvBackpropApi::CeilHkWk(bL1N, self->ctx.hwK_);
        if (self->ctx.hwK_ > bL1N) {
            if (self->ctx.hwK_ % bL1N != 0) {
                ++bL1cin1CopyLen; // If cin1 moves one line, it will be larger than the basic block size. Just move one more line
            }
        } else if (ConvBackpropApi::RemainderOfHkWk(2 * bL1N, self->ctx.hwK_) != 0) {
            ++bL1cin1CopyLen; // Unless the tail block is exactly 0.5, you have to move another line to consider the tail
        }
        uint64_t cin1RemainLen = ConvBackpropApi::ShiftDivChannelSize<Intf>(self->ctx.singleShapeCin_, self->ctx.tiling_->channelSize) - b1SrCin;
        params.bL1cin1CopyLen = cin1RemainLen > bL1cin1CopyLen ? bL1cin1CopyLen: cin1RemainLen;

        params.singleShapeHi = self->ctx.tiling_->hi > singleShapeHi ? singleShapeHi : self->ctx.tiling_->hi;
    }
}

template <class Intf>
static __aicore__ inline void InitLoadToA2Params(Intf *self)
{
    self->ctx.dstL12L0aOffset_ = 0;
    self->ctx.srcL12L0aOffset_ = 0;
    // posK
    self->ctx.load3dA_.kStartPt = 0;
    // posM
    self->ctx.load3dA_.mStartPt = 0;
    self->ctx.load3dA_.strideW = 1;
    self->ctx.load3dA_.strideH = 1;
    self->ctx.load3dA_.filterW = 1;
    self->ctx.load3dA_.filterH = 1;
    self->ctx.load3dA_.dilationFilterW = 1;
    self->ctx.load3dA_.dilationFilterH = 1;
    self->ctx.load3dA_.filterSizeW = 0;
    self->ctx.load3dA_.filterSizeH = 0;
    self->ctx.load3dA_.enTranspose = 1;
    self->ctx.load3dA_.fMatrixCtrl = 0;
    self->ctx.load3dA_.channelSize = AscendC::BLOCK_CUBE;
}

// Calculate the instruction parameters of Load2B2
template <class Intf>
static __aicore__ inline void InitLoadToB2Params(Intf *self) {
    // load3dStepK
    self->ctx.load3dB_.kExtension = self->ctx.tiling_->baseN;
    // load3dStepM
    self->ctx.load3dB_.mExtension = self->ctx.tiling_->baseM;
    // posK
    self->ctx.load3dB_.kStartPt = 0;
     // posM
    self->ctx.load3dB_.mStartPt = 0;
    self->ctx.load3dB_.strideW = self->ctx.tiling_->strideW;
    self->ctx.load3dB_.strideH = self->ctx.tiling_->strideH;
    self->ctx.load3dB_.filterW = self->ctx.tiling_->wk;
    self->ctx.load3dB_.filterH = self->ctx.tiling_->hk;
    self->ctx.load3dB_.dilationFilterW = self->ctx.tiling_->dilationW;
    self->ctx.load3dB_.dilationFilterH = self->ctx.tiling_->dilationH;
    self->ctx.load3dB_.filterSizeW = (self->ctx.tiling_->wk >> 8) & 255;
    self->ctx.load3dB_.filterSizeH = (self->ctx.tiling_->hk >> 8) & 255;
    self->ctx.load3dB_.enTranspose = 0;
    self->ctx.load3dB_.fMatrixCtrl = 1;
    self->ctx.load3dB_.channelSize = 16;
}

template <class Intf>
static __aicore__ inline void InitSetFmatrixParams(Intf *self)
{
    self->ctx.load3dA_.padList[0] = 0;
    self->ctx.load3dA_.padList[1] = 0;
    self->ctx.load3dA_.padList[2] = 0;
    self->ctx.load3dA_.padList[3] = 0;

    // W
    self->ctx.load3dB_.l1W = self->ctx.tiling_->wi;
    // H
    self->ctx.load3dB_.l1H = 1;
    self->ctx.load3dB_.padList[0] = self->ctx.tiling_->padLeft;
    self->ctx.load3dB_.padList[1] = self->ctx.tiling_->padRight;
    // padUp now is set in Load BL1
    self->ctx.load3dB_.padList[3] = self->ctx.tiling_->padDown;
}

template <class Intf>
static __aicore__ inline void CalcParamsMmad(Intf *self, uint64_t kPos)
{
    self->ctx.mmad_.m = self->ctx.baseUseM_;
    self->ctx.mmad_.n = self->ctx.baseUseN_;
}

template <class Intf>
static __aicore__ inline void InitMmadParams(Intf *self)
{
    self->ctx.dstL0cOffset_ = 0;
    self->ctx.srcL0aOffset_ = 0;
    self->ctx.srcL0bOffset_ = 0;
    self->ctx.mmad_.m = self->ctx.tiling_->baseM;
    self->ctx.mmad_.k = self->ctx.tiling_->baseK;
    self->ctx.mmad_.n = self->ctx.tiling_->baseN;
    self->ctx.mmad_.unitFlag = 0;
    if constexpr (AscendC::IsSameType<typename Intf::SrcT, float>::value) {
        self->ctx.mmad_.kDirectionAlign = 1;
    } else {
        self->ctx.mmad_.kDirectionAlign = 0;
    }
    self->ctx.mmad_.cmatrixSource = 0;
    self->ctx.mmad_.cmatrixInitVal = 0;
}

template <class Intf>
static __aicore__ inline void LoadL12L0a(Intf *self, const AscendC::LocalTensor<typename Intf::SrcT> &l1AMatrix,
                                         uint32_t kPos, AscendC::LocalTensor<typename Intf::SrcT> &l0a,
                                         const Out2L1ScalarParams& params, uint64_t kaStepIdx)
{
    uint32_t kl1 = self->ctx.kal1_;
    if (self->ctx.stepKaRound == (kaStepIdx + 1)) {
        kl1 = self->ctx.singleShapeHo_ * self->ctx.tiling_->wo - kaStepIdx * self->ctx.kal1_;
    }

    if constexpr (AscendC::IsSameType<typename Intf::SrcT, float>::value) {
        self->ctx.load3dA_.l1W = kl1;
        self->ctx.load3dA_.l1H = 1;

        // load3dStepK
        self->ctx.load3dA_.kExtension = ConvBackpropApi::Ceil(self->ctx.baseUseM_, self->ctx.tiling_->m0) * self->ctx.tiling_->m0;
        // load3dStepM
        self->ctx.load3dA_.mExtension = ConvBackpropApi::Ceil(self->ctx.baseUseK_, self->ctx.tiling_->k0) * self->ctx.tiling_->k0;
        self->ctx.load3dA_.channelSize = ConvBackpropApi::Ceil(self->ctx.baseUseM_, self->ctx.tiling_->m0) * self->ctx.tiling_->m0;

        LoadData(l0a, l1AMatrix[self->ctx.srcL12L0aOffset_], self->ctx.load3dA_);
    } else {
        if (kl1 <= MAX_L1W) {
            self->ctx.load3dA_.l1W = kl1;
            self->ctx.load3dA_.l1H = 1;
        } else {
            uint64_t a1SrcKAlign = kaStepIdx * self->ctx.kal1_;
            uint32_t a1SrcHo = a1SrcKAlign / self->ctx.tiling_->wo;
            uint32_t hoCopyLen = ConvBackpropApi::CalRows2Copy(kl1, self->ctx.tiling_->wo);
            int32_t hoRemain = self->ctx.singleShapeHo_ - a1SrcHo;
            hoCopyLen = hoRemain > hoCopyLen ? hoCopyLen : hoRemain;
            self->ctx.load3dA_.l1W = self->ctx.tiling_->wo;
            self->ctx.load3dA_.l1H = hoCopyLen;
        }
        LoadData(l0a, l1AMatrix[self->ctx.srcL12L0aOffset_], self->ctx.load3dA_);
    }
}

template <class Intf>
static __aicore__ inline void MmadLocal(Intf *self, const AscendC::LocalTensor<typename Intf::SrcT> &l0a,
                                        const AscendC::LocalTensor<typename Intf::SrcT> &l0b,
                                        AscendC::LocalTensor<typename Intf::L0cT> &l0c)
{
    MmadImpl(l0c[self->ctx.dstL0cOffset_],
        l0a[self->ctx.srcL0aOffset_],
        l0b[self->ctx.srcL0bOffset_],
        self->ctx.mmad_);
    // When the MMAD calculation amount baseM*baseN is less than a certain threshold, PIPE_M synchronization needs to be added. The current platform threshold is 10*256
    if (self->ctx.mmad_.m * self->ctx.mmad_.n < 2560) {
        AscendC::PipeBarrier<PIPE_M>();
    }
}

template <class Intf, class src0_T>
__aicore__ inline void LoadToA1(Intf *self, bool cachePosA1, uint64_t kaIdx, const Out2L1ScalarParams& params,
                                bool isLoadA1, uint64_t kaStepIdx)
{
    if (!isLoadA1) {
        return;
    }
    if (params.isLoad2L1A) {
        AscendC::LocalTensor<typename Intf::SrcT> useA1Buf;
        if (cachePosA1) {
            useA1Buf = self->ctx.a1Ping_.template AllocTensor<typename Intf::SrcT>();
        } else {
            useA1Buf = self->ctx.a1Pong_.template AllocTensor<typename Intf::SrcT>();
        }
        uint64_t out2A1SrcAddrOffset =
            params.out2A1SrcAddr + kaStepIdx * self->ctx.kal1_ * self->ctx.tiling_->channelSize;

        AscendC::DataCopyParams dataCopyParams;
        dataCopyParams.dstStride = 0;
        if (self->ctx.stepKaRound == (kaStepIdx + 1)) {
            // The last piece kAL1, consider tailK, 32 means 32Byte
            dataCopyParams.blockLen =
                (self->ctx.singleShapeHo_ * self->ctx.tiling_->wo - kaIdx * self->ctx.tiling_->baseK);
        } else {
            dataCopyParams.blockLen = self->ctx.kal1_;
        }
        self->ctx.curLoadKal1_ = dataCopyParams.blockLen;

        uint32_t blockCount = 0;
        if (params.isLastMAL1) {
            // The last piece of mAL1 needs to consider tailM
            blockCount = ConvBackpropApi::ShiftDivChannelSize<Intf>(((self->ctx.curStepM_ - 1) * self->ctx.tiling_->baseM + self->ctx.tailM_),
                                        self->ctx.tiling_->channelSize);
        } else {
            blockCount = ConvBackpropApi::ShiftDivChannelSize<Intf>(self->ctx.curStepM_ * self->ctx.tiling_->baseM, self->ctx.tiling_->channelSize);
        }

        if constexpr (AscendC::IsSameType<typename Intf::SrcT, float>::value) {
            if (blockCount & 0x1) {
                InitConstValue(
                    useA1Buf[blockCount * dataCopyParams.blockLen * AscendC::ONE_BLK_SIZE / sizeof(float)],
                    {1, dataCopyParams.blockLen, 0, 0U});
            }
        }

        // blockcount and blockLen are associated with L1, and the risk of overflow is low
        uint64_t srcStride = self->ctx.hwO_ - dataCopyParams.blockLen;
        if (srcStride <= MAX_16BITS_STRIDE) {
            dataCopyParams.srcStride = srcStride;
            dataCopyParams.blockCount = blockCount;
            DataCopy(useA1Buf, self->ctx.outBackPropGlobal_[out2A1SrcAddrOffset], dataCopyParams);
        } else {
            dataCopyParams.srcStride = 0;
            dataCopyParams.blockCount = 1;
            uint64_t srcOffset = out2A1SrcAddrOffset;
            uint64_t dstOffset = 0;
            for (uint32_t idx = 0; idx < blockCount; ++idx) {
                DataCopy(useA1Buf[dstOffset], self->ctx.outBackPropGlobal_[srcOffset], dataCopyParams);
                srcOffset += self->ctx.hwO_ * self->ctx.tiling_->channelSize;
                dstOffset += dataCopyParams.blockLen * self->ctx.tiling_->channelSize;
            }
        }

        if (cachePosA1) {
            self->ctx.a1Ping_.EnQue(useA1Buf);
        } else {
            self->ctx.a1Pong_.EnQue(useA1Buf);
        }
    }
}

template <class Intf, class src1_T>
__aicore__ inline void LoadToB1(Intf *self, bool cachePosB1, uint64_t kbIdx, const Out2L1ScalarParams& params,
                                bool isLoadB1, uint64_t kbStepIdx)
{
    if (!isLoadB1) {
        return;
    }
    // The condition that needs to be loaded into BL1 is that the calculated BL0 block is the first block of data on BL1, and the entire BL1 size is loaded at one time
    // At this time, BL1 needs to be loaded if one of the following conditions is metï¼?
    // 1.There is no db on BL1, and more than one buffer is needed in the K direction, which needs to be loaded every time; BL1 has db, and the number of buffers in the K direction is less than or equal to 2
    // 2.singleShapeK / stepKb > 2, priority is given to looping in the k direction, and data on BL1 cannot be reused
    // 3.When order_M, AL1 resides on L1, and BL1 data is not reused
    // 4.When order_N, BL1 resides on L1, and K <=
    // 2ï¼ŒThat is, all Kb can be placed on L1. At this time, the M direction is traversed, and the data on BL1 will not be overwritten. BL1 will only be loaded in the first cycle of the M direction
    if (params.isLoad2L1B) {
        AscendC::LocalTensor<typename Intf::SrcT> useB1Buf;
        if (cachePosB1) {
            useB1Buf = self->ctx.b1Ping_.template AllocTensor<typename Intf::SrcT>();
        } else {
            useB1Buf = self->ctx.b1Pong_.template AllocTensor<typename Intf::SrcT>();
        }

        // The correspondence between L0shape and orgShape, L0 and L1 are 16 aligned, orgShape is Wi aligned, Wo alignment is calculated first and Wi alignment is calculated
        // First calculate the starting address of the BL1 block where L0B is located, which is 16 aligned
        uint64_t b1SrcKAlign = kbStepIdx * self->ctx.kbl1_;
        // load3d must have a complete Wo, do Wo alignment, and calculate the Ho where the starting address is
        uint32_t b1SrcHo = b1SrcKAlign / self->ctx.tiling_->wo;
        uint32_t b1SrcHoGm = b1SrcHo + self->ctx.hoStartIdx_;
        // Calculate Hi corresponding to Ho, based on the convolution principle
        int64_t b1SrcHiGm = static_cast<uint64_t>(b1SrcHoGm) * self->ctx.tiling_->strideH - self->ctx.tiling_->padUp;
        uint32_t b1SrcHi = 0;
        if (b1SrcHiGm > 0 && self->ctx.hiStartIdx_ > 0) {
            b1SrcHi = b1SrcHiGm - self->ctx.hiStartIdx_;
        } else if (b1SrcHiGm > 0) {
            b1SrcHi = b1SrcHiGm;
        }

        uint32_t kbl1 = self->ctx.kbl1_;
        if (self->ctx.stepKbRound == (kbStepIdx + 1)) {
            kbl1 = self->ctx.singleShapeHo_ * self->ctx.tiling_->wo - b1SrcKAlign;
        }
        uint32_t ho = ConvBackpropApi::CalRows2Copy(kbl1, self->ctx.tiling_->wo);
        uint32_t hiCopyLen = ho * self->ctx.tiling_->strideH + self->ctx.strideKernelDilationH;

        uint32_t padUp = 0;
        if (b1SrcHiGm < 0) {
            hiCopyLen = hiCopyLen + b1SrcHiGm;
            padUp = -b1SrcHiGm;
        }
        if (b1SrcHiGm + hiCopyLen > self->ctx.tiling_->hi) {
            hiCopyLen = self->ctx.tiling_->hi - b1SrcHiGm;
        }

        uint32_t hiRemainLen = params.singleShapeHi - b1SrcHi;
        hiCopyLen = hiRemainLen > hiCopyLen ? hiCopyLen: hiRemainLen;

        AscendC::DataCopyParams dataCopyParams;
        dataCopyParams.dstStride = 0;

        uint64_t blockLen = static_cast<uint64_t>(hiCopyLen) * self->ctx.tiling_->wi;
        uint64_t srcStride = self->ctx.hwI_ - blockLen;
        if constexpr (Intf::Config::xType::format == ConvCommonApi::ConvFormat::NCDHW) { // Transdata merge x input
            srcStride = 0;
        }

        // Get the offset of gm
        uint64_t srcOffset = (params.out2B1SrcAddr + static_cast<uint64_t>(b1SrcHi) * self->ctx.tiling_->wi) * self->ctx.tiling_->channelSize;
        uint64_t dstOffset = 0;
        if (blockLen <= MAX_BLOCK_LEN && srcStride <= MAX_16BITS_STRIDE) {
            dataCopyParams.srcStride = srcStride;
            dataCopyParams.blockLen = blockLen;
            dataCopyParams.blockCount = MAX_BLOCK_COUNT;

            uint32_t loop = params.bL1cin1CopyLen / MAX_BLOCK_COUNT;
            for (uint32_t idx = 0; idx < loop; ++idx) {
                DataCopy(useB1Buf[dstOffset], self->ctx.fmapGlobal_[srcOffset], dataCopyParams);
                if constexpr (Intf::Config::xType::format == ConvCommonApi::ConvFormat::NCDHW) { // Transdata merge x input
                    srcOffset += MAX_BLOCK_COUNT * blockLen * self->ctx.tiling_->channelSize;
                } else {
                    srcOffset += MAX_BLOCK_COUNT * self->ctx.hwI_ * self->ctx.tiling_->channelSize;
                }
                dstOffset += MAX_BLOCK_COUNT * blockLen * self->ctx.tiling_->channelSize;
            }

            dataCopyParams.blockCount = params.bL1cin1CopyLen - loop * MAX_BLOCK_COUNT;
            if (dataCopyParams.blockCount > 0) {
                DataCopy(useB1Buf[dstOffset], self->ctx.fmapGlobal_[srcOffset], dataCopyParams);
            }
        } else {
            dataCopyParams.srcStride = 0;
            dataCopyParams.blockCount = hiCopyLen;
            dataCopyParams.blockLen = self->ctx.tiling_->wi;
            for (uint32_t idx = 0; idx < params.bL1cin1CopyLen; ++idx) {
                DataCopy(useB1Buf[dstOffset], self->ctx.fmapGlobal_[srcOffset], dataCopyParams);
                if constexpr (Intf::Config::xType::format == ConvCommonApi::ConvFormat::NCDHW) { // Transdata merge x input
                    srcOffset += blockLen * self->ctx.tiling_->channelSize;
                } else {
                    srcOffset += self->ctx.hwI_ * self->ctx.tiling_->channelSize;
                }
                dstOffset += blockLen * self->ctx.tiling_->channelSize;
            }
        }

        if (cachePosB1) {
            self->ctx.bL1HiCopyLenPing = hiCopyLen;
            self->ctx.bL1PadUpPing = padUp;
            self->ctx.b1Ping_.EnQue(useB1Buf);
        } else {
            self->ctx.bL1HiCopyLenPong = hiCopyLen;
            self->ctx.bL1PadUpPong = padUp;
            self->ctx.b1Pong_.EnQue(useB1Buf);
        }
    }
}

template <class Intf>
static __aicore__ inline void LoadL0c2Gm(
    Intf *self, const AscendC::GlobalTensor<typename Intf::DstT> &output, uint8_t enAtomic = 0, bool enSequentialWrite = false)
{
    AscendC::LocalTensor<typename Intf::L0cT> l0c;
    if (self->ctx.l0cPingPongFlag_) {
        l0c = self->ctx.l0cPing_.template DeQue<typename Intf::L0cT>();
    } else {
        l0c = self->ctx.l0cPong_.template DeQue<typename Intf::L0cT>();
    }

    if (enAtomic == 1) {
        AscendC::SetAtomicAdd<typename Intf::DstT>();
    }
    if constexpr (Intf::Config::dType::format == ConvCommonApi::ConvFormat::FRACTAL_Z_3D) {
        if (!enSequentialWrite) {
            uint64_t alignCoutG = static_cast<uint64_t>(self->ctx.tiling_->cout1G) * self->ctx.tiling_->channelSize;
            uint64_t dstOffset =
                static_cast<uint64_t>(self->ctx.curNL0Idx_) * self->ctx.tiling_->baseN * alignCoutG +
                static_cast<uint64_t>(self->ctx.curML0Idx_) * self->ctx.tiling_->baseM * self->ctx.tiling_->channelSize;
            // bf16 c0_size is 16 * sizeof(float) , fp32 enable channel split, c0_size is 8 * sizeof(float)
            uint64_t dstStrideIn =
                alignCoutG * self->ctx.tiling_->channelSize * sizeof(typename Intf::DstT) / AscendC::ONE_BLK_SIZE;
            AscendC::FixpipeParamsV220 fixpipeParams(
                static_cast<uint16_t>(self->ctx.baseUseN_), static_cast<uint16_t>(self->ctx.baseUseM_),
                ConvBackpropApi::ShiftCeilM0(self->ctx.baseUseM_, self->ctx.tiling_->m0) * self->ctx.tiling_->m0, dstStrideIn, 0);
            if constexpr (AscendC::IsSameType<typename Intf::SrcT, float>::value) {
                fixpipeParams.isChannelSplit = true;
            }
            AscendC::Fixpipe<typename Intf::DstT, typename Intf::L0cT, AscendC::CFG_NZ>(output[dstOffset], l0c, fixpipeParams);
        } else {
            uint64_t dstStrideIn =
                self->ctx.tiling_->baseM * self->ctx.tiling_->channelSize * sizeof(typename Intf::DstT) / AscendC::ONE_BLK_SIZE;
            AscendC::FixpipeParamsV220 fixpipeParams(
                static_cast<uint16_t>(self->ctx.baseUseN_), static_cast<uint16_t>(self->ctx.tiling_->baseM),
                ConvBackpropApi::ShiftCeilM0(self->ctx.tiling_->baseM, self->ctx.tiling_->m0) * self->ctx.tiling_->m0, dstStrideIn, 0);
            if constexpr (AscendC::IsSameType<typename Intf::SrcT, float>::value) {
                fixpipeParams.isChannelSplit = true;
            }
            AscendC::Fixpipe<typename Intf::DstT, typename Intf::L0cT, AscendC::CFG_NZ>(output, l0c, fixpipeParams);
        }
    }
    if (enAtomic == 1) {
        AscendC::SetAtomicNone();
    }
    if (self->ctx.l0cPingPongFlag_) {
        self->ctx.l0cPing_.FreeTensor(l0c);
    } else {
        self->ctx.l0cPong_.FreeTensor(l0c);
    }
    if (self->ctx.tiling_->cl0Pbuffer > 1) {
        self->ctx.l0cPingPongFlag_ = !self->ctx.l0cPingPongFlag_;
    }
}
}  // namespace ConvBackpropFilterFunc

#endif