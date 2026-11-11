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
 * \file conv3d_bp_filter_intf.h
 * \brief
 */


#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_FILTER_INTF_H__
#endif
#ifndef CONV3D_BP_FILTER_INTF_H
#define CONV3D_BP_FILTER_INTF_H

#include "../../../impl/adv_api/detail/conv_backprop/conv3d_backprop_filter/conv3d_bp_filter_func.h"
#include "../../../impl/adv_api/detail/conv_backprop/common/conv3d_bp_util.h"
#include "common/conv3d_bp_config_base.h"

namespace ConvBackpropApi {
template <class Config_, template <typename, class> class Impl>
struct Conv3DBpFilterIntf {
    using Config = Config_;
    using Ext = Impl<Conv3DBpFilterIntf, Config>;
    using SrcT = typename Config::SrcT;
    using DstT = typename Config::DstT;
    using L0cT = typename Config::L0cT;
    using ContextData = typename Ext::ContextData;

public:
    ContextData ctx;

public:
    __aicore__ inline Conv3DBpFilterIntf() {}

    __aicore__ inline void Init(const TConv3DBpFilterTiling *__restrict tiling)
    {
        using Local = typename Ext::Init;
        if constexpr (CHECK_FUN(Local, ConvBackpropFilterFunc, this, tiling)) {
            Local::call(this, tiling);
        }
    }

    __aicore__ inline void SetInput(const AscendC::GlobalTensor<SrcT> &input)
    {
        using Local = typename Ext::SetInput;
        if constexpr (CHECK_FUN(Local, ConvBackpropFilterFunc, this, input)) {
            Local::call(this, input);
        }
    }

    __aicore__ inline void SetGradOutput(const AscendC::GlobalTensor<SrcT> &gradOutput)
    {
        using Local = typename Ext::SetGradOutput;
        if constexpr (CHECK_FUN(Local, ConvBackpropFilterFunc, this, gradOutput)) {
            Local::call(this, gradOutput);
        }
    }

    __aicore__ inline void SetSingleShape(uint64_t singleCoreM, uint64_t singleCoreN, uint64_t singleCoreK)
    {
        using Local = typename Ext::SetSingleShape;
        if constexpr (CHECK_FUN(Local, ConvBackpropFilterFunc, this, singleCoreM, singleCoreN, singleCoreK)) {
            Local::call(this, singleCoreM, singleCoreN, singleCoreK);
        }
    }

    __aicore__ inline void SetStartPosition(uint32_t hoStartIdx)
    {
        using Local = typename Ext::SetStartPosition;
        if constexpr (CHECK_FUN(Local, ConvBackpropFilterFunc, this, hoStartIdx)) {
            Local::call(this, hoStartIdx);
        }
    }

    template <bool sync = true>
    __aicore__ inline bool Iterate(bool enPartialSum = false)
    {
        using Local = typename Ext::template Iterate<sync>;
        if constexpr (CHECK_FUN(Local, ConvBackpropFilterFunc, this, enPartialSum)) {
            return Local::call(this, enPartialSum);
        }
    }

    template <bool sync = true>
    __aicore__ inline void GetTensorC(
        const AscendC::GlobalTensor<DstT> &output, uint8_t enAtomic = 1, bool enSequentialWrite = false)
    {
        using Local = typename Ext::template GetTensorC<sync>;
        if constexpr (CHECK_FUN(Local, ConvBackpropFilterFunc, this, output, enAtomic, enSequentialWrite)) {
            Local::call(this, output, enAtomic, enSequentialWrite);
        }
    }

    __aicore__ inline void End()
    {
        using Local = typename Ext::End;
        if constexpr (CHECK_FUN(Local, ConvBackpropFilterFunc, this)) {
            Local::call(this);
        }
    }
};

}  // namespace ConvBackpropApi

#endif // CONV3D_BP_FILTER_CONFIG_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_FILTER_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_FILTER_INTF_H__
#endif