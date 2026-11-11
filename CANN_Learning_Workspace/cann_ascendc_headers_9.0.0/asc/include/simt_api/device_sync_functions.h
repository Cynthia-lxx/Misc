/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef INCLUDE_SIMT_API_DEVICE_SYNC_FUNCTIONS_H
#define INCLUDE_SIMT_API_DEVICE_SYNC_FUNCTIONS_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_SYNC_FUNCTIONS_H__
#endif
#include "simt_api/device_types.h"

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_syncthreads();

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_threadfence();

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_threadfence_block();

#include "impl/simt_api/device_sync_functions_impl.h"

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_SYNC_FUNCTIONS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_SYNC_FUNCTIONS_H__
#endif

#endif  // INCLUDE_SIMT_API_DEVICE_SYNC_FUNCTIONS_H
