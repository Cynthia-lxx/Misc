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
* \file l1_manager.h
* \brief l1 address manager
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/resource/l1_manager/l1_manager.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_L1_MANAGER_L1_MANAGER_H__
#endif

#ifndef IMPL_MATMUL_RESOURCE_L1_MANAGER_L1_MANAGER_H
#define IMPL_MATMUL_RESOURCE_L1_MANAGER_L1_MANAGER_H

namespace AscendC {
namespace Impl {
namespace Detail {

// only support 310
constexpr uint32_t L1SIZE = 512 * 1024;
constexpr uint16_t L1_SPLIT_NUM = 2;

template <typename IMPL, const auto& MM_CFG, typename = void>
class L1Manager
{
public:
    __aicore__ inline L1Manager() = default;
    __aicore__ inline ~L1Manager() = default;
};

template <typename IMPL, const auto& MM_CFG>
class L1Manager<IMPL, MM_CFG, enable_if_t<(ToMatmulConfig(MM_CFG).enableL1BankConflictOptimise)>>
{
public :
    __aicore__ inline L1Manager() = default;
    __aicore__ inline ~L1Manager() = default;

    __aicore__ inline uint32_t GetCurrentUpperHalfAddr() const
    {
        return curL1UpperHalfAddr_;
    }

    __aicore__ inline uint32_t GetCurrentLowerHalfAddr() const
    {
        return curL1LowerHalfAddr_;
    }

    __aicore__ inline void SetCurrentAddress(uint32_t upperHalfSize, uint32_t lowerHalfSize = 0) 
    {
        curL1UpperHalfAddr_ += upperHalfSize;
        curL1LowerHalfAddr_ += lowerHalfSize;
        ASCENDC_ASSERT((!CheckBufferOverflow()), { KERNEL_LOG(KERNEL_ERROR, "Larger than upper or lower half of L1 buffer size."); });
    }

private:
    __aicore__ inline bool CheckBufferOverflow()
    {
        if (curL1UpperHalfAddr_ > (L1SIZE / L1_SPLIT_NUM) || curL1LowerHalfAddr_ > L1SIZE) {
            return true;
        }
        return false;
    }

    uint32_t curL1UpperHalfAddr_ = 0;
    uint32_t curL1LowerHalfAddr_ = L1SIZE / L1_SPLIT_NUM;
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_L1_MANAGER_L1_MANAGER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_L1_MANAGER_L1_MANAGER_H__
#endif // IMPL_MATMUL_RESOURCE_L1_MANAGER_L1_MANAGER_H