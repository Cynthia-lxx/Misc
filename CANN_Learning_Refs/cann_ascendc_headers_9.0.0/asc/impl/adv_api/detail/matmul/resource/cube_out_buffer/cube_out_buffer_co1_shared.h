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
* \file cube_out_buffer_co1_shared.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/resource/cube_out_buffer/cube_out_buffer_co1_shared.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_OUT_BUFFER_CUBE_OUT_BUFFER_CO1_SHARED_H__
#endif

#ifndef IMPL_MATMUL_MODULES_RESOURCE_CUBE_OUT_BUFFER_CUBE_OUT_BUFFER_CO1_SHARED_H
#define IMPL_MATMUL_MODULES_RESOURCE_CUBE_OUT_BUFFER_CUBE_OUT_BUFFER_CO1_SHARED_H

#include "cube_out_buffer_base.h"
#include "../../utils/matmul_utils.h"
#include "include/adv_api/matmul/tiling.h"
#include "../../feature_trait/matmul_feature_trait.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    CubeOutBuffer is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    CubeOutBuffer is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, typename A_TYPE, typename B_TYPE, typename L0cT, class C_TYPE, const auto &MM_CFG>
class CubeOutBuffer<IMPL, A_TYPE, B_TYPE, L0cT, C_TYPE, MM_CFG, enable_if_t<ToMatmulConfig(MM_CFG).isCO1Shared &&
    !PhyPosIsL0C(C_TYPE::pos)>>
{
public:
    __aicore__ inline CubeOutBuffer() {};
    __aicore__ inline ~CubeOutBuffer() {};
    __aicore__ inline void Init(int32_t cacheSize = 1, uint32_t lenFactor = 1) {
        GetTPipePtr()->InitBuffer(*static_cast<gCO1QueType*>(gCO1Que), L0C_BUF_BLOCK_NUM, L0C_BUF_BLOCK_LEN);
    };

    __aicore__ inline LocalTensor<L0cT> AllocTensor()
    {
        cMatrix_ = (static_cast<gCO1QueType*>(gCO1Que))->AllocTensor<L0cT>(Ceil(ToMatmulConfig(MM_CFG).sharedCO1BufferSize, L0C_BUF_BLOCK_LEN));
        if constexpr (ToMatmulConfig(MM_CFG).iterateMode != IterateMode::ITERATE_MODE_ALL) {
            co1UsedList[co1UsedNumber++] = cMatrix_.GetBufferHandle();
        }
        return cMatrix_;
    }

    __aicore__ inline LocalTensor<L0cT> GetTensor()
    { 
        if constexpr (ToMatmulConfig(MM_CFG).iterateMode != IterateMode::ITERATE_MODE_ALL) {
            TBuffAddr addr = (static_cast<gCO1QueType*>(gCO1Que))->GetBufferAddr(co1UsedList[0]);
            cMatrix_.SetAddr(addr);
        }
        return cMatrix_;
    }

    __aicore__ inline void EnQue(LocalTensor<L0cT>& tensor)
    {
        (static_cast<gCO1QueType*>(gCO1Que))->EnQue(tensor);
    }

    __aicore__ inline LocalTensor<L0cT> DeQue()
    {
        return (static_cast<gCO1QueType*>(gCO1Que))->DeQue<L0cT>();
    }

    __aicore__ inline void FreeTensor(LocalTensor<L0cT> &co1Local)
    {
        (static_cast<gCO1QueType*>(gCO1Que))->FreeTensor(co1Local);
        if constexpr (ToMatmulConfig(MM_CFG).iterateMode != IterateMode::ITERATE_MODE_ALL) {
            co1UsedNumber--;
            for (int i = 0; i < co1UsedNumber; i++) {
                co1UsedList[i] = co1UsedList[i + 1];
            }
        }
    }

    __aicore__ inline void Destroy()
    {}

private:
    LocalTensor<L0cT> cMatrix_;
    constexpr static int32_t MAX_CO1_BUFFER_NUM = 8;
    TBufHandle co1UsedList[MAX_CO1_BUFFER_NUM];
    int co1UsedNumber = 0;
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_MODULES_RESOURCE_CUBE_OUT_BUFFER_CUBE_OUT_BUFFER_UNIT_FLAG_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_OUT_BUFFER_CUBE_OUT_BUFFER_CO1_SHARED_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_OUT_BUFFER_CUBE_OUT_BUFFER_CO1_SHARED_H__
#endif