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
 * \file asc_cpu_log.h
 * \brief
 */

#ifndef ASC_CPU_LOG_H
#define ASC_CPU_LOG_H
#include <cassert>
#include <cstdint>
// relative to CANN_PACKAGE_PATH/include
#include "../pkg_inc/base/dlog_pub.h"
#include "aicpu/cust_cpu_utils.h"

#define ASCENDC_MODULE_NAME static_cast<int32_t>(ASCENDCKERNEL)

// 0 debug, 1 info, 2 warning, 3 error
#if defined(DEVICE_OP_TILING_LIB) && defined(DEVICE_OP_LOG_BY_DUMP)
// [aicpu] log by dump
#define ASC_CPU_LOG_ERROR(format, ...)                                         \
  do {                                                                         \
    aicpu::CustCpuKernelUtils::DumpCustomLog(                                  \
      ASCENDC_MODULE_NAME, DLOG_ERROR, "[%s:%d][%s] " format,                  \
      __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                        \
  } while (0)
#define ASC_CPU_LOG_INFO(format, ...)                                          \
  do {                                                                         \
    aicpu::CustCpuKernelUtils::DumpCustomLog(                                  \
      ASCENDC_MODULE_NAME, DLOG_INFO, "[%s:%d][%s] " format,                   \
      __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                        \
  } while (0)
#define ASC_CPU_LOG_WARNING(format, ...)                                       \
  do {                                                                         \
    aicpu::CustCpuKernelUtils::DumpCustomLog(                                  \
      ASCENDC_MODULE_NAME, DLOG_WARN, "[%s:%d][%s] " format,                   \
      __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                        \
  } while (0)
#define ASC_CPU_LOG_DEBUG(format, ...)                                         \
  do {                                                                         \
    aicpu::CustCpuKernelUtils::DumpCustomLog(                                  \
      ASCENDC_MODULE_NAME, DLOG_DEBUG, "[%s:%d][%s] " format,                  \
      __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                        \
  } while (0)

#else
// [aicpu] log by slog
#define ASC_CPU_LOG_ERROR(format, ...)                                         \
  do {                                                                         \
    dlog_error(ASCENDC_MODULE_NAME, "[%s:%d][%s] " format "\n",                \
      __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                        \
  } while (0)
#define ASC_CPU_LOG_INFO(format, ...)                                          \
  do {                                                                         \
    dlog_info(ASCENDC_MODULE_NAME, "[%s:%d][%s] " format "\n",                 \
      __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                        \
  } while (0)
#define ASC_CPU_LOG_WARNING(format, ...)                                       \
  do {                                                                         \
    dlog_warn(ASCENDC_MODULE_NAME, "[%s:%d][%s] " format "\n",                 \
      __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                        \
  } while (0)
#define ASC_CPU_LOG_DEBUG(format, ...)                                         \
  do {                                                                         \
    dlog_debug(ASCENDC_MODULE_NAME, "[%s:%d][%s] " format "\n",                \
      __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                        \
  } while (0)

#endif

#endif // ASC_CPU_LOG_H
