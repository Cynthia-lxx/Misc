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
 * \file scheduler_mdl_mx.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/base/scheduler_mdl_mx.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_MX_H__
#endif
#ifndef IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_MX_H
#define IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_MX_H
 
#include "scheduler_intf.h"
#include "scheduler_mdl_mx_base.h"
namespace AscendC {
namespace Impl {
namespace Detail {
 
/*
    MatmulScheduler for A/B/ScaleA/ScaleB is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MatmulScheduler is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG,
    PolicyType POLICY_TYPE>
class MatmulScheduler<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE,
    enable_if_t<DoMatmulMDL(MM_CFG) && isMxMatmul<A_TYPE, B_TYPE>>>
    : public MxMatmulSchedulerBase<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>
{
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(CopyCubeInA);
    MATMUL_USE_MODULE(CopyCubeInB);
    MATMUL_USE_MODULE(CopyCubeInScaleA);
    MATMUL_USE_MODULE(CopyCubeInScaleB);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(LoadToA2);
    MATMUL_USE_MODULE(LoadToB2);
    MATMUL_USE_MODULE(BiasScheduler);
    MATMUL_USE_MODULE(CubeOutBuffer);
    MATMUL_USE_MODULE(MatmulCrossCoreSync);
    MATMUL_USE_MODULE(TBufPoolL0);

    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using ScaleT = fp8_e8m0_t;
    using BiasT = typename BIAS_TYPE::T;
    using SrcA2T = typename GetL0DataType<typename A_TYPE::T, true>::Type;
    using SrcB2T = typename GetL0DataType<typename B_TYPE::T, true>::Type;
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
 
public:
    using BASE_MODULE = AscendC::Impl::Detail::MxMatmulSchedulerBase<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>;
    __aicore__ inline void Init(const TCubeTiling *__restrict cubeTiling, TPipe *tpipe)
    {
        BASE_MODULE::Init(cubeTiling, tpipe);
        MATMUL_MODULE(CopyCubeInScaleA)->Init();
        MATMUL_MODULE(CopyCubeInScaleB)->Init();
        InitShareBufEnd(tpipe);
    }

    __aicore__ inline bool ScheduleOnce(bool enPartialSum)
    {
        MATMUL_MODULE(BiasScheduler)->SetBias(!enPartialSum && MATMUL_MODULE(BiasScheduler)->IsBias());
        if (!BASE_MODULE::MoveNext()) {
            return false;
        }
        if (!enPartialSum) {
            MATMUL_MODULE(CubeOutBuffer)->AllocTensor();
        }
        if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
            PartialKMultiIter(enPartialSum);
        } else {
            ReduceK(enPartialSum);
        }
        return true;
    }
 
private:
    __aicore__ inline void SplitPrepare(const bool enPartialSum, const bool isATranspose, const bool isBTranspose,
        MxSplitParams& aL0Params, MxSplitParams& bL0Params, bool& sL0CInit, bool& sL0CLast)
    {
        UpdateSplitParams(aL0Params, bL0Params);
        BASE_MODULE::UpdateComputeParams(enPartialSum, sL0CInit, sL0CLast);
        BASE_MODULE::UpdateTransParams(aL0Params, bL0Params, isATranspose, isBTranspose);
        MATMUL_MODULE(LoadToA2)->Prepare(isATranspose, aL0Params.kAxisL1Len, aL0Params.axisL1Len);
        MATMUL_MODULE(LoadToB2)->Prepare(isBTranspose, bL0Params.kAxisL1Len);
    }

    __aicore__ inline void UpdateSplitParams(MxSplitParams& aL0Params, MxSplitParams& bL0Params)
    {
        int32_t kInnerIdx = MATMUL_MODULE(KLoop)->GetInnerStartIdx();
        int32_t tilingBaseK = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            aL0Params.kAxisL1Offset = kInnerIdx * tilingBaseK;
        } else {
            // Mx scene K direction should be aligned to 64, baseK is already guaranteed to be aligned to 64
            aL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_BASEK_FACTOR);
            int32_t tilingStepKa = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa();
            aL0Params.kAxisL1Offset = (kInnerIdx - kInnerIdx / tilingStepKa * tilingStepKa) * tilingBaseK;
        }

        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::scalePosition)) {
            if constexpr (PhyPosIsL1(A_TYPE::scalePosition)) {
                int16_t orgKa = MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() != -1 ? MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() :
                    MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK();
                aL0Params.kAuxMatrixL1Len = Ceil(orgKa * MATMUL_MODULE(KLoop)->GetScaleFactorKa(), MX_K_FACTOR);
            }
            aL0Params.kAuxMatrixL1Offset = Ceil(kInnerIdx * tilingBaseK, MX_K_FACTOR);
        } else {
            int32_t tilingStepKa = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa();
            aL0Params.kAuxMatrixL1Len = MATMUL_MODULE(KLoop)->GetTileShapeScaleKa();
            aL0Params.kAuxMatrixL1Offset = (kInnerIdx - kInnerIdx / (tilingStepKa * MATMUL_MODULE(KLoop)->GetScaleFactorKa())
            * (tilingStepKa * MATMUL_MODULE(KLoop)->GetScaleFactorKa())) * Ceil(tilingBaseK, MX_K_FACTOR);
        }

        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
            bL0Params.kAxisL1Offset = kInnerIdx * tilingBaseK;
        } else {
            if constexpr (!IsStaticPaddingEnable(MM_CFG)) {
                // Mx scene K direction should be aligned to 64, baseK is already guaranteed to be aligned to 64
                bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeB(), MX_BASEK_FACTOR);
            }
            int32_t tilingStepKb = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb();
            bL0Params.kAxisL1Offset = (kInnerIdx - kInnerIdx / tilingStepKb * tilingStepKb) * tilingBaseK;
        }

        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::scalePosition)) {
            if constexpr (PhyPosIsL1(B_TYPE::scalePosition)) {
                int16_t orgKb = MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() != -1 ? MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() :
                    MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK();
                bL0Params.kAuxMatrixL1Len = Ceil(orgKb * MATMUL_MODULE(KLoop)->GetScaleFactorKb(), MX_K_FACTOR);
            }
            bL0Params.kAuxMatrixL1Offset = Ceil(kInnerIdx * tilingBaseK, MX_K_FACTOR);
        } else {
            int32_t tilingStepKb = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb();
            bL0Params.kAuxMatrixL1Len = MATMUL_MODULE(KLoop)->GetTileShapeScaleKb();
            bL0Params.kAuxMatrixL1Offset = (kInnerIdx - kInnerIdx / (tilingStepKb * MATMUL_MODULE(KLoop)->GetScaleFactorKb()) *
            (tilingStepKb * MATMUL_MODULE(KLoop)->GetScaleFactorKb()))* Ceil(tilingBaseK, MX_K_FACTOR);
        }
    }

    __aicore__ inline void CopyIn(LocalTensor<SrcAT>& a1, LocalTensor<SrcBT>& b1, LocalTensor<ScaleT>& scaleA1, LocalTensor<ScaleT>& scaleB1, 
        int32_t& padCount)
    {
        a1 = MATMUL_MODULE(CopyCubeInA)->LoadData(MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(KLoop)->GetInnerStartIdx(),
            MATMUL_MODULE(MLoop)->GetTileShape(), MATMUL_MODULE(KLoop)->GetTileShapeA());
        b1 = MATMUL_MODULE(CopyCubeInB)->LoadData(MATMUL_MODULE(KLoop)->GetInnerStartIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
            MATMUL_MODULE(KLoop)->GetTileShapeB(), MATMUL_MODULE(NLoop)->GetTileShape());
        scaleA1 = MATMUL_MODULE(CopyCubeInScaleA)->LoadData(MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(KLoop)->GetInnerStartIdx(),
            MATMUL_MODULE(MLoop)->GetTileShapeScaleM(), MATMUL_MODULE(KLoop)->GetTileShapeScaleKa());
        scaleB1 = MATMUL_MODULE(CopyCubeInScaleB)->LoadData(MATMUL_MODULE(KLoop)->GetInnerStartIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
            MATMUL_MODULE(KLoop)->GetTileShapeScaleKb(), MATMUL_MODULE(NLoop)->GetTileShapeScaleN());
        if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
            if (padCount == 0) {
                BASE_MODULE::PadZeroForABL1(a1, b1);
            }
            padCount += 1;
            MATMUL_MODULE(MatmulCrossCoreSync)->WaitL1Ready();
        }
        DoPreloadLoad();
    }

    __aicore__ inline void DoPreloadLoad()
    {
        if (MATMUL_MODULE(KLoop)->FirstOuterIter()) {
            if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_M) {
                uint32_t nextInnerIdx = MATMUL_MODULE(MLoop)->GetInnerIdx() + MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM();
                if (BASE_MODULE::cacheA1Factor_ == 1 && (MATMUL_MODULE(NLoop)->GetInnerIdx() %
                    MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() == 0) &&
                    (nextInnerIdx < MATMUL_MODULE(MLoop)->GetTotalIter())) {
                    MATMUL_MODULE(CopyCubeInA)->AsyncLoadData(nextInnerIdx, 0,
                        MATMUL_MODULE(MLoop)->GetTileShapeOf(MATMUL_MODULE(MLoop)->GetOuterIdx() + 1),
                        MATMUL_MODULE(KLoop)->GetTileShapeA());
                    MATMUL_MODULE(CopyCubeInScaleA)->AsyncLoadData(nextInnerIdx, 0,
                        MATMUL_MODULE(MLoop)->GetTileShapeOf(MATMUL_MODULE(MLoop)->GetOuterIdx() + 1),
                        MATMUL_MODULE(KLoop)->GetTileShapeScaleKa());
                }
            } else if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_N) {
                uint32_t nextInnerIdx = MATMUL_MODULE(NLoop)->GetInnerIdx() + MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN();
                if ((BASE_MODULE::cacheB1Factor_ == 1) && (MATMUL_MODULE(MLoop)->GetInnerIdx() %
                    MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() == 0) &&
                    (nextInnerIdx < MATMUL_MODULE(NLoop)->GetTotalIter())) {
                    MATMUL_MODULE(CopyCubeInB)->AsyncLoadData(0, nextInnerIdx,
                        MATMUL_MODULE(KLoop)->GetTileShapeB(),
                        MATMUL_MODULE(NLoop)->GetTileShapeOf(MATMUL_MODULE(NLoop)->GetOuterIdx() + 1));
                    MATMUL_MODULE(CopyCubeInScaleB)->AsyncLoadData(0, nextInnerIdx,
                        MATMUL_MODULE(KLoop)->GetTileShapeScaleKb(),
                        MATMUL_MODULE(NLoop)->GetTileShapeOf(MATMUL_MODULE(NLoop)->GetOuterIdx() + 1));
                }
            }
        }
    }

    __aicore__ inline void DoPreloadAWait()
    {
        if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_M) {
            if ((BASE_MODULE::cacheA1Factor_ == 1) && BASE_MODULE::IsPreloadDeque() &&
                (MATMUL_MODULE(MLoop)->GetInnerIdx() + MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() <
                MATMUL_MODULE(MLoop)->GetTotalIter())) {
                MATMUL_MODULE(CopyCubeInA)->AwaitLoadData();
                MATMUL_MODULE(CopyCubeInScaleA)->AwaitLoadData();
            }
        } else if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_N) {
            if ((BASE_MODULE::cacheB1Factor_ == 1) && BASE_MODULE::IsPreloadDeque() &&
                (MATMUL_MODULE(NLoop)->GetInnerIdx() + MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() <
                MATMUL_MODULE(NLoop)->GetTotalIter())) {
                MATMUL_MODULE(CopyCubeInB)->AwaitLoadData();
                MATMUL_MODULE(CopyCubeInScaleB)->AwaitLoadData();
            }
        }
    }

    __aicore__ inline void ReduceKOneIter(bool enPartialSum)
    {
        // init split params for left and right matrix
        MxSplitParams aL0Params = BASE_MODULE::InitSplitAParams();
        MxSplitParams bL0Params = BASE_MODULE::InitSplitBParams();
        MATMUL_MODULE(KLoop)->OuterStart();
        // CopyIn
        LocalTensor<SrcAT> a1;
        LocalTensor<SrcBT> b1;
        LocalTensor<ScaleT> scaleA1;
        LocalTensor<ScaleT> scaleB1;
        int32_t padCount = 0;
        CopyIn(a1, b1, scaleA1, scaleB1, padCount);
        bool isATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA();
        bool isBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB();
        bool isScaleATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleA();
        bool isScaleBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleB();
        SplitPrepareOneIter(isATranspose, isBTranspose, aL0Params, bL0Params);
        LocalTensor<BiasT> bias;
        if (MATMUL_MODULE(BiasScheduler)->IsBias()) {
            bias = BASE_MODULE::SplitBias(bL0Params.axisL0Len);
        }
        int32_t kL1Stride;
        int32_t kAuxL1Stride;
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            kL1Stride = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        } else {
            kL1Stride = MATMUL_MODULE(KLoop)->GetBaseBlockShape() * BASE_MODULE::c0Size_;
        }
        kAuxL1Stride = Ceil(kL1Stride, MX_K_FACTOR);
        // start k inner loop
        MATMUL_MODULE(KLoop)->InnerStart();
        do {
            if constexpr (MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()) {
                if constexpr(ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_N) {
                    ComputeMDB(a1, b1, scaleA1, scaleB1, bias, aL0Params, bL0Params, isATranspose, isBTranspose, !enPartialSum, true);
                } else {
                    ComputeNDB(a1, b1, scaleA1, scaleB1, bias, aL0Params, bL0Params, isATranspose, isBTranspose, !enPartialSum, true);
                }
            } else {
                BASE_MODULE::ComputeKDB(a1, b1, scaleA1, scaleB1, aL0Params, bL0Params, isATranspose, isBTranspose, !enPartialSum, true);
            }
            aL0Params.kAxisL1Offset += kL1Stride;
            bL0Params.kAxisL1Offset += kL1Stride;
            aL0Params.kAuxMatrixL1Offset += kAuxL1Stride;
            bL0Params.kAuxMatrixL1Offset += kAuxL1Stride;
        } while (MATMUL_MODULE(KLoop)->InnerNext());
        if constexpr (MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()) {
            MATMUL_MODULE(BiasScheduler)->Free(bias);
        }
        DoPreloadAWait();
    }
 
    __aicore__ inline void ReduceK(bool enPartialSum)
    {
        if (BASE_MODULE::IsMDLKFullLoad()) {
            ReduceKOneIter(enPartialSum);
        } else {
            ReduceKMultiIter(enPartialSum);
        }
    }

    __aicore__ inline void ReduceKMultiIter(bool enPartialSum)
    {
        // init split params for left and right matrix
        MxSplitParams aL0Params = BASE_MODULE::InitSplitAParams();
        MxSplitParams bL0Params = BASE_MODULE::InitSplitBParams();
        bool isATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA();
        bool isBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB();
        // start K outer loop
        MATMUL_MODULE(KLoop)->OuterStart();
        // curKaOuterIdx and curKbOuterIdx are used to decide if left or right matrix need to clear its l1 buffer
        int32_t curKaOuterIdx = MATMUL_MODULE(KLoop)->GetOuterKaIdx();
        int32_t curKbOuterIdx = MATMUL_MODULE(KLoop)->GetOuterKbIdx();
        int32_t curScaleKaOuterIdx = MATMUL_MODULE(KLoop)->GetOuterScaleKaIdx();
        int32_t curScaleKbOuterIdx = MATMUL_MODULE(KLoop)->GetOuterScaleKbIdx();
        int32_t padCount = 0;
        LocalTensor<BiasT> bias;
        do {
            // CopyIn
            LocalTensor<SrcAT> a1;
            LocalTensor<SrcBT> b1;
            LocalTensor<ScaleT> scaleA1;
            LocalTensor<ScaleT> scaleB1;
            CopyIn(a1, b1, scaleA1, scaleB1, padCount);
            if (MATMUL_MODULE(BiasScheduler)->IsBias()) {
                bias = BASE_MODULE::SplitBias(bL0Params.axisL0Len);
            }
            Compute(a1, b1, scaleA1, scaleB1, bias, enPartialSum, isATranspose, isBTranspose, aL0Params, bL0Params);
            if constexpr (MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()) {
                MATMUL_MODULE(BiasScheduler)->Free(bias);
            }
            DoPreloadAWait();
            ClearL1BufferCache(curKaOuterIdx, curKbOuterIdx, curScaleKaOuterIdx, curScaleKbOuterIdx);
        } while (MATMUL_MODULE(KLoop)->OuterNext());
        ResetCopyInBuffer();
    }

    __aicore__ inline void ClearL1BufferCache(int32_t& curKaOuterIdx, int32_t& curKbOuterIdx, int32_t& curScaleKaOuterIdx, int32_t& curScaleKbOuterIdx)
    {
        constexpr bool preloadEnable = (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_M ||
            ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_N);
        if constexpr (!PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos) && !preloadEnable) {
            int32_t curKaIdx = MATMUL_MODULE(KLoop)->GetNextOuterKaIdx();
            // if next outerKaIdx is not equal to curKaOuterIdx, clear left matrix's data in L1 buffer
            if (curKaIdx != curKaOuterIdx && !MATMUL_MODULE(KLoop)->IsAKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInA)->ClearLoadData();
                curKaOuterIdx = curKaIdx;
            }
        }

        if constexpr (!PhyPosIsL1OrUB<MM_CFG>(A_TYPE::scalePosition) && !preloadEnable) {
            int32_t curKaIdx = MATMUL_MODULE(KLoop)->GetNextOuterScaleKaIdx();
            // if next outerKaIdx is not equal to curScaleKaOuterIdx, clear leftScale matrix's data in L1 buffer
            if (curKaIdx != curScaleKaOuterIdx && !MATMUL_MODULE(KLoop)->IsScaleAKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInScaleA)->ClearLoadData();
                curScaleKaOuterIdx = curKaIdx;
            }
        }

        if constexpr (!PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos) && !preloadEnable) {
            // if next outerKbIdx is not equal to curKbOuterIdx, clear right matrix's data in L1 buffer
            int32_t curKbIdx = MATMUL_MODULE(KLoop)->GetNextOuterKbIdx();
            if (curKbIdx != curKbOuterIdx && !MATMUL_MODULE(KLoop)->IsBKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInB)->ClearLoadData();
                curKbOuterIdx = curKbIdx;
            }
        }

        if constexpr (!PhyPosIsL1OrUB<MM_CFG>(B_TYPE::scalePosition) && !preloadEnable) {
            int32_t curKbIdx = MATMUL_MODULE(KLoop)->GetNextOuterScaleKbIdx();
            bool isLastIter = MATMUL_MODULE(KLoop)->LastOuterIter();
            // if next outerKaIdx is not equal to curScaleKbOuterIdx, clear leftScale matrix's data in L1 buffer
            if ((curKbIdx != curScaleKbOuterIdx || isLastIter) && !MATMUL_MODULE(KLoop)->IsScaleBKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInScaleB)->ClearLoadData();
                curScaleKbOuterIdx = curKbIdx;
            }
        }
    }

    __aicore__ inline void ResetCopyInBuffer()
    {
        // clear L1 buffers
        if constexpr (!PhyPosIsL1(A_TYPE::pos)) {
            if (!MATMUL_MODULE(KLoop)->IsAKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInA)->Reset();
            }
        }
        if constexpr (!PhyPosIsL1(A_TYPE::scalePosition)) {
            if (!MATMUL_MODULE(KLoop)->IsScaleAKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInScaleA)->Reset();
            }
        }
        if constexpr (!PhyPosIsL1(B_TYPE::pos)) {
            if (!MATMUL_MODULE(KLoop)->IsBKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInB)->Reset();
            }
        }
        if constexpr (!PhyPosIsL1(B_TYPE::scalePosition)) {
            if (!MATMUL_MODULE(KLoop)->IsScaleBKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInB)->Reset();
            }
        }
    }
        
    __aicore__ inline void Compute(const LocalTensor<SrcAT>& a1, const LocalTensor<SrcBT>& b1, LocalTensor<ScaleT>& scaleA1, LocalTensor<ScaleT>& scaleB1,
        LocalTensor<BiasT>& bias, const bool enPartialSum, const bool isATranspose, const bool isBTranspose,
        MxSplitParams& aL0Params, MxSplitParams& bL0Params)
    {
        bool sL0CInit = false;
        bool sL0CLast = false;
        SplitPrepare(enPartialSum, isATranspose, isBTranspose, aL0Params, bL0Params, sL0CInit, sL0CLast);
        int32_t kL1Stride = MATMUL_MODULE(KLoop)->GetBaseBlockShape() * BASE_MODULE::c0Size_;
        int32_t kAuxL1Stride = Ceil(kL1Stride, MX_K_FACTOR);
        MATMUL_MODULE(KLoop)->InnerStart();
        do {
            if constexpr (MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()) {
                if constexpr(ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_N) {
                    ComputeMDB(a1, b1, scaleA1, scaleB1, bias, aL0Params, bL0Params, isATranspose, isBTranspose, sL0CInit, sL0CLast);
                } else {
                    ComputeNDB(a1, b1, scaleA1, scaleB1, bias, aL0Params, bL0Params, isATranspose, isBTranspose, sL0CInit, sL0CLast);
                }
            } else {
                BASE_MODULE::ComputeKDB(a1, b1, scaleA1, scaleB1, aL0Params, bL0Params, isATranspose, isBTranspose, sL0CInit, sL0CLast);
            }
            aL0Params.kAxisL1Offset += kL1Stride;
            bL0Params.kAxisL1Offset += kL1Stride;
            aL0Params.kAuxMatrixL1Offset += kAuxL1Stride;
            bL0Params.kAuxMatrixL1Offset += kAuxL1Stride;
        } while (MATMUL_MODULE(KLoop)->InnerNext());
    }
 
    __aicore__ inline void ComputeMDB(const LocalTensor<SrcAT>& a1, const LocalTensor<SrcBT>& b1,
        LocalTensor<ScaleT>& scaleA1, LocalTensor<ScaleT>& scaleB1,
        LocalTensor<BiasT>& bias, MxSplitParams& aL0Params, MxSplitParams& bL0Params,
        const bool isATranspose, const bool isBTranspose, const bool sL0CInit, const bool sL0CLast)
    {
        uint32_t l0aDBLoop = MATMUL_MODULE(MLoop)->GetL0DBLoopNum();
        uint32_t dstOffset = BASE_MODULE::GetL0cDbBufferOffset();
        for (uint32_t idx = 0; idx < l0aDBLoop; ++idx) {
            // allocate L0 buffer
            auto& bufferPool = MATMUL_MODULE(TBufPoolL0)->Allocate();
            // Split a2
            aL0Params.axisL1Offset += (idx * aL0Params.axisL0Len);
            aL0Params.auxMatrixL1Offset += (idx * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM());
            LocalTensor<SrcA2T> a2 = BASE_MODULE::SplitA(a1, scaleA1, aL0Params, isATranspose);
            // Split b2
            LocalTensor<SrcB2T> b2 = BASE_MODULE::SplitB(b1, scaleB1, bL0Params, isBTranspose);
            bufferPool.EnQue();
            bufferPool.DeQue();
            // prepare params and compute
            BASE_MODULE::CubeCompute(MATMUL_MODULE(CubeOutBuffer)->GetTensor()[idx * dstOffset],
                a2, b2, aL0Params.axisL0Len, bL0Params.axisL0Len,
                CeilAlign(MATMUL_MODULE(KLoop)->GetBaseShape(), MX_BASEK_FACTOR),
                isATranspose, isBTranspose, sL0CInit, sL0CLast);
            bufferPool.Free();
        }
        MATMUL_MODULE(BiasScheduler)->Free();
    }

    __aicore__ inline void ComputeNDB(const LocalTensor<SrcAT>& a1, const LocalTensor<SrcBT>& b1,
        LocalTensor<ScaleT>& scaleA1, LocalTensor<ScaleT>& scaleB1,
        LocalTensor<BiasT>& bias, MxSplitParams& aL0Params, MxSplitParams& bL0Params,
        const bool isATranspose, const bool isBTranspose, const bool sL0CInit, const bool sL0CLast)
    {
        uint32_t l0bDBLoop = MATMUL_MODULE(NLoop)->GetL0DBLoopNum();
        uint32_t dstOffset = BASE_MODULE::GetL0cDbBufferOffset();
        for (uint32_t idx = 0; idx < l0bDBLoop; ++idx) {
            // allocate L0 buffer
            auto& bufferPool = MATMUL_MODULE(TBufPoolL0)->Allocate();
            LocalTensor<SrcA2T> a2 = BASE_MODULE::SplitA(a1, scaleA1, aL0Params, isATranspose);
            bL0Params.axisL1Offset += (idx * bL0Params.axisL0Len);
            bL0Params.auxMatrixL1Offset += (idx * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN());
            LocalTensor<SrcB2T> b2 = BASE_MODULE::SplitB(b1, scaleB1, bL0Params, isBTranspose);
            bufferPool.EnQue();
            bufferPool.DeQue();
            // load bias
            MATMUL_MODULE(BiasScheduler)->SplitLoad(bias, MATMUL_MODULE(NLoop)->GetBaseShape(),
                idx * MATMUL_MODULE(NLoop)->GetBaseShape());
            // prepare params and compute
            BASE_MODULE::CubeCompute(MATMUL_MODULE(CubeOutBuffer)->GetTensor()[idx * dstOffset], a2, b2,
                aL0Params.axisL0Len, bL0Params.axisL0Len,
                CeilAlign(MATMUL_MODULE(KLoop)->GetBaseShape(), MX_BASEK_FACTOR),
                isATranspose, isBTranspose, sL0CInit, sL0CLast);
            bufferPool.Free();
            MATMUL_MODULE(BiasScheduler)->Free();
        }
    }

    __aicore__ inline void SplitPrepareOneIter(const bool isATranspose, const bool isBTranspose,
        MxSplitParams& aL0Params, MxSplitParams& bL0Params)
    {
        aL0Params.kAxisL1Offset = 0;
        bL0Params.kAxisL1Offset = 0;
        aL0Params.kAuxMatrixL1Offset = 0;
        bL0Params.kAuxMatrixL1Offset = 0;

        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            if constexpr (IsStaticPaddingEnable(MM_CFG)) {
                aL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK();
            } else {
                aL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() != -1 ?
                    MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() : MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK();
            }
        } else {
            aL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_BASEK_FACTOR);
        }

        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            aL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK(), MX_K_FACTOR);
        } else {
            if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::scalePosition)) {
                aL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() != -1 ?
                    MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() : MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK(), MX_K_FACTOR);
            } else {
                aL0Params.kAuxMatrixL1Len = MATMUL_MODULE(KLoop)->GetTileShapeScaleKa();
            }
        }

        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
            if constexpr (IsStaticPaddingEnable(MM_CFG)) {
                bL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK();
            } else {
                bL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() != -1 ?
                    MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() : MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK();
            }
        } else if constexpr (!IsStaticPaddingEnable(MM_CFG)) {
            bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeB(), MX_BASEK_FACTOR);
        }

        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::scalePosition)) {
            if constexpr (IsStaticPaddingEnable(MM_CFG)) {
                bL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK(), MX_K_FACTOR);
            } else {
                bL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() != -1 ?
                    MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() : MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreK(), MX_K_FACTOR);
            }
        } else {
            bL0Params.kAuxMatrixL1Len = MATMUL_MODULE(KLoop)->GetTileShapeScaleKb();
        }
        BASE_MODULE::UpdateTransParams(aL0Params, bL0Params, isATranspose, isBTranspose);
    }

    __aicore__ inline void PartialCompute(const LocalTensor<SrcAT>& a1, const LocalTensor<SrcBT>& b1, LocalTensor<ScaleT>& scaleA1, LocalTensor<ScaleT>& scaleB1,
        LocalTensor<BiasT>& bias, const bool enPartialSum, const bool isATranspose, const bool isBTranspose,
        MxSplitParams& aL0Params, MxSplitParams& bL0Params)
    {
        bool sL0CInit = false;
        bool sL0CLast = false;
        SplitPrepare(enPartialSum, isATranspose, isBTranspose, aL0Params, bL0Params, sL0CInit, sL0CLast);
        int32_t kL1Stride = MATMUL_MODULE(KLoop)->GetBaseBlockShape() * BASE_MODULE::c0Size_;
        int32_t kAuxL1Stride = Ceil(kL1Stride, MX_K_FACTOR);

        aL0Params.kAxisL1Offset += MATMUL_MODULE(KLoop)->GetStepInnerIdx() * kL1Stride;
        bL0Params.kAxisL1Offset += MATMUL_MODULE(KLoop)->GetStepInnerIdx() * kL1Stride;
        aL0Params.kAuxMatrixL1Offset += MATMUL_MODULE(KLoop)->GetStepInnerIdx() * kAuxL1Stride;
        bL0Params.kAuxMatrixL1Offset += MATMUL_MODULE(KLoop)->GetStepInnerIdx() * kAuxL1Stride;
        BASE_MODULE::ComputeKDB(a1, b1, scaleA1, scaleB1, aL0Params, bL0Params, isATranspose,
            isBTranspose, sL0CInit, sL0CLast);
    }

    __aicore__ inline void PartialKMultiIter(bool enPartialSum)
    {
        // init split params for left and right matrix
        MxSplitParams aL0Params = BASE_MODULE::InitSplitAParams();
        MxSplitParams bL0Params = BASE_MODULE::InitSplitBParams();
        bool isATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA();
        bool isBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB();
        // curKaOuterIdx and curKbOuterIdx are used to decide if left or right matrix need to clear its l1 buffer
        int32_t curKaOuterIdx = MATMUL_MODULE(KLoop)->GetOuterKaIdx();
        int32_t curKbOuterIdx = MATMUL_MODULE(KLoop)->GetOuterKbIdx();
        int32_t curScaleKaOuterIdx = MATMUL_MODULE(KLoop)->GetOuterScaleKaIdx();
        int32_t curScaleKbOuterIdx = MATMUL_MODULE(KLoop)->GetOuterScaleKbIdx();
        // CopyIn
        LocalTensor<SrcAT> a1;
        LocalTensor<SrcBT> b1;
        LocalTensor<ScaleT> scaleA1;
        LocalTensor<ScaleT> scaleB1;
        int32_t padCount = 0;
        CopyIn(a1, b1, scaleA1, scaleB1, padCount);
        LocalTensor<BiasT> bias = BASE_MODULE::SplitBias(bL0Params.axisL0Len);
        PartialCompute(a1, b1, scaleA1, scaleB1, bias, enPartialSum, isATranspose, isBTranspose, aL0Params, bL0Params);
        BASE_MODULE::DoPreloadAWait();
        ClearL1BufferCache(curKaOuterIdx, curKbOuterIdx, curScaleKaOuterIdx, curScaleKbOuterIdx);
        ResetCopyInBuffer();
    }
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_MX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_MX_H__
#endif