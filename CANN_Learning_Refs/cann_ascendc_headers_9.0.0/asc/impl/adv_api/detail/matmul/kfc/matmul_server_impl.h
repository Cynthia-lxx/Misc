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
 * \file matmul_server_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/kfc/matmul_server_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul_client.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_IMPL_H__
#endif

#ifndef IMPL_MATMUL_KFC_MATMUL_SERVER_IMPL_H
#define IMPL_MATMUL_KFC_MATMUL_SERVER_IMPL_H

#include "matmul_server.h"

namespace AscendC {
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SetTensorA(MsgTmpPos MatmulConfigParams* body)
{
#if defined(USE_WORKSPACE)
    if (!body->setTensorA)
        return;
#endif
    if constexpr (A_TYPE::format == CubeFormat::SCALAR) {
        SrcAT scalar;
        auto temp1 = reinterpret_cast<MsgTmpPos uint8_t*>(&(body->aAddr));
        auto temp2 = (uint8_t*)&scalar;

        for (int i = 0; i < sizeof(SrcAT); i++, temp1++, temp2++) {
            *temp2 = *temp1;
        }
        mul.SetTensorA(scalar);
        return;
    }
    uint64_t size = 0;
#if defined(USE_WORKSPACE)
    size = (uint64_t)(body->sizeAmatrix); // not defined in C310
#endif
    if constexpr (NeedTransitByGm(A_TYPE::pos)) {
        GlobalTensor<SrcAT> aGlobal;
        aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ SrcAT*>(body->aAddr), size);
        mul.SetTensorA(aGlobal, body->isTransA);
    } else {
#if defined(USE_SSBUF)
        // use addr to send intraId in C310
        if constexpr (PhyPosIsL1(A_TYPE::pos)) {
            KfcSetIntraAId(mul, body->aAddr >> VALID_ADDR_BITS_NUM);
        }
#endif
        const auto& aLocal = GetLocalTensor<typename A_TYPE::T, A_TYPE::pos>(body->aAddr, size);
        mul.SetTensorA(aLocal, body->isTransA);
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SetTensorA(
    MsgTmpPos MatmulConfigParams* body, const uint64_t size, const uint64_t offset)
{
    if (!body->setTensorA) {
        return;
    }
    if constexpr (A_TYPE::format == CubeFormat::SCALAR) {
        SrcAT scalar;
        auto temp1 = reinterpret_cast<MsgTmpPos uint8_t*>(&(body->aAddr) + offset);
        auto temp2 = (uint8_t*)&scalar;

        for (int i = 0; i < sizeof(SrcAT); i++, temp1++, temp2++) {
            *temp2 = *temp1;
        }
        mul.SetTensorA(scalar);
        return;
    }
    if constexpr (PhyPosIsL1(A_TYPE::pos)) {
        const auto& aLocal = GetLocalTensor<typename A_TYPE::T, A_TYPE::pos>(body->aAddr + offset, size);
        mul.SetTensorA(aLocal, body->isTransA);
    } else {
        GlobalTensor<SrcAT> aGlobal;
        aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ SrcAT*>(body->aAddr + offset), size);
        mul.SetTensorA(aGlobal, body->isTransA);
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SetTensorB(MsgTmpPos MatmulConfigParams* body)
{
#if defined(USE_WORKSPACE)
    if (!body->setTensorB)
        return;
#endif
    if constexpr (B_TYPE::format == CubeFormat::SCALAR) {
        SrcBT scalar;
        auto temp1 = reinterpret_cast<MsgTmpPos uint8_t*>(&(body->bAddr));
        auto temp2 = (uint8_t*)&scalar;

        for (int i = 0; i < sizeof(SrcBT); i++, temp1++, temp2++) {
            *temp2 = *temp1;
        }
        mul.SetTensorB(scalar);
        return;
    }
    uint64_t size = 0;
#if defined(USE_WORKSPACE)
    size = (uint64_t)(body->sizeBmatrix); // not defined in C310
#endif
    if constexpr (NeedTransitByGm(B_TYPE::pos)) {
        GlobalTensor<SrcBT> bGlobal;
        bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ SrcBT*>(body->bAddr), size);
        mul.SetTensorB(bGlobal, body->isTransB);
    } else {
#if defined(USE_SSBUF)
        // use addr to send intraId in C310
        if constexpr (PhyPosIsL1(B_TYPE::pos)) {
            KfcSetIntraBId(mul, body->bAddr >> VALID_ADDR_BITS_NUM);
        }
#endif
        const auto& bLocal = GetLocalTensor<typename B_TYPE::T, B_TYPE::pos>(body->bAddr, size);
        mul.SetTensorB(bLocal, body->isTransB);
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SetTensorB(
    MsgTmpPos MatmulConfigParams* body, const uint64_t size, const uint64_t offset)
{
    if (!body->setTensorB) {
        return;
    }
    if constexpr (B_TYPE::format == CubeFormat::SCALAR) {
        SrcBT scalar;
        auto temp1 = reinterpret_cast<MsgTmpPos uint8_t*>(&(body->bAddr) + offset);
        auto temp2 = (uint8_t*)&scalar;

        for (int i = 0; i < sizeof(SrcBT); i++, temp1++, temp2++) {
            *temp2 = *temp1;
        }
        mul.SetTensorB(scalar);
        return;
    }
    if constexpr (PhyPosIsL1(B_TYPE::pos)) {
        const auto& bLocal = GetLocalTensor<typename B_TYPE::T, B_TYPE::pos>(body->bAddr + offset, size);
        mul.SetTensorB(bLocal, body->isTransB);
    } else {
        GlobalTensor<SrcBT> bGlobal;
        bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ SrcBT*>(body->bAddr + offset), size);
        mul.SetTensorB(bGlobal, body->isTransB);
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SetBias(MsgTmpPos MatmulConfigParams* body)
{
    if (body->setTensorBias) {
        const uint64_t size = (uint64_t)tiling_.GetSingleCoreN();
        if constexpr (NeedTransitByGm(BIAS_TYPE::pos)) {
            GlobalTensor<typename BIAS_TYPE::T> biasGlobal;
            biasGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ typename BIAS_TYPE::T*>(body->biasAddr), size);
            mul.SetBias(biasGlobal);
        } else {
            const auto& biasLocal = GetLocalTensor<typename BIAS_TYPE::T, BIAS_TYPE::pos>(body->biasAddr, size);
            mul.SetBias(biasLocal);
        }
    } else {
        mul.DisableBias();
    }
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
__aicore__ inline void MatmulService<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>::SetBias(
    MsgTmpPos MatmulConfigParams* body, const uint64_t offset)
{
    if (body->setTensorBias) {
        const uint64_t size = (uint64_t)tiling_.GetSingleCoreN();
        if constexpr (PhyPosIsL1(BIAS_TYPE::pos)) {
            const auto& biasLocal = GetLocalTensor<typename BIAS_TYPE::T, BIAS_TYPE::pos>(body->biasAddr + offset, size);
            mul.SetBias(biasLocal);
        } else {
            GlobalTensor<typename BIAS_TYPE::T> biasGlobal;
            biasGlobal.SetGlobalBuffer(
                reinterpret_cast<__gm__ typename BIAS_TYPE::T*>(body->biasAddr + offset), size);
            mul.SetBias(biasGlobal);
        }
    } else {
        mul.DisableBias();
    }
}
} // namespace AscendC
#endif // __MATMUL_SERVER_IMPL_H__

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_KFC_MATMUL_SERVER_IMPL_H__
#endif