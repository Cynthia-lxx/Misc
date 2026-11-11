/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSDATA_COMMON_H__
#endif

#ifndef LIB_TRANSPOSE_TRANSDATA_COMMON_H
#define LIB_TRANSPOSE_TRANSDATA_COMMON_H

#include "kernel_basic_intf.h"

namespace AscendC {
template <typename T, typename U>
struct TransDataParams {
    T srcLayout;
    U dstLayout;
};

#ifndef ASCC_PARAM_TRANSDATACONFIG
#define ASCC_PARAM_TRANSDATACONFIG
struct TransDataConfig {
    DataFormat srcFormat;
    DataFormat dstFormat;
};
#endif // ASCC_PARAM_TRANSDATACONFIG
} // namespace AscendC

#endif // LIB_TRANSPOSE_TRANSDATA_COMMON_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSDATA_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSDATA_COMMON_H__
#endif