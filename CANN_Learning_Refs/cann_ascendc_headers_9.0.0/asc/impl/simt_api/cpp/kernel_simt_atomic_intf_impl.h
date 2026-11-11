/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef IMPL_SIMT_API_CPP_KERNEL_SIMT_ATOMIC_INTERFACE_IMPL_H
#define IMPL_SIMT_API_CPP_KERNEL_SIMT_ATOMIC_INTERFACE_IMPL_H

#include "impl/simt_api/cpp/dav_c310/kernel_simt_atomic_impl.h"

namespace AscendC {
namespace Simt {

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicAdd(__ubuf__ T *address, T val)
{
    return AtomicAddImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicAdd(__gm__ T *address, T val)
{
    return AtomicAddImpl(address, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicSub(__ubuf__ T *address, T val)
{
    return AtomicSubImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicSub(__gm__ T *address, T val)
{
    return AtomicSubImpl(address, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicExch(__ubuf__ T *address, T val)
{
    return AtomicExchImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicExch(__gm__ T *address, T val)
{
    return AtomicExchImpl(address, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicMax(__ubuf__ T *address, T val)
{
    return AtomicMaxImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicMax(__gm__ T *address, T val)
{
    return AtomicMaxImpl(address, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicMin(__ubuf__ T *address, T val)
{
    return AtomicMinImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicMin(__gm__ T *address, T val)
{
    return AtomicMinImpl(address, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicInc(__ubuf__ T *address, T val)
{
    return AtomicIncImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicInc(__gm__ T *address, T val)
{
    return AtomicIncImpl(address, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicDec(__ubuf__ T *address, T val)
{
    return AtomicDecImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicDec(__gm__ T *address, T val)
{
    return AtomicDecImpl(address, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicCas(__ubuf__ T *address, T compare, T val)
{
    return AtomicCasImpl(address, compare, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicCas(__gm__ T *address, T compare, T val)
{
    return AtomicCasImpl(address, compare, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicAnd(__ubuf__ T *address, T val)
{
    return AtomicAndImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicAnd(__gm__ T *address, T val)
{
    return AtomicAndImpl(address, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicOr(__ubuf__ T *address, T val)
{
    return AtomicOrImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicOr(__gm__ T *address, T val)
{
    return AtomicOrImpl(address, val);
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicXor(__ubuf__ T *address, T val)
{
    return AtomicXorImpl(address, val);
}
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AtomicXor(__gm__ T *address, T val)
{
    return AtomicXorImpl(address, val);
}
}  // namespace Simt
}  // namespace AscendC
#endif  // IMPL_SIMT_API_CPP_KERNEL_SIMT_ATOMIC_INTERFACE_IMPL_H
