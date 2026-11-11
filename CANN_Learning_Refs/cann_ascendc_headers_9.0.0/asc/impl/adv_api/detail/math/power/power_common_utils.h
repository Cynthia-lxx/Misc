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
 * \file power_common_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/math/power/power_common_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/power.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_POWER_POWER_COMMON_UTILS_H__
#endif
#ifndef IMPL_MATH_POWER_POWER_COMMON_UTILS_H
#define IMPL_MATH_POWER_POWER_COMMON_UTILS_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {

enum class PowerAlgo {
    INTRINSIC = 0,
    DOUBLE_FLOAT_TECH, 
};

struct PowerConfig {
    PowerAlgo algo = PowerAlgo::INTRINSIC;
};

constexpr PowerConfig defaultPowerConfig = { PowerAlgo::INTRINSIC };

// PowerF temp Tensor
struct AscPowerFParams {
    __aicore__ AscPowerFParams() {};
    LocalTensor<float> tmpTensor1;
    LocalTensor<float> tmpTensor2;
    LocalTensor<float> tmpTensor3;
    LocalTensor<float> tmpTensor4;

    LocalTensor<uint8_t> tmpMask1;
    LocalTensor<uint8_t> tmpMask2;
    LocalTensor<uint8_t> tmpMask3;
    LocalTensor<uint8_t> finiteIntegerYMask;
};

// PowerI tmp Tensor
struct AscPowerIParams {
    __aicore__ AscPowerIParams() {};
    float expIterateSum;

    LocalTensor<int32_t> expUBIterate;
    LocalTensor<int32_t> oriAbsExp;
    LocalTensor<int32_t> recordExpNode;
    LocalTensor<int32_t> tmpTensor1;
    LocalTensor<int32_t> tmpTensor2;
    LocalTensor<int32_t> tmpTensor3;

    LocalTensor<uint8_t> negMask;
    LocalTensor<uint8_t> mask;
    LocalTensor<uint8_t> tmpScalar;
};

// select from tensor src0 and scalar src1 with sel for power
__aicore__ inline void VselPowerTensorScalar(const LocalTensor<float>& dst, const LocalTensor<uint8_t>& sel,
    const LocalTensor<float>& src0, const LocalTensor<float>& tmpScalar,
    SELMODE selMode, int32_t repeat, const BinaryRepeatParams &binaryParam, const uint32_t calCount)
{
    SetCmpMask<float>(tmpScalar);
    PipeBarrier<PIPE_V>();

    Select<float, uint8_t>(dst, sel, src0, repeat, binaryParam);
}

// select from tensor src0 and tensor src1 with sel for power
__aicore__ inline void VselPowerTensorTensor(const LocalTensor<float>& dst, const LocalTensor<uint8_t>& sel,
    const LocalTensor<float>& src0, const LocalTensor<float>& src1, const LocalTensor<float>& tmpScalar,
    SELMODE selMode, int32_t repeat, const BinaryRepeatParams& binaryParam, const uint32_t calCount)
{
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
    tmpScalar.ReinterpretCast<int64_t>().SetValue(0,
        reinterpret_cast<int64_t>(reinterpret_cast<__ubuf__ int64_t*>(sel.GetPhyAddr())));
#else
    uint32_t selAddr = static_cast<uint32_t>(
        reinterpret_cast<int64_t>(reinterpret_cast<__ubuf__ int64_t*>(sel.GetPhyAddr())));
    SetVectorMask<uint32_t>(0, 1);
    Duplicate<uint32_t, false>(tmpScalar.ReinterpretCast<uint32_t>(), selAddr, MASK_PLACEHOLDER, 1,
        DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
#endif
    SetVectorMask<float>(0, calCount);
    SetCmpMask<int64_t>(tmpScalar.ReinterpretCast<int64_t>());
    PipeBarrier<PIPE_V>();
    Select<float, SELMODE::VSEL_TENSOR_TENSOR_MODE>(dst, src0, src1, repeat, binaryParam);
}
} // namespace AscendC
#endif // IMPL_MATH_POWER_POWER_COMMON_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_POWER_POWER_COMMON_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_POWER_POWER_COMMON_UTILS_H__
#endif
