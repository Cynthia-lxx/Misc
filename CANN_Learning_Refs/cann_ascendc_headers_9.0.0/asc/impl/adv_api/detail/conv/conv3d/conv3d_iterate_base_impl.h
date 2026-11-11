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
 * \file conv3d_iterate_base_impl.h
 * \brief
 */
#ifndef API_CONV3D_ITERATE_BASE_IMPL_H
#define API_CONV3D_ITERATE_BASE_IMPL_H

#include "conv3d_common_init_func.h"
#include "conv3d_common_set_func.h"

namespace Conv3dApiFunc {

template <class Intf>
__aicore__ inline uint64_t CalcL0CurrentN(Intf *self)
{
    uint64_t n = (self->ctx.nBL1Iter == self->ctx.maxNBL1Iter && self->ctx.nBL0Iter == self->ctx.maxNL0Iter)
                        ? self->ctx.nL0Tail
                        : self->ctx.conv3dTiling->nL0;
    return n;
}

template <class Intf>
__aicore__ inline uint64_t CalcL0CurrentM(Intf *self)
{
    uint64_t m = (self->ctx.mAL1Iter == self->ctx.maxMAL1Iter && self->ctx.mAL0Iter == self->ctx.maxML0Iter)
                        ? self->ctx.mAL0Tail
                        : self->ctx.conv3dTiling->mL0;
    return m;
}

template <class Intf>
__aicore__ void inline FirstIterateImpl(Intf *self)
{
    self->ctx.nBL0Iter = 0;
    self->ctx.mAL0Iter = 0;
    self->ctx.mAL1Iter = 0;
    self->ctx.nBL1Iter = 0;
    self->ctx.dOutIter = 0;
    self->ctx.loadAL1Flag = true;
    self->ctx.loadBL1Flag = !Intf::bl1bypass;
    self->ctx.loadAL0Flag = true;
    self->ctx.loadBL0Flag = true;
    if constexpr (Intf::formatType != ConvCommonApi::ConvFormat::NCDHW) {
        self->ctx.loadAl1Ins.SetLoadData3DParams();
        if (self->ctx.conv3dTiling->mL0 % self->ctx.orgWo == 0) {
            self->ctx.mL0IsDivisibleByWo = true;
        }
    }
    self->ctx.isFirstIterate = false;
}

template <class Intf>
__aicore__ bool inline IterateMFirst(Intf *self)
{
    // ReorderN: First offset towards the M-axis direction and then offset towards the N-axis direction. Input Reuse Weight.
    //    M
    //    |
    //    |
    //    |----------N-------->
    // ==================L0========================
    self->ctx.mAL0Iter++;
    if (self->ctx.mAL0Iter == self->ctx.l12l0LoopM) {
        self->ctx.mAL0Iter = 0;
        self->ctx.nBL0Iter++;
    }
    if (self->ctx.nBL0Iter == self->ctx.l12l0LoopN) {
        self->ctx.mAL0Iter = 0;
        self->ctx.nBL0Iter = 0;
        self->ctx.mAL1Iter++;
        self->ctx.loadAL1Flag = true;
    }
    if (self->ctx.mAL1Iter == self->ctx.ddr2l1LoopM) {
        self->ctx.mAL0Iter = 0;
        self->ctx.nBL0Iter = 0;
        self->ctx.mAL1Iter = 0;
        self->ctx.dOutIter++;
        self->ctx.loadAL1Flag = true;
    }
    if (self->ctx.dOutIter == self->ctx.ddr2l1LoopD) {
        self->ctx.mAL0Iter = 0;
        self->ctx.nBL0Iter = 0;
        self->ctx.mAL1Iter = 0;
        self->ctx.dOutIter = 0;
        self->ctx.nBL1Iter++;
        self->ctx.loadAL1Flag = true;
        self->ctx.loadBL1Flag = true;
    }
    if (self->ctx.nBL1Iter == self->ctx.ddr2l1LoopN) {
        return false;
    }
    return true;
}

template <class Intf>
__aicore__ bool inline IterateNFirst(Intf *self)
{
    // ReorderM: Shift towards the N axis first and then towards the M axis. Weight reuse Input.
    //    ----------N-------->
    //                       |
    //                       |
    //                       M
    //                       |
    //                       |
    // ==================L0========================
    self->ctx.nBL0Iter++;
    if (self->ctx.nBL0Iter == self->ctx.l12l0LoopN) {
        self->ctx.nBL0Iter = 0;
        self->ctx.mAL0Iter++;
    }
    if (self->ctx.mAL0Iter == self->ctx.l12l0LoopM) {
        self->ctx.mAL0Iter = 0;
        self->ctx.nBL0Iter = 0;
        self->ctx.nBL1Iter++;
        self->ctx.loadBL1Flag = true;
    }
    if (self->ctx.nBL1Iter == self->ctx.ddr2l1LoopN) {
        self->ctx.mAL0Iter = 0;
        self->ctx.nBL0Iter = 0;
        self->ctx.nBL1Iter = 0;
        self->ctx.mAL1Iter++;
        self->ctx.loadAL1Flag = true;
        self->ctx.loadBL1Flag = true;
    }
    if (self->ctx.mAL1Iter == self->ctx.ddr2l1LoopM) {
        self->ctx.mAL0Iter = 0;
        self->ctx.nBL0Iter = 0;
        self->ctx.mAL1Iter = 0;
        self->ctx.dOutIter++;
        self->ctx.loadAL1Flag = true;
    }
    if (self->ctx.dOutIter == self->ctx.ddr2l1LoopD) {
        return false;
    }
    return true;
}

template <class Intf, bool isFirst = false, bool isLast = false>
__aicore__ void inline ReduceKNoPingPongBL1ByPass(Intf *self) {
    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID0);
    self->ctx.kAL0Iter = self->ctx.kIter % self->ctx.multiKAL1;
    self->ctx.loadAL0Ins.LoadAL0();
    AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID0);

    AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID0);
    self->ctx.kBL0Iter = self->ctx.kIter % self->ctx.multiKBL1;
    self->ctx.loadBL0Ins.LoadBL0();
    AscendC::SetFlag<AscendC::HardEvent::MTE2_M>(event_t::EVENT_ID1);

    AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_M>(event_t::EVENT_ID1);

    self->ctx.madIns.template Mad<isFirst, isLast>();
    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID0);
    AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID0);
}

template <class Intf, bool isFirst = false, bool isLast = false>
__aicore__ void inline ReduceKNoPingPongBL1NoByPass(Intf *self) {
    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID0);
    self->ctx.kAL0Iter = self->ctx.kIter % self->ctx.multiKAL1;
    self->ctx.loadAL0Ins.LoadAL0();

    self->ctx.kBL0Iter = self->ctx.kIter % self->ctx.multiKBL1;
    self->ctx.loadBL0Ins.LoadBL0();
    AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID0);

    self->ctx.madIns.template Mad<isFirst, isLast>();
    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID0);
}

template <class Intf, bool isFirst = false, bool isLast = false>
__aicore__ void inline ReduceKL0APingPongBL1ByPass(Intf *self, const uint16_t& l0aFlag) {
    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(l0aFlag);
    self->ctx.al0 = l0aFlag ? self->ctx.al0Pong : self->ctx.al0Ping;
    self->ctx.kAL0Iter = self->ctx.kIter % self->ctx.multiKAL1;
    self->ctx.loadAL0Ins.LoadAL0();
    AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(l0aFlag);

    AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID2);
    if (self->ctx.loadBL0Flag) {
        self->ctx.kBL0Iter = self->ctx.kIter % self->ctx.multiKBL1;
        self->ctx.loadBL0Ins.LoadBL0();
        AscendC::SetFlag<AscendC::HardEvent::MTE2_M>(event_t::EVENT_ID2);
    }

    AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(l0aFlag);
    if (self->ctx.loadBL0Flag) {
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_M>(event_t::EVENT_ID2);
    }
    self->ctx.madIns.template Mad<isFirst, isLast>();
    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(l0aFlag);
    AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID2);
}

template <class Intf, bool isFirst = false, bool isLast = false>
__aicore__ void inline ReduceKL0APingPongBL1NoByPass(Intf *self, const uint16_t& l0aFlag) {
    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(l0aFlag);
    self->ctx.al0 = l0aFlag ? self->ctx.al0Pong : self->ctx.al0Ping;
    self->ctx.kAL0Iter = self->ctx.kIter % self->ctx.multiKAL1;
    self->ctx.loadAL0Ins.LoadAL0();

    if (self->ctx.loadBL0Flag) {
        self->ctx.kBL0Iter = self->ctx.kIter % self->ctx.multiKBL1;
        self->ctx.loadBL0Ins.LoadBL0();
    }
    AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(l0aFlag);
    AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(l0aFlag);
    self->ctx.madIns.template Mad<isFirst, isLast>();
    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(l0aFlag);
}

template <class Intf, bool isFirst = false, bool isLast = false>
__aicore__ void inline ReduceKL0BPingPongBL1ByPass(Intf *self, const uint16_t& l0bFlag) {
    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID2);
    if (self->ctx.loadAL0Flag) {
        self->ctx.kAL0Iter = self->ctx.kIter % self->ctx.multiKAL1;
        self->ctx.loadAL0Ins.LoadAL0();
        AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID2);
    }

    AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(l0bFlag);
    self->ctx.bl0 = l0bFlag ? self->ctx.bl0Pong : self->ctx.bl0Ping;
    self->ctx.kBL0Iter = self->ctx.kIter % self->ctx.multiKBL1;
    self->ctx.loadBL0Ins.LoadBL0();
    AscendC::SetFlag<AscendC::HardEvent::MTE2_M>(l0bFlag);

    if (self->ctx.loadAL0Flag) {
        AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID2);
    }
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_M>(l0bFlag);

    self->ctx.madIns.template Mad<isFirst, isLast>();
    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID2);
    AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(l0bFlag);
}

template <class Intf, bool isFirst = false, bool isLast = false>
__aicore__ void inline ReduceKL0BPingPongBL1NoByPass(Intf *self, const uint16_t& l0bFlag) {
    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID2);
    if (self->ctx.loadAL0Flag) {
        self->ctx.kAL0Iter = self->ctx.kIter % self->ctx.multiKAL1;

        self->ctx.loadAL0Ins.LoadAL0();
        AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID2);
    }

    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(l0bFlag);
    self->ctx.bl0 = l0bFlag ? self->ctx.bl0Pong : self->ctx.bl0Ping;
    self->ctx.kBL0Iter = self->ctx.kIter % self->ctx.multiKBL1;
    self->ctx.loadBL0Ins.LoadBL0();
    AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(l0bFlag);

    if (self->ctx.loadAL0Flag) {
        AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID2);
    }
    AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(l0bFlag);

    self->ctx.madIns.template Mad<isFirst, isLast>();
    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID2);
    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(l0bFlag);
}

template <class Intf, bool isFirst = false, bool isLast = false>
__aicore__ void inline ReduceKL0AL0BPingPong(Intf *self, const uint16_t& l0abFlag) {
    if (l0abFlag) {
        self->ctx.al0 = self->ctx.al0Pong;
        self->ctx.bl0 = self->ctx.bl0Pong;
    } else {
        self->ctx.al0 = self->ctx.al0Ping;
        self->ctx.bl0 = self->ctx.bl0Ping;
    }

    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(l0abFlag);

    self->ctx.kAL0Iter = self->ctx.kIter % self->ctx.multiKAL1;
    self->ctx.loadAL0Ins.LoadAL0();

    if constexpr (Intf::bl1bypass) {
        AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(l0abFlag);
    }
    self->ctx.kBL0Iter = self->ctx.kIter % self->ctx.multiKBL1;
    self->ctx.loadBL0Ins.LoadBL0();
    AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(l0abFlag);
    if constexpr (Intf::bl1bypass) {
        AscendC::SetFlag<AscendC::HardEvent::MTE2_M>(l0abFlag);
    }

    AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(l0abFlag);
    if constexpr (Intf::bl1bypass) {
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_M>(l0abFlag);
    }

    AscendC::PipeBarrier<PIPE_M>();
    self->ctx.madIns.template Mad<isFirst, isLast>();
    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(l0abFlag);
    if constexpr (Intf::bl1bypass) {
        AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(l0abFlag);
    }
}

template <class Intf>
__aicore__ void inline LoadAL1Process(Intf *self, uint64_t kAL1Iter)
{
    self->ctx.al1 = self->ctx.queueAL1.template AllocTensor<typename Intf::InputT>();
    self->ctx.kAL1Iter = kAL1Iter;
    self->ctx.loadAl1Ins.LoadAL1();
    self->ctx.queueAL1.EnQue(self->ctx.al1);
    self->ctx.al1 = self->ctx.queueAL1.template DeQue<typename Intf::InputT>();
    self->ctx.loadAL1Flag = false;  // Only the K direction in LoopK can be reloaded.
    self->ctx.freeAL1TensorFlag = true;
}

template <class Intf>
__aicore__ void inline LoadBL1Process(Intf *self, uint64_t kBL1Iter)
{
    self->ctx.bl1 = self->ctx.queueBL1.template AllocTensor<typename Intf::WeightT>();
    self->ctx.kBL1Iter = kBL1Iter;
    self->ctx.loadBL1Ins.LoadBL1();
    self->ctx.queueBL1.EnQue(self->ctx.bl1);
    self->ctx.bl1 = self->ctx.queueBL1.template DeQue<typename Intf::WeightT>();
    self->ctx.loadBL1Flag = false;  // Only the K direction in LoopK can be reloaded.
    self->ctx.freeBL1TensorFlag = true;
}

template <class Intf>
__aicore__ void inline LoadAL1PreloadProcess(Intf *self, uint64_t kAL1Iter)
{
    self->ctx.al1 = self->ctx.queueAL1.template AllocTensor<typename Intf::InputT>();
    self->ctx.kAL1Iter = kAL1Iter;
    self->ctx.loadAl1Ins.LoadAL1();
    self->ctx.queueAL1.EnQue(self->ctx.al1);
    self->ctx.loadAL1Flag = true;
    self->ctx.freeAL1TensorFlag = false;
}

template <class Intf>
__aicore__ void inline LoadBL1PreloadProcess(Intf *self, uint64_t kBL1Iter)
{
    self->ctx.bl1 = self->ctx.queueBL1.template AllocTensor<typename Intf::WeightT>();
    self->ctx.kBL1Iter = kBL1Iter;
    self->ctx.loadBL1Ins.LoadBL1();
    self->ctx.queueBL1.EnQue(self->ctx.bl1);
    self->ctx.loadBL1Flag = true;
    self->ctx.freeBL1TensorFlag = false;
}

template <class Intf>
__aicore__ void inline CalcBias(Intf *self)
{
    if constexpr(Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::L0B_OPEN)) {
        AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID2);
        self->ctx.loadBiasBTIns.LoadBiasL0WithBroadcast();
        AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID0);
        AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID2);
        AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID2);
        self->ctx.madIns.MadBias();
        AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID0);
        AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID2);
    } else {
        AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID0);
        self->ctx.loadBiasBTIns.LoadBiasL0WithBroadcast();
        AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(event_t::EVENT_ID0);
        self->ctx.madIns.MadBias();
        AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(event_t::EVENT_ID0);
    }
}

template <class Intf>
__aicore__ void inline InitBiasWithPointWise(Intf *self, uint64_t m, uint64_t n)
{
    if (self->ctx.enableBias) {
        self->ctx.loadBiasL1Ins.SetN(ConvApi::AlignB(n, ConvApi::BLOCK_L0_M));
        self->ctx.loadBiasBTIns.SetMN(ConvApi::AlignB(n, ConvApi::BLOCK_L0_M), ConvApi::AlignB(m, ConvApi::BLOCK_L0_N));
        if (!self->ctx.biasFullLoadFlag) {
            self->ctx.biasL1 = self->ctx.queueBiasL1.template AllocTensor<typename Intf::BiasT>();
            self->ctx.loadBiasL1Ins.LoadChannelWiseL1(self->ctx.biasL1, self->ctx.biasgm);
            self->ctx.queueBiasL1.EnQue(self->ctx.biasL1);
            self->ctx.biasL1 = self->ctx.queueBiasL1.template DeQue<typename Intf::BiasT>();
        }
        CalcBias<Intf>(self);
    }
}

template <class Intf>
__aicore__ void inline InitBiasWithNormal(Intf *self, uint64_t m, uint64_t n)
{
    if (self->ctx.enableBias) {
        self->ctx.loadBiasL1Ins.SetN(n);
        self->ctx.loadBiasBTIns.SetN(ConvApi::AlignB(n, ConvApi::BLOCK_L0_N));
        if (!self->ctx.biasFullLoadFlag) {
            self->ctx.biasL1 = self->ctx.queueBiasL1.template AllocTensor<typename Intf::BiasT>();
            self->ctx.loadBiasL1Ins.LoadChannelWiseL1(self->ctx.biasL1, self->ctx.biasgm);
            self->ctx.queueBiasL1.EnQue(self->ctx.biasL1);
            self->ctx.biasL1 = self->ctx.queueBiasL1.template DeQue<typename Intf::BiasT>();
        }
        self->ctx.biasBT = self->ctx.queueBiasBT.template AllocTensor<typename Intf::L0cT>();
        self->ctx.loadBiasBTIns.LoadBiasBt();
        self->ctx.queueBiasBT.EnQue(self->ctx.biasBT);
        self->ctx.biasBT = self->ctx.queueBiasBT.template DeQue<typename Intf::L0cT>();
    }
}

template <class Intf>
__aicore__ void inline UpdateL1TailLoop(Intf *self)
{
    self->ctx.l12l0LoopM = self->ctx.mAL1Iter == self->ctx.maxMAL1Iter
                                ? ConvApi::CeilDIV(self->ctx.mAL1Tail, self->ctx.conv3dTiling->mL0)
                                : self->ctx.conv3dTiling->mAL1DivmL0;
    self->ctx.maxML0Iter = self->ctx.l12l0LoopM - 1;

    if constexpr (Intf::bl1bypass) {
        return;
    }
    self->ctx.l12l0LoopN = self->ctx.nBL1Iter == self->ctx.maxNBL1Iter
                                ? ConvApi::CeilDIV(self->ctx.nBL1Tail, self->ctx.conv3dTiling->nL0)
                                : self->ctx.conv3dTiling->nBL1DivnL0;
    self->ctx.maxNL0Iter = self->ctx.l12l0LoopN - 1;
}

}  // namespace Conv3dApiFunc

#endif