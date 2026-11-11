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
* \file cube_in_buffer_intf.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/resource/cube_in_buffer/cube_in_buffer_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_INTF_H__
#endif

#ifndef IMPL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_INTF_H
#define IMPL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_INTF_H

#include "../../utils/matmul_module.h"
#include "../../utils/matmul_utils.h"

#include "cube_in_buffer_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    CubeInBuffer is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    CubeInBuffer is only for internal usage, does not support extension or customized specialization!
*/
/*
CubeInBuffer: responsible for L1 buffer management.
This module provides abilities to allocate or free one l1 buffer block, and pipeline synchronization.
*/
template <typename IMPL, class INPUT_TYPE, const auto& MM_CFG,
    PolicyType POLICY_TYPE = PolicyType::MATMUL_DEFAULT, typename = void>
class CubeInBuffer {
    using TransT = typename INPUT_TYPE::TRANS_T;
public:
    __aicore__ inline CubeInBuffer() {}
    __aicore__ inline ~CubeInBuffer() {}
    /**
     * @description: Init of buffer, should be called when matmul is inited.
     * @param: baseBlockSize: element nums of basic block when loading to L1
     * @param: cacheNum: describe the nums of basic block when loading to L1
     * @return: void
     */
    __aicore__ inline void Init(int32_t baseBlockSize, int32_t cacheNum) {}

    /**
     * @description: Reset all should be called when matmul end
     * @param: void
     * @return: void
     */
    __aicore__ inline void Destroy() {}

    /**
     * @description: Judge if data of current iteration is already in buffer
     * @param: iterIndex: current index of iteration
     * @param: bufferPos: current buffer position
     * @return: true if already in buffer, else false
     */
    __aicore__ inline bool Hit(int32_t iterIndex, int32_t bufferPos = -1)
    {
        return false;
    }

    /**
     * @description: Get buffer only when hit
     * @param: iterIndex: current index of iteration
     * @param: bufferPos: current buffer position
     * @return: tensor on L1
     */
    __aicore__ inline LocalTensor<TransT> GetBuffer(int32_t iterIndex, int32_t bufferPos = -1)
    {
        return LocalTensor<TransT>{};
    }

    /**
     * @description: Allocate one block of buffer, should be called only when current iterindex does not hit
     * @param: bufferPos: current buffer position
     * @return: void
     */
    __aicore__ inline LocalTensor<TransT> AllocTensor(int32_t bufferPos = -1)
    {
        return LocalTensor<TransT>{};
    }

    /**
     * @description: Free tensor, should be called after AllocTensor
     * @param: bufferPos: current buffer position
     * @param: tensor: tensor allocated by AllocTensor or null tensor
     * @return: void
     */
    __aicore__ inline void FreeTensor(int32_t bufferPos = -1, const LocalTensor<TransT>& tensor = LocalTensor<TransT>{}) {}

    /**
     * @description: Reset the status of que in CubeInBuffer
     * @return: void
     */
    __aicore__ inline void Reset() {}

    /**
     * @description: Put tensor to buffer que
     * @param: tensor: target tensor on L1
     * @param: iterIndex: current index of iteration
     * @return: void
     */
    __aicore__ inline void EnQue(LocalTensor<TransT>& tensor) {}

    /**
     * @description: Fetch tensor from que
     * @param: void
     * @return: void
     */
    __aicore__ inline void DeQue() {}

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
#endif // _CUBE_IN_BUFFER_INTF_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_IN_BUFFER_CUBE_IN_BUFFER_INTF_H__
#endif