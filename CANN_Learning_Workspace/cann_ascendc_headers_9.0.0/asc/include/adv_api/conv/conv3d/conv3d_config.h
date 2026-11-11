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
 * \file conv3d_config.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_CONFIG_H__
#endif
#ifndef ADV_API_CONV_CONV3D_CONV3D_CONFIG_H
#define ADV_API_CONV_CONV3D_CONV3D_CONFIG_H

#include "../../../../impl/adv_api/detail/conv/common/conv_forward_framework_util.h"
#include "include/adv_api/conv/common/conv_forward_config.h"

namespace Conv3dApi {
/**
* @enum class ConvL0PingPong
* @brief L0 Buffer Ping-Pong State Control
*/
enum class ConvL0PingPong : uint32_t {
    ALL_CLOSE = 0, ///< Close both L0A and L0B Buffers
    L0A_OPEN,      ///< Open only L0A Buffer
    L0B_OPEN,      ///< Open only L0B Buffer
    ALL_OPEN       ///< Open both L0A and L0B Buffers
};

/**
* @enum class ConvBL1ByPass
* @brief B Matrix L1 Buffer Bypass Control
*/
enum class ConvBL1ByPass : uint32_t {
    BYPASS_OFF = 0, ///< Disable L1 Buffer bypass
    BYPASS_ON = 1   ///< Enable L1 Buffer bypass
};

/**
* @enum class GroupConvType
* @brief Group Convolution Type Definition
*/
enum class GroupConvType : uint32_t {
    NoGroup_Conv = 0,      ///< Standard convolution, no grouping
    GroupConv_Weight_Gfz   ///< Group convolution, weights organized in Gfz format
};

struct Conv3dParam : public ConvApi::ConvParam {
    __aicore__ inline Conv3dParam(){};
};

template <class ConvDataType>
struct Conv3dCfg : public ConvApi::ConvConfig<ConvDataType> {
public:
    __aicore__ inline Conv3dCfg()
    {}

    using ContextData = struct _ : public ConvApi::ConvConfig<ConvDataType>::ContextData {
        __aicore__ inline _()
        {}
    };
};
}  // namespace Conv3dApi

#endif // ADV_API_CONV_CONV3D_CONV3D_CONFIG_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_CONFIG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_CONFIG_H__
#endif