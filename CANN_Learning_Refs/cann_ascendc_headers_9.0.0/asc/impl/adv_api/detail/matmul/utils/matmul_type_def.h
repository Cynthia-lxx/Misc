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
* \file matmul_type_def.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/utils/matmul_type_def.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_TYPE_DEF_H__
#endif

#ifndef IMPL_MATMUL_UTILS_MATMUL_TYPE_DEF_H
#define IMPL_MATMUL_UTILS_MATMUL_TYPE_DEF_H

#include "include/adv_api/matmul/tiling.h"

namespace AscendC {

enum class InputTypeTag : uint8_t {
    A = 0,
    B = 1,
    C = 2,
    scaleA = 3,
    scaleB = 4,
};

template <TPosition POSITION, CubeFormat FORMAT, typename TYPE, bool ISTRANS = false,
          LayoutMode LAYOUT = LayoutMode::NONE, bool IBSHARE = false, TPosition SRCPOS = TPosition::GM>
struct MatmulType {
    constexpr static TPosition pos = POSITION;
    constexpr static CubeFormat format = FORMAT;
    using T = TYPE;
    constexpr static bool isTrans = ISTRANS;
    constexpr static LayoutMode layout = LAYOUT;
    constexpr static bool ibShare = IBSHARE;
    constexpr static TPosition srcPos = SRCPOS;
};

template <class INPUT_TYPE, typename TRANS_TYPE = typename INPUT_TYPE::T>
struct MatmulInputAType : INPUT_TYPE {
    using TRANS_T = TRANS_TYPE;
    constexpr static InputTypeTag TAG = InputTypeTag::A;
};

template <class INPUT_TYPE, typename TRANS_TYPE = typename INPUT_TYPE::T>
struct MatmulInputBType : INPUT_TYPE {
    using TRANS_T = TRANS_TYPE;
    constexpr static InputTypeTag TAG = InputTypeTag::B;
};

template <class INPUT_TYPE, typename TRANS_TYPE = typename INPUT_TYPE::T>
struct MatmulInputScaleAType : INPUT_TYPE {
    using TRANS_T = TRANS_TYPE;
    constexpr static InputTypeTag TAG = InputTypeTag::scaleA;
};

template <class INPUT_TYPE, typename TRANS_TYPE = typename INPUT_TYPE::T>
struct MatmulInputScaleBType : INPUT_TYPE {
    using TRANS_T = TRANS_TYPE;
    constexpr static InputTypeTag TAG = InputTypeTag::scaleB;
};

template <class INPUT_TYPE, typename TRANS_TYPE = typename INPUT_TYPE::T>
struct MatmulInputCType : INPUT_TYPE {
    using TRANS_T = TRANS_TYPE;
    constexpr static InputTypeTag TAG = InputTypeTag::C;
};

template <typename Type, Type valueIn>
struct ConstantType
{
    static constexpr Type value = valueIn;
    typedef Type value_type;
    typedef ConstantType<Type, valueIn> type;
    constexpr __aicore__ inline operator value_type() const noexcept {return value;}
};

typedef ConstantType<bool, false> falseType;
typedef ConstantType<bool, true> trueType;

template <typename...> using voidT = void;

template <typename INPUT_TYPE, typename = void>
struct HasSparseIndex : falseType {};

template <typename INPUT_TYPE>
struct HasSparseIndex<INPUT_TYPE, voidT<decltype(&INPUT_TYPE::indexPosition)>> : trueType {};

template <TPosition POSITION, TPosition INDEX_POSITION, CubeFormat FORMAT, typename TYPE, bool ISTRANS = false,
          LayoutMode LAYOUT = LayoutMode::NONE, bool IBSHARE = false>
struct SparseMatmulType: public MatmulType <POSITION, FORMAT, TYPE, ISTRANS, LAYOUT, IBSHARE> {
    constexpr static TPosition indexPosition = INDEX_POSITION;
};

template<typename INPUT_TYPE, typename = void>
struct HasScalePosition : falseType {};

template<typename INPUT_TYPE>
struct HasScalePosition<INPUT_TYPE, voidT<decltype(&INPUT_TYPE::scalePosition)>> : trueType {};

template <TPosition POSITION, TPosition SCALE_POSITION, CubeFormat FORMAT, typename TYPE, bool ISTRANS = false,
    TPosition SRCPOS = TPosition::GM, CubeFormat SCALE_FORMAT = FORMAT, bool SCALE_ISTRANS = ISTRANS, TPosition SCALE_SRCPOS = SRCPOS>
struct MatmulTypeWithScale : public MatmulType<POSITION, FORMAT, TYPE, ISTRANS, LayoutMode::NONE, false, SRCPOS> {
    constexpr static TPosition scalePosition = SCALE_POSITION;
    constexpr static CubeFormat scaleFormat = SCALE_FORMAT;
    constexpr static bool isScaleTrans = SCALE_ISTRANS;
    constexpr static TPosition srcScalePos = SCALE_SRCPOS;
};

template <typename A_TYPE, typename B_TYPE>
constexpr bool isMxMatmul = HasScalePosition<A_TYPE>::value && HasScalePosition<B_TYPE>::value;

}  // namespace AscendC
#endif // _MATMUL_TYPE_DEF_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_TYPE_DEF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_TYPE_DEF_H__
#endif