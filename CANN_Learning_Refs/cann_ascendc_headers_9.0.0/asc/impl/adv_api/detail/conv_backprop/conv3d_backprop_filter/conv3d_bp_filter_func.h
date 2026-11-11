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
 * \file conv3d_bp_filter_func.h
 * \brief
 */

#ifndef CONV3D_BP_FILTER_FUNC_H
#define CONV3D_BP_FILTER_FUNC_H

#include "include/adv_api/conv_backprop/common/conv3d_bp_config_base.h"
#include "../common/conv3d_bp_util.h"
#include "kernel_tiling/kernel_tiling.h"
#include "kernel_basic_intf.h"

struct Out2L1ScalarParams {
    // to L1A
    bool isLoad2L1A{false};
    bool isLastMAL1{false};
    bool isFreeAL1{false};
    uint64_t out2A1SrcAddr{0};

    // to L1B
    bool isLoad2L1B{false};
    bool isFreeBL1{false};
    uint64_t out2B1SrcAddr{0};
    uint64_t singleShapeHi{0};
    uint32_t bL1cin1CopyLen{0};
};

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "dav_v220/conv3d_bp_filter_sub_func.h"
#endif

DECLARE_CHECK_IMPL(ConvBackpropFilterFunc, Init);
DECLARE_CHECK_IMPL(ConvBackpropFilterFunc, SetInput);
DECLARE_CHECK_IMPL(ConvBackpropFilterFunc, SetGradOutput);
DECLARE_CHECK_IMPL(ConvBackpropFilterFunc, SetSingleShape);
DECLARE_CHECK_IMPL(ConvBackpropFilterFunc, SetStartPosition);
DECLARE_CHECK_SYNC_IMPL(ConvBackpropFilterFunc, Iterate);
DECLARE_CHECK_SYNC_IMPL(ConvBackpropFilterFunc, IterateAll);
DECLARE_CHECK_SYNC_IMPL(ConvBackpropFilterFunc, GetTensorC);
DECLARE_CHECK_IMPL(ConvBackpropFilterFunc, End);
namespace ConvBackpropFilterFunc {
using TypeFalse = struct {
    __uint128_t _[1024];
};

enum class IterateOrder {
    ORDER_M = 0,
    ORDER_N,
    UNDEF,
};

template <class Intf>
__aicore__ inline void CheckTiling(Intf *self)
{
#ifdef ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT((self->ctx.tiling_->batch > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original batch is %d , which should be larger than 0", self->ctx.tiling_->batch); });
    ASCENDC_ASSERT((self->ctx.tiling_->cin > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original cin is %d , which should be larger than 0", self->ctx.tiling_->cin);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->cout > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original cout is %d , which should be larger than 0", self->ctx.tiling_->cout);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->cin1G > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original cin1G is %d , which should be larger than 0", self->ctx.tiling_->cin1G);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->cout1G > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original cout1G is %d , which should be larger than 0", self->ctx.tiling_->cout1G);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->dout > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original dout is %d , which should be larger than 0", self->ctx.tiling_->dout); });
    ASCENDC_ASSERT((self->ctx.tiling_->ho > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original ho is %d , which should be larger than 0", self->ctx.tiling_->ho); });
    ASCENDC_ASSERT((self->ctx.tiling_->wo > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original wo is %d , which should be larger than 0", self->ctx.tiling_->wo); });
    ASCENDC_ASSERT((self->ctx.tiling_->di > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original di is %d , which should be larger than 0", self->ctx.tiling_->di); });
    ASCENDC_ASSERT((self->ctx.tiling_->hi > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original hi is %d , which should be larger than 0", self->ctx.tiling_->hi); });
    ASCENDC_ASSERT((self->ctx.tiling_->wi > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original wi is %d , which should be larger than 0", self->ctx.tiling_->wi); });
    ASCENDC_ASSERT((self->ctx.tiling_->dk > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original dk is %d , which should be larger than 0", self->ctx.tiling_->dk); });
    ASCENDC_ASSERT((self->ctx.tiling_->hk > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original hk is %d , which should be larger than 0", self->ctx.tiling_->hk); });
    ASCENDC_ASSERT((self->ctx.tiling_->wk > 0),
        { KERNEL_LOG(KERNEL_ERROR, "original wk is %d , which should be larger than 0", self->ctx.tiling_->wk); });
    ASCENDC_ASSERT((self->ctx.tiling_->singleCoreBatch > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "singleCoreBatch is %d , which should be larger than 0", self->ctx.tiling_->singleCoreBatch);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->singleCoreCout > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "singleCoreCout is %d , which should be larger than 0", self->ctx.tiling_->singleCoreCout);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->singleCoreHo > 0), {
        KERNEL_LOG(KERNEL_ERROR, "singleCoreHo is %d , which should be larger than 0", self->ctx.tiling_->singleCoreHo);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->singleCoreCin > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "singleCoreCin is %d , which should be larger than 0", self->ctx.tiling_->singleCoreCin);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->baseM > 0),
        { KERNEL_LOG(KERNEL_ERROR, "baseM is %d , which should be larger than 0", self->ctx.tiling_->baseM); });
    ASCENDC_ASSERT((self->ctx.tiling_->baseK > 0),
        { KERNEL_LOG(KERNEL_ERROR, "baseK is %d , which should be larger than 0", self->ctx.tiling_->baseK); });
    ASCENDC_ASSERT((self->ctx.tiling_->baseN > 0),
        { KERNEL_LOG(KERNEL_ERROR, "baseN is %d , which should be larger than 0", self->ctx.tiling_->baseN); });
    ASCENDC_ASSERT((self->ctx.tiling_->stepM > 0),
        { KERNEL_LOG(KERNEL_ERROR, "stepM is %d , which should be larger than 0", self->ctx.tiling_->stepM); });
    ASCENDC_ASSERT((self->ctx.tiling_->stepN > 0),
        { KERNEL_LOG(KERNEL_ERROR, "stepN is %d , which should be larger than 0", self->ctx.tiling_->stepN); });
    ASCENDC_ASSERT((self->ctx.tiling_->stepKa > 0),
        { KERNEL_LOG(KERNEL_ERROR, "stepKa is %d , which should be larger than 0", self->ctx.tiling_->stepKa); });
    ASCENDC_ASSERT((self->ctx.tiling_->stepKb > 0),
        { KERNEL_LOG(KERNEL_ERROR, "stepKb is %d , which should be larger than 0", self->ctx.tiling_->stepKb); });
#endif
}

template <class Intf>
__aicore__ inline void InitStepMParams(Intf *self)
{
    self->ctx.mIter_ = ConvBackpropApi::Ceil(self->ctx.singleShapeCout_, self->ctx.tiling_->baseM);
    self->ctx.tailM_ = self->ctx.singleShapeCout_ - self->ctx.tiling_->baseM * (self->ctx.mIter_ - 1);
#ifdef ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT((self->ctx.mIter_ > 0),
        { KERNEL_LOG(KERNEL_ERROR, "ctx.mIter_ is %d , which should be larger than 0", self->ctx.mIter_); });
#endif
}

template <class Intf>
__aicore__ inline void InitStepKParams(Intf *self)
{
    uint64_t singleCoreHoWo = static_cast<uint64_t>(self->ctx.singleShapeHo_) * self->ctx.tiling_->wo;
    uint64_t kIter = ConvBackpropApi::Ceil(singleCoreHoWo, self->ctx.tiling_->baseK);
    self->ctx.kIter_ = kIter;
    self->ctx.tailK_ = singleCoreHoWo - self->ctx.tiling_->baseK * (kIter - 1);
    self->ctx.stepKaRound = ConvBackpropApi::Ceil(kIter, self->ctx.tiling_->stepKa);
    self->ctx.stepKbRound = ConvBackpropApi::Ceil(kIter, self->ctx.tiling_->stepKb);
#ifdef ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT((self->ctx.kIter_ > 0),
        { KERNEL_LOG(KERNEL_ERROR, "ctx.kIter_ is %d , which should be larger than 0", self->ctx.kIter_); });
#endif
}

template <class Intf>
__aicore__ inline void InitStepNParams(Intf *self)
{
    self->ctx.nIter_ = ConvBackpropApi::Ceil(ConvBackpropApi::ShiftCeilChannelSize<Intf>(self->ctx.singleShapeCin_, self->ctx.tiling_->channelSize) *
        self->ctx.tiling_->channelSize * self->ctx.hwK_, self->ctx.tiling_->baseN);
    self->ctx.tailN_ =
        self->ctx.singleShapeCin_ * self->ctx.hwK_ - self->ctx.tiling_->baseN * (self->ctx.nIter_ - 1);
#ifdef ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT((self->ctx.nIter_ > 0),
        { KERNEL_LOG(KERNEL_ERROR, "ctx.nIter_ is %d , which should be larger than 0", self->ctx.nIter_); });
#endif
}

template <class Intf>
__aicore__ inline void InitParams(Intf *self)
{
    self->ctx.baseMK_ = self->ctx.tiling_->baseM * self->ctx.tiling_->baseK;
    self->ctx.baseKN_ = self->ctx.tiling_->baseK * self->ctx.tiling_->baseN;
    self->ctx.baseMN_ = self->ctx.tiling_->baseM * self->ctx.tiling_->baseN;
    self->ctx.hwK_ = self->ctx.tiling_->hk * self->ctx.tiling_->wk;
    self->ctx.hwO_ = static_cast<uint64_t>(self->ctx.tiling_->ho) * self->ctx.tiling_->wo;
    self->ctx.hwI_ = static_cast<uint64_t>(self->ctx.tiling_->hi) * self->ctx.tiling_->wi;
    self->ctx.kal1_ = self->ctx.tiling_->stepKa * self->ctx.tiling_->baseK;
    self->ctx.kbl1_ = self->ctx.tiling_->stepKb * self->ctx.tiling_->baseK;

    // simplify the calculation of hi = (ho - 1) * strideh + (hk - 1) * dilationh + 1
    self->ctx.strideKernelDilationH = static_cast<int64_t>(self->ctx.tiling_->hk - 1) * self->ctx.tiling_->dilationH
        + 1 - self->ctx.tiling_->strideH;

    self->ctx.isFirstIter_ = true;
    self->ctx.l0aPingPongFlag_ = 0;
    self->ctx.l0bPingPongFlag_ = 0;
    self->ctx.l0cPingPongFlag_ = 1;
    self->ctx.useL0PingPong_ = (self->ctx.tiling_->al0Pbuffer - 1) & (self->ctx.tiling_->bl0Pbuffer - 1);
    InitLoadToB2Params<Intf>(self);
    InitLoadToA2Params<Intf>(self);
    InitSetFmatrixParams<Intf>(self);
    InitMmadParams<Intf>(self);
}

template <class Intf>
__aicore__ inline void InitTque(Intf *self)
{
    uint32_t aMatrixByteSize = self->ctx.tiling_->stepM * self->ctx.tiling_->baseM *
        self->ctx.tiling_->stepKa * self->ctx.tiling_->baseK * sizeof(typename Intf::SrcT);
    uint32_t bl1BoundByteSize = self->ctx.tiling_->bl1Bound * sizeof(typename Intf::SrcT);
    self->ctx.pipe_.InitBuffer(self->ctx.a1Ping_, 1, aMatrixByteSize);
    self->ctx.pipe_.InitBuffer(self->ctx.b1Ping_, 1, bl1BoundByteSize);
    if (self->ctx.tiling_->al1Pbuffer > 1) {
        self->ctx.pipe_.InitBuffer(self->ctx.a1Pong_, 1, aMatrixByteSize);
    }
    if (self->ctx.tiling_->bl1Pbuffer > 1) {
        self->ctx.pipe_.InitBuffer(self->ctx.b1Pong_, 1, bl1BoundByteSize);
    }
    self->ctx.pipe_.InitBuffer(self->ctx.l0cPing_, 1, self->ctx.baseMN_ * sizeof(typename Intf::L0cT));
    if (self->ctx.tiling_->cl0Pbuffer > 1) {
        self->ctx.pipe_.InitBuffer(self->ctx.l0cPong_, 1, self->ctx.baseMN_ * sizeof(typename Intf::L0cT));
    }
    self->ctx.pipe_.InitBuffer(self->ctx.l0aBuf_, AscendC::TOTAL_L0A_SIZE);
    self->ctx.pipe_.InitBuffer(self->ctx.l0bBuf_, AscendC::TOTAL_L0B_SIZE);
}

template <class Intf>
__aicore__ inline void CalcParamsL12L0b(Intf *self, uint64_t kPos)
{
    // load3dStepK
    self->ctx.load3dB_.kExtension = self->ctx.baseUseN_;
    // posK
    uint32_t localN = ConvBackpropApi::ShiftDivChannelSize<Intf>(self->ctx.tiling_->baseN, self->ctx.tiling_->channelSize);
    uint32_t localUseN = ConvBackpropApi::ShiftDivChannelSize<Intf>(self->ctx.baseUseN_, self->ctx.tiling_->channelSize);
    uint32_t kStartLocal = ConvBackpropApi::RemainderOfHkWk(self->ctx.curNL1Idx_ * localN, self->ctx.hwK_) +
        ConvBackpropApi::RemainderStepN(self->ctx.curNL0Idx_, self->ctx.tiling_->stepN) * localN;
    self->ctx.load3dB_.kStartPt = kStartLocal * self->ctx.tiling_->channelSize;
    self->ctx.load3dB_.channelSize =
        ConvBackpropApi::CeilHkWk(kStartLocal + localUseN, self->ctx.hwK_) * self->ctx.tiling_->channelSize;
}

template <class Intf>
__aicore__ inline void CalcParamsL12L0a(Intf *self, uint64_t kPos)
{
    uint32_t alignedBaseUseM = ConvBackpropApi::ShiftCeilM0(self->ctx.baseUseM_, self->ctx.tiling_->m0) * self->ctx.tiling_->m0;
    self->ctx.load3dA_.kExtension = alignedBaseUseM;
    self->ctx.load3dA_.channelSize = alignedBaseUseM;
}

template <class Intf>
__aicore__ inline void LoadL12L0b(Intf *self, const AscendC::LocalTensor<typename Intf::SrcT> &l1BMatrix,
                                  AscendC::LocalTensor<typename Intf::SrcT> &l0b)
{
    static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG_220 = {false, true};
    SetFmatrix(self->ctx.load3dB_.l1H, self->ctx.load3dB_.l1W, self->ctx.load3dB_.padList,
               AscendC::FmatrixMode::FMATRIX_RIGHT);
    AscendC::LoadData<typename Intf::SrcT, LOAD3D_CONFIG_220>(l0b, l1BMatrix, self->ctx.load3dB_);
}

template <class Intf>
__aicore__ inline void FreeA1Tensor(Intf *self, bool a1PingPongFlag)
{
    if (a1PingPongFlag) {
        self->ctx.a1Ping_.FreeTensor(self->ctx.cacheA1BufPing_);
#ifdef ASCENDC_CPU_DEBUG
        self->ctx.cacheA1BufPing_.SetSize(0);
#endif
    } else {
        self->ctx.a1Pong_.FreeTensor(self->ctx.cacheA1BufPong_);
#ifdef ASCENDC_CPU_DEBUG
        self->ctx.cacheA1BufPong_.SetSize(0);
#endif
    }
}

template <class Intf>
__aicore__ inline void FreeB1Tensor(Intf *self, bool b1PingPongFlag)
{
    if (b1PingPongFlag) {
        self->ctx.b1Ping_.FreeTensor(self->ctx.cacheB1BufPing_);
#ifdef ASCENDC_CPU_DEBUG
        self->ctx.cacheB1BufPing_.SetSize(0);
#endif
    } else {
        self->ctx.b1Pong_.FreeTensor(self->ctx.cacheB1BufPong_);
#ifdef ASCENDC_CPU_DEBUG
        self->ctx.cacheB1BufPong_.SetSize(0);
#endif
    }
}

template <class Intf>
__aicore__ inline void Compute(Intf *self, Out2L1ScalarParams& out2L1Params)
{
    CalcParamsL12L0b<Intf>(self, 0);
    CalcParamsL12L0a<Intf>(self, 0);
    CalcParamsMmad<Intf>(self, 0);
    AscendC::LocalTensor<typename Intf::SrcT> l0a;
    AscendC::LocalTensor<typename Intf::SrcT> l0b;
    AscendC::LocalTensor<typename Intf::L0cT> l0c;
    uint32_t curML0IdxModstepKaMulBaseM = ConvBackpropApi::RemainderStepM(self->ctx.curML0Idx_, self->ctx.tiling_->stepM) * self->ctx.tiling_->baseM;
    constexpr uint32_t l0aPingPongAddr = AscendC::TOTAL_L0A_SIZE / 2 / sizeof(typename Intf::SrcT);
    constexpr uint32_t l0bPingPongAddr = AscendC::TOTAL_L0B_SIZE / 2 / sizeof(typename Intf::SrcT);
    CalOut2L1ScalarParams(self, out2L1Params);

    if (self->ctx.l0cPingPongFlag_) {
        l0c = self->ctx.l0cPing_.template AllocTensor<typename Intf::L0cT>();
    } else {
        l0c = self->ctx.l0cPong_.template AllocTensor<typename Intf::L0cT>();
    }

    bool a1PingPongFlag = true;
    bool b1PingPongFlag = true;
    bool isAL1PingPong = self->ctx.tiling_->al1Pbuffer > 1;
    bool isBL1PingPong = self->ctx.tiling_->bl1Pbuffer > 1;
    uint32_t kaIdx = 0;
    uint32_t kbIdx = 0;
    uint64_t kaStepIdx = 0;
    uint64_t kbStepIdx = 0;
    uint64_t curMKL1Idx = self->ctx.stepKaRound * ConvBackpropApi::DivStepM(self->ctx.curML1Idx_, self->ctx.tiling_->stepM);
    uint64_t curNKL1Idx = self->ctx.stepKbRound * ConvBackpropApi::DivStepN(self->ctx.curNL1Idx_, self->ctx.tiling_->stepN);

    for (uint64_t k = 0; k < self->ctx.kIter_; k++) {
        bool isLastKIter = k + 1 == self->ctx.kIter_;
        bool isLastStepKa = kaIdx + 1 == self->ctx.tiling_->stepKa;
        bool isLastStepKb = kbIdx + 1 == self->ctx.tiling_->stepKb;
        bool isLoadA1 = kaIdx == 0;
        bool isLoadB1 = kbIdx == 0;
        self->ctx.baseUseK_ = isLastKIter ? self->ctx.tailK_ : self->ctx.tiling_->baseK;

        /*
        Use the parity of M*K to determine whether the load is L1A ping or L1A pong. The same is true for BL1
                    kL1Idx=0  kL1Idx=1 kL1Idx=2
                    ----------------------------
        mL1Idx=0    |  ping  |  pong  |  ping  |
                    ----------------------------
        mL1Idx=1    |  pong  |  ping  |  pong  |
                    ----------------------------
        mL1Idx=2    |  ping  |  pong  |  ping  |
                    ----------------------------
        */
        if (self->ctx.tiling_->bl1Pbuffer > 1) {
            b1PingPongFlag = (curNKL1Idx + kbStepIdx + 1) & 1;
        }
        ConvBackpropFilterFunc::LoadToB1<Intf, typename Intf::SrcT>(self, b1PingPongFlag, k, out2L1Params, isLoadB1,
                                                                     kbStepIdx);

        if (self->ctx.tiling_->al1Pbuffer > 1) {
            a1PingPongFlag = (curMKL1Idx + kaStepIdx + 1) & 1;
        }
        ConvBackpropFilterFunc::LoadToA1<Intf, typename Intf::SrcT>(self, a1PingPongFlag, k, out2L1Params, isLoadA1,
                                                                     kaStepIdx);

        AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(self->ctx.l0aPingPongFlag_);

        uint32_t alignedBaseUseK =
            ConvBackpropApi::ShiftCeilChannelSize<Intf>(self->ctx.baseUseK_, self->ctx.tiling_->k0) * self->ctx.tiling_->k0;
        l0b = self->ctx.l0bBuf_.template Get<typename Intf::SrcT>();
        if (self->ctx.l0aPingPongFlag_) {
            l0b = l0b[l0bPingPongAddr];
        }
        // posM
        self->ctx.load3dB_.mStartPt = (k - kbIdx) * self->ctx.tiling_->baseK % self->ctx.tiling_->wo +
            kbIdx * self->ctx.tiling_->baseK;
        // load3dStepM
        self->ctx.load3dB_.mExtension = alignedBaseUseK;
        if (unlikely(out2L1Params.isLoad2L1B && isLoadB1)) {
            if (b1PingPongFlag) {
                self->ctx.cacheB1BufPing_ = self->ctx.b1Ping_.template DeQue<typename Intf::SrcT>();
            } else {
                self->ctx.cacheB1BufPong_ = self->ctx.b1Pong_.template DeQue<typename Intf::SrcT>();
            }
        }

        if (b1PingPongFlag) {
            self->ctx.load3dB_.l1H = self->ctx.bL1HiCopyLenPing;
            self->ctx.load3dB_.padList[2] = self->ctx.bL1PadUpPing;
            LoadL12L0b<Intf>(self, self->ctx.cacheB1BufPing_, l0b);
        } else {
            self->ctx.load3dB_.l1H = self->ctx.bL1HiCopyLenPong;
            self->ctx.load3dB_.padList[2] = self->ctx.bL1PadUpPong;
            LoadL12L0b<Intf>(self, self->ctx.cacheB1BufPong_, l0b);
        }

        if (out2L1Params.isFreeBL1 && (isLastStepKb || isLastKIter)) {
            FreeB1Tensor(self, b1PingPongFlag);
        }

        l0a = self->ctx.l0aBuf_.template Get<typename Intf::SrcT>();
        if (self->ctx.l0aPingPongFlag_) {
            l0a = l0a[l0aPingPongAddr];
        }
        uint32_t mOffset = curML0IdxModstepKaMulBaseM * self->ctx.curLoadKal1_;
        self->ctx.srcL12L0aOffset_ = kaIdx * self->ctx.tiling_->baseK * self->ctx.tiling_->channelSize + mOffset;
        self->ctx.load3dA_.mExtension = alignedBaseUseK;
        if (unlikely(out2L1Params.isLoad2L1A && isLoadA1)) {
            if (a1PingPongFlag) {
                self->ctx.cacheA1BufPing_ = self->ctx.a1Ping_.template DeQue<typename Intf::SrcT>();
            } else {
                self->ctx.cacheA1BufPong_ = self->ctx.a1Pong_.template DeQue<typename Intf::SrcT>();
            }
	    }

        if (a1PingPongFlag) {
            LoadL12L0a<Intf>(self, self->ctx.cacheA1BufPing_, k, l0a, out2L1Params, kaStepIdx);
        } else {
            LoadL12L0a<Intf>(self, self->ctx.cacheA1BufPong_, k, l0a, out2L1Params, kaStepIdx);
        }

        if (out2L1Params.isFreeAL1 && (isLastStepKa || isLastKIter)) {
            FreeA1Tensor(self, a1PingPongFlag);
        }

        AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(self->ctx.l0aPingPongFlag_);
        AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(self->ctx.l0aPingPongFlag_);
        self->ctx.mmad_.cmatrixInitVal = k == 0;
        self->ctx.mmad_.k = self->ctx.baseUseK_;
        MmadLocal<Intf>(self, l0a, l0b, l0c);
        AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(self->ctx.l0aPingPongFlag_);

        self->ctx.l0aPingPongFlag_ ^= self->ctx.useL0PingPong_;
        if (isLastStepKa) {
            ++kaStepIdx;
            kaIdx = 0;
        } else {
            ++kaIdx;
        }
        if (isLastStepKb) {
            ++kbStepIdx;
            kbIdx = 0;
        } else {
            ++kbIdx;
        }
    }

    if (self->ctx.l0cPingPongFlag_) {
        self->ctx.l0cPing_.EnQue(l0c);
    } else {
        self->ctx.l0cPong_.EnQue(l0c);
    }
}
template <class Intf>
__aicore__ inline void UpdateIdxAndStep(Intf *self)
{
    self->ctx.curML0Idx_ = self->ctx.curML1Idx_;
    self->ctx.curNL0Idx_ = self->ctx.curNL1Idx_;
    self->ctx.curStepM_ = (self->ctx.mIter_ - self->ctx.curML0Idx_) > self->ctx.tiling_->stepM
                                ? self->ctx.tiling_->stepM
                                : (self->ctx.mIter_ - self->ctx.curML1Idx_);
    self->ctx.curStepN_ = (self->ctx.nIter_ - self->ctx.curNL0Idx_) > self->ctx.tiling_->stepN
                                ? self->ctx.tiling_->stepN
                                : (self->ctx.nIter_ - self->ctx.curNL1Idx_);
}

template <class Intf>
struct Init {
    // Define the default overloaded function of the call function, supporting any number of parameters of any type
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropFilterFunc);
    static __aicore__ inline void call(Intf *self, const TConv3DBpFilterTiling *__restrict tiling)
    {
        self->ctx.tiling_ = tiling;
        CheckTiling<Intf>(self);
        InitParams<Intf>(self);
        InitTque<Intf>(self);
        AscendC::SetHF32Mode(self->ctx.tiling_->hf32Flag);
    }
};

template <class Intf>
struct SetInput {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropFilterFunc);
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::SrcT> &input)
    {
        self->ctx.fmapGlobal_ = input;
    }
};

template <class Intf>
struct SetGradOutput {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropFilterFunc);
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::SrcT> &gradOutput)
    {
        self->ctx.outBackPropGlobal_ = gradOutput;
    }
};

template <class Intf>
struct SetSingleShape {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropFilterFunc);
    static __aicore__ inline void call(Intf *self, uint64_t singleShapeM, uint64_t singleShapeN, uint64_t singleShapeK)
    {
        self->ctx.singleShapeCout_ = singleShapeM;
        self->ctx.singleShapeCin_ = ConvBackpropApi::DivHkWk(singleShapeN, self->ctx.hwK_);
        // 8 alignment on GM in fp32 scenario
        self->ctx.singleShapeCin_ =
            ConvBackpropApi::Ceil(self->ctx.singleShapeCin_, self->ctx.tiling_->channelSize) * self->ctx.tiling_->channelSize;
        self->ctx.singleShapeHo_ = singleShapeK / self->ctx.tiling_->wo;
        InitStepMParams<Intf>(self);
        InitStepKParams<Intf>(self);
        InitStepNParams<Intf>(self);
    }
};

template <class Intf>
struct SetStartPosition {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropFilterFunc);
    static __aicore__ inline void call(Intf *self, uint32_t hoStartIdx) {
        self->ctx.hoStartIdx_ = hoStartIdx;
        self->ctx.hiStartIdx_ = hoStartIdx * self->ctx.tiling_->strideH - self->ctx.tiling_->padUp;
    }
};

template <class Intf, bool sync>
struct Iterate {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropFilterFunc);
    static __aicore__ inline bool call(Intf *self, bool enPartialSum)
    {
        /*
        |   <---------singleShapeM------->        |
        |  <---L1A_ping--->  |  <---L1A_pong--->  |
        |_L0A1_|_L0A2_|_L0A3_|_L0A4_|_L0A5_|_L0A6_|
        â†? <--curStepM_-->    |                    â†?
        curML0Idx_            â†?                 mIter_
        curML1Idx_        next_curML1Idx

        |   <---------singleShapeN------->        |
        |  <---L1B_ping--->  |  <---L1B_pong--->  |
        |_L0B1_|_L0B2_|_L0B3_|_L0B4_|_L0B5_|_L0B6_|
        â†? <--curStepN_-->    |                    â†?
        curNL0Idx_            â†?                  nIter_
        curNL1Idx_       next_curNL1Idx

        order_N represents the resident B loop A on L1, the order is L1A_ping * L1B_ping, L1A_pong * L1B_ping, L1A_ping * L1B_pong, L1A_pong * L1B_pong
        L0 also resides on B and loops on A
        order_N: L0A1*L0B1, L0A2*L0B1, L0A3*L0B1, L0A1*L0B2 â€¦â€¦â€¦â€?L0A3*L0B3ï¼ŒL0A4*L0B1ï¼ŒL0A5*L0B1 â€¦â€?L0A6*L0B6
        order_M: L0A1*L0B1, L0A1*L0B2, L0A1*L0B3, L0A2*L0B1 â€¦â€¦â€¦â€?L0A3*L0B3ï¼ŒL0A1*L0B4ï¼ŒL0A1*L0B5 â€¦â€?L0A6*L0B6
        */
        // Update idx, use three pointers L1, L1step, L0 to control the position and calculate offset, indicating which mL0 * baseN is calculated

        // Taking opening DB as an example, when looping sequence NMK, if two buffers are needed in the K direction, the M axis needs to release the ping pong buffer on AL1 every time it loops to stepM for the last time, and load it in the first loop
        // If the cycle sequence is MNK, if two buffers are needed in the K direction, the ping pong buffer on AL1 needs to be released during the last cycle of the M axis and loaded in the first cycle
        // If the number of buffers required in the K direction is greater than bl1Pbuffer, AL1 needs to be replaced when K loops to stepKa
        // The calculation idea of â€‹â€‹matrix B is the same as that of matrix A. The difference is that MN is reversed
        Out2L1ScalarParams out2L1Params;
        bool kIterCeilStepKaGreaterAl1Pbuffer =
            self->ctx.kIter_ > self->ctx.tiling_->stepKa * self->ctx.tiling_->al1Pbuffer;
        bool kIterCeilStepKbGreaterBl1Pbuffer =
            self->ctx.kIter_ > self->ctx.tiling_->stepKb * self->ctx.tiling_->bl1Pbuffer;
        out2L1Params.isLoad2L1A = kIterCeilStepKaGreaterAl1Pbuffer;
        out2L1Params.isFreeAL1 = kIterCeilStepKaGreaterAl1Pbuffer;
        out2L1Params.isLoad2L1B = kIterCeilStepKbGreaterBl1Pbuffer;
        out2L1Params.isFreeBL1 = kIterCeilStepKbGreaterBl1Pbuffer;
        if (unlikely(self->ctx.isFirstIter_)) {
            self->ctx.curML0Idx_ = 0;
            self->ctx.curNL0Idx_ = 0;
            self->ctx.curML1Idx_ = 0;
            self->ctx.curNL1Idx_ = 0;
            self->ctx.isFirstIter_ = false;
            self->ctx.curStepM_ = self->ctx.mIter_ > self->ctx.tiling_->stepM
                                        ? self->ctx.tiling_->stepM
                                        : self->ctx.mIter_;
            self->ctx.curStepN_ = self->ctx.nIter_ > self->ctx.tiling_->stepN
                                        ? self->ctx.tiling_->stepN
                                        : self->ctx.nIter_;
            bool isLastNLoop = self->ctx.nIter_ == 1;
            bool isLastMLoop = self->ctx.mIter_ == 1;
            bool isNLastStep = isLastNLoop || self->ctx.tiling_->stepN == 1;
            bool isMLastStep = isLastMLoop || self->ctx.tiling_->stepM == 1;
            out2L1Params.isLoad2L1A = true;
            out2L1Params.isFreeAL1 = kIterCeilStepKaGreaterAl1Pbuffer ||
                (self->ctx.tiling_->iterateOrder && isMLastStep) ||                  // OrderN
                (!(self->ctx.tiling_->iterateOrder) && isLastNLoop && isMLastStep);  // OrderM
            out2L1Params.isLoad2L1B = true;
            out2L1Params.isFreeBL1 = kIterCeilStepKbGreaterBl1Pbuffer ||
                (self->ctx.tiling_->iterateOrder && isLastMLoop && isNLastStep) ||    // OrderN
                (!(self->ctx.tiling_->iterateOrder) && isNLastStep);                  // OrderM
        } else if (likely(self->ctx.tiling_->iterateOrder == static_cast<int>(IterateOrder::ORDER_N))) {
            if (++self->ctx.curML0Idx_ >= self->ctx.curML1Idx_ + self->ctx.curStepM_) {
                self->ctx.curML0Idx_ = self->ctx.curML1Idx_;
                if (++self->ctx.curNL0Idx_ >= self->ctx.curNL1Idx_ + self->ctx.curStepN_) {
                    self->ctx.curML1Idx_ += self->ctx.curStepM_;
                    if (self->ctx.curNL0Idx_ >= self->ctx.nIter_ && self->ctx.curML1Idx_ >= self->ctx.mIter_) {
                        return false;
                    }
                    if (self->ctx.curML1Idx_ >= self->ctx.mIter_) {
                        self->ctx.curML1Idx_ = 0;
                        self->ctx.curNL1Idx_ += self->ctx.curStepN_;
                    }
                    UpdateIdxAndStep<Intf>(self);
                    if (self->ctx.curML0Idx_ == 0) {
                        out2L1Params.isLoad2L1B = true; // OrderN, N axis cycle ends, BL1 needs to be replaced
                    }
                }
                out2L1Params.isLoad2L1A = true; // OrderN, M-axis cycle ends, AL1 needs to be replaced
            }
            if (unlikely(self->ctx.curML0Idx_ == self->ctx.mIter_ - 1) ||
                self->ctx.curML0Idx_ == self->ctx.curML1Idx_ + self->ctx.curStepM_ - 1) {
                out2L1Params.isFreeAL1 = true; // OrderN, the last cycle of the M axis, AL1 needs to be released
            }
            if (unlikely(self->ctx.curML0Idx_ == self->ctx.mIter_ - 1) &&
                (unlikely(self->ctx.curNL0Idx_ == self->ctx.nIter_ - 1) ||
                self->ctx.curNL0Idx_ == self->ctx.curNL1Idx_ + self->ctx.curStepN_ - 1)) {
                out2L1Params.isFreeBL1 = true; // OrderN, the last cycle of N axis, needs to release BL1
            }
        } else {  // order_M
            if (++self->ctx.curNL0Idx_ >= self->ctx.curNL1Idx_ + self->ctx.curStepN_) {
                self->ctx.curNL0Idx_ = self->ctx.curNL1Idx_;
                if (++self->ctx.curML0Idx_ >= self->ctx.curML1Idx_ + self->ctx.curStepM_) {
                    self->ctx.curNL1Idx_ += self->ctx.curStepN_;
                    if (self->ctx.curML0Idx_ >= self->ctx.mIter_ && self->ctx.curNL1Idx_ >= self->ctx.nIter_) {
                        return false;
                    }
                    if (self->ctx.curNL1Idx_ >= self->ctx.nIter_) {
                        self->ctx.curNL1Idx_ = 0;
                        self->ctx.curML1Idx_ += self->ctx.curStepM_;
                    }
                    UpdateIdxAndStep<Intf>(self);
                    if (self->ctx.curNL0Idx_ == 0) {
                        out2L1Params.isLoad2L1A = true; // OrderM, M axis cycle ends, AL1 needs to be replaced
                    }
                }
                out2L1Params.isLoad2L1B = true; // OrderM, N-axis cycle ends, BL1 needs to be replaced
            }
            if (unlikely(self->ctx.curNL0Idx_ == self->ctx.nIter_ - 1) &&
                (unlikely(self->ctx.curML0Idx_ == self->ctx.mIter_ - 1) ||
                self->ctx.curML0Idx_ == self->ctx.curML1Idx_ + self->ctx.curStepM_ - 1)) {
                out2L1Params.isFreeAL1 = true; // OrderM, the last cycle of the M axis, AL1 needs to be released
            }
            if (unlikely(self->ctx.curNL0Idx_ == self->ctx.nIter_ - 1) ||
                self->ctx.curNL0Idx_ == self->ctx.curNL1Idx_ + self->ctx.curStepN_ - 1) {
                out2L1Params.isFreeBL1 = true; // OrderM, the last cycle of N axis, needs to release BL1
            }
        }
        self->ctx.baseUseM_ =
            (self->ctx.curML0Idx_ + 1 == self->ctx.mIter_) ? self->ctx.tailM_ : self->ctx.tiling_->baseM;
        self->ctx.baseUseN_ =
            (self->ctx.curNL0Idx_ + 1 == self->ctx.nIter_) ? self->ctx.tailN_ : self->ctx.tiling_->baseN;
        if ASCEND_IS_AIC {
            Compute<Intf>(self, out2L1Params);
        }
        return true;
    }
};

template <class Intf, bool sync>
struct IterateAll {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropFilterFunc);
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::DstT> &output, uint8_t enAtomic)
    {
        while (self->template Iterate<sync>()) {
            self->template GetTensorC<sync>(output, enAtomic);
        }
        self->ctx.isFirstIter_ = true;
    }
};

template <class Intf, bool sync>
struct GetTensorC {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropFilterFunc);
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::DstT> &output,
        uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        LoadL0c2Gm<Intf>(self, output, enAtomic, enSequentialWrite);
    }
};

template <class Intf>
struct End {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropFilterFunc);
    static __aicore__ inline void call(Intf *self)
    {
        self->ctx.a1Ping_.FreeAllEvent();
        if (self->ctx.tiling_->al1Pbuffer > 1) {
            self->ctx.a1Pong_.FreeAllEvent();
        }
        self->ctx.b1Ping_.FreeAllEvent();
        if (self->ctx.tiling_->bl1Pbuffer > 1) {
            self->ctx.b1Pong_.FreeAllEvent();
        }
        self->ctx.l0cPing_.FreeAllEvent();
        if (self->ctx.tiling_->cl0Pbuffer > 1) {
            self->ctx.l0cPong_.FreeAllEvent();
        }

        if (self->ctx.tiling_->hf32Flag) {
            AscendC::SetHF32Mode(false);
        }
    }
};

}  // namespace ConvBackpropFilterFunc
#endif // CONV3D_BP_FILTER_FUNC_H
