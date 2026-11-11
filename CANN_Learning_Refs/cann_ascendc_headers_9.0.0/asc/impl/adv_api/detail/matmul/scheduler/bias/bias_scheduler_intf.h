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
* \file bias_scheduler_intf.h
* \brief
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/bias/bias_scheduler_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_INTF_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_INTF_H
#define IMPL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_INTF_H

namespace AscendC {
namespace Impl {
namespace Detail {

/**
 * BiasScheduler: responsible for bias management module.
 * This module provides abilities to copy bias to C2 or L0C.
 * We retain the freedom to make incompatible changes, but do not guarantee the stability.
 * BiasScheduler is only for internal usage, does not support extension or customized specialization!
 */
template <typename IMPL, class A_TYPE, class B_TYPE, class BIAS_TYPE, const auto &MM_CFG, typename = void>
class BiasScheduler {
    using BiasT = typename BIAS_TYPE::T;
    using TensorT = typename Conditional<(PhyPosIsGM(BIAS_TYPE::pos) || (PhyPosIsUB(BIAS_TYPE::pos) && !MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1()
        && !MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape())), GlobalTensor<BiasT>, LocalTensor<BiasT>>::type;

public:
    __aicore__ inline BiasScheduler() = default;
    __aicore__ inline ~BiasScheduler() = default;
    /**
     * @description: Set bias flag to true or false
     * @param: enableBias: Whether to use bias
     * @return: void
     */
    __aicore__ inline void SetBias(bool enableBias = false) {}
    /**
     * @description: Get bias flag
     * @return: bool true: bias is enabled, false: bias is disabled
     */
    __aicore__ inline bool IsBias() const
    {
        return false;
    }
    /**
     * @description: Get bias flag
     * @param: srcTensor: tensor of source data
     * @return: void
     */
    __aicore__ inline void SetInput(const TensorT& srcTensor) {}
    /**
     * @description: Initialize bias buffer
     * @param: batchNum: Number of batch, only batchmatmul need this param
     * @return: void
     */
    __aicore__ inline void Init(int32_t batchNum = 0) {}
    /**
     * @description: Free bias buffer at iterate end
     * @return: void
     */
    __aicore__ inline void End() {}
    /**
     * @description: Copy bias data in
     * @param: dataLen: Length of bias blocks
     * @param: dataNum: Number of bias blocks, only batchmatmul need this param
     * @param: srcOffset: position offset of source data
     * @return: Tensor on L1
     */
    __aicore__ inline LocalTensor<BiasT> CopyIn(int32_t dataLen, int32_t dataNum = 1, int32_t srcOffset = 0)
    {
        LocalTensor<BiasT> bias;
        return bias;
    }
    /**
     * @description: Release bias tensor on c1 at the end of K inner loop
     * @return: void
     */
    __aicore__ inline void Free(LocalTensor<BiasT> &biasC1) {}
    /**
     * @description: Split and load bias data from C1 to C2
     * @param: bias: src tensor from l1
     * @param: dataLen: block length of bias need to load
     * @param: srcOffset: position offset of source data
     * @return: void
     */
    __aicore__ inline void SplitLoad(LocalTensor<BiasT> &biasC1, int32_t dataLen = 0, int32_t srcOffset = 0) {}
    /**
     * @description: Release bias tensor on c2 at compute end
     * @return: void
     */
    __aicore__ inline void Free() {}
    /**
     * @description: Get bias flag, only single batch matmul need this api
     * @param: offset: source bias offset of batch idx
     * @return: void
     */
    __aicore__ inline void SetSingleOffset(int32_t offset = 0) {}

    __aicore__ inline void Destroy(LocalTensor<BiasT>& bias = LocalTensor<BiasT>{}) {}

    __aicore__ inline void StopBias(LocalTensor<BiasT> &bias) {}
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _BIAS_SCHEDULER_INTF_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_INTF_H__
#endif