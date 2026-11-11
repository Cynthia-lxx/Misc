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
* \file c2_buffer_intf.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/resource/bias_buffer/c2_buffer/c2_buffer_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_INTF_H__
#endif

#ifndef IMPL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_INTF_H
#define IMPL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_INTF_H

#include "../../../utils/matmul_module.h"
#include "../../../utils/matmul_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {

/**
 * C2Buffer: responsible for C2 buffer management.
 * This module provides abilities to allocate or free one C2 buffer block, and pipeline synchronization.
 * We retain the freedom to make incompatible changes, but do not guarantee the stability.
 * CopyBiasIn is only for internal usage, does not support extension or customized specialization!
 */
template <typename IMPL, typename L0cT, class A_TYPE, const auto& MM_CFG, typename = void>
class C2Buffer {
public:
    __aicore__ inline C2Buffer() {}
    __aicore__ inline ~C2Buffer() {}
    /**
     * @description: Init of TBuf, should be called when matmul is inited.
     * @return: void
     */
    __aicore__ inline void Init() {}
    /**
     * @description: Allocate one block of buffer, should be called only when load bias
     * @return: null tensor on l1
     */
    __aicore__ inline LocalTensor<L0cT> Allocate()
    {
        return LocalTensor<L0cT>{};
    }
    /**
     * @description: Free tensor, should be called after AllocTensor
     * @param: tensor: tensor allocated by AllocTensor or null tensor
     * @return: void
     */
    __aicore__ inline void Free() {}
    /**
     * @description: Put tensor to buffer que
     * @param: tensor: target tensor on L1
     * @return: void
     */
    __aicore__ inline void EnQue() {}
    /**
     * @description: Fetch tensor from que
     * @param: void
     * @return: bias tensor on L1
     */
    __aicore__ inline void DeQue() {}
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _C2_BUFFER_INTF_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_BIAS_BUFFER_C2_BUFFER_C2_BUFFER_INTF_H__
#endif