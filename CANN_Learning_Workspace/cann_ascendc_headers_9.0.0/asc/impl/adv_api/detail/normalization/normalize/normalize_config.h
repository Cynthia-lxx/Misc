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
 * \file normalize_config.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/normalization/normalize/normalize_config.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_NORMALIZE_NORMALIZE_CONFIG_H__
#endif
#ifndef LIB_NORMALIZATION_NORMALIZE_CONFIG_H
#define LIB_NORMALIZATION_NORMALIZE_CONFIG_H

#include "include/adv_api/normalization/normalize_utils.h"

namespace AscendC {
__aicore__ constexpr NormalizeConfig GetNormalizeConfig(bool isNoBeta, bool isNoGamma)
{
    return {.reducePattern = ReducePattern::AR,
        .aLength = -1,
        .isNoBeta = isNoBeta,
        .isNoGamma = isNoGamma,
        .isOnlyOutput = false};
}

constexpr NormalizeConfig NLCFG_NORM = GetNormalizeConfig(false, false);

constexpr NormalizeConfig NLCFG_NOBETA = GetNormalizeConfig(true, false);

constexpr NormalizeConfig NLCFG_NOGAMMA = GetNormalizeConfig(false, true);

constexpr NormalizeConfig NLCFG_NOOPT = GetNormalizeConfig(true, true);

}; // namespace AscendC
#endif // LIB_NORMALIZATION_NORMALIZE_CONFIG_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_NORMALIZE_NORMALIZE_CONFIG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_NORMALIZE_NORMALIZE_CONFIG_H__
#endif
