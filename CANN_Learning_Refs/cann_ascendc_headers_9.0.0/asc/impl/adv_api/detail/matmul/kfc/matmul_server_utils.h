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
 * \file matmul_server_utils.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/kfc/matmul_server_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul_client.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_UTILS_H__
#endif

#ifndef IMPL_MATMUL_KFC_MATMUL_SERVER_UTILS_H
#define IMPL_MATMUL_KFC_MATMUL_SERVER_UTILS_H

#include "include/adv_api/matmul/matmul.h"
#include "kernel_basic_intf.h"

namespace AscendC {

template <bool IS_IBSHARE> struct IBShareCache {
    __aicore__ inline IBShareCache() {};
};

template <>
struct IBShareCache<false> {
    __aicore__ inline IBShareCache() {};
    using ShareCache = uint16_t;
};

template <>
struct IBShareCache<true> {
    __aicore__ inline IBShareCache() {};
    using ShareCache = Impl::Detail::GlobalCache;
};
template <class A_TYPE, class B_TYPE> __aicore__ constexpr bool IsIBShare()
{
    if constexpr (A_TYPE::ibShare == true) {
        return true;
    }
    if constexpr (B_TYPE::ibShare == true) {
        return true;
    }
    return false;
}

struct MatmulMsg {
    uint32_t setOrgShape : 1;
    uint32_t orgM;
    uint32_t orgN;
    uint32_t orgKa;
    uint32_t orgKb;
    uint32_t orgKc;
};

struct ShareMatmulBase {
    __aicore__ inline ShareMatmulBase() {};
};

struct ShareMatmul : ShareMatmulBase {
    __aicore__ inline ShareMatmul(){};
    MatmulMsg msg0;
    MatmulMsg msg1;
};

template <bool SHARED>
struct ShareMatmulAux {
    __aicore__ inline ShareMatmulAux(){};
};

template <>
struct ShareMatmulAux<false> {
    __aicore__ inline ShareMatmulAux(){};
    using MSG = ShareMatmulBase;
};

template <>
struct ShareMatmulAux<true> {
    __aicore__ inline ShareMatmulAux(){};
    using MSG = ShareMatmul;
};

template <const auto& MM_CFG = CFG_NORM>
__aicore__ inline constexpr bool IsSharedMatmul()
{
    if constexpr (!AscendC::ToMatmulConfig(MM_CFG).enableInit ||
        AscendC::ToMatmulConfig(MM_CFG).enableMixDualMaster) {
        return true;
    }
    return false;
}

__aicore__ inline constexpr bool NeedTransitByGm(TPosition tPos)
{
#if defined(USE_SSBUF)
    // supported UB->L1, no need through gm
    if (PhyPosIsUB(tPos)) {
        return false;
    }
#else
    if (PhyPosIsUB(tPos)) {
        return true;
    }
#endif
    if (PhyPosIsL1(tPos)) {
        return false;
    }
    if (PhyPosIsGM(tPos)) {
        return true;
    }
    return true;
}

template <const auto& MM_CFG = CFG_NORM>
__aicore__ inline uint64_t GetBaseOffsetC(bool enSequentialWrite, int32_t baseM, int32_t baseN)
{
    if constexpr (AscendC::ToMatmulConfig(MM_CFG).baseMN != 0) {
        return (enSequentialWrite ? AscendC::ToMatmulConfig(MM_CFG).baseMN : 0);
    } else {
        return (enSequentialWrite ? (baseM * baseN) : 0);
    }
}

#if defined(USE_SSBUF)
// c310 msg stored in mmserver obj
#define MsgTmpPos
#else
// c220 msg stored in gm
#define MsgTmpPos __gm__
#endif

} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_UTILS_H__
#endif // _MATMUL_SERVER_H_