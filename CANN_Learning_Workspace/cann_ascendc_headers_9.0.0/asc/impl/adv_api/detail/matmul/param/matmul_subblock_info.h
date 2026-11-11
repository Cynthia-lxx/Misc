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
 * \file matmul_subblock_info.h
 * \brief matmul variable manager
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/param/matmul_subblock_info.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_SUBBLOCK_INFO_H__
#endif

#ifndef IMPL_MATMUL_PARAM_MATMUL_SUBBLOCK_INFO_H
#define IMPL_MATMUL_PARAM_MATMUL_SUBBLOCK_INFO_H

#include "../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, const auto &MM_CFG>
class MatmulSubBlockInfo {
public:
    __aicore__ inline void SetSubBlockIdx(uint8_t subBlockIdx)
    {
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
        ASCENDC_ASSERT((subBlockIdx < MIX_NUM),
            { KERNEL_LOG(KERNEL_ERROR, "subBlockIdx is %d , which should only be [0,%d) ", subBlockIdx, MIX_NUM); });
#endif
        subBlockIdx_ = subBlockIdx;
    }

    __aicore__ inline void SetFakeMsg(bool fakeMsg)
    {
        fakeMsg_ = fakeMsg;
    }

    __aicore__ inline uint8_t GetSubBlockIdx() const
    {
        return subBlockIdx_;
    }

    __aicore__ inline bool GetFakeMsg() const
    {
        return fakeMsg_;
    }

    __aicore__ inline bool IsFakeIntraBlock() const
    {
        return fakeMsg_ || subBlockIdx_ == 0;
    }

private:
    uint8_t subBlockIdx_ {0};
    bool fakeMsg_ {false};
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_PARAM_MATMUL_SUBBLOCK_INFO_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_SUBBLOCK_INFO_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_SUBBLOCK_INFO_H__
#endif