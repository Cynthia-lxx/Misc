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
 * \file conv3d_api.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_API_H__
#endif
#ifndef ADV_API_CONV_CONV3D_CONV3D_API_H
#define ADV_API_CONV_CONV3D_CONV3D_API_H

#include "conv3d_intf.h"
#include "conv3d_config.h"
#include "../../../../impl/adv_api/detail/conv/conv3d/conv3d_api_impl.h"

namespace Conv3dApi {
using biasType = ConvApi::ConvType<AscendC::TPosition::GM, ConvCommonApi::ConvFormat::ND, float>;

template <class Config, template <typename, class, bool> class Impl = Conv3dApiImpl,
    template <class, template <typename, class, bool> class> class Intf = Conv3dIntf>
struct Conv3dIntfExt : public Intf<Config, Impl> {
    __aicore__ inline Conv3dIntfExt()
    {}
};

#define REGISTER_CONV3D_API(name, Config, Impl, Intf) \
    template <class INPUT_TYPE,                       \
        class WEIGHT_TYPE,                            \
        class OUTPUT_TYPE,                            \
        class BIAS_TYPE = biasType,                   \
        class CONV_CFG = Conv3dParam>                 \
    using name =                                      \
        Conv3dIntfExt<Config<ConvApi::ConvDataType<INPUT_TYPE, WEIGHT_TYPE, OUTPUT_TYPE, BIAS_TYPE, CONV_CFG>>, Impl, Intf>

REGISTER_CONV3D_API(Conv3D, Conv3dCfg, Conv3dApiImpl, Conv3dIntf);
}  // namespace Conv3dApi
#endif // ADV_API_CONV_CONV3D_CONV3D_API_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_API_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_API_H__
#endif