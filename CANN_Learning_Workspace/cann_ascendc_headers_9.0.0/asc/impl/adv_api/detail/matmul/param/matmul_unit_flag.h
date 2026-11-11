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
 * \file matmul_unit_flag.h
 * \brief matmul unit flag manager
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/param/matmul_unit_flag.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_UNIT_FLAG_H__
#endif

#ifndef IMPL_MATMUL_PARAM_MATMUL_UNIT_FLAG_H
#define IMPL_MATMUL_PARAM_MATMUL_UNIT_FLAG_H

#include "../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, const auto &MM_CFG, typename = void>
class MatmulUnitFlag
{
public:
    __aicore__ inline uint8_t GetUnitFlag(bool isLast)
    {
        if constexpr (EnUnitFlag(MM_CFG)) {
            return isLast ? UNIT_FLAG_SET : UNIT_FLAG_CHECK;
        } else {
            return 0;
        }
    }
};

template <typename IMPL, const auto &MM_CFG>
class MatmulUnitFlag<IMPL, MM_CFG, enable_if_t<!MatmulFeatureTrait<MM_CFG>::IsUnitFlagEnabled()>>
{
public:
    __aicore__ inline uint8_t GetUnitFlag(bool isLast)
    {
        return 0;
    }
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_PARAM_MATMUL_UNIT_FLAG_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_UNIT_FLAG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_UNIT_FLAG_H__
#endif