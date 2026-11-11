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
 * \file conv3d_bp_filter_api.h
 * \brief
 */


#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_FILTER_API_H__
#endif
#ifndef CONV3D_BP_FILTER_API_H
#define CONV3D_BP_FILTER_API_H

#include "conv3d_bp_filter_intf.h"
#include "conv3d_bp_filter_config.h"
#include "../../../impl/adv_api/detail/conv_backprop/conv3d_backprop_filter/conv3d_bp_filter_impl.h"

namespace ConvBackpropApi {
    template <class INPUT_TYPE, class WEIGHT_TYPE, class GRAD_OUTPUT_TYPE, class GRAD_WEIGHT_TYPE>
    using Conv3DBackpropFilter = Conv3DBpFilterIntf<Conv3DBpFilterCfg<INPUT_TYPE, WEIGHT_TYPE, GRAD_OUTPUT_TYPE, GRAD_WEIGHT_TYPE>, Conv3DBpFilterImpl>;

}  // namespace ConvBackpropApi
#endif // CONV3D_BP_FILTER_API_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_FILTER_API_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV3D_BP_FILTER_API_H__
#endif