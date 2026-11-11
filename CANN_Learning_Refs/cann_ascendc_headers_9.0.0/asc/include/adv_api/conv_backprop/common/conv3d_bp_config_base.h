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
 * \file conv3d_bp_config_base.h
 * \brief
 */

#ifndef CONV3D_BP_CONFIG_BASE_H
#define CONV3D_BP_CONFIG_BASE_H

#include "kernel_basic_intf.h"
#include "include/adv_api/conv/common/conv_common.h"

namespace ConvBackpropApi {
template <typename T>
struct GetDstType {
    using Type = T;
};

template <>
struct GetDstType<float> {
    using Type = float;
};

template <>
struct GetDstType<half> {
    using Type = float;
};

template <>
struct GetDstType<int8_t> {
    using Type = int32_t;
};

template <>
struct GetDstType<bfloat16_t> {
    using Type = float;
};

// ConvType，定义卷积输入输出对象的属�?
template <AscendC::TPosition POSITION, ConvCommonApi::ConvFormat FORMAT, typename T>
struct ConvType {
    constexpr static AscendC::TPosition pos = POSITION;    // Convolution输入或输出时的scope
    constexpr static ConvCommonApi::ConvFormat format = FORMAT;  // Convolution输入或者输出的format
    using Type = T;                               // Convolution输入或输出的数据类型
};

enum class B2Condition : uint8_t {
    BASEK_LT_HKWK = 0,
    BASEK_GE_HKWK,
    HKWK_EQ_ONE
};

struct Conv3dConfig {
    // loadB2Condition含义:
    // 0: kRepeat < Hk * Wk
    // 1: kRepeat >= Hk * Wk
    // 2: Hk * Wk = 1
    B2Condition loadB2Condition = B2Condition::BASEK_LT_HKWK;
    bool enableKernelSplit = false;
};

constexpr Conv3dConfig CONV3D_CFG_DEFAULT = {
        .loadB2Condition = B2Condition::BASEK_GE_HKWK,
        .enableKernelSplit = false,
    };

// 打包字段，内部实现的上下文，包含了用户构造的ConvBpParam
template <class A, class B, class C, class D>
struct ConvBpContext {
    using xType = A;
    using cType = C;
    using dType = D;
    using SrcT = typename A::Type;
    using SrcAT = typename A::Type;
    using SrcBT = typename C::Type;
    using DstT = typename D::Type;
    using L0cT = typename GetDstType<SrcT>::Type;

    constexpr static auto formatA = A::format;
    constexpr static auto formatB = B::format;
    constexpr static auto formatC = C::format;
    constexpr static auto formatD = D::format;

    constexpr static auto posA = A::pos;
    constexpr static auto posB = B::pos;
    constexpr static auto posC = C::pos;
    constexpr static auto posD = D::pos;

    using ContextData = struct _ {
        __aicore__ inline _() {}
    };
};
}  // namespace ConvBackpropApi
#endif