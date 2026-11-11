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
 * \file conv3d_bp_filter_config.h
 * \brief
 */


#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_FILTER_CONFIG_H__
#endif
#ifndef CONV3D_BP_FILTER_CONFIG_H
#define CONV3D_BP_FILTER_CONFIG_H

#include "common/conv3d_bp_config_base.h"

namespace ConvBackpropApi {

template <class A, class B, class C, class D>
struct Conv3DBpFilterCfg : public ConvBpContext<A, B, C, D>{
public:
    __aicore__ inline Conv3DBpFilterCfg() {}

    using ContextData = struct _ : public ConvBpContext<A, B, C, D>::ContextData {
        __aicore__ inline _() {}
    };
};

}  // namespace ConvBackpropApi
#endif // CONV3D_BP_FILTER_CONFIG_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_FILTER_CONFIG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_FILTER_CONFIG_H__
#endif