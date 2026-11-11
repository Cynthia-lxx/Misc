/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/


#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)  
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS  
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H  
#endif     

#ifndef INCLUDE_C_API_SYNC_SYNC_H
#define INCLUDE_C_API_SYNC_SYNC_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)

#include "instr_impl/npu_arch_2201/sync_impl.h"

#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

#include "instr_impl/npu_arch_3510/sync_impl.h"

#endif

#define asc_sync_notify(pipe, tpipe, id) asc_sync_notify_impl(pipe, tpipe, id)

#define asc_sync_wait(pipe, tpipe, id) asc_sync_wait_impl(pipe, tpipe, id)

#define asc_sync_pipe(pipe) asc_sync_pipe_impl(pipe)

__aicore__ inline void asc_sync_vec();

__aicore__ inline void asc_sync_mte3(int id);

__aicore__ inline void asc_sync_mte2(int id);

__aicore__ inline void asc_sync();

__aicore__ inline void asc_sync_block_arrive(pipe_t pipe, uint8_t mode, int64_t flagID);

__aicore__ inline void asc_sync_block_wait(int64_t flagID);

__aicore__ inline void asc_sync_data_barrier(mem_dsb_t arg);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)  
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS  
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H  
#endif    

