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
 * \file conv3d_intf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_INTF_H__
#endif
#ifndef ADV_API_CONV_CONV3D_CONV3D_INTF_H
#define ADV_API_CONV_CONV3D_CONV3D_INTF_H

#include "kernel_utils.h"
#include "../../../../impl/adv_api/detail/conv/conv3d/conv3d_util.h"
#include "../../../../impl/adv_api/detail/conv/conv3d/conv3d_common_func.h"
#include "../../../../impl/adv_api/detail/conv/common/conv_forward_common_func.h"
#include "../../../../impl/adv_api/detail/conv/common/conv_forward_framework_util.h"

namespace Conv3dApi {

template <class Config, template <typename, class, bool> class Impl>
struct Conv3dIntf {
    using Ext = Impl<Conv3dIntf, Config, false>;
    using InputT = typename Config::SrcAT;
    using WeightT = typename Config::SrcBT;
    using OutputT = typename Config::DstT;
    using BiasT = typename Config::BiasT;
    using L0cT = typename Config::L0cT;
    using ContextType = typename Ext::ContextData;
    using ImplDataType = typename Ext::ImplDataType;
    using ConvParam = typename Config::ConvParam;

public:
    ContextType ctx;
    ImplDataType impl;
    constexpr static bool outputOrder = Ext::outputOrder;
    constexpr static int8_t l0pingpong = Ext::l0pingpong;
    constexpr static int8_t bl1bypass = Ext::bl1bypass;
    constexpr static int8_t groupConvType = Ext::groupConvType;
    constexpr static auto formatType = Config::formatA;

public:
    __aicore__ inline Conv3dIntf()
    {}

    /**
    * @brief Initialize the convolution with tiling parameters
    * @param cubeTiling Pointer to tiling parameters
    */
    __aicore__ inline void Init(const void *__restrict cubeTiling)
    {
        using local = typename Ext::Init;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, cubeTiling)) {
            local::call(this, cubeTiling);
        }
    }

    /**
    * @brief Set input tensor
    * @param input Global tensor containing input data
    */
    __aicore__ inline void SetInput(const AscendC::GlobalTensor<InputT> &input)
    {
        using local = typename Ext::SetInput;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, input)) {
            local::call(this, input);
        }
    }

    /**
    * @brief Set weight tensor
    * @param weight Global tensor containing weight data
    */
    __aicore__ inline void SetWeight(const AscendC::GlobalTensor<WeightT> &weight)
    {
        using local = typename Ext::SetWeight;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, weight)) {
            local::call(this, weight);
        }
    }

    /**
    * @brief Set bias tensor
    * @param bias Global tensor containing bias data
    */
    __aicore__ inline void SetBias(const AscendC::GlobalTensor<BiasT> &bias)
    {
        using local = typename Ext::SetBias;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, bias)) {
            local::call(this, bias);
        }
    }

    /**
    * @brief Set single output shape for 2D-like convolution
    * @param singleCo Single output channel dimension
    * @param singleDo Single output depth dimension
    * @param singleM Single M dimension (height*width)
    */
    __aicore__ inline void SetSingleOutputShape(uint64_t singleCo, uint64_t singleDo, uint64_t singleM)
    {
        using local = typename Ext::SetSingleOutputShape;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, singleCo, singleDo, singleM, 0)) {
            local::call(this, singleCo, singleDo, singleM, 0);
        }
    }

    /**
    * @brief Set input start position for 2D-like convolution
    * @param diStartPos Starting position in depth dimension
    * @param mStartPos Starting position in M dimension (height*width)
    */
    __aicore__ inline void SetInputStartPosition(int64_t diStartPos, int64_t mStartPos)
    {
        using local = typename Ext::SetInputStartPosition;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, diStartPos, mStartPos, 0)) {
            local::call(this, diStartPos, mStartPos, 0);
        }
    }

    /**
    * @brief Iterate through all convolution operations
    * @tparam This template parameter is not enabled, reserved parameter
    * @param output Output tensor
    * @param enPartialSum Enable partial sum accumulation
    */
    template <bool sync = true>
    __aicore__ inline void IterateAll(const AscendC::GlobalTensor<OutputT> &output, bool enPartialSum = false)
    {
        using local = typename Ext::IterateAll;
        if constexpr (CONV_CHECK_FUN_TEMPLATE(local, ConvApiFunc, sync, this, output, enPartialSum)) {
            local::template call<sync>(this, output, enPartialSum);
        }
    }

    /**
    * @brief End the convolution operation and clean up resources
    */
    __aicore__ inline void End()
    {
        using local = typename Ext::End;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this)) {
            local::call(this);
        }
    }

private:
    template <bool sync = true>
    __aicore__ inline bool Iterate(bool enPartialSum = false)
    {
        using local = typename Ext::Iterate;
        if constexpr (CONV_CHECK_FUN_TEMPLATE(local, ConvApiFunc, sync, this, enPartialSum)) {
            return local::template call<sync>(this, enPartialSum);
        }
        return false;
    }

    template <bool sync = true>
    __aicore__ inline void GetTensorC(const AscendC::GlobalTensor<OutputT> &output, bool enSequentialWrite = false)
    {
        using local = typename Ext::GetTensorC;
        if constexpr (CONV_CHECK_FUN_TEMPLATE(local, ConvApiFunc, sync, this, output, enSequentialWrite)) {
            local::template call<sync>(this, output, enSequentialWrite);
        }
    }

    __aicore__ inline void SetInputStartPosition(int64_t diStartPos, int64_t hiStartPos, int64_t wiStartPos)
    {
        using local = typename Ext::SetInputStartPosition;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, diStartPos, hiStartPos, wiStartPos, 0)) {
            local::call(this, diStartPos, hiStartPos, wiStartPos, 0);
        }
    }

    __aicore__ inline void SetSingleOutputShape(
        uint64_t singleCo, uint64_t singleDo, uint64_t singleHo, uint64_t singleWo)
    {
        using local = typename Ext::SetSingleOutputShape;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, singleCo, singleDo, singleHo, singleWo, 0)) {
            local::call(this, singleCo, singleDo, singleHo, singleWo, 0);
        }
    }

    __aicore__ inline void SetOrgInputShape(uint64_t orgCi, uint64_t orgDi, uint64_t orgHi, uint64_t orgWi)
    {
        using local = typename Ext::SetOrgInputShape;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, orgCi, orgDi, orgHi, orgWi)) {
            local::call(this, orgCi, orgDi, orgHi, orgWi);
        }
    }

    __aicore__ inline void SetOrgWeightShape(
        uint64_t orgCo, uint64_t orgCi, uint64_t orgKd, uint64_t orgKh, uint64_t orgKw)
    {
        using local = typename Ext::SetOrgWeightShape;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, orgCo, orgCi, orgKd, orgKh, orgKw)) {
            local::call(this, orgCo, orgCi, orgKd, orgKh, orgKw);
        }
    }

    __aicore__ inline void SetOrgOutputShape(uint64_t orgCo, uint64_t orgDo, uint64_t orgHo, uint64_t orgWo)
    {
        using local = typename Ext::SetOrgOutputShape;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, orgCo, orgDo, orgHo, orgWo)) {
            local::call(this, orgCo, orgDo, orgHo, orgWo);
        }
    }

    __aicore__ inline void SetSingleInputShape(
        uint64_t singleCi, uint64_t singleDi, uint64_t singleHi, uint64_t singleWi)
    {
        using local = typename Ext::SetSingleInputShape;
        if constexpr (CONV_CHECK_FUN(local, ConvApiFunc, this, singleCi, singleDi, singleHi, singleWi)) {
            local::call(this, singleCi, singleDi, singleHi, singleWi);
        }
    }
};

}  // namespace Conv3dApi

#endif // ADV_API_CONV_CONV3D_CONV3D_INTF_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_INTF_H__
#endif