/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef IMPL_SIMT_API_CPP_KERNEL_SIMT_BESSEL_INTERFACE_IMPL_H
#define IMPL_SIMT_API_CPP_KERNEL_SIMT_BESSEL_INTERFACE_IMPL_H

#include "impl/simt_api/cpp/dav_c310/kernel_simt_bessel_impl.h"

namespace AscendC {
namespace Simt {

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T J0(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");
    return J0Impl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T J1(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");
    return J1Impl(x);
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline U Jn(T n, U x)
{
    static_assert(SupportTypeSimtInternel<T, int>, "Input(n) type only supports int.");
    static_assert(SupportTypeSimtInternel<U, float>, "Input(x) type only supports float.");
    return JnImpl(n, x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Y0(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");
    return Y0Impl(x);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Y1(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");
    return Y1Impl(x);
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline U Yn(T n, U x)
{
    static_assert(SupportTypeSimtInternel<T, int>, "Input(n) type only supports int.");
    static_assert(SupportTypeSimtInternel<U, float>, "Input(x) type only supports float.");
    return YnImpl(n, x);
}

}  // namespace Simt
}  // namespace AscendC
#endif  // IMPL_SIMT_API_CPP_KERNEL_SIMT_BESSEL_INTERFACE_IMPL_H
