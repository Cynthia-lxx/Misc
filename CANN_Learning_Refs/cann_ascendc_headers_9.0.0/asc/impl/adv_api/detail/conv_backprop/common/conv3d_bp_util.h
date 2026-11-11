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
 * \file conv3d_bp_util.h
 * \brief
 */

#ifndef CONV3D_BP_UTIL_H
#define CONV3D_BP_UTIL_H

#include "kernel_utils.h"

namespace ConvBackpropApi {
static __aicore__ inline uint64_t ShiftCeilBlockCube(uint64_t a)
{
    return (a + 15) >> 4; // 4: bit; 15: 2**4 - 1
}

static __aicore__ inline uint64_t CalcDiv(uint64_t a, uint32_t b)
{
    if (b > a) {
        return 0;
    } else if (b == a) {
        return 1;
    } else if (b == 1) {
        return a;
    } else {
        return a / b;
    }
}

static __aicore__ inline uint64_t CalcFloorAlign(uint64_t a, uint32_t b)
{
    if (b >= a) {
        return b;
    } else if (b == 1) {
        return a;
    } else {
        return a / b * b;
    }
}

static __aicore__ inline uint64_t CalcRemainder(uint64_t a, uint32_t b)
{
    if (b > a) {
        return a;
    } else if (b == 1 || b == a) {
        return 0;
    } else {
        return a % b;
    }
}

const uint32_t STEP_2 = 2;

static __aicore__ inline uint64_t Ceil(uint64_t a, uint32_t b)
{
#ifdef ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT(b != 0, { KERNEL_LOG(KERNEL_ERROR, "Division by zero error"); });
#endif
    return (a + b - 1) / b;
}

static __aicore__ inline uint64_t DivStepM(uint64_t a, uint32_t stepM)
{
    if (stepM == 1) {
        return a;
    } else if (stepM == STEP_2) {
        return a >> 1;
    } else {
        return a / stepM;
    }
}

static __aicore__ inline uint64_t CeilStepM(uint64_t a, uint32_t stepM)
{
    if (stepM == 1) {
        return a;
    } else if (stepM == STEP_2) {
        return (a + 1) >> 1;
    } else {
        return (a + stepM - 1) / stepM;
    }
}

static __aicore__ inline uint64_t RemainderStepM(uint64_t a, uint32_t stepM)
{
    if (stepM == 1) {
        return 0;
    } else if (stepM == STEP_2) {
        return a & 1;
    } else {
        return a % stepM;
    }
}

static __aicore__ inline uint64_t DivStepN(uint64_t a, uint32_t stepN)
{
    if (stepN == 1) {
        return a;
    } else if (stepN == STEP_2) {
        return a >> 1;
    } else {
        return a / stepN;
    }
}

static __aicore__ inline uint64_t CeilStepN(uint64_t a, uint32_t stepN)
{
    if (stepN == 1) {
        return a;
    } else if (stepN == STEP_2) {
        return (a + 1) >> 1;
    } else {
        return (a + stepN - 1) / stepN;
    }
}

static __aicore__ inline uint64_t RemainderStepN(uint64_t a, uint32_t stepN)
{
    if (stepN == 1) {
        return 0;
    } else if (stepN == STEP_2) {
        return a & 1;
    } else {
        return a % stepN;
    }
}

static __aicore__ inline uint64_t DivHkWk(uint64_t a, uint32_t hkWk)
{
    if (hkWk > a) {
        return 0;
    } else if (hkWk == 1) {
        return a;
    } else {
        return a / hkWk;
    }
}

static __aicore__ inline uint64_t CeilHkWk(uint64_t a, uint32_t hkWk)
{
    if (hkWk > a) {
        return 1;
    } else if (hkWk == 1) {
        return a;
    } else {
        return Ceil(a, hkWk);
    }
}

static __aicore__ inline uint64_t RemainderOfHkWk(uint64_t a, uint32_t hkWk)
{
    if (hkWk > a) {
        return a;
    } else if (hkWk == 1) {
        return 0;
    } else {
        return a % hkWk;
    }
}

// only use for div channelSize
template <class Intf>
static __aicore__ inline uint64_t ShiftDivChannelSize(uint64_t a, uint32_t channelSize)
{
    if constexpr (AscendC::IsSameType<typename Intf::SrcT, float>::value) {
        return a >> 3; // 3: bit
    } else if constexpr (AscendC::IsSameType<typename Intf::SrcT, half>::value) {
        return a >> 4; // 4: bit
    } else if constexpr (AscendC::IsSameType<typename Intf::SrcT, bfloat16_t>::value) {
        return a >> 4; // 4: bit
    } else {
        return a >> 4; // 4: bit
    }
}

template <class Intf>
static __aicore__ inline uint64_t ShiftCeilChannelSize(uint64_t a, uint32_t channelSize)
{
    if constexpr (AscendC::IsSameType<typename Intf::SrcT, float>::value) {
        return (a + 7) >> 3; // (a + channelSize - 1) / channelSize
    } else if constexpr (AscendC::IsSameType<typename Intf::SrcT, half>::value) {
        return (a + 15) >> 4; // (a + channelSize - 1) / channelSize
    } else if constexpr (AscendC::IsSameType<typename Intf::SrcT, bfloat16_t>::value) {
        return (a + 15) >> 4; // (a + channelSize - 1) / channelSize
    }  else {
        return (a + 15) >> 4; // (a + channelSize - 1) / channelSize
    }
}

static __aicore__ inline uint64_t ShiftDivM0(uint64_t a, uint32_t m0)
{
    return a >> 4; // 4: bit
}

static __aicore__ inline uint64_t ShiftCeilM0(uint64_t a, uint32_t m0)
{
    return (a + 15) >> 4; // 4: bit
}

static __aicore__ inline uint32_t CalRows2Copy(uint32_t copySize, uint32_t width)
{
    uint32_t rows = 1;
    //Determine the scene according to the hit rate, and deal with the multi-line handling scene first.
    if (copySize > width) {
        rows = AscendC::Ceil(copySize, width);
        if (copySize == rows * width) {
            return rows; // Integer division returns directly. When not integer division, Ceil defaults to moving one more line
        } else if ((2 * copySize) % width != 0) {
            return rows + 1; // Unless the tail block is 0.5 lines, you will need to move one more line when trailing up and down
        }
        return rows;
    } else if (copySize == width) {
        return rows;
    } else {
        if (width % copySize != 0) {
            return rows + 1;
        }
        return rows; // When the width is an integer multiple of size, there is no trailing, just move it to one line, otherwise move it to two lines
    }
}

// The default overloaded function of the call function defined in the API class supports any number of parameters of any type
#define DECLARE_DEFAULT_OVERLOADING_FUN(T, NAMESPACE)                       \
    template <class... Ts>                                                  \
    static __aicore__ inline NAMESPACE::TypeFalse call(T *self, Ts... args) \
    {                                                                       \
        return ((NAMESPACE::TypeFalse){0});                                   \
    }

// Check whether there is a call(...) member function in class T
#define CHECK_FUN(T, NAMESPACE, ...) (!AscendC::IsSameType<decltype(T::call(__VA_ARGS__)), NAMESPACE::TypeFalse>::value)

/*
Define a verification template class to determine whether type T has the template member function MEMBER<U>
Used with macro DECLARE_IMPLï¼ŒCalling method_has_impl_MEMBER<T, U>::value
Line 49: decltype gets the type of expression. declval is a template function that gets the rvalue reference of template parameter T. If T does not have MEMBER<U>, an error will be reported, otherwise TrueType will be returned
*/
#define DECLARE_CHECK_IMPL(NAMESPACE, MEMBER, args...)                                                                     \
    namespace __##NAMESPACE {                                                                                  \
    template <typename T, typename U>                                                                           \
    struct _has_impl_##MEMBER {                                                                                 \
        template <typename C>                                                                                   \
        static auto check(int) -> decltype(std::declval<typename C::template MEMBER<U, ##args>>(), AscendC::TrueType()); \
        template <typename C>                                                                                   \
        static AscendC::FalseType check(...);                                                                            \
        static constexpr bool value = AscendC::IsSameType<decltype(check<T>(0)), AscendC::TrueType>::value;                       \
    };                                                                                                          \
    }

// Define a template class to determine whether type T has a template member function MEMBER<typename U, bool sync>
#define DECLARE_CHECK_SYNC_IMPL(NAMESPACE, MEMBER, args...)                                                                      \
    namespace __##NAMESPACE {                                                                                        \
    template <typename T, typename U, bool sync>                                                                      \
    struct _has_impl_##MEMBER {                                                                                       \
        template <typename C>                                                                                         \
        static auto check(int) -> decltype(std::declval<typename C::template MEMBER<U, sync, ##args>>(), AscendC::TrueType()); \
        template <typename C>                                                                                         \
        static AscendC::FalseType check(...);                                                                                  \
        static constexpr bool value = AscendC::IsSameType<decltype(check<T>(0)), AscendC::TrueType>::value;                             \
    };                                                                                                                \
    }

// Define the member function MEMBER<U>. If the MEMBER member exists in Config, the MEMBER function points to the Config member, otherwise it points to Current::Init
#define DECLARE_IMPL(NAMESPACE, Config, Current, MEMBER, U)     \
    template <bool Default, class T>                 \
    struct __##MEMBER {                              \
        using Type = typename Current::MEMBER<U>;    \
    };                                               \
    template <class T>                               \
    struct __##MEMBER<true, T> {                     \
        using Type = typename T::template MEMBER<U>; \
    };                                               \
    using MEMBER = typename __##MEMBER<__##NAMESPACE::_has_impl_##MEMBER<Config, U>::value, Config>::Type

// Define the member function MEMBER<U, sync>. If the MEMBER member exists in Config, the MEMBER function points to the Config member, otherwise it points to Current::Init
#define DECLARE_SYNC_IMPL(NAMESPACE, Config, Current, MEMBER, U)      \
    template <bool Default, class T, bool sync>            \
    struct __##MEMBER {                                    \
        using Type = typename Current::MEMBER<U, sync>;    \
    };                                                     \
    template <class T, bool sync>                          \
    struct __##MEMBER<true, T, sync> {                     \
        using Type = typename T::template MEMBER<U, sync>; \
    };                                                     \
    template <bool sync>                                   \
    using MEMBER = typename __##MEMBER<__##NAMESPACE::_has_impl_##MEMBER<Config, U, sync>::value, Config, sync>::Type

/*
Define a template class to determine whether type T has member MEMBER
Used in conjunction with macro CHECK_MEMBER, calling method _has_member_MEMBER<T>::value
*/
#define DECLARE_CHECK_MEMBER(MEMBER)                                                        \
    namespace __AuxCheck {                                                                  \
    template <typename T>                                                                   \
    struct _has_member_##MEMBER {                                                           \
        template <typename U>                                                               \
        static void check((decltype(&U::MEMBER)));                                            \
        template <typename U>                                                               \
        static int check(...);                                                              \
        static constexpr bool value = IsSameType<decltype(check<T>(nullptr)), void>::value; \
    };                                                                                      \
    }

// Check whether there is a member variable MEMBER in class OBJ
#define CHECK_MEMBER(OBJ, MEMBER) (__AuxCheck::_has_member_##MEMBER<typename OBJ>::value)

/*
Define two auxiliary template classes, one member M is a variable, and the other member M is a constant;
At the same time, a verification template function is defined. The function returns the corresponding template class based on whether the template parameter T has a constant and a member M with a value > 0.
Used with macro DEFINE_STUCTï¼?
*/
#define DECLARE_DEFINE_STRUCT(T, M, U)                                                                               \
    namespace __AuxTiling {                                                                                          \
    template <typename TT>                                                                                           \
    struct T##_##M {                                                                                                 \
        U M;                                                                                                         \
        constexpr static bool __CONST_TYPE_##M = false;                                                              \
    };                                                                                                               \
    template <typename TT>                                                                                           \
    struct T##_CT_##M {                                                                                              \
        constexpr static U M = (TT::M);                                                                                \
        constexpr static bool __CONST_TYPE_##M = true;                                                               \
    };                                                                                                               \
    template <typename TT>                                                                                           \
    constexpr bool _is_const_##T##_##M()                                                                             \
    {                                                                                                                \
        return (TT::M) > 0;                                                                                            \
    };                                                                                                               \
    template <typename TT>                                                                                           \
    typename std::conditional<_is_const_##T##_##M<TT>(), T##_CT_##M<TT>, T##_##M<TT>>::type T##_##M##_checkdefine(); \
    }

// For class inheritance, returns a parent class type for inheritance
#define DEFINE_STUCT(T, M) \
public                     \
    decltype(__AuxTiling::T##_##M##_checkdefine<T>())

#define DEFINE_STUCT_FIELD(T, FIELD) \
    T FIELD;                         \
    constexpr static bool __CONST_TYPE_##FIELD = false

#define CHECK_CONST(T, M) ((T::__CONST_TYPE_##M))

#define DEFINE_STUCT_TEMPLATE_FIELD(T, FIELD, C, ...) \
    T<C, ##__VA_ARGS__> FIELD;                        \
    constexpr static bool __CONST_TYPE_##FIELD = false

#define DEFINE_STUCT_ARRAY_FIELD(T, FIELD, NUM) \
    T FIELD[NUM];                               \
    constexpr static bool __CONST_TYPE_##FIELD = false
}

#endif // CONV3D_BP_UTIL_H
