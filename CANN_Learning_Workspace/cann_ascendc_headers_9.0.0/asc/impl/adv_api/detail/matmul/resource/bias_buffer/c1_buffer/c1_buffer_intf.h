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
* \file c1_buffer_intf.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/resource/bias_buffer/c1_buffer/c1_buffer_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_INTF_H__
#endif

#ifndef IMPL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_INTF_H
#define IMPL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_INTF_H

#include "../../../utils/matmul_module.h"
#include "../../../utils/matmul_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {

/**
 * C1Buffer: responsible for L1 bias buffer management.
 * This module provides abilities to allocate or free one l1 bias buffer block, and pipeline synchronization.
 * We retain the freedom to make incompatible changes, but do not guarantee the stability.
 * CopyBiasIn is only for internal usage, does not support extension or customized specialization!
 */
template <typename IMPL, class BIAS_TYPE, class A_TYPE, const auto& MM_CFG, typename = void>
class C1Buffer {
    using BiasT = typename BIAS_TYPE::T;
public:
    __aicore__ inline C1Buffer() {}
    __aicore__ inline ~C1Buffer() {}
    /**
     * @description: Init of buffer, should be called when matmul is inited.
     * @param: biasLen: element nums of bias when loading to L1
     * @return: void
     */
    __aicore__ inline void Init(int32_t biasLen) {}
    /**
     * @description: Allocate one block of buffer, should be called only when load bias
     * @param: void
     * @return: null tensor on l1
     */
    __aicore__ inline LocalTensor<BiasT> AllocTensor()
    {
        return LocalTensor<BiasT>{};
    }
    /**
     * @description: Free tensor, should be called after AllocTensor
     * @param: tensor: tensor allocated by AllocTensor or null tensor
     * @return: void
     */
    __aicore__ inline void FreeTensor(const LocalTensor<BiasT>& tensor = LocalTensor<BiasT>{}) {}
    /**
     * @description: Put tensor to buffer que
     * @param: tensor: target tensor on L1
     * @return: void
     */
    __aicore__ inline void EnQue(LocalTensor<BiasT>& tensor) {}
    /**
     * @description: Fetch tensor from buffer que
     * @param: void
     * @return: bias tensor on L1
     */
    __aicore__ inline LocalTensor<BiasT> DeQue()
    {
        return LocalTensor<BiasT>{};
    }
    /**
     * @description: Reset all should be called when matmul end
     * @param: void
     * @return: void
     */
    __aicore__ inline void Destroy() {}

    /**
     * @description: Get buffer head address
     * @param: void
     * @return: Buffer head address
     */
    __aicore__ inline uint64_t GetBufferHeadAddr()
    {
        return 0;
    }
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C1_BUFFER_C1_BUFFER_INTF_H__
#endif // _C1_BUFFER_INTF_H_