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
 * \file conv3d_iterate_impl.h
 * \brief
 */
#ifndef API_CONV3D_ITERATE_IMPL_H
#define API_CONV3D_ITERATE_IMPL_H

#include "conv3d_iterate_base_impl.h"

namespace Conv3dApiFunc {

template <class Intf, uint32_t ImplType>
struct Iterate {
    template <bool sync = true>
    static __aicore__ inline bool call(Intf *self, bool enPartialSum = false)
    {
        return IterateImpl(self, enPartialSum);
    }

    // The first iteration in the K direction (iter==0) loads L0A and L0B
    template <bool isLast = false>
    static __aicore__ void inline ReduceKFirstIterLoadL0(Intf *self)
    {
        if constexpr(Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::ALL_CLOSE)) {
            // for L0PingPong::ALL_CLOSE, BL1ByPass is always ON
            self->ctx.al0 = self->ctx.al0Ping;
            self->ctx.bl0 = self->ctx.bl0Ping;
            if constexpr (Intf::bl1bypass) {
                AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID0);
                ReduceKNoPingPongBL1ByPass<Intf, true, isLast>(self);
            } else {
                ReduceKNoPingPongBL1NoByPass<Intf, true, isLast>(self);
            }
        } else if constexpr(Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::L0A_OPEN)) {
            // for L0PingPong::L0A_OPEN, BL1ByPass is always ON
            self->ctx.bl0 = self->ctx.bl0Ping;
            if constexpr (Intf::bl1bypass) {
                AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID2);
                ReduceKL0APingPongBL1ByPass<Intf, true, isLast>(self, event_t::EVENT_ID0);
            } else {
                ReduceKL0APingPongBL1NoByPass<Intf, true, isLast>(self, event_t::EVENT_ID0);
            }
        } else if constexpr(Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::L0B_OPEN)) {
            self->ctx.al0 = self->ctx.al0Ping;
            if constexpr (Intf::bl1bypass) {
                AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID0);
                if (self->ctx.ddr2l1LoopD > 1) {
                    AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID1);
                }
                ReduceKL0BPingPongBL1ByPass<Intf, true, isLast>(self, event_t::EVENT_ID0);
            } else {
                ReduceKL0BPingPongBL1NoByPass<Intf, true, isLast>(self, event_t::EVENT_ID0);
            }
        } else if constexpr(Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::ALL_OPEN)) {
            if constexpr (Intf::bl1bypass) {
                AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID0);
                AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID1);
                if (self->ctx.ddr2l1LoopD > 1) {
                    AscendC::SetFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID4);
                }
            }
            ReduceKL0AL0BPingPong<Intf, true, isLast>(self, event_t::EVENT_ID0);
        }
    }

    // K-direction iteration (iter>0) loading L0A, L0B
    template <bool isLast = false>
    static __aicore__ void inline ReduceKIterLoadL0(Intf *self, const uint16_t& isOdd)
    {
        if constexpr (Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::ALL_CLOSE)) {
            if constexpr (Intf::bl1bypass) {
                ReduceKNoPingPongBL1ByPass<Intf, false, isLast>(self);
            } else {
                ReduceKNoPingPongBL1NoByPass<Intf, false, isLast>(self);
            }
        } else if constexpr(Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::L0A_OPEN)) {
            if constexpr (Intf::bl1bypass) {
                ReduceKL0APingPongBL1ByPass<Intf, false, isLast>(self, isOdd);
            } else {
                ReduceKL0APingPongBL1NoByPass<Intf, false, isLast>(self, isOdd);
            }
        } else if constexpr(Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::L0B_OPEN)) {
            if constexpr (Intf::bl1bypass) {
                ReduceKL0BPingPongBL1ByPass<Intf, false, isLast>(self, isOdd);
            } else {
                ReduceKL0BPingPongBL1NoByPass<Intf, false, isLast>(self, isOdd);
            }
        } else if constexpr(Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::ALL_OPEN)) {
            ReduceKL0AL0BPingPong<Intf, false, isLast>(self, isOdd);
        }
    }

    static __aicore__ void inline ReduceKIterLoadL1(Intf *self)
    {
        if (self->ctx.loadAL1Flag || (!self->ctx.kAL1fullload && self->ctx.kIter % self->ctx.multiKAL1 == 0)) {
            self->ctx.queueAL1.FreeTensor(self->ctx.al1);
            self->ctx.freeAL1TensorFlag = false;
            LoadAL1Process<Intf>(self, self->ctx.kIter / self->ctx.multiKAL1);
        }
        if constexpr (!Intf::bl1bypass) {
            if (self->ctx.loadBL1Flag || (!self->ctx.kBL1fullload && self->ctx.kIter % self->ctx.multiKBL1 == 0)) {
                self->ctx.queueBL1.FreeTensor(self->ctx.bl1);
                self->ctx.freeBL1TensorFlag = false;
                LoadBL1Process<Intf>(self, self->ctx.kIter / self->ctx.multiKBL1);
            }
        }
    }

    // Post processing for K-direction iteration, currently only for bl1 bypass, AscendC:: WaitFlag needs to be added
    static __aicore__ void inline ReduceKPostProcessLoadL0(Intf *self)
    {
        if constexpr((Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::ALL_CLOSE)) && Intf::bl1bypass) {
            AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID0);
        } else if constexpr((Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::L0A_OPEN)) && Intf::bl1bypass) {
            AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID2);
        } else if constexpr((Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::L0B_OPEN)) && Intf::bl1bypass) {
            AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID0);
            if (self->ctx.ddr2l1LoopD > 1) {
                AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID1);
            }
        } else if constexpr((Intf::l0pingpong == static_cast<int8_t>(Conv3dApi::ConvL0PingPong::ALL_OPEN)) && Intf::bl1bypass) {
            AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID1);
            if (self->ctx.ddr2l1LoopD > 1) {
                AscendC::WaitFlag<AscendC::HardEvent::M_MTE2>(event_t::EVENT_ID4);
            }
        }
    }

    static __aicore__ void inline ReduceK(Intf *self)
    {
        KERNEL_LOG(KERNEL_DEBUG, "no preload in ReduceK: loadAl1Flag: %d, kAL1fullload: %d, freeAL1TensorFlag: %d\n",
            self->ctx.loadAL1Flag, self->ctx.kAL1fullload, self->ctx.freeAL1TensorFlag);

        if (self->ctx.loadAL1Flag || !(self->ctx.kAL1fullload)) {
            if (self->ctx.freeAL1TensorFlag) {
                self->ctx.queueAL1.FreeTensor(self->ctx.al1);
                self->ctx.freeAL1TensorFlag = false;
            }
            LoadAL1Process<Intf>(self, 0);
        }
        if constexpr (!Intf::bl1bypass) {
            if (self->ctx.loadBL1Flag || !(self->ctx.kBL1fullload)) {
                if (self->ctx.freeBL1TensorFlag) {
                    self->ctx.queueBL1.FreeTensor(self->ctx.bl1);
                    self->ctx.freeBL1TensorFlag = false;
                }
                LoadBL1Process<Intf>(self, 0);
            }
        }
        if (self->ctx.ddr2l0LoopK == 1) {
            ReduceKFirstIterLoadL0<true>(self);
        } else {
            ReduceKFirstIterLoadL0(self);
        }

        self->ctx.kIter = 1;
        uint16_t isOdd = 1;
        while (self->ctx.kIter < self->ctx.ddr2l0LoopK - 1) {
            ReduceKIterLoadL1(self);
            ReduceKIterLoadL0(self, isOdd);
            self->ctx.kIter++;
            isOdd = self->ctx.kIter & 0x1;
        }

        if (self->ctx.kIter < self->ctx.ddr2l0LoopK) {
            ReduceKIterLoadL1(self);
            ReduceKIterLoadL0<true>(self, isOdd);
        }

        ReduceKPostProcessLoadL0(self);
    }

    static __aicore__ void inline ReduceKPreloadDbAllLoadL1(Intf *self, const uint64_t& maxKAL1PreloadIter,
                                                            const uint64_t& maxKBL1PreloadIter)
    {
        if (self->ctx.kIter == maxKAL1PreloadIter) {
            self->ctx.queueAL1.FreeTensor(self->ctx.al1);
            self->ctx.al1 = self->ctx.queueAL1.template DeQue<typename Intf::InputT>();
        } else if (self->ctx.kIter < maxKAL1PreloadIter &&
            (self->ctx.loadAL1Flag || (!self->ctx.kAL1fullload && self->ctx.kIter % self->ctx.multiKAL1 == 0))) {
            self->ctx.queueAL1.FreeTensor(self->ctx.al1);
            LoadAL1Process<Intf>(self, (self->ctx.kIter / self->ctx.multiKAL1) + 1);
        }

        if (self->ctx.kIter == maxKBL1PreloadIter) {
            self->ctx.queueBL1.FreeTensor(self->ctx.bl1);
            self->ctx.bl1 = self->ctx.queueBL1.template DeQue<typename Intf::WeightT>();
        } else if (self->ctx.kIter < maxKBL1PreloadIter &&
            (self->ctx.loadBL1Flag || (!self->ctx.kBL1fullload && self->ctx.kIter % self->ctx.multiKBL1 == 0))) {
            self->ctx.queueBL1.FreeTensor(self->ctx.bl1);
            LoadBL1Process<Intf>(self, (self->ctx.kIter / self->ctx.multiKBL1) + 1);
        }
    }

    static __aicore__ void inline ReduceKPreloadDbAll(Intf *self)
    {
        KERNEL_LOG(KERNEL_DEBUG, "AL1 and BL1 db case, preload reduce k\n");

        if (self->ctx.loadAL1Flag || !(self->ctx.kAL1fullload)) {
            if (self->ctx.freeAL1TensorFlag) {
                self->ctx.queueAL1.FreeTensor(self->ctx.al1);
            }
            LoadAL1PreloadProcess<Intf>(self, 0);
        }

        if (self->ctx.loadBL1Flag || !(self->ctx.kBL1fullload)) {
            if (self->ctx.freeBL1TensorFlag) {
                self->ctx.queueBL1.FreeTensor(self->ctx.bl1);
            }
            LoadBL1PreloadProcess<Intf>(self, 0);
        }

        LoadAL1Process<Intf>(self, 1);
        LoadBL1Process<Intf>(self, 1);

        if (self->ctx.ddr2l0LoopK == 1) {
            ReduceKFirstIterLoadL0<true>(self);
        } else {
            ReduceKFirstIterLoadL0(self);
        }

        self->ctx.kIter = 1;
        uint16_t isOdd = 1;
        uint64_t maxKAL1PreloadIter = self->ctx.ddr2l0LoopK - self->ctx.multiKAL1;
        uint64_t maxKBL1PreloadIter = self->ctx.ddr2l0LoopK - self->ctx.multiKBL1;
        while (self->ctx.kIter < self->ctx.ddr2l0LoopK - 1) {
            ReduceKPreloadDbAllLoadL1(self, maxKAL1PreloadIter, maxKBL1PreloadIter);
            ReduceKIterLoadL0(self, isOdd);
            self->ctx.kIter++;
            isOdd = self->ctx.kIter & 0x1;
        }

        if (self->ctx.kIter < self->ctx.ddr2l0LoopK) {
            ReduceKPreloadDbAllLoadL1(self, maxKAL1PreloadIter, maxKBL1PreloadIter);
            ReduceKIterLoadL0<true>(self, isOdd);
        }
    }

    static __aicore__ void inline ReduceKPreloadDbInputLoadL1(Intf *self,  const uint64_t& maxKAL1PreloadIter)
    {
        if (self->ctx.kIter == maxKAL1PreloadIter) {
            self->ctx.queueAL1.FreeTensor(self->ctx.al1);
            self->ctx.al1 = self->ctx.queueAL1.template DeQue<typename Intf::InputT>();
        } else if (self->ctx.kIter < maxKAL1PreloadIter && self->ctx.kIter % self->ctx.multiKAL1 == 0) {
            self->ctx.queueAL1.FreeTensor(self->ctx.al1);
            LoadAL1Process<Intf>(self, (self->ctx.kIter / self->ctx.multiKAL1) + 1);
        }

        if constexpr (!Intf::bl1bypass) {
            if (self->ctx.loadBL1Flag || (!self->ctx.kBL1fullload && self->ctx.kIter % self->ctx.multiKBL1 == 0)) {
                self->ctx.queueBL1.FreeTensor(self->ctx.bl1);
                LoadBL1Process<Intf>(self, self->ctx.kIter / self->ctx.multiKBL1);
            }
        }
    }

    static __aicore__ void inline ReduceKPreloadDbInput(Intf *self)
    {
        KERNEL_LOG(KERNEL_DEBUG, "AL1 db case, preload reduce k\n");

        if (self->ctx.freeAL1TensorFlag) {
            self->ctx.queueAL1.FreeTensor(self->ctx.al1);
        }
        LoadAL1PreloadProcess<Intf>(self, 0);
        LoadAL1Process<Intf>(self, 1);

        if constexpr (!Intf::bl1bypass) {
            if (self->ctx.loadBL1Flag || !(self->ctx.kBL1fullload)) {
                if (self->ctx.freeBL1TensorFlag) {
                    self->ctx.queueBL1.FreeTensor(self->ctx.bl1);
                }
                LoadBL1Process<Intf>(self, 0);
            }
        }

        if (self->ctx.ddr2l0LoopK == 1) {
            ReduceKFirstIterLoadL0<true>(self);
        } else {
            ReduceKFirstIterLoadL0(self);
        }

        self->ctx.kIter = 1;
        uint16_t isOdd = 1;
        uint64_t maxKAL1PreloadIter = self->ctx.ddr2l0LoopK - self->ctx.multiKAL1;
        while (self->ctx.kIter < self->ctx.ddr2l0LoopK - 1) {
            ReduceKPreloadDbInputLoadL1(self, maxKAL1PreloadIter);
            ReduceKIterLoadL0(self, isOdd);
            self->ctx.kIter++;
            isOdd = self->ctx.kIter & 0x1;
        }

        if (self->ctx.kIter < self->ctx.ddr2l0LoopK) {
            ReduceKPreloadDbInputLoadL1(self, maxKAL1PreloadIter);
            ReduceKIterLoadL0<true>(self, isOdd);
        }

        ReduceKPostProcessLoadL0(self);
    }

    static __aicore__ void inline IterateK(Intf *self)
    {
        // in each iterate k, cal current m,n value
        uint64_t n = CalcL0CurrentN<Intf>(self);
        uint64_t m = CalcL0CurrentM<Intf>(self);
        self->ctx.cl0 = self->ctx.queueCL0.template AllocTensor<typename Intf::L0cT>();

        self->ctx.loadAL0Ins.SetM(ConvApi::AlignB(m, ConvApi::BLOCK_L0_N));
        self->ctx.loadBL0Ins.SetN(ConvApi::AlignB(n, ConvApi::BLOCK_L0_M));
        if constexpr (Intf::formatType == ConvCommonApi::ConvFormat::NCDHW) {
            self->ctx.madIns.SetMN(ConvApi::AlignB(n, ConvApi::BLOCK_L0_M), ConvApi::AlignB(m, ConvApi::BLOCK_L0_N));
            self->ctx.copyOutIns.SetMN(n, m);
            InitBiasWithPointWise<Intf>(self, m, n);
        } else {
            self->ctx.madIns.SetMN(ConvApi::AlignB(m, ConvApi::BLOCK_L0_M), ConvApi::AlignB(n, ConvApi::BLOCK_L0_N));
            self->ctx.copyOutIns.SetMN(m, ConvApi::AlignB(n, self->ctx.cin0));
            InitBiasWithNormal<Intf>(self, m, n);
        }

        if (self->ctx.preloadABL1DbFlag) {
            ReduceKPreloadDbAll(self);
        } else if (self->ctx.preloadAL1DbFlag) {
            ReduceKPreloadDbInput(self);
        } else {
            ReduceK(self);
        }
        self->ctx.queueCL0.EnQue(self->ctx.cl0);
        self->ctx.cl0 = self->ctx.queueCL0.template DeQue<typename Intf::L0cT>();
        self->ctx.kIter = 0;
    }

    static __aicore__ bool inline IterateImpl(Intf *self, bool enPartialSum)
    {
        if (self->ctx.isFirstIterate) {
            FirstIterateImpl<Intf>(self);
        } else if (likely(self->ctx.conv3dTiling->iterateMNOrder == static_cast<int>(ConvApi::IterateOrder::ORDER_MTERFIRST))) {
            if (IterateMFirst<Intf>(self) == false) {
                return false;
            }
        } else if (likely(self->ctx.conv3dTiling->iterateMNOrder == static_cast<int>(ConvApi::IterateOrder::ORDER_NTERFIRST))) {
            if (IterateNFirst<Intf>(self) == false) {
                return false;
            }
        }
        IterateK(self);
        UpdateL1TailLoop<Intf>(self);
        return true;
    }
};

}  // namespace Conv3dApiFunc

#endif