/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/* !
 * \file kernel_operator_layernorm_intf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LAYERNORM_INTF_H__
#endif

#ifndef LIB_NORMALIZATION_KERNEL_OPERATOR_LAYERNORM_INTF_H
#define LIB_NORMALIZATION_KERNEL_OPERATOR_LAYERNORM_INTF_H

#include "kernel_tensor.h"
namespace AscendC {
[[deprecated(__FILE__ " is deprecated, please use layernorm.h instead!")]] typedef void LayerNormDeprecatedHeader;
using LibLayernormInterface = LayerNormDeprecatedHeader;
} // namespace AscendC
#endif // LIB_NORMALIZATION_KERNEL_OPERATOR_LAYERNORM_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LAYERNORM_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LAYERNORM_INTF_H__
#endif