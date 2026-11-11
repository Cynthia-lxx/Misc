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
* \file matmul_policy.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/policy/matmul_policy.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_POLICY_MATMUL_POLICY_H__
#endif

#ifndef IMPL_MATMUL_POLICY_MATMUL_POLICY_H
#define IMPL_MATMUL_POLICY_MATMUL_POLICY_H

#include "../context/context.h"
#include "..//utils/matmul_utils.h"
#include "../resource/cube_in_buffer/cube_in_buffer.h"
#include "../resource/cube_out_buffer/cube_out_buffer.h"
#include "../scheduler/bias/bias_scheduler.h"
#include "../scheduler/scheduler.h"
#include "../stage/copy_cube_in/copy_cube_in.h"
#include "../stage/copy_cube_out/copy_cube_out.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    MatmulPolicy is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MatmulPolicy is only for internal usage, does not support extension or customized specialization!
*/
template <const auto& MM_CFG, typename IMPL, typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE>
struct MatmulPolicy
{
public:
    constexpr static PolicyType POLICY_TYPE = PolicyType::MATMUL_DEFAULT;
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    using Context = MatmulContext<IMPL, MM_CFG>;
    using CubeOutBuffer = AscendC::Impl::Detail::CubeOutBuffer<IMPL, A_TYPE, B_TYPE, L0cT, C_TYPE, MM_CFG>;
    using CopyCubeOut = AscendC::Impl::Detail::CopyCubeOut<IMPL, A_TYPE, B_TYPE, C_TYPE, MM_CFG>;

    using TransT_B = decltype(GetTransBDataType<A_TYPE, B_TYPE, MM_CFG>());
    using CopyCubeInA = AscendC::Impl::Detail::CopyCubeIn<IMPL, MatmulInputAType<A_TYPE, typename A_TYPE::T>, MM_CFG>;
    using CopyCubeInB = AscendC::Impl::Detail::CopyCubeIn<IMPL, MatmulInputBType<B_TYPE, typename TransT_B::T>, MM_CFG>;
    using CubeInBufferA = AscendC::Impl::Detail::CubeInBuffer<IMPL, MatmulInputAType<A_TYPE, typename A_TYPE::T>, MM_CFG>;
    using CubeInBufferB = AscendC::Impl::Detail::CubeInBuffer<IMPL, MatmulInputBType<B_TYPE, typename TransT_B::T>, MM_CFG>;

    using Scheduler = AscendC::Impl::Detail::MatmulScheduler<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;
    using BatchScheduler = AscendC::Impl::Detail::BatchScheduler<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG>;
    using BiasScheduler = AscendC::Impl::Detail::BiasScheduler<IMPL, A_TYPE, B_TYPE, BIAS_TYPE, MM_CFG>;

    using UserDefDataType = uint64_t;
};

template <const auto& MM_CFG, typename IMPL, typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE>
struct MatmulWithScalePolicy : public MatmulPolicy<MM_CFG, IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>
{
public:
    // for scale
    using CubeInBufferScaleA = AscendC::Impl::Detail::CubeInBuffer<IMPL, MatmulInputScaleAType<A_TYPE, fp8_e8m0_t>, MM_CFG>;
    using CopyCubeInScaleA = AscendC::Impl::Detail::CopyCubeIn<IMPL, MatmulInputScaleAType<A_TYPE, fp8_e8m0_t>, MM_CFG>;
    using CubeInBufferScaleB = AscendC::Impl::Detail::CubeInBuffer<IMPL, MatmulInputScaleBType<B_TYPE, fp8_e8m0_t>, MM_CFG>;
    using CopyCubeInScaleB = AscendC::Impl::Detail::CopyCubeIn<IMPL, MatmulInputScaleBType<B_TYPE, fp8_e8m0_t>, MM_CFG>;
};

/*
    NBuffer33MatmulPolicy is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    NBuffer33MatmulPolicy is only for internal usage, does not support extension or customized specialization!
*/
template <const auto& MM_CFG, typename IMPL, typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE>
struct NBuffer33MatmulPolicy : public MatmulPolicy<MM_CFG, IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>
{
public:
    constexpr static PolicyType POLICY_TYPE = PolicyType::MATMUL_NBUFFER_33;
    using Scheduler = MatmulScheduler<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>;
    using CubeInBufferA = CubeInBuffer<IMPL, MatmulInputAType<A_TYPE, typename A_TYPE::T>, MM_CFG, POLICY_TYPE>;
};

/*
    TrianUpperMatmulPolicy is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    TrianUpperMatmulPolicy is only for internal usage, does not support extension or customized specialization!
*/
template <const auto& MM_CFG, typename IMPL, typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE>
struct TrianUpperMatmulPolicy : public MatmulPolicy<MM_CFG, IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>
{
public:
    constexpr static PolicyType POLICY_TYPE = PolicyType::MATMUL_UPPER_TRIANGULAR;
    using Scheduler = MatmulScheduler<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>;
};

/*
    TrianLowerMatmulPolicy is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    TrianLowerMatmulPolicy is only for internal usage, does not support extension or customized specialization!
*/
template <const auto& MM_CFG, typename IMPL, typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE>
struct TrianLowerMatmulPolicy : public MatmulPolicy<MM_CFG, IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>
{
public:
    constexpr static PolicyType POLICY_TYPE = PolicyType::MATMUL_LOWER_TRIANGULAR;
    using Scheduler = MatmulScheduler<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>;
};

/*
    SplitMMatmulPolicy is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    SplitMMatmulPolicy is only for internal usage, does not support extension or customized specialization!
*/
template <const auto& MM_CFG, typename IMPL, typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE>
struct SplitMMatmulPolicy : public MatmulPolicy<MM_CFG, IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>
{
public:
    using CopyCubeOut = AscendC::Impl::Detail::CopyCubeOut<IMPL, A_TYPE, B_TYPE, C_TYPE, MM_CFG, McgShfMode::DUAL_DST_SPLIT_M>;
};

/*
    SplitNMatmulPolicy is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    SplitNMatmulPolicy is only for internal usage, does not support extension or customized specialization!
*/
template <const auto& MM_CFG, typename IMPL, typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE>
struct SplitNMatmulPolicy : public MatmulPolicy<MM_CFG, IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>
{
public:
    using CopyCubeOut = AscendC::Impl::Detail::CopyCubeOut<IMPL, A_TYPE, B_TYPE, C_TYPE, MM_CFG, McgShfMode::DUAL_DST_SPLIT_N>;
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _MATMUL_POLICY_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_POLICY_MATMUL_POLICY_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_POLICY_MATMUL_POLICY_H__
#endif