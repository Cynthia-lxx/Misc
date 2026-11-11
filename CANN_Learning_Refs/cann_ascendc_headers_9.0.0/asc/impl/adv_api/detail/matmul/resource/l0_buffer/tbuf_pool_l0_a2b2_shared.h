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
 * \file tbuf_pool_l0_a2b2_shared.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/resource/l0_buffer/tbuf_pool_l0_a2b2_shared.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_L0_BUFFER_TBUF_POOL_L0_A2B2_SHARED_H__
#endif

#ifndef IMPL_MATMUL_RESOURCE_L0_BUFFER_TBUF_POOL_L0_A2B2_SHARED_H
#define IMPL_MATMUL_RESOURCE_L0_BUFFER_TBUF_POOL_L0_A2B2_SHARED_H

#include "tbuf_pool_l0_intf.h"
#include "tbuf_pool_l0_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {

__BLOCK_LOCAL__ __inline__ uint64_t gA2B2DBFlag_;
/*
    TBufPoolL0 is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    TBufPoolL0 is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, typename A_TYPE, typename B_TYPE, const auto& MM_CFG>
class TBufPoolL0<IMPL, A_TYPE, B_TYPE, MM_CFG,
    enable_if_t<!MatmulFeatureTrait<MM_CFG>::IsNeedUB() && (DoMatmulMDL(MM_CFG) ||
        isNormEnableScheduler<A_TYPE, MM_CFG> || DoMatmulIBShareNorm(MM_CFG)) &&
        IsA2B2Shared(MM_CFG)>>
    : public TBufPoolL0Base<IMPL, A_TYPE, B_TYPE, MM_CFG>
{
public:
    using BASE_MODULE = AscendC::Impl::Detail::TBufPoolL0Base<IMPL, A_TYPE, B_TYPE, MM_CFG>;
    __aicore__ inline TBufPoolL0() = default;
    __aicore__ inline ~TBufPoolL0() = default;

    __aicore__ inline TBufPoolL0& Allocate() {
        WaitFlag<HardEvent::M_MTE1>(gA2B2DBFlag_ & 0x1);
        return *this;
    }

    template <TPosition Pos, typename T>
    __aicore__ inline LocalTensor<T> GetBuffer()
    {
        constexpr int dbSize = 2;
        LocalTensor<T> tempTensor;
        if constexpr (Pos == TPosition::A2) {
            tempTensor = BASE_MODULE::l0aBuf_.template Get<T>();
            if ((gA2B2DBFlag_ & 0x1) != 0) {
                if constexpr (IsSameType<T, int4b_t>::value) {
                    tempTensor = tempTensor[L0AUF_SIZE / sizeof(T)];
                } else {
                    tempTensor = tempTensor[L0AUF_SIZE / dbSize / sizeof(T)];
                }
            }
        } else {
            tempTensor = BASE_MODULE::l0bBuf_.template Get<T>();
            if ((gA2B2DBFlag_ & 0x1) != 0) {
                if constexpr (IsSameType<T, int4b_t>::value) {
                    tempTensor = tempTensor[L0BUF_SIZE / sizeof(T)];
                } else {
                    tempTensor = tempTensor[L0BUF_SIZE / dbSize / sizeof(T)];
                }
            }
        }
        return tempTensor;
    }

    __aicore__ inline void EnQue()
    {
        SetFlag<HardEvent::MTE1_M>(gA2B2DBFlag_ & 0x1);
    }

    __aicore__ inline void DeQue()
    {
        WaitFlag<HardEvent::MTE1_M>(gA2B2DBFlag_ & 0x1);
    }

    __aicore__ inline void Free()
    {
        SetFlag<HardEvent::M_MTE1>(gA2B2DBFlag_ & 0x1);
        ++gA2B2DBFlag_;
    }
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_RESOURCE_L0_BUFFER_TBUF_POOL_L0_A2B2_SHARED_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_L0_BUFFER_TBUF_POOL_L0_A2B2_SHARED_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_L0_BUFFER_TBUF_POOL_L0_A2B2_SHARED_H__
#endif