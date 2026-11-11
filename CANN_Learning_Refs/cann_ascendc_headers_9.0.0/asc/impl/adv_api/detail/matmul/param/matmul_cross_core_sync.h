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
 * \file matmul_cross_core_sync.h
 * \brief matmul cross core sync manager
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/param/matmul_cross_core_sync.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_CROSS_CORE_SYNC_H__
#endif

#ifndef IMPL_MATMUL_MODULES_PARAM_MATMUL_CROSS_CORE_SYNC_H
#define IMPL_MATMUL_MODULES_PARAM_MATMUL_CROSS_CORE_SYNC_H

#include "../utils/mx_matmul_utils.h"
#include "../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {

constexpr uint16_t CROSS_CORE_SYNC_FACTOR = 16;
constexpr uint16_t CROSS_CORE_INTRA_MODE = 4;

template <typename INPUT_TYPE>
__aicore__ constexpr bool IsScalePosUB()
{
    if constexpr (HasScalePosition<INPUT_TYPE>::value) {
        return (GetPhyType(INPUT_TYPE::scalePosition) == Hardware::UB);
    }
    return false;
}

template <typename IMPL, typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE, const auto &MM_CFG, typename = void>
class MatmulCrossCoreSync
{
public:
    __aicore__ inline void SetIntraAId(uint8_t intraId)
    {}

    __aicore__ inline void SetIntraBId(uint8_t intraId)
    {}

    __aicore__ inline void SetIntraScaleAId(uint8_t intraId)
    {}

    __aicore__ inline void SetIntraScaleBId(uint8_t intraId)
    {}

    __aicore__ inline void WaitL1Ready()
    {}

    __aicore__ inline void SetL1FinishedUse()
    {}

    __aicore__ inline void End()
    {}
};

template <typename IMPL, typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE, const auto &MM_CFG>
class MatmulCrossCoreSync<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, enable_if_t<MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()>>
{
    MATMUL_USE_MODULE(MatmulSubBlockInfo);
public:
    __aicore__ inline void SetIntraAId(uint8_t intraId)
    {
        intraAId_ = intraId;
    }

    __aicore__ inline void SetIntraBId(uint8_t intraId)
    {
        intraBId_ = intraId;
    }

    __aicore__ inline void SetIntraScaleAId(uint8_t intraId)
    {
        intraScaleAId_ = intraId;
    }

    __aicore__ inline void SetIntraScaleBId(uint8_t intraId)
    {
        intraScaleBId_ = intraId;
    }

    __aicore__ inline void WaitL1ReadyForInputUB()
    {
        if constexpr (GetPhyType(A_TYPE::pos) == Hardware::UB || GetPhyType(B_TYPE::pos) == Hardware::UB ||
            IsScalePosUB<A_TYPE>() || IsScalePosUB<B_TYPE>() || GetPhyType(BIAS_TYPE::pos) == Hardware::UB) {
            if (needWaitIntra_) {
                CrossCoreWaitFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(
                    MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() * CROSS_CORE_SYNC_FACTOR);
                if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                    CrossCoreWaitFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(CROSS_CORE_SYNC_FACTOR);
                }
            }
        }
    }
    
    __aicore__ inline void WaitL1Ready()
    {
        WaitL1ReadyForInputUB();
        if constexpr (GetPhyType(A_TYPE::pos) == Hardware::L1 && GetPhyType(A_TYPE::srcPos) == Hardware::UB) {
            if (needWaitIntra_) {
                CrossCoreWaitFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(
                    intraAId_ + MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() * CROSS_CORE_SYNC_FACTOR);
                if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                    CrossCoreWaitFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(intraAId_ + CROSS_CORE_SYNC_FACTOR);
                }
                intraAIdBefore_ = intraAId_;
            }
        }
        if constexpr (GetPhyType(B_TYPE::pos) == Hardware::L1 && GetPhyType(B_TYPE::srcPos) == Hardware::UB) {
            if (needWaitIntra_) {
                CrossCoreWaitFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(
                    intraBId_ + MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() * CROSS_CORE_SYNC_FACTOR);
                if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                    CrossCoreWaitFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(intraBId_ + CROSS_CORE_SYNC_FACTOR);
                }
                intraBIdBefore_ = intraBId_;
            }
        }
        if constexpr (HasScalePosition<A_TYPE>::value) {
            if constexpr (PhyPosIsL1(A_TYPE::scalePosition) && PhyPosIsUB(A_TYPE::srcScalePos)) {
                if (needWaitIntra_) {
                    CrossCoreWaitFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(
                        intraScaleAId_ + MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() * CROSS_CORE_SYNC_FACTOR);
                    // mx not support ibshare now
                }
            }
        }
        if constexpr (HasScalePosition<B_TYPE>::value) {
            if constexpr (PhyPosIsL1(B_TYPE::scalePosition) && PhyPosIsUB(B_TYPE::srcScalePos)) {
                if (needWaitIntra_) {
                    CrossCoreWaitFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(
                        intraScaleBId_ + MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() * CROSS_CORE_SYNC_FACTOR);
                    // mx not support ibshare now
                }
            }
        }
        needWaitIntra_ = false;
    }

    __aicore__ inline void SetL1FinishedUse()
    {
        if constexpr (GetPhyType(A_TYPE::pos) == Hardware::L1 && GetPhyType(A_TYPE::srcPos) == Hardware::UB) {
            if (needSetIntra_) {
                CrossCoreSetFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(intraAId_ + MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() * CROSS_CORE_SYNC_FACTOR);
                if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                    CrossCoreSetFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(intraAId_ + CROSS_CORE_SYNC_FACTOR);
                }
            }
        }
        if constexpr (GetPhyType(B_TYPE::pos) == Hardware::L1 && GetPhyType(B_TYPE::srcPos) == Hardware::UB) {
            if (needSetIntra_) {
                CrossCoreSetFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(intraBId_ + MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() * CROSS_CORE_SYNC_FACTOR);
                if constexpr (A_TYPE::ibShare && B_TYPE::ibShare) {
                    CrossCoreSetFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(intraBId_ + CROSS_CORE_SYNC_FACTOR);
                }
            }
        }
        if constexpr (HasScalePosition<A_TYPE>::value) {
            if constexpr (PhyPosIsL1(A_TYPE::scalePosition) && PhyPosIsUB(A_TYPE::srcScalePos)) {
                if (needSetIntra_) {
                    CrossCoreSetFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(
                        intraScaleAId_ + MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() * CROSS_CORE_SYNC_FACTOR);
                }
            }
        }
        if constexpr (HasScalePosition<B_TYPE>::value) {
            if constexpr (PhyPosIsL1(B_TYPE::scalePosition) && PhyPosIsUB(B_TYPE::srcScalePos)) {
                if (needSetIntra_) {
                    CrossCoreSetFlag<CROSS_CORE_INTRA_MODE, PIPE_MTE1>(
                        intraScaleBId_ + MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx() * CROSS_CORE_SYNC_FACTOR);
                }
            }
        }
        needSetIntra_ = false;
    }

    __aicore__ inline void End()
    {
        SetL1FinishedUse();
        needWaitIntra_ = true;
        needSetIntra_ = true;
    }

private:
    uint8_t intraAIdBefore_ = 0;
    uint8_t intraBIdBefore_ = 0;
    bool needWaitIntra_ = true;
    bool needSetIntra_ = true;
    uint8_t intraAId_ = 0;
    uint8_t intraBId_ = 0;
    uint8_t intraScaleAId_ = 0;
    uint8_t intraScaleBId_ = 0;
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_MODULES_PARAM_MATMUL_CROSS_CORE_SYNC_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_CROSS_CORE_SYNC_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_CROSS_CORE_SYNC_H__
#endif