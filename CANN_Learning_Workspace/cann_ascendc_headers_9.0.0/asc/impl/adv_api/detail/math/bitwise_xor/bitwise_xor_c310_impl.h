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
 * \file bitwise_xor_c310_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/bitwise_xor/bitwise_xor_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/bitwise_xor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_XOR_BITWISE_XOR_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_BITWISE_XOR_BITWISE_XOR_C310_IMPL_H
#define IMPL_MATH_BITWISE_XOR_BITWISE_XOR_C310_IMPL_H
#include "../bitwise_template/bitwise_template.h"
#include "kernel_basic_intf.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/bitwise_xor/bitwise_xor_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
struct BitwiseXorConfig {
    bool isReuseSource;
};
constexpr BitwiseXorConfig DEFAULT_BITWISE_XOR_CONFIG = {false};
template <const BitwiseXorConfig& config, typename T>
__aicore__ inline void BitwiseXorImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
                                      const uint32_t count)
{
    if ASCEND_IS_AIC {
        return;
    }

    CHECK_FUNC_HIGHLEVEL_API(BitwiseXor, (T, config.isReuseSource), (dst, src0, src1, count));

    if constexpr (sizeof(T) == 8) {
        BitwiseTemplateImpl<
            Reg::Xor<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>, T>(
            dst, src0, src1, count);
    } else {
        BitwiseTemplateImpl<
            Reg::Xor<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumOne>>, T>(
            dst, src0, src1, count);
    }
}
} // namespace AscendC

#endif // IMPL_MATH_BITWISE_XOR_BITWISE_XOR_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_XOR_BITWISE_XOR_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_XOR_BITWISE_XOR_C310_IMPL_H__
#endif
