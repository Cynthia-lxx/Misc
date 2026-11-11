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
 * \file kernel_operator_dump_tensor_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/basic_api/kernel_operator_dump_tensor_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_dump_tensor_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_IMPL_H
#include "kernel_tensor.h"
#if __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 2002
#include "impl/utils/debug/asc_debug_utils.h"
#endif

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_dump_tensor_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_dump_tensor_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_dump_tensor_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_dump_tensor_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_c310/kernel_operator_dump_tensor_impl.h"
#endif

#ifdef ASCENDC_CPU_DEBUG
#include <cstdio>
#include <utility>
#endif

namespace AscendC {
template <typename T>
__aicore__ inline void DumpTensor(const LocalTensor<T> &input, uint32_t desc, uint32_t dumpSize)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpTensor is not supported in cpu mode.");});
#endif
#ifdef ASCENDC_DUMP
    DumpTensorLocal2GMImpl(input, desc, dumpSize);
#endif
    return;
}
template <typename T>
__aicore__ inline void DumpTensor(const GlobalTensor<T>& input, uint32_t desc, uint32_t dumpSize)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpTensor is not supported in cpu mode.");});
#endif
#ifdef ASCENDC_DUMP
    DumpTensorGM2GMImpl(input, desc, dumpSize);
#endif
    return;
}
template <typename T>
__aicore__ inline void DumpTensor(const GlobalTensor<T>& input, uint32_t desc, uint32_t dumpSize, const ShapeInfo& shapeInfo)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpTensor is not supported in cpu mode.");});
#endif
#ifdef ASCENDC_DUMP
    DumpShapeImpl(shapeInfo);
    DumpTensorGM2GMImpl(input, desc, dumpSize);
#endif
    return;
}
template <typename T>
__aicore__ inline void DumpTensor(const LocalTensor<T>& input, uint32_t desc, uint32_t dumpSize, const ShapeInfo& shapeInfo)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpTensor is not supported in cpu mode.");});
#endif
#ifdef ASCENDC_DUMP
    DumpShapeImpl(shapeInfo);
    DumpTensorLocal2GMImpl(input, desc, dumpSize);
#endif
    return;
}

template <typename T>
__aicore__ inline void DumpAccChkPoint(const LocalTensor<T> &input, uint32_t index, uint32_t countOff, uint32_t dumpSize)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpAccChkPoint is not supported in cpu mode.");});
#endif
#if defined(ASCENDC_DUMP) || defined(ASCENDC_ACC_DUMP)
    if (countOff > input.GetSize()) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "tensor offset [%d] exceeds limit [%d]",
                        countOff, input.GetSize()); });
        return;
    }
    LocalTensor<T> tmp = input[countOff];
    DumpTensorLocal2GMImpl(tmp, index, dumpSize);
#endif
    return;
}
template <typename T>
__aicore__ inline void DumpAccChkPoint(const GlobalTensor<T> &input, uint32_t index, uint32_t countOff, uint32_t dumpSize)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpAccChkPoint is not supported in cpu mode.");});
#endif
#if defined(ASCENDC_DUMP) || defined(ASCENDC_ACC_DUMP)
    if (countOff > input.GetSize()) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "tensor offset [%d] exceeds limit [%d]",
                        countOff, input.GetSize()); });
        return;
    }
    GlobalTensor<T> tmp = input[countOff];
    DumpTensorGM2GMImpl(tmp, index, dumpSize);
#endif
    return;
}

#if defined(__NPU_DEVICE__) || defined(__ASCC_DEVICE__)
template <class... Args>
__aicore__ inline void PRINTF(__gm__ const char* fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
    PrintfImpl(DumpType::DUMP_SCALAR, fmt, args...);
#endif
}
template <class... Args>
__aicore__ inline void printf(__gm__ const char* fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
    PrintfImpl(DumpType::DUMP_SCALAR, fmt, args...);
#endif
}

#else // !defined(__NPU_DEVICE__) && !defined(__ASCC_DEVICE__)

#ifdef ASCENDC_CPU_DEBUG
using ::printf;

template<typename... Args>
inline auto PRINTF(Args&&... args) -> decltype(printf(std::forward<Args>(args)...))
{
#ifdef ASCENDC_DUMP
    return printf(std::forward<Args>(args)...);
#else
    return 0;
#endif
}

#if defined(__NPU_HOST__) || defined(__ASCC_HOST__)
template <class... Args>
inline void PRINTF(const char* fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
    PrintfImpl(DumpType::DUMP_SCALAR, fmt, args...);
#endif
}
#endif // defined(__NPU_HOST__) || defined(__ASCC_HOST__)

#else
template <class... Args>
__aicore__ inline void PRINTF(__gm__ const char* fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
    PrintfImpl(DumpType::DUMP_SCALAR, fmt, args...);
#endif
}

template <class... Args>
__aicore__ inline void printf(__gm__ const char* fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
    PrintfImpl(DumpType::DUMP_SCALAR, fmt, args...);
#endif
}
#endif // ASCENDC_CPU_DEBUG
#endif // defined(__NPU_DEVICE__) || defined(__ASCC_DEVICE__)

#if defined(ASCENDC_DUMP_ASSERT_ONLY)
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_dumpAddrAssertOnlyReserved;
__BLOCK_LOCAL__ __inline__ bool g_mixFlagAssertOnlyReserved;
__aicore__ inline void StoreArgsOfInitDump(bool mixFlag, __gm__ uint8_t* dumpAddr = nullptr)
{
    g_mixFlagAssertOnlyReserved = mixFlag;
    g_dumpAddrAssertOnlyReserved = dumpAddr;
}
#endif

template <class... Args>
__aicore__ inline void AssertImpl(__gm__ const char* fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
#ifdef ASCENDC_DUMP_ASSERT_ONLY
    if (g_dumpAddrAssertOnlyReserved == nullptr) {
        InitDump(g_mixFlagAssertOnlyReserved, ONE_CORE_DUMP_SIZE);
    } else {
        InitDump(g_mixFlagAssertOnlyReserved, g_dumpAddrAssertOnlyReserved, ONE_CORE_DUMP_SIZE);
    }
#endif
    PrintfImpl(DumpType::DUMP_ASSERT, fmt, args...);
#else
    return;
#endif
}

// for auto open ASCENDC_DUMP macros
#ifdef __CHECK_FEATURE_AT_PRECOMPILE
#if !defined(__NPU_HOST__) && !defined(__NPU_DEVICE__) && !defined(__ASCC_HOST__) && !defined(__ASCC_DEVICE__)
#define DumpTensor(...)            \
    do {                           \
        ENABLE_PRINTF();           \
        ENABLE_PRINTF_DUMP_SIZE(); \
    } while (0)

#define DumpAccChkPoint(...)       \
    do {                           \
        ENABLE_PRINTF();           \
        ENABLE_PRINTF_DUMP_SIZE(); \
    } while (0)

#define printf(...)                \
    do {                           \
        ENABLE_PRINTF();           \
        ENABLE_PRINTF_DUMP_SIZE(); \
    } while (0)

#define PRINTF(...)                \
    do {                           \
        ENABLE_PRINTF();           \
        ENABLE_PRINTF_DUMP_SIZE(); \
    } while (0)
#endif // !defined(__NPU_HOST__) && !defined(__NPU_DEVICE__) && !defined(__ASCC_HOST__) && !defined(__ASCC_DEVICE__)
#endif // __CHECK_FEATURE_AT_PRECOMPILE

__aicore__ inline void PrintTimeStamp(uint32_t descId)
{
#ifdef ASCENDC_TIME_STAMP_ON  // 打点开关宏
    DumpTimeStampImpl(descId);
#endif
}

}  // namespace AscendC
#endif  // END OF ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_IMPL_H__
#endif
