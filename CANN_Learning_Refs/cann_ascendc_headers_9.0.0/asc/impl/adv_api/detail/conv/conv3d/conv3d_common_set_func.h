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
 * \file conv3d_common_set_func.h
 * \brief
 */

#ifndef API_CONV3D_COMMON_SET_FUNC_H
#define API_CONV3D_COMMON_SET_FUNC_H

#include "../common/conv_forward_framework_util.h"
#include "conv3d_common_sub_api.h"
#include "include/adv_api/conv/conv3d/conv3d_config.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "kernel_utils.h"

namespace Conv3dApiFunc {

template <class Intf, uint32_t ImplType>
struct SetOrgInputShape {
    static __aicore__ inline void call(Intf *self, uint64_t orgCi, uint64_t orgDi, uint64_t orgHi, uint64_t orgWi)
    {
        self->ctx.oriCi = orgCi;
        self->ctx.orgDi = orgDi;
        self->ctx.orgHi = orgHi;
        self->ctx.orgWi = orgWi;
    }
};

template <class Intf, uint32_t ImplType>
struct SetOrgWeightShape {
    static __aicore__ inline void call(
        Intf *self, uint64_t orgCo, uint64_t orgCi, uint64_t orgKd, uint64_t orgKh, uint64_t orgKw)
    {
        self->ctx.orgCo = orgCo;
        self->ctx.orgCi = orgCi;
        self->ctx.kernelD = orgKd;
        self->ctx.kernelH = orgKh;
        self->ctx.kernelW = orgKw;
    }
};

template <class Intf, uint32_t ImplType>
struct SetOrgOutputShape {
    static __aicore__ inline void call(Intf *self, uint64_t orgCo, uint64_t orgDo, uint64_t orgHo, uint64_t orgWo)
    {
        self->ctx.orgCo = orgCo;
        self->ctx.orgDo = orgDo;
        self->ctx.orgHo = orgHo;
        self->ctx.orgWo = orgWo;
    }
};

template <class Intf, uint32_t ImplType>
struct SetSingleInputShape {
    static __aicore__ inline void call(
        Intf *self, uint64_t singleCi, uint64_t singleDi, uint64_t singleHi, uint64_t singleWi)
    {
        self->ctx.singleCoreCin = singleCi;
        InitKDirectionBaseValue<Intf>(self);
    }
};

template <class Intf, uint32_t ImplType>
struct SetSingleOutputShape {
    static __aicore__ inline void call(
        Intf *self, uint64_t singleCo, uint64_t singleDo, uint64_t singleHo,
        uint64_t singleWo, uint64_t singleGroupOpt)
    {
        self->ctx.singleCoreCo = singleCo;
        self->ctx.singleCoreDo = singleDo;
        self->ctx.singleCoreHo = singleHo;
        self->ctx.singleCoreGroupOpt = singleGroupOpt;
        InitCoutDirectionBaseValue<Intf>(self);
        InitDoutDirectionBaseValue<Intf>(self);
        InitGroupOptDirectionValue<Intf>(self);
    }

    static __aicore__ inline void call(
        Intf *self, uint64_t singleCo, uint64_t singleDo, uint64_t singleCoreM, uint64_t singleGroupOpt)
    {
        self->ctx.singleCoreCo = singleCo;
        self->ctx.singleCoreDo = singleDo;
        self->ctx.singleCoreM = singleCoreM;
        self->ctx.singleCoreGroupOpt = singleGroupOpt;
        InitMDirectionBaseValue<Intf>(self);
        InitCoutDirectionBaseValue<Intf>(self);
        InitDoutDirectionBaseValue<Intf>(self);
        InitGroupOptDirectionValue<Intf>(self);
    }
};

template <class Intf, uint32_t ImplType>
struct SetInputStartPosition {
    static __aicore__ inline void call(
        Intf *self, int64_t diStartPos, int64_t hiStartPos, int64_t wiStartPos, int64_t ciStartPos)
    {
        self->ctx.diStartPos = diStartPos;
    }

    static __aicore__ inline void call(Intf *self, int64_t diStartPos, int64_t mStartPos, int64_t ciStartPos)
    {
        self->ctx.diStartPos = diStartPos;
        self->ctx.mStartPos = mStartPos;
    }
};

template <class Intf, uint32_t ImplType>
struct SetGroupOptInfo {
    static __aicore__ inline void call(
        Intf *self, uint64_t singleCoreCinTail, uint64_t singleCoreCoutTail, bool isGroupOptDimTail = false)
    {
        self->ctx.singleCoreCinTail = singleCoreCinTail;
        self->ctx.singleCoreCoutTail = singleCoreCoutTail;
        self->ctx.isGroupOptDimTail = isGroupOptDimTail;
    }
};

}  // namespace Conv3dApiFunc

#endif
