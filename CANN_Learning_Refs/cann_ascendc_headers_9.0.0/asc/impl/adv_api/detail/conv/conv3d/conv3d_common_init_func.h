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
 * \file conv3d_common_init_func.h
 * \brief
 */
#ifndef API_CONV3D_COMMON_INIT_FUNC_H
#define API_CONV3D_COMMON_INIT_FUNC_H

#include "../common/conv_forward_framework_util.h"
#include "conv3d_common_sub_api.h"
#include "include/adv_api/conv/conv3d/conv3d_config.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "kernel_utils.h"

namespace Conv3dApiFunc {

template <class Intf>
__aicore__ inline void InitKDirectionBaseValue(
    Intf *self, uint64_t updateKAL1 = 0, uint64_t updateKBL1 = 0, uint64_t updateKL0 = 0)
{
    // K-direction variable calculation
    uint64_t currentKAL1 = updateKAL1 == 0 ? self->ctx.conv3dTiling->kAL1 : updateKAL1;
    uint64_t currentKBL1 = updateKBL1 == 0 ? self->ctx.conv3dTiling->kBL1 : updateKBL1;

    self->ctx.cin1 = ConvApi::CeilDIV(self->ctx.singleCoreCin, self->ctx.cin0);
    if constexpr (!Intf::bl1bypass && Intf::groupConvType) {
        self->ctx.singleCoreKL0 = updateKL0 == 0 ? self->ctx.conv3dTiling->kL0 : updateKL0;
    } else {
        self->ctx.singleCoreKL0 = self->ctx.conv3dTiling->kL0;
    }
    
    uint64_t alignCinKhKwKd =
        ConvApi::AlignB(self->ctx.singleCoreCin, ConvApi::GetInputkInOneC0Block<Intf>()) * self->ctx.kernelHxkernelWxkernelD;
    self->ctx.maxKAL1Iter = ConvApi::CeilDIV(alignCinKhKwKd, currentKAL1) - 1;
    self->ctx.ddr2l0LoopK = ConvApi::CeilDIV(alignCinKhKwKd, self->ctx.singleCoreKL0);
    self->ctx.maxKL0Iter = self->ctx.ddr2l0LoopK - 1;
    if constexpr (!Intf::bl1bypass) {
        self->ctx.maxKBL1Iter = ConvApi::CeilDIV(alignCinKhKwKd, currentKBL1) - 1;
        self->ctx.multiKBL1 = ConvApi::CeilDIV(currentKBL1, self->ctx.singleCoreKL0);
        self->ctx.kBL1fullload = (alignCinKhKwKd == currentKBL1);
        if (Intf::groupConvType) {
            self->ctx.groupKBL1Tail = alignCinKhKwKd % currentKBL1;
            self->ctx.groupKBL1Tail = self->ctx.groupKBL1Tail == 0 ? currentKBL1 : self->ctx.groupKBL1Tail;
        }
    } else {
        self->ctx.multiKBL1 = self->ctx.ddr2l0LoopK;
    }

    if constexpr (Intf::formatType == ConvCommonApi::ConvFormat::NCDHW) {
        self->ctx.kL0Tail = self->ctx.singleCoreCin % self->ctx.conv3dTiling->kL0;
    } else {
        self->ctx.kL0Tail = alignCinKhKwKd % self->ctx.singleCoreKL0;
    }
    
    self->ctx.kL0Tail = self->ctx.kL0Tail == 0 ? self->ctx.singleCoreKL0 : self->ctx.kL0Tail;
    self->ctx.multiKAL1 = ConvApi::CeilDIV(currentKAL1, self->ctx.singleCoreKL0);
    self->ctx.kAL1fullload = alignCinKhKwKd == currentKAL1;

    if (Intf::groupConvType) {
        self->ctx.groupKAL1Tail = alignCinKhKwKd % currentKAL1;
        self->ctx.groupKAL1Tail = self->ctx.groupKAL1Tail == 0 ? currentKAL1 : self->ctx.groupKAL1Tail;
        self->ctx.groupKAL1 = currentKAL1;
        self->ctx.groupKBL1 = currentKBL1;
    }
}

template <class Intf>
__aicore__ inline void InitMDirectionBaseValue(Intf *self)
{
    // M-direction variable calculation
    self->ctx.mAL1Tail = self->ctx.singleCoreM % self->ctx.conv3dTiling->mAL1;
    self->ctx.mAL1Tail = self->ctx.mAL1Tail == 0 ? self->ctx.conv3dTiling->mAL1 : self->ctx.mAL1Tail;
    self->ctx.ddr2l1LoopM = ConvApi::CeilDIV(self->ctx.singleCoreM, self->ctx.conv3dTiling->mAL1);
    self->ctx.maxMAL1Iter = self->ctx.ddr2l1LoopM - 1;
    self->ctx.mAL0Tail = self->ctx.mAL1Tail % self->ctx.conv3dTiling->mL0;
    self->ctx.mAL0Tail = self->ctx.mAL0Tail == 0 ? self->ctx.conv3dTiling->mL0 : self->ctx.mAL0Tail;
    self->ctx.l12l0LoopM = self->ctx.conv3dTiling->mAL1DivmL0;
    self->ctx.maxML0Iter = self->ctx.l12l0LoopM - 1;
}

template <class Intf>
__aicore__ inline void InitCoutDirectionBaseValue(Intf *self)
{
    // weight by pass
    if constexpr (Intf::bl1bypass) {
        self->ctx.nL0Tail = self->ctx.singleCoreCo % self->ctx.conv3dTiling->nL0;
        self->ctx.nL0Tail = self->ctx.nL0Tail == 0 ? self->ctx.conv3dTiling->nL0 : self->ctx.nL0Tail;
        self->ctx.maxNBL1Iter = 0;
        self->ctx.ddr2l1LoopN = 1;
        self->ctx.l12l0LoopN = ConvApi::CeilDIV(self->ctx.singleCoreCo, self->ctx.conv3dTiling->nL0);
        self->ctx.maxNL0Iter = self->ctx.l12l0LoopN - 1;
        return;
    }
    // N-direction variable calculation
    self->ctx.maxNBL1Iter = ConvApi::CeilDIV(self->ctx.singleCoreCo, self->ctx.conv3dTiling->nBL1) - 1;
    self->ctx.nBL1Tail = self->ctx.singleCoreCo % self->ctx.conv3dTiling->nBL1;
    self->ctx.nBL1Tail = self->ctx.nBL1Tail == 0 ? self->ctx.conv3dTiling->nBL1 : self->ctx.nBL1Tail;
    self->ctx.nBL1TailAlign = ConvApi::AlignB(self->ctx.nBL1Tail, ConvApi::BLOCK_L0_N);
    self->ctx.nL0Tail = self->ctx.nBL1Tail % self->ctx.conv3dTiling->nL0;
    self->ctx.nL0Tail = self->ctx.nL0Tail == 0 ? self->ctx.conv3dTiling->nL0 : self->ctx.nL0Tail;
    self->ctx.ddr2l1LoopN = self->ctx.maxNBL1Iter + 1;
    self->ctx.l12l0LoopN = self->ctx.conv3dTiling->nBL1DivnL0;
    self->ctx.maxNL0Iter = self->ctx.l12l0LoopN - 1;
}

template <class Intf>
__aicore__ inline void InitDoutDirectionBaseValue(Intf *self)
{
    // Dout-direction variable calculation
    self->ctx.ddr2l1LoopD = self->ctx.singleCoreDo;
}

template <class Intf>
__aicore__ inline void InitGroupOptDirectionValue(Intf *self)
{
    // GroupOpt-direction variable calculation
    self->ctx.maxGroupOptIter = self->ctx.singleCoreGroupOpt;
}

template <class Intf, uint32_t ImplType>
struct Init {
    static __aicore__ inline bool call(Intf *self, const void *__restrict tiling)
    {
        if constexpr (Intf::outputOrder) {
            self->ctx.pipe = GetTPipePtr();
            InitParams(self, tiling);
            if constexpr (Intf::formatType == ConvCommonApi::ConvFormat::NCDHW) {
                InitBufferPointWise(self);
            } else {
                InitBuffer(self);
            }
            InitKDirectionBaseValue(self);
            InitMDirectionBaseValue(self);
            InitCoutDirectionBaseValue(self);
            InitDoutDirectionBaseValue(self);
            InitGroupOptDirectionValue(self);
            InitHf32Mode(self);
            // preload case flag set: pbBL1, pbAL1 bit
            uint16_t preloadAL1Flag = (self->ctx.conv3dTiling->pBufferFlag & 0x18) >> 3;
            uint8_t L0ASet2dFlag = self->ctx.padHead != 0 || self->ctx.padTail != 0 ||
                    self->ctx.padUp >= self->ctx.kernelH || self->ctx.padDown >= self->ctx.kernelH;
            bool kAL1TailCase = self->ctx.conv3dTiling->kAL1Tail != self->ctx.conv3dTiling->kAL1;
            self->ctx.preloadAL1DbFlag =
                preloadAL1Flag == 1 && !self->ctx.kAL1fullload && !L0ASet2dFlag && !kAL1TailCase;
            self->ctx.preloadABL1DbFlag = preloadAL1Flag == 3 && !self->ctx.kAL1fullload &&
                                        !self->ctx.kBL1fullload && !L0ASet2dFlag && !kAL1TailCase;
            self->ctx.preloadAL1DbFlag = false;
            self->ctx.preloadABL1DbFlag = false;
        } else {
            KERNEL_LOG(KERNEL_DEBUG, "[Init] outputOrder false!\n");
        }
        return false;
    }

    static __aicore__ inline void InitParams(Intf *self, const void *__restrict tiling)
    {
        self->ctx.conv3dTiling = (TConv3DApiTiling *)tiling;
        self->ctx.kernelH = self->ctx.conv3dTiling->kernelH;
        self->ctx.kernelW = self->ctx.conv3dTiling->kernelW;
        self->ctx.kernelD = self->ctx.conv3dTiling->kernelD;
        self->ctx.dilatedKernelH = 1 + (self->ctx.kernelH - 1) * self->ctx.conv3dTiling->dilationH;
        self->ctx.dilatedKernelW = 1 + (self->ctx.kernelW - 1) * self->ctx.conv3dTiling->dilationW;
        self->ctx.dilatedKernelD = 1 + (self->ctx.kernelD - 1) * self->ctx.conv3dTiling->dilationD;
        self->ctx.kernelHxkernelW = self->ctx.kernelH * self->ctx.kernelW;
        self->ctx.kernelHxkernelWxkernelD = self->ctx.kernelHxkernelW * self->ctx.kernelD;
        self->ctx.orgHi = self->ctx.conv3dTiling->orgHi;
        self->ctx.orgWi = self->ctx.conv3dTiling->orgWi;
        self->ctx.orgDi = self->ctx.conv3dTiling->orgDi;
        self->ctx.orgHo = self->ctx.conv3dTiling->orgHo;
        self->ctx.orgWo = self->ctx.conv3dTiling->orgWo;
        self->ctx.orgCo = self->ctx.conv3dTiling->coutOpt;
        self->ctx.orgCi = self->ctx.conv3dTiling->cinOpt;
        self->ctx.orgDo = self->ctx.conv3dTiling->orgDo;
        self->ctx.singleCoreCo = self->ctx.conv3dTiling->singleCoreCo;
        self->ctx.singleCoreDo = self->ctx.conv3dTiling->singleCoreDo;
        self->ctx.singleCoreM = self->ctx.conv3dTiling->singleCoreM;
        self->ctx.singleCoreCin = self->ctx.conv3dTiling->cinOpt;
        self->ctx.singleCoreGroupOpt = self->ctx.conv3dTiling->singleCoreGroupOpt;
        self->ctx.strideH = self->ctx.conv3dTiling->strideH;
        self->ctx.strideW = self->ctx.conv3dTiling->strideW;
        self->ctx.strideD = self->ctx.conv3dTiling->strideD;
        self->ctx.dilationH = self->ctx.conv3dTiling->dilationH;
        self->ctx.dilationW = self->ctx.conv3dTiling->dilationW;
        self->ctx.dilationD = self->ctx.conv3dTiling->dilationD;
        self->ctx.padHead = self->ctx.conv3dTiling->padHead;
        self->ctx.padTail = self->ctx.conv3dTiling->padTail;
        self->ctx.padUp = self->ctx.conv3dTiling->padUp;
        self->ctx.padDown = self->ctx.conv3dTiling->padDown;
        self->ctx.padLeft = self->ctx.conv3dTiling->padLeft;
        self->ctx.padRight = self->ctx.conv3dTiling->padRight;
        self->ctx.biasFullLoadFlag = self->ctx.conv3dTiling->biasFullLoadFlag;
        self->ctx.sizeOfInput = sizeof(typename Intf::InputT);
        self->ctx.sizeOfWeight = sizeof(typename Intf::WeightT);
        self->ctx.sizeOfBias = sizeof(typename Intf::BiasT);
        self->ctx.sizeOfL0c = sizeof(typename Intf::L0cT);
        self->ctx.cin0 = ConvApi::C0_SIZE / self->ctx.sizeOfInput;
        self->ctx.orgCoAlignK0 = ConvApi::AlignB(self->ctx.orgCo, self->ctx.cin0);
        self->ctx.orgCoAlignN0 = ConvApi::AlignB(self->ctx.orgCo, ConvApi::BLOCK_L0_N);
    }

    static __aicore__ inline void InitBuffer(Intf *self)
    {
        uint64_t cl0Spacesize = self->ctx.conv3dTiling->mL0 * self->ctx.conv3dTiling->nL0;
        uint64_t bl1Spacesize = self->ctx.conv3dTiling->nBL1 * self->ctx.conv3dTiling->kBL1;
        uint64_t biasl1Spacesize =
            self->ctx.conv3dTiling->nL0 * sizeof(typename Intf::BiasT);  // Bias accompanied by L0C splitting when not fully loaded
        if (self->ctx.biasFullLoadFlag) {
            biasl1Spacesize = ConvApi::AlignB(
                self->ctx.singleCoreCo * sizeof(typename Intf::BiasT), ConvApi::BLOCK_L0_N * sizeof(typename Intf::BiasT));
        }
        uint64_t biasBTSpacesize = self->ctx.conv3dTiling->nL0;

        uint64_t hoAL1Max = ((self->ctx.conv3dTiling->mAL1 < self->ctx.singleCoreM ? self->ctx.conv3dTiling->mAL1
                                                                                   : self->ctx.singleCoreM) /
                                self->ctx.orgWo) +
                            2;
        uint64_t hiAL1Max = (hoAL1Max - 1) * self->ctx.strideH + self->ctx.dilatedKernelH;
        hiAL1Max = hiAL1Max > self->ctx.orgHi ? self->ctx.orgHi : hiAL1Max;
        uint64_t al1Spacesize = self->ctx.conv3dTiling->cin1InAL1 * self->ctx.cin0 * hiAL1Max * self->ctx.orgWi;
        InitBufferWithDoubleBuf(self, cl0Spacesize, al1Spacesize, bl1Spacesize);
        self->ctx.pipe->InitBuffer(self->ctx.queueBiasL1, 1, ConvApi::AlignB(biasl1Spacesize, ConvApi::C0_SIZE));
        self->ctx.pipe->InitBuffer(self->ctx.queueBiasBT, 1, ConvApi::AlignB(biasBTSpacesize * self->ctx.sizeOfL0c, ConvApi::BT_SIZE));
    }

    static __aicore__ inline void InitBufferPointWise(Intf *self)
    {
        uint64_t cl0Spacesize = self->ctx.conv3dTiling->mL0 * self->ctx.conv3dTiling->nL0;
        uint64_t bl1Spacesize = self->ctx.conv3dTiling->nBL1 * self->ctx.conv3dTiling->kBL1;
        uint64_t biasl1Spacesize =
            self->ctx.conv3dTiling->nL0 * sizeof(typename Intf::BiasT);  // Bias accompanied by L0C splitting when not fully loaded
        if (self->ctx.biasFullLoadFlag) {
            biasl1Spacesize = ConvApi::AlignB(
                self->ctx.singleCoreCo * sizeof(typename Intf::BiasT), ConvApi::BLOCK_L0_N * sizeof(typename Intf::BiasT));
        }
        biasl1Spacesize = biasl1Spacesize * ConvApi::K0_BIAS;
        uint64_t al1Spacesize = (self->ctx.conv3dTiling->mAL1 < self->ctx.singleCoreM ? self->ctx.conv3dTiling->mAL1
                                    : ConvApi::AlignB(self->ctx.singleCoreM, ConvApi::DATA_COPY_OP_LEN))
                                    * ConvApi::AlignB(self->ctx.conv3dTiling->kAL1, ConvApi::DATA_COPY_OP_LEN);
        InitBufferWithDoubleBuf(self, cl0Spacesize, al1Spacesize, bl1Spacesize);
        self->ctx.pipe->InitBuffer(self->ctx.queueBiasL1, 1, ConvApi::AlignB(biasl1Spacesize, ConvApi::C0_SIZE));
    }

    static __aicore__ inline void InitHf32Mode(Intf *self) 
    {
        AscendC::SetHF32Mode(self->ctx.conv3dTiling->hf32Enable);
        AscendC::SetHF32TransMode(false);
        KERNEL_LOG(KERNEL_DEBUG, "[InitHf32Mode] hf32Mode: %d, hf32TransMode: %d \n",
            self->ctx.conv3dTiling->hf32Enable, self->ctx.conv3dTiling->hf32TransMode);
    }

    static __aicore__ inline void InitBufferWithDoubleBuf(Intf *self, uint64_t cl0Spacesize, uint64_t al1Spacesize,
                                                          uint64_t bl1Spacesize)
    {
        int8_t cl0db = (self->ctx.conv3dTiling->pBufferFlag & 0x04) >> 2;
        int8_t al1db = (self->ctx.conv3dTiling->pBufferFlag & 0x08) >> 3;
        int8_t bl1db = (self->ctx.conv3dTiling->pBufferFlag & 0x10) >> 4;

        self->ctx.pipe->InitBuffer(self->ctx.l0aBuf, Conv3dApi::L0A_SIZE);
        self->ctx.pipe->InitBuffer(self->ctx.l0bBuf, Conv3dApi::L0B_SIZE);

        self->ctx.al0Ping = self->ctx.l0aBuf.template Get<typename Intf::InputT>();
        self->ctx.al0Pong = self->ctx.al0Ping[(Conv3dApi::L0A_SIZE / 2) / sizeof(typename Intf::InputT)];
        self->ctx.bl0Ping = self->ctx.l0bBuf.template Get<typename Intf::WeightT>();
        self->ctx.bl0Pong = self->ctx.bl0Ping[(Conv3dApi::L0B_SIZE / 2) / sizeof(typename Intf::WeightT)];

        if constexpr (Intf::formatType == ConvCommonApi::ConvFormat::NCDHW) {
            // l0bBuf actual point to l0a memory, al0BiasB need to point to l0a, so al0BiasB -> l0bBuf;
            // as well as bl0BiasB.
            self->ctx.al0BiasB = self->ctx.l0bBuf.template Get<typename Intf::BiasT>();
            self->ctx.bl0BiasB = self->ctx.l0aBuf.template Get<typename Intf::BiasT>();
        }

        if (!cl0db) {
            self->ctx.pipe->InitBuffer(self->ctx.queueCL0, 1, cl0Spacesize * self->ctx.sizeOfL0c);
        } else {
            self->ctx.pipe->InitBuffer(self->ctx.queueCL0, 2, cl0Spacesize * self->ctx.sizeOfL0c);
        }
        if (!al1db) {
            self->ctx.pipe->InitBuffer(self->ctx.queueAL1, 1, ConvApi::AlignB(al1Spacesize * self->ctx.sizeOfInput, ConvApi::C0_SIZE));
        } else {
            self->ctx.pipe->InitBuffer(self->ctx.queueAL1, 2, ConvApi::AlignB(al1Spacesize * self->ctx.sizeOfInput, ConvApi::C0_SIZE));
        }
        if constexpr (!Intf::bl1bypass) {
            if (!bl1db) {
                self->ctx.pipe->InitBuffer(
                    self->ctx.queueBL1, 1, ConvApi::AlignB(bl1Spacesize * self->ctx.sizeOfWeight, ConvApi::C0_SIZE));
            } else {
                self->ctx.pipe->InitBuffer(
                    self->ctx.queueBL1, 2, ConvApi::AlignB(bl1Spacesize * self->ctx.sizeOfWeight, ConvApi::C0_SIZE));
            }
        }
    }
};

}  // namespace Conv3dApiFunc

#endif