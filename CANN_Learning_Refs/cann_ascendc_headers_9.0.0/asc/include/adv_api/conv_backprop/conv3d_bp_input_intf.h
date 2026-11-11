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
 * \file conv3d_bp_input_intf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_INPUT_INTF_H__
#endif
#ifndef CONV3D_BP_INPUT_INTF_H
#define CONV3D_BP_INPUT_INTF_H

#include "../../../impl/adv_api/detail/conv_backprop/conv3d_backprop_input/conv3d_bp_input_func.h"
#include "../../../impl/adv_api/detail/conv_backprop/common/conv3d_bp_util.h"
#include "common/conv3d_bp_config_base.h"

namespace ConvBackpropApi {
template <class Config_, template <typename, class> class Impl>
struct Conv3DBpInputIntf {
    using Config = Config_;
    using Ext = Impl<Conv3DBpInputIntf, Config>;
    using SrcT = typename Config::SrcT;
    using DstT = typename Config::DstT;
    using L0cT = typename Config::L0cT;
    using ContextData = typename Ext::ContextData;

public:
    ContextData ctx;
    constexpr static Conv3dConfig conv3dConfig = Config::conv3dConfig_;

public:
    __aicore__ inline Conv3DBpInputIntf() {}

    __aicore__ inline void Init(const TConv3DBackpropInputTiling *__restrict tiling)
    {
        using Local = typename Ext::Init;
        if constexpr (CHECK_FUN(Local, ConvBackpropInputFunc, this, tiling)) {
            Local::call(this, tiling);
        }
    }

    __aicore__ inline void SetWeight(const AscendC::GlobalTensor<SrcT> &weight)
    {
        using Local = typename Ext::SetWeight;
        if constexpr (CHECK_FUN(Local, ConvBackpropInputFunc, this, weight)) {
            Local::call(this, weight);
        }
    }

    __aicore__ inline void SetGradOutput(const AscendC::GlobalTensor<SrcT> &gradOutput)
    {
        using Local = typename Ext::SetOutBackprop;
        if constexpr (CHECK_FUN(Local, ConvBackpropInputFunc, this, gradOutput)) {
            Local::call(this, gradOutput);
        }
    }

    __aicore__ inline void SetSingleShape(uint64_t singleShapeM, uint64_t singleShapeK, uint32_t singleShapeN)
    {
        using Local = typename Ext::SetSingleShape;
        if constexpr (CHECK_FUN(Local, ConvBackpropInputFunc, this, singleShapeM, singleShapeK, singleShapeN)) {
            Local::call(this, singleShapeM, singleShapeK, singleShapeN);
        }
    }

    __aicore__ inline void SetStartPosition(uint32_t curDinStartIdx, int32_t curHoStartIdx)
    {
        using Local = typename Ext::SetStartPosition;
        if constexpr (CHECK_FUN(Local, ConvBackpropInputFunc, this, curDinStartIdx, curHoStartIdx)) {
            Local::call(this, curDinStartIdx, curHoStartIdx);
        }
    }

    template <bool sync = true>
    __aicore__ inline bool Iterate(bool enPartialSum = false)
    {
        using Local = typename Ext::template Iterate<sync>;
        if constexpr (CHECK_FUN(Local, ConvBackpropInputFunc, this, enPartialSum)) {
            return Local::call(this, enPartialSum);
        }
    }

    template <bool sync = true>
    __aicore__ inline void GetTensorC(const AscendC::GlobalTensor<DstT> &output, uint8_t enAtomic = 0,
                                      bool enSequentialWrite = false)
    {
        using Local = typename Ext::template GetTensorC<sync>;
        if constexpr (CHECK_FUN(Local, ConvBackpropInputFunc, this, output, enAtomic, enSequentialWrite)) {
            Local::call(this, output, enAtomic, enSequentialWrite);
        }
    }

    __aicore__ inline void End()
    {
        using Local = typename Ext::End;
        if constexpr (CHECK_FUN(Local, ConvBackpropInputFunc, this)) {
            Local::call(this);
        }
    }
};

}  // namespace ConvBackpropApi

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_INPUT_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_INPUT_INTF_H__
#endif