/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef ASCENDC_MODULE_SIMT_MATH_INTERFACE_H
#define ASCENDC_MODULE_SIMT_MATH_INTERFACE_H

#include "simt_api/device_types.h"

namespace AscendC {
namespace Simt {
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Abs(T x);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T UintDiv(T dividend, T magic, T shift);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Fma(T x, T y, T z);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Max(T x, T y);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Min(T x, T y);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Fdim(T x, T y);

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T RemQuo(T x, T y, U *quo);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Mod(T x, T y);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Remainder(T x, T y);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CopySign(T x, T y);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T NearbyInt(T x);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T NextAfter(T x, T y);

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ScaLbn(T x, U n);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Brev(T x);

// count the leading zero bits
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t Clz(T x);

// count the number of set 1 bit
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t Popc(T x);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T BytePerm(T x, T y, T s);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t Ffs(T x);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T MulHi(T x, T y);
}  // namespace Simt
}  // namespace AscendC

#include "impl/simt_api/cpp/kernel_simt_math_intf_impl.h"
#endif  // ASCENDC_MODULE_SIMT_MATH_INTERFACE_H
