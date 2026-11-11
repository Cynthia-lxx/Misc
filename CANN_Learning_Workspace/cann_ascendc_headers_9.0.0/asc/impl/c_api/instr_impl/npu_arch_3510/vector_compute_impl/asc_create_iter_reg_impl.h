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
 * \file asc_create_iter_reg_impl.h
 * \brief
 */

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_VECTOR_COMPUTE_IMPL_ASC_CREATE_ITER_REG_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_VECTOR_COMPUTE_IMPL_ASC_CREATE_ITER_REG_IMPL_H

#include "instr_impl/npu_arch_3510/utils_impl.h"

__simd_callee__ inline iter_reg asc_create_iter_reg_b32_impl(uint32_t offset)
{
    if ASC_IS_AIV {
        return vag_b32(offset);
    }
    return iter_reg{};
}

__simd_callee__ inline iter_reg asc_create_iter_reg_b16_impl(uint32_t offset)
{
    if ASC_IS_AIV {
        return vag_b16(offset);
    }
    return iter_reg{};
}

__simd_callee__ inline iter_reg asc_create_iter_reg_b8_impl(uint32_t offset)
{
    if ASC_IS_AIV {
        return vag_b8(offset);
    }
    return iter_reg{};
}

#endif
