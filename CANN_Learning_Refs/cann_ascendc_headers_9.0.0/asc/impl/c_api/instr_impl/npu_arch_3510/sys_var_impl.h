/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_H

#include "instr_impl/npu_arch_3510/sys_var_impl/asc_get_ar_spr_impl.h"
#include "instr_impl/npu_arch_3510/sys_var_impl/asc_get_program_counter_impl.h"
#include "instr_impl/npu_arch_3510/sys_var_impl/asc_set_ctrl_impl.h"
#include "instr_impl/npu_arch_3510/sys_var_impl/asc_get_block_num_impl.h"
#include "instr_impl/npu_arch_3510/sys_var_impl/asc_get_system_cycle_impl.h"
#include "instr_impl/npu_arch_3510/sys_var_impl/asc_get_ctrl_impl.h"

__aicore__ inline int64_t asc_get_ar_spr()
{
    return asc_get_ar_spr_impl();
}

__aicore__ inline int64_t asc_get_program_counter()
{
    return asc_get_program_counter_impl();
}

__aicore__ inline int64_t asc_get_block_num()
{
    return asc_get_block_num_impl();
}

__aicore__ inline int64_t asc_get_system_cycle()
{
    return asc_get_system_cycle_impl();
}

__aicore__ inline void asc_set_ctrl(uint64_t config)
{
    asc_set_ctrl_impl(config);
}

__aicore__ inline int64_t asc_get_ctrl()
{
    return asc_get_ctrl_impl();
}
#endif
