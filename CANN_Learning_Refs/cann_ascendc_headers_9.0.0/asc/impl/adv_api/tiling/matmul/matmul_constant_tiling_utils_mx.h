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
 * \file matmul_constant_tiling_shape_utils.h
 * \brief
 */
#ifndef IMPL_MATMUL_TILING_MATMUL_CONSTANT_TILING_SHAPE_UTILS_H
#define IMPL_MATMUL_TILING_MATMUL_CONSTANT_TILING_SHAPE_UTILS_H

#include "matmul_constant_tiling_struct.h"
#include "matmul_constant_tiling_utils.h"

namespace AscendC {
namespace Impl {
constexpr int32_t SCALE_K_SIZE = 32;
constexpr int32_t MIN_MX_PARAM = 0x01010101;// scaleFactorM/N/Ka/Kb = 0x01
constexpr int32_t MX_L1_BUFFER_NUM = 4;// A/B/scaleA/scaleB buffer
constexpr uint32_t SCALE_FACTOR_MAX_VALUE = 127;// scaleFactorKa/scaleFactorKb is 7 bit, max value is 127
}

struct MxScaleStatus {
    uint8_t scaleFactorKa;
    uint8_t scaleFactorKb;
    uint8_t scaleFactorM;
    uint8_t scaleFactorN;
    int32_t mxTypePara;
};

template <typename A_TYPE>
__aicore__ constexpr int32_t GetScaleAL1Size(const L1Status &l1Status, const MatmulConfig &mmCFG)
{
    int32_t curScaleA1Size = 0;
    int32_t kL0 = GetKL0<A_TYPE>(mmCFG);
    if constexpr (PhyPosIsL1(A_TYPE::scalePosition)) {
        curScaleA1Size = 0;
    } else if constexpr (PhyPosIsUB(A_TYPE::scalePosition)) {
        curScaleA1Size = mmCFG.singleCoreM * CeilNoLog<int32_t>(mmCFG.singleCoreK, Impl::MX_BASEK_FACTOR) * Impl::ALIGN_TWO;
    } else {
        // be consistent with initbuffer
        curScaleA1Size = l1Status.dbAL1 * l1Status.mAL1 * mmCFG.basicM * CeilNoLog<int32_t>(l1Status.kAL1, kL0) *
            CeilNoLog<int32_t>(mmCFG.basicK, Impl::C0_BYTE_SIZE) * GetBitSize<fp8_e8m0_t>() / ONE_BYTE_BIT_SIZE;
    }
    return curScaleA1Size;
}

template <typename B_TYPE>
__aicore__ constexpr int32_t GetScaleBL1Size(const L1Status &l1Status, const MatmulConfig &mmCFG)
{
    int32_t curScaleB1Size = 0;
    int32_t kL0 = GetKL0<B_TYPE>(mmCFG);
    if constexpr (PhyPosIsL1(B_TYPE::scalePosition)) {
        curScaleB1Size = 0;
    } else if constexpr (PhyPosIsUB(B_TYPE::scalePosition)) {
        curScaleB1Size = mmCFG.singleCoreN * CeilNoLog<int32_t>(mmCFG.singleCoreK, Impl::MX_BASEK_FACTOR) * Impl::ALIGN_TWO;
    } else {
        // be consistent with initbuffer
        curScaleB1Size = l1Status.dbBL1 * l1Status.nBL1 * mmCFG.basicN * CeilNoLog<int32_t>(l1Status.kBL1, kL0) *
            CeilNoLog<int32_t>(mmCFG.basicK, Impl::C0_BYTE_SIZE) * GetBitSize<fp8_e8m0_t>() / ONE_BYTE_BIT_SIZE;
    }
    return curScaleB1Size;
}

__aicore__ constexpr int32_t FixMxScaleFactor(int32_t factor, int32_t maxFactor)
{
    factor = factor < maxFactor ? factor : maxFactor;
    // scaleFactor is in range of [1, 127]
    factor = factor > 1 ? factor : 1;
    factor = factor < Impl::SCALE_FACTOR_MAX_VALUE ? factor : Impl::SCALE_FACTOR_MAX_VALUE;
    return factor;
}

__aicore__ constexpr int32_t GetScaleABaseHeightAlign(const MatmulApiStaticTiling &tiling)
{
    return Align<int32_t>(tiling.baseM, GetReduceC0Size<fp8_e8m0_t>());
}

__aicore__ constexpr int32_t GetScaleABaseWidthAlign(const MatmulApiStaticTiling &tiling)
{
    return CeilNoLog<int32_t>(tiling.baseK, Impl::SCALE_K_SIZE);
}

__aicore__ constexpr int32_t GetScaleBBaseHeightAlign(const MatmulApiStaticTiling &tiling)
{
    return Align<int32_t>(CeilNoLog<int32_t>(tiling.baseK, Impl::SCALE_K_SIZE), GetReduceC0Size<fp8_e8m0_t>());
}

template <typename B_TYPE>
__aicore__ constexpr int32_t GetScaleBBaseWidthAlign(const MatmulApiStaticTiling &tiling)
{
    if (B_TYPE::isScaleTrans == false) {
        return Align<int32_t>(tiling.baseN, GetReduceC0Size<fp8_e8m0_t>());
    } else {
        return tiling.baseN;
    }
}

template <typename A_TYPE>
__aicore__ constexpr int32_t GetMatrixScaleAByteSize(const MatmulApiStaticTiling &tiling)
{
    if constexpr (PhyPosIsUB(A_TYPE::scalePosition)) {
        return Align<int32_t>(tiling.singleCoreM, Impl::HW_C0) *
            Align<int32_t>(CeilNoLog<int32_t>(tiling.singleCoreK, Impl::SCALE_K_SIZE), Impl::C0_BYTE_SIZE);
    } else if constexpr (PhyPosIsGM(A_TYPE::scalePosition)) {
        return GetScaleABaseHeightAlign(tiling) * GetScaleABaseWidthAlign(tiling);
    } else {
        return 0;
    }
}

template <typename B_TYPE>
__aicore__ constexpr int32_t GetMatrixScaleBByteSize(const MatmulApiStaticTiling &tiling)
{
    if constexpr (PhyPosIsUB(B_TYPE::scalePosition)) {
        return Align<int32_t>(CeilNoLog<int32_t>(tiling.singleCoreK, Impl::SCALE_K_SIZE), Impl::HW_C0) *
            Align<int32_t>(tiling.singleCoreN, Impl::C0_BYTE_SIZE);
    } else if constexpr (PhyPosIsGM(B_TYPE::scalePosition)) {
        return GetScaleBBaseHeightAlign(tiling) * GetScaleBBaseWidthAlign<B_TYPE>(tiling);
    } else {
        return 0;
    }
}

template <typename A_TYPE, typename B_TYPE>
__aicore__ constexpr void GetMxScaleSize(const MatmulApiStaticTiling &tiling, int& scaleA1Size, int& scaleB1Size)
{
    if constexpr (PhyPosIsL1(A_TYPE::scalePosition)) {
        scaleA1Size = Align<int32_t>(tiling.singleCoreM, Impl::C0_BYTE_SIZE) *
            (CeilNoLog<int32_t>(tiling.singleCoreK, Impl::MX_BASEK_FACTOR) * Impl::ALIGN_TWO);
    } else {
        scaleA1Size = tiling.stepKa * tiling.stepM * 
            (GetMatrixScaleAByteSize<A_TYPE>(tiling) * GetBitSize<fp8_e8m0_t>() / ONE_BYTE_BIT_SIZE);
    }

    if constexpr (PhyPosIsL1(B_TYPE::scalePosition)) {
        scaleB1Size = Align<int32_t>(tiling.singleCoreN, Impl::C0_BYTE_SIZE) *
            (CeilNoLog<int32_t>(tiling.singleCoreK, Impl::MX_BASEK_FACTOR) * Impl::ALIGN_TWO);
    } else {
        scaleB1Size = tiling.stepKb * tiling.stepN * 
            (GetMatrixScaleBByteSize<B_TYPE>(tiling) * GetBitSize<fp8_e8m0_t>() / ONE_BYTE_BIT_SIZE);
    }
}

template <typename A_TYPE, typename B_TYPE, typename BIAS_TYPE>
__aicore__ constexpr MxScaleStatus GetMxScaleFactor(const MatmulApiStaticTiling &tiling, int32_t l1Size)
{
    MxScaleStatus mxScaleFactor{ 1, 1, 1, 1, 0 };

    int remainedL1BufferSize = (l1Size - GetL1UsedSize<A_TYPE, B_TYPE, BIAS_TYPE>(tiling, mxScaleFactor)) / Impl::MX_L1_BUFFER_NUM;
    int kStep = CeilNoLog<int32_t>(tiling.singleCoreK, tiling.baseK);

    int scaleA1Size = 0;
    int scaleB1Size = 0;
    GetMxScaleSize<A_TYPE, B_TYPE>(tiling, scaleA1Size, scaleB1Size);
    GetMxScaleSize<A_TYPE, B_TYPE>(tiling, scaleA1Size, scaleB1Size);

    int oriScaleFactorKa = remainedL1BufferSize / scaleA1Size + 1;
    int maxScaleFactorKa = CeilNoLog<int32_t>(kStep, tiling.stepKa);
    mxScaleFactor.scaleFactorKa = FixMxScaleFactor(oriScaleFactorKa, maxScaleFactorKa);

    int oriScaleFactorKb = remainedL1BufferSize / scaleB1Size + 1;
    int maxScaleFactorKb = CeilNoLog<int32_t>(kStep, tiling.stepKb);
    mxScaleFactor.scaleFactorKb = FixMxScaleFactor(oriScaleFactorKb, maxScaleFactorKb);

    if (mxScaleFactor.scaleFactorKa == maxScaleFactorKa) {
        int mStep = CeilNoLog<int32_t>(tiling.singleCoreM, tiling.baseM);
        int oriScaleFactorM = remainedL1BufferSize / (mxScaleFactor.scaleFactorKa * scaleA1Size);
        int maxScaleFactorM = CeilNoLog<int32_t>(mStep, tiling.stepM);
        mxScaleFactor.scaleFactorM = FixMxScaleFactor(oriScaleFactorM, maxScaleFactorM);
    }   

    if (mxScaleFactor.scaleFactorKb == maxScaleFactorKb) {
        int nStep = CeilNoLog<int32_t>(tiling.singleCoreN, tiling.baseN);
        int oriScaleFactorN = remainedL1BufferSize / (mxScaleFactor.scaleFactorKb * scaleB1Size);
        int maxScaleFactorN = CeilNoLog<int32_t>(nStep, tiling.stepN);
        mxScaleFactor.scaleFactorN = FixMxScaleFactor(oriScaleFactorN, maxScaleFactorN);
    }

    if constexpr ((A_TYPE::format == CubeFormat::ND && A_TYPE::isTrans == true && A_TYPE::scalePosition == TPosition::TSCM) &&
        (B_TYPE::format == CubeFormat::ND && B_TYPE::isTrans == false && B_TYPE::scalePosition == TPosition::TSCM)) {
        mxScaleFactor.scaleFactorM = static_cast<uint8_t>(1);
        mxScaleFactor.scaleFactorN = static_cast<uint8_t>(1);
        mxScaleFactor.scaleFactorKa = static_cast<uint8_t>(1);
        mxScaleFactor.scaleFactorKb = static_cast<uint8_t>(1);
    } else {
        if constexpr (A_TYPE::scalePosition == TPosition::TSCM) {
            mxScaleFactor.scaleFactorM = static_cast<uint8_t>(1);
            mxScaleFactor.scaleFactorKa = static_cast<uint8_t>(1);
        }

        if constexpr (B_TYPE::scalePosition == TPosition::TSCM) {
            mxScaleFactor.scaleFactorN = static_cast<uint8_t>(1);
            mxScaleFactor.scaleFactorKb = static_cast<uint8_t>(1);
        }
    }

    // 8bit: 0~6bit:scaleFactor, 7bit(reserved):double buffer flag
    mxScaleFactor.mxTypePara = static_cast<int32_t>(static_cast<uint32_t>(mxScaleFactor.mxTypePara) | mxScaleFactor.scaleFactorKa);
    mxScaleFactor.mxTypePara = static_cast<int32_t>(static_cast<uint32_t>(mxScaleFactor.mxTypePara) | (mxScaleFactor.scaleFactorKb << 8U));
    mxScaleFactor.mxTypePara = static_cast<int32_t>(static_cast<uint32_t>(mxScaleFactor.mxTypePara) | (mxScaleFactor.scaleFactorM << 16U));
    mxScaleFactor.mxTypePara = static_cast<int32_t>(static_cast<uint32_t>(mxScaleFactor.mxTypePara) | (mxScaleFactor.scaleFactorN << 24U));
    return mxScaleFactor;
}
} // namespace AscendC
#endif // _MATMUL_CONSTANT_TILING_SHAPE_UTILS_H