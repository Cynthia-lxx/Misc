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
 * \file load_to_l0b_intf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/split/load_to_l0b/load_to_l0b_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD_TO_L0B_INTF_H__
#endif

#ifndef IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_INTF_H
#define IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_INTF_H

#include "../load_to_l0_utils.h"

#include "../load_to_l0_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    LoadToL0B is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    LoadToL0B is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class INPUT_TYPE, const auto& MM_CFG, typename = void>
class LoadToL0B
{
        using TransT = typename INPUT_TYPE::TRANS_T;
        using AuxDtype = decltype(GetAuxDataType<INPUT_TYPE>());

public:
    __aicore__ inline LoadToL0B() = default;
    __aicore__ inline ~LoadToL0B() = default;

    /**
     * @description: Prepare for LoadToL0B
     * @param: isBTranspose: B matrix transpose status
     * @param: bL1K: the length of K_axis for original bMatrix
     * @return: void
     */
    __aicore__ inline void Prepare(bool isBTranspose, uint16_t bL1K) const {};

    /**
     * @description: load a base block from L1 to L0
     * @param: l0B: dst tensor in L0
     * @param: l1B: src tensor in L1
     * @param: bL1N: the length of N_axis for original bMatrix in L1
     * @param: bL1K: the length of K_axis for original bMatrix in L1
     * @param: madN: the length of N_axis for one base block
     * @param: madK: the length of K_axis for one base block
     * @param: bL1NOffset: Offset of the basic block relative to the original bMatrix in the n direction
     * @param: bL1KOffset: Offset of the basic block relative to the original bMatrix in the k direction
     * @param: isBTranspose: B matrix transpose status
     * @param: l1BAuxMatrix: B auxiliary matrix in L1
     * @param: bAuxL1K: the length of K_axis for original auxiliary matrix in L1
     * @param: bAuxL1KOffset: Offset of the basic block relative to the original auxiliary matrix in the k direction
     * @return: void
     */
    __aicore__ inline void Load(const LocalTensor<TransT> &l0B, const LocalTensor<TransT> &l1B,
     uint16_t bL1N, uint16_t bL1K, uint16_t madN, uint16_t madK, uint16_t bL1NOffset, uint16_t bL1KOffset,
     bool isBTranspose, const LocalTensor<AuxDtype> &l1BAuxMatrix = {}, uint16_t bAuxL1K = 0,
     uint16_t bAuxL1KOffset = 0) const {};
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD_TO_L0B_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_LOAD_TO_L0B_INTF_H__
#endif // IMPL_MATMUL_STAGE_SPLIT_LOAD_TO_L0B_INTF_H