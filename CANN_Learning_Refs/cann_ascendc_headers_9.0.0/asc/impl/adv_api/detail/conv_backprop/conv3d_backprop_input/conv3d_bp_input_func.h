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
 * \file conv3d_bp_input_func.h
 * \brief
 */

#ifndef CONV3D_BP_INPUT_FUNC_H
#define CONV3D_BP_INPUT_FUNC_H

#include "include/adv_api/conv_backprop/common/conv3d_bp_config_base.h"
#include "../common/conv3d_bp_util.h"
#include "conv3d_bp_input_init_func.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"

DECLARE_CHECK_IMPL(ConvBackpropInputFunc, Init);
DECLARE_CHECK_IMPL(ConvBackpropInputFunc, SetOutBackprop);
DECLARE_CHECK_IMPL(ConvBackpropInputFunc, SetWeight);
DECLARE_CHECK_IMPL(ConvBackpropInputFunc, SetSingleShape);
DECLARE_CHECK_IMPL(ConvBackpropInputFunc, SetStartPosition);
DECLARE_CHECK_SYNC_IMPL(ConvBackpropInputFunc, Iterate);
DECLARE_CHECK_SYNC_IMPL(ConvBackpropInputFunc, IterateAll);
DECLARE_CHECK_SYNC_IMPL(ConvBackpropInputFunc, GetTensorC);
DECLARE_CHECK_IMPL(ConvBackpropInputFunc, End);
namespace ConvBackpropInputFunc {
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
    ASCENDC_ASSERT((self->ctx.tiling_->batch > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original batch is %d , which should be larger than 0", self->ctx.tiling_->batch);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->cin > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original cin is %d , which should be larger than 0", self->ctx.tiling_->cin);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->cout > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original cout is %d , which should be larger than 0", self->ctx.tiling_->cout);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->ho > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original ho is %d , which should be larger than 0", self->ctx.tiling_->ho);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->wo > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original wo is %d , which should be larger than 0", self->ctx.tiling_->wo);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->hi > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original hi is %d , which should be larger than 0", self->ctx.tiling_->hi);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->wi > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original wi is %d , which should be larger than 0", self->ctx.tiling_->wi);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->hk > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original hk is %d , which should be larger than 0", self->ctx.tiling_->hk);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->wk > 0), {
        KERNEL_LOG(KERNEL_ERROR, "original wk is %d , which should be larger than 0", self->ctx.tiling_->wk);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->stepM == 1 && self->ctx.tiling_->stepN == 1), {
        KERNEL_LOG(KERNEL_ERROR, "stepM or stepN is invalid.");
    });
    ASCENDC_ASSERT((self->ctx.tiling_->singleCoreBatch > 0), {
        KERNEL_LOG(KERNEL_ERROR, "singleCoreBatch is %d , which should be larger than 0",
                   self->ctx.tiling_->singleCoreBatch);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->singleCoreCout > 0), {
        KERNEL_LOG(KERNEL_ERROR, "singleCoreCout is %d , which should be larger than 0",
                   self->ctx.tiling_->singleCoreCout);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->singleCoreHo > 0), {
        KERNEL_LOG(KERNEL_ERROR, "singleCoreHo is %d , which should be larger than 0", self->ctx.tiling_->singleCoreHo);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->singleCoreCin > 0), {
        KERNEL_LOG(KERNEL_ERROR, "singleCoreCin is %d , which should be larger than 0",
                   self->ctx.tiling_->singleCoreCin);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->baseM > 0), {
        KERNEL_LOG(KERNEL_ERROR, "baseM is %d , which should be larger than 0", self->ctx.tiling_->baseM);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->baseK > 0), {
        KERNEL_LOG(KERNEL_ERROR, "baseK is %d , which should be larger than 0", self->ctx.tiling_->baseK);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->baseN > 0), {
        KERNEL_LOG(KERNEL_ERROR, "baseN is %d , which should be larger than 0", self->ctx.tiling_->baseN);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->stepM > 0), {
        KERNEL_LOG(KERNEL_ERROR, "stepM is %d , which should be larger than 0", self->ctx.tiling_->stepM);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->stepN > 0), {
        KERNEL_LOG(KERNEL_ERROR, "stepN is %d , which should be larger than 0", self->ctx.tiling_->stepN);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->stepKa > 0), {
        KERNEL_LOG(KERNEL_ERROR, "stepKa is %d , which should be larger than 0", self->ctx.tiling_->stepKa);
    });
    ASCENDC_ASSERT((self->ctx.tiling_->stepKb > 0), {
        KERNEL_LOG(KERNEL_ERROR, "stepKb is %d , which should be larger than 0", self->ctx.tiling_->stepKb);
    });
#endif
}

template <class Intf>
static __aicore__ inline void UpdateIdxAndStep(Intf *self)
{
    self->ctx.curML0Idx_ = self->ctx.curML1Idx_;
    self->ctx.curNL0Idx_ = self->ctx.curNL1Idx_;
    // Ho absolute coordinates after current magnification
    if constexpr (Intf::conv3dConfig.enableKernelSplit) {
        self->ctx.curHoIdx_ =
            self->ctx.curHoStartIdx_ + self->ctx.curML0Idx_ * self->ctx.tiling_->baseM / self->ctx.splitWi_ *
            self->ctx.tiling_->strideH;
    } else {
        self->ctx.curHoIdx_ =
            self->ctx.curHoStartIdx_ + self->ctx.curML0Idx_ * self->ctx.tiling_->baseM / self->ctx.tiling_->wi;
    }

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
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropInputFunc);
    static __aicore__ inline void call(Intf *self, const TConv3DBackpropInputTiling *__restrict tiling)
    {
        self->ctx.tiling_ = tiling;
        AscendC::SetHF32Mode(self->ctx.tiling_->hf32Flag);
        CheckTiling<Intf>(self);
        InitParams<Intf>(self);
        InitTque<Intf>(self);    
    }
};

template <class Intf>
struct SetWeight {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropInputFunc);
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::SrcT> &weight)
    {
        self->ctx.weightGlobal_ = weight;
    }
};

template <class Intf>
struct SetOutBackprop {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropInputFunc);
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::SrcT> &outBackprop)
    {
        self->ctx.outBackPropGlobal_ = outBackprop;
    }
};

template <class Intf>
struct SetSingleShape {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropInputFunc);
    static __aicore__ inline void call(Intf *self, uint64_t singleShapeM, uint64_t singleShapeK, uint32_t singleShapeN)
    {
        self->ctx.singleShapeDin_ = 1;
        self->ctx.singleShapeM_ = singleShapeM;
        self->ctx.singleShapeCin1_ = (singleShapeN + self->ctx.tiling_->c0 -1) >> self->ctx.tiling_->c0Bits;
        self->ctx.singleShapeCout1_ = singleShapeK / self->ctx.DkHkWkC0_;
        if constexpr (Intf::conv3dConfig.enableKernelSplit) {
            self->ctx.splitSingleShapeM_ = singleShapeM / (self->ctx.tiling_->strideH * self->ctx.tiling_->strideW);
        }
        self->ctx.singleShapeHin_ = singleShapeM / self->ctx.tiling_->wi;
        self->ctx.singleShapeCin_ = singleShapeN;
        InitStepMParams<Intf>(self);
        InitStepKParams<Intf>(self);
        InitStepNParams<Intf>(self);

        self->ctx.kIterStepKaTail = (ConvBackpropApi::Ceil(self->ctx.kIter_, self->ctx.tiling_->stepKa) - 1) * self->ctx.tiling_->stepKa;
        self->ctx.kIterStepKbTail = (ConvBackpropApi::Ceil(self->ctx.kIter_, self->ctx.tiling_->stepKb) - 1) * self->ctx.tiling_->stepKb;
        self->ctx.stepKaTail = self->ctx.kIter_ - self->ctx.kIterStepKaTail;
        self->ctx.stepKbTail = self->ctx.kIter_ - self->ctx.kIterStepKbTail;
    }
};

template <class Intf>
struct SetStartPosition {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropInputFunc);
    static __aicore__ inline void call(Intf *self, uint32_t curDinStartIdx, int32_t curHoStartIdx) {
        self->ctx.curDinStartIdx_ = curDinStartIdx;
        self->ctx.curHoStartIdx_ = curHoStartIdx;
    }
};

template <class Intf>
static __aicore__ inline void JudgeIterateSkip(Intf *self)
{
    self->ctx.needComputeFlag_ = true;
    UpdateCurHoSize<Intf>(self);

    // Use needComputeFlag_ to mark whether you need to skip the calculation of curML0Idx, curNL0Idx, curDinIdx
    uint32_t hDstDataSkipLine = CalcHDstDataSkipLine(self);

    if (self->ctx.curHoSize_ <= hDstDataSkipLine && self->ctx.tiling_->initOutputFlag == 1) {
        // After skipping the calculation logic, if there are some additional operations that do not need to be skipped, they will be processed uniformly in the compute logic. The current existing operation is isFreeB1
        self->ctx.needComputeFlag_ = false;
    }
}

template <class Intf, bool sync>
struct Iterate {
    // An iterate calculation (baseM, baseN, baseD), current baseD=1
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropInputFunc);
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

        order_N represents the resident B loop A on L1, the order is L1A_ping * L1B_ping, L1A_pong * L1B_ping, L1A_ping * L1B_pong, L1A_pong *
        L1B_pong L0 also has resident B, loop A order_N: L0A1*L0B1, L0A2*L0B1, L0A3*L0B1, L0A1*L0B2â€¦â€¦â€¦â€?
        L0A3*L0B3ï¼ŒL0A4*L0B1ï¼ŒL0A5*L0B1 â€¦â€?L0A6*L0B6 order_M: L0A1*L0B1, L0A1*L0B2, L0A1*L0B3, L0A2*L0B1 â€¦â€¦â€¦â€?
        L0A3*L0B3ï¼ŒL0A1*L0B4ï¼ŒL0A1*L0B5 â€¦â€?L0A6*L0B6
        */
        // Update idx, use three pointers L1, L1step, L0 to control the position and calculate offset, indicating which mL0 * baseN is calculated
        if (unlikely(self->ctx.isFirstIter_)) {
            self->ctx.curML0Idx_ = 0;
            self->ctx.curNL0Idx_ = 0;
            self->ctx.curML1Idx_ = 0;
            self->ctx.curNL1Idx_ = 0;
            self->ctx.curDinIdx_ = self->ctx.curDinStartIdx_;
            self->ctx.curHoIdx_ = self->ctx.curHoStartIdx_;
            if constexpr(std::is_same<typename Intf::SrcT, float>::value) {
                self->ctx.curPingCoutIdx_ = 0;
                self->ctx.curPongCoutIdx_ = 0;
            }
            self->ctx.isFirstIter_ = false;
            self->ctx.isLoadB1_ = true;
            self->ctx.isFreeB1_ = false;
            self->ctx.curStepM_ = (self->ctx.mIter_ - self->ctx.curML0Idx_) > self->ctx.tiling_->stepM
                                      ? self->ctx.tiling_->stepM
                                      : (self->ctx.mIter_ - self->ctx.curML1Idx_);
            self->ctx.curStepN_ = (self->ctx.nIter_ - self->ctx.curNL0Idx_) > self->ctx.tiling_->stepN
                                      ? self->ctx.tiling_->stepN
                                      : (self->ctx.nIter_ - self->ctx.curNL1Idx_);
        } else if (likely(self->ctx.tiling_->iterateOrder == static_cast<int>(IterateOrder::ORDER_N))) {
            if (++self->ctx.curML0Idx_ >= self->ctx.curML1Idx_ + self->ctx.curStepM_) {
                self->ctx.curML0Idx_ = self->ctx.curML1Idx_;
                if (++self->ctx.curNL0Idx_ >= self->ctx.curNL1Idx_ + self->ctx.curStepN_) {
                    self->ctx.curML1Idx_ += self->ctx.curStepM_;
                    if (self->ctx.curNL0Idx_ >= self->ctx.nIter_ && self->ctx.curML1Idx_ >= self->ctx.mIter_) {
                        self->ctx.curML1Idx_ = 0;
                        self->ctx.curNL1Idx_ = 0;
                        if (++self->ctx.curDinIdx_ >= self->ctx.curDinStartIdx_ + self->ctx.singleShapeDin_) {
                            return false;
                        }
                    }
                    if (self->ctx.curML1Idx_ >= self->ctx.mIter_) {
                        self->ctx.curML1Idx_ = 0;
                        self->ctx.curNL1Idx_ += self->ctx.curStepN_;
                    }
                    UpdateIdxAndStep<Intf>(self);
                }
            }
        } else {  // order_M
            if (++self->ctx.curNL0Idx_ >= self->ctx.curNL1Idx_ + self->ctx.curStepN_) {
                self->ctx.curNL0Idx_ = self->ctx.curNL1Idx_;
                if (++self->ctx.curML0Idx_ >= self->ctx.curML1Idx_ + self->ctx.curStepM_) {
                    self->ctx.curNL1Idx_ += self->ctx.curStepN_;
                    if (self->ctx.curML0Idx_ >= self->ctx.mIter_ && self->ctx.curNL1Idx_ >= self->ctx.nIter_) {
                        self->ctx.curML1Idx_ = 0;
                        self->ctx.curNL1Idx_ = 0;
                        if (++self->ctx.curDinIdx_ >= self->ctx.curDinStartIdx_ + self->ctx.singleShapeDin_) {
                            return false;
                        }
                    }
                    if (self->ctx.curNL1Idx_ >= self->ctx.nIter_) {
                        self->ctx.curNL1Idx_ = 0;
                        self->ctx.curML1Idx_ += self->ctx.curStepM_;
                    }
                    UpdateIdxAndStep<Intf>(self);
                }
            }
        }
        self->ctx.isFreeB1_ = self->ctx.isB1FullLoadFlag_ && (self->ctx.curML0Idx_ == self->ctx.mIter_ - 1) &&
            (self->ctx.curNL0Idx_ == self->ctx.nIter_ - 1) &&
            (self->ctx.curDinIdx_ == self->ctx.curDinStartIdx_ + self->ctx.singleShapeDin_ - 1);
        if (self->ctx.curML0Idx_ + 1 == self->ctx.mIter_) {
            self->ctx.baseUseM_ = self->ctx.tailM_;
        } else if (self->ctx.curML0Idx_ == 0) {
            self->ctx.baseUseM_ = self->ctx.tiling_->baseM;
        }
        if (self->ctx.curNL0Idx_ + 1 == self->ctx.nIter_) {
            self->ctx.baseUseN_ = self->ctx.tailN_;
        } else if (self->ctx.curNL0Idx_ == 0) {
            self->ctx.baseUseN_ = self->ctx.tiling_->baseN;
        }
        if constexpr (std::is_same<typename Intf::DstT, float>::value) {
            // baseN may not be 16 aligned, but 8 aligned. At this time, L0B is still calculated according to the 512B aligned address offset
            self->ctx.baseUseAlignN_ = (self->ctx.baseUseN_ + AscendC::BLOCK_CUBE - 1) / AscendC::BLOCK_CUBE * AscendC::BLOCK_CUBE;
        }
        JudgeIterateSkip<Intf>(self);
        Compute<Intf>(self);
        return true;
    }
};

template <class Intf, bool sync>
struct IterateAll {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropInputFunc);
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
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropInputFunc);
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::DstT> &output,
                                       uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        LoadL0c2Gm<Intf>(self, output, enAtomic, enSequentialWrite);
    }
};

template <class Intf>
struct End {
    DECLARE_DEFAULT_OVERLOADING_FUN(Intf, ConvBackpropInputFunc);
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
        self->ctx.c1Ping_.FreeAllEvent();
        if (self->ctx.tiling_->cl0Pbuffer > 1) {
            self->ctx.c1Pong_.FreeAllEvent();
        }
        if (self->ctx.tiling_->hf32Flag) {
            AscendC::SetHF32Mode(false);
        }
    }
};

}  // namespace ConvBackpropInputFunc
#endif
