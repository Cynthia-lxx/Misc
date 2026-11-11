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
 * \file conv_common_func.h
 * \brief
 */

#ifndef API_CONV_COMMON_FUNC_H
#define API_CONV_COMMON_FUNC_H

#include "conv_forward_util.h"
#include "conv_forward_framework_util.h"

namespace ConvApiFunc {

CONV_DECLARE_REG_IMPL(SetInput);
CONV_DECLARE_REG_IMPL(SetWeight);
CONV_DECLARE_REG_IMPL(SetBias);
CONV_DECLARE_REG_IMPL(End);

using TypeFalse = struct {
    __uint128_t _[1024];
};

template <class Intf, uint32_t ImplType>
struct SetInput {
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::InputT> &input)
    {
        self->ctx.agm.SetGlobalBuffer(input.GetPhyAddr(0), input.GetSize());
    }
};

template <class Intf, uint32_t ImplType>
struct SetWeight {
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::WeightT> &weight)
    {
        self->ctx.bgm.SetGlobalBuffer(weight.GetPhyAddr(0), weight.GetSize());
    }
};

template <class Intf, uint32_t ImplType>
struct SetBias {
    static __aicore__ inline void call(Intf *self, const AscendC::GlobalTensor<typename Intf::BiasT> &bias)
    {
        self->ctx.biasgm.SetGlobalBuffer(bias.GetPhyAddr(0), bias.GetSize());
        self->ctx.enableBias = true;
    }
};

template <class Intf, uint32_t ImplType>
struct End {
    static __aicore__ inline void call(Intf *self)
    {
        if (self->ctx.freeAL1TensorFlag) {
            self->ctx.queueAL1.FreeTensor(self->ctx.al1);
            self->ctx.freeAL1TensorFlag = false;
        }
        if (self->ctx.freeBL1TensorFlag) {
            self->ctx.queueBL1.FreeTensor(self->ctx.bl1);
            self->ctx.freeBL1TensorFlag = false;
        }
        self->ctx.queueAL1.FreeAllEvent();
        self->ctx.queueBL1.FreeAllEvent();
        self->ctx.queueBiasL1.FreeAllEvent();
        if constexpr (Intf::formatType != ConvCommonApi::ConvFormat::NCDHW) {
            self->ctx.queueBiasBT.FreeAllEvent();
        }
        self->ctx.queueCL0.FreeAllEvent();
    }
};

}  // namespace ConvApiFunc
#endif
