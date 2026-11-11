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
 * \file scheduler_norm_mx.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/base/scheduler_norm_mx.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_NORM_MX_H__
#endif

#ifndef IMPL_MATMUL_MODULES_SCHEDULER_SCHEDULER_NORM_MX_H
#define IMPL_MATMUL_MODULES_SCHEDULER_SCHEDULER_NORM_MX_H
 
#include "scheduler_intf.h"
#include "scheduler_norm_base.h"
 
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
    enable_if_t<isNormEnableScheduler<A_TYPE, MM_CFG> && !MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()
    && isMxMatmul<A_TYPE, B_TYPE>>>
    : public MatmulNormSchedulerBase<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>
{
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(CopyCubeInA);
    MATMUL_USE_MODULE(CopyCubeInB);
    MATMUL_USE_MODULE(CopyCubeInScaleA);
    MATMUL_USE_MODULE(CopyCubeInScaleB);
    MATMUL_USE_MODULE(CubeOutBuffer);
    MATMUL_USE_MODULE(LoadToA2);
    MATMUL_USE_MODULE(LoadToB2);
    MATMUL_USE_MODULE(TBufPoolL0);
    MATMUL_USE_MODULE(MmadCompute);
    MATMUL_USE_MODULE(BiasScheduler);
    MATMUL_USE_MODULE(MatmulUnitFlag);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulCrossCoreSync);
 
    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using ScaleT = fp8_e8m0_t;
    using SrcA2T = typename GetL0DataType<typename A_TYPE::T, true>::Type;
    using SrcB2T = typename GetL0DataType<typename B_TYPE::T, true>::Type;
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
 
public:
    using BASE_MODULE =
        AscendC::Impl::Detail::MatmulNormSchedulerBase<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>;

    __aicore__ inline void Init(const TCubeTiling *__restrict cubeTiling, TPipe *tpipe)
    {
        BASE_MODULE::Init(cubeTiling, tpipe);
        MATMUL_MODULE(CopyCubeInScaleA)->Init();
        MATMUL_MODULE(CopyCubeInScaleB)->Init();
        InitShareBufEnd(tpipe);
    }
 
    __aicore__ inline void Compute(bool enPartialSum = false)
    {
        if constexpr (!IsBasic(MM_CFG)) {
            // K outer loop only circulates multi-times
            ComputeMultiIter(enPartialSum);
        } else {
            // K outer loop only circulates once
            ComputeOneIter(enPartialSum);
        }
    }

    __aicore__ inline bool ScheduleOnce(bool enPartialSum)
    {
        MATMUL_MODULE(BiasScheduler)->SetBias(MATMUL_MODULE(BiasScheduler)->IsBias() && !enPartialSum);
        if (!MoveNext()) {
            return false;
        }
        if (!enPartialSum) {
            MATMUL_MODULE(CubeOutBuffer)->AllocTensor();
        }
        Compute(enPartialSum);
        return true;
    }
protected:

    __aicore__ inline bool MoveOnFirstIterate()
    {
        // start MLoop and NLoop on the first iteration
        MATMUL_MODULE(MLoop)->OuterStart();
        MATMUL_MODULE(MLoop)->InnerStart();
        MATMUL_MODULE(NLoop)->OuterStart();
        MATMUL_MODULE(NLoop)->InnerStart();
        BASE_MODULE::isFirstIter_ = false;
        return true;
    }
 
    __aicore__ inline bool MoveNext()
    {
        if constexpr (ToMatmulConfig(MM_CFG).singleCoreM != 0 && GetMIter(MM_CFG) == 1 && GetNIter(MM_CFG) == 1) {
            // only iterate once
            if (unlikely(BASE_MODULE::isFirstIter_)) {
                return MoveOnFirstIterate();
            } else {
                return false;
            }
        } else {
            return MoveNextMulti();
        }
    }
 
    __aicore__ inline bool MoveNextMulti()
    {
        if (unlikely(BASE_MODULE::isFirstIter_)) {
            return MoveOnFirstIterate();
        } else {
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                return MoveOnIterateOrderM();
            } else  if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_N) {
                return MoveOnIterateOrderN();
            } else {
                if (likely(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                    static_cast<int>(IterateOrder::ORDER_N))) {
                    ASCENDC_ASSERT((MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                        static_cast<int>(IterateOrder::ORDER_N)), { KERNEL_LOG(KERNEL_ERROR,
                        "iterateOrder is %d , which should be ORDER_N",
                        MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder());
                    });
                    return MoveOnIterateOrderN();
                } else {
                    return MoveOnIterateOrderM();
                }
            }
        }
        return true;
    }
 
    __aicore__ inline bool MoveOnIterateOrderM()
    {
        if constexpr (DoMatmulIBShareNorm(MM_CFG) && B_TYPE::ibShare) {
            ASCENDC_ASSERT(
                (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() >= MATMUL_MODULE(NLoop)->GetTotalIter()), {
                KERNEL_LOG(KERNEL_ERROR, "When iterateOrder is orderM and B is IBShare, stepN >= nIter is required");
            });
        }
        // when N inner loop is finished, clear left matrix's data in L1 buffer, and restart N inner loop
        if (!MATMUL_MODULE(NLoop)->InnerNext()) {
            MATMUL_MODULE(CopyCubeInA)->Reset();
            MATMUL_MODULE(CopyCubeInScaleA)->Reset();
            MATMUL_MODULE(NLoop)->InnerStart();
            // when M outer and inner loop both are finished, clear right matrix's data in L1 buffer,
            // and restart M outer and inner loop
            if (!MATMUL_MODULE(MLoop)->InnerNext()) {
                if (!MATMUL_MODULE(MLoop)->OuterNext()) {
                    MATMUL_MODULE(MLoop)->OuterStart();
                    MATMUL_MODULE(MLoop)->InnerStart();
                    MATMUL_MODULE(CopyCubeInB)->Reset();
                    MATMUL_MODULE(CopyCubeInScaleB)->Reset();
                    // when N outer loop is finished, all the iterations are finished, end process
                    if (!MATMUL_MODULE(NLoop)->OuterNext()) {
                        return false;
                    }
                    // M loop is restarted, N inner loop should be restarted
                    MATMUL_MODULE(NLoop)->InnerStart();
                }
            }
        }
        return true;
    }
 
    __aicore__ inline bool MoveOnIterateOrderN()
    {
        // when M inner loop is finished, clear right matrix's data in L1 buffer, and restart M inner loop
        if (!MATMUL_MODULE(MLoop)->InnerNext()) {
            MATMUL_MODULE(CopyCubeInB)->Reset();
            MATMUL_MODULE(CopyCubeInScaleB)->Reset();
            MATMUL_MODULE(MLoop)->InnerStart();
            if (!MoveOnIterateOrderNInner()) {
                return false;
            }
        }
        return true;
    }
 
private:
    __aicore__ inline bool MoveOnIterateOrderNInner()
    {
        // when N outer and inner loop both are finished, clear left matrix's data in L1 buffer,
        // and restart N outer and inner loop.
        if (!MATMUL_MODULE(NLoop)->InnerNext()) {
            if (!MATMUL_MODULE(NLoop)->OuterNext()) {
                MATMUL_MODULE(NLoop)->OuterStart();
                MATMUL_MODULE(NLoop)->InnerStart();
                MATMUL_MODULE(CopyCubeInA)->Reset();
                MATMUL_MODULE(CopyCubeInScaleA)->Reset();
                // when M outer loop is finished, all the iterations are finished, end process
                if (!MATMUL_MODULE(MLoop)->OuterNext()) {
                    return false;
                }
                // N loop is restarted, M inner loop should be restarted
                MATMUL_MODULE(MLoop)->InnerStart();
            }
        }
        return true;
    }

    __aicore__ inline void ComputeOneIter(bool enPartialSum)
    {
        // init split params for left and right matrix
        MxSplitParams aL0Params = InitSplitAParams();
        MxSplitParams bL0Params = InitSplitBParams();
        // start K outer loop
        MATMUL_MODULE(KLoop)->OuterStart();
        // CopyIn
        LocalTensor<SrcAT> a1 = MATMUL_MODULE(CopyCubeInA)->LoadData(
            0, 0, MATMUL_MODULE(MLoop)->GetTileShape(), MATMUL_MODULE(KLoop)->GetTileShapeA());
        LocalTensor<SrcBT> b1 = MATMUL_MODULE(CopyCubeInB)->LoadData(
            0, 0, MATMUL_MODULE(KLoop)->GetTileShapeB(), MATMUL_MODULE(NLoop)->GetTileShape());
        // scaleA/scaleB
        LocalTensor<ScaleT> scaleA1 = MATMUL_MODULE(CopyCubeInScaleA)->LoadData(
            0, 0, MATMUL_MODULE(MLoop)->GetTileShape(), Ceil(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_K_FACTOR));
        LocalTensor<ScaleT> scaleB1 = MATMUL_MODULE(CopyCubeInScaleB)->LoadData(
            0, 0, Ceil(MATMUL_MODULE(KLoop)->GetTileShapeB(), MX_K_FACTOR), MATMUL_MODULE(NLoop)->GetTileShape());

        if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
            BASE_MODULE::PadZeroForABL1(a1, b1);
            MATMUL_MODULE(MatmulCrossCoreSync)->WaitL1Ready();
        }
        BASE_MODULE::SplitBias(bL0Params.axisL0Len);
 
        // prepare for Split
        bool isBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB();
        bool isATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA();
        bool isScaleATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleA();
        bool isScaleBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleB();
        // update some params in MxSplitParams which is related to k loop
        SplitPrepare(isATranspose, isBTranspose, isScaleATranspose, isScaleBTranspose, aL0Params, bL0Params);
        MATMUL_MODULE(TBufPoolL0)->Allocate();
        // L1->L0
        LocalTensor<SrcA2T> a2 = SplitA(a1, scaleA1, aL0Params, isATranspose, isScaleATranspose);
        LocalTensor<SrcB2T> b2 = SplitB(b1, scaleB1, bL0Params, isBTranspose, isScaleBTranspose);
        MATMUL_MODULE(TBufPoolL0)->EnQue();
        MATMUL_MODULE(TBufPoolL0)->DeQue();
 
        // prepare params and compute
        CubeCompute(MATMUL_MODULE(CubeOutBuffer)->GetTensor(), a2, b2, aL0Params.axisL0Len, bL0Params.axisL0Len,
            CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_BASEK_FACTOR), isATranspose, isBTranspose, enPartialSum, !enPartialSum, true);
 
        MATMUL_MODULE(CopyCubeInA)->ClearLoadData(a1);
        MATMUL_MODULE(CopyCubeInB)->ClearLoadData(b1);
        MATMUL_MODULE(CopyCubeInScaleA)->ClearLoadData(scaleA1);
        MATMUL_MODULE(CopyCubeInScaleB)->ClearLoadData(scaleB1);
    }
 
    __aicore__ inline void ComputeMultiIter(bool enPartialSum)
    {
        // init split params for left and right matrix
        MxSplitParams aL0Params = InitSplitAParams();
        MxSplitParams bL0Params = InitSplitBParams();
 
        // start K outer loop
        MATMUL_MODULE(KLoop)->OuterStart();
        int32_t padCount = 0;
        do {
            int32_t kOuterIdx = MATMUL_MODULE(KLoop)->GetOuterIdx();
            // CopyIn
            LocalTensor<SrcAT> a1 = MATMUL_MODULE(CopyCubeInA)->LoadData(
                MATMUL_MODULE(MLoop)->GetInnerIdx(), kOuterIdx,
                MATMUL_MODULE(MLoop)->GetTileShape(), MATMUL_MODULE(KLoop)->GetTileShapeA());
            LocalTensor<SrcBT> b1 = MATMUL_MODULE(CopyCubeInB)->LoadData(
                kOuterIdx, MATMUL_MODULE(NLoop)->GetInnerIdx(),
                MATMUL_MODULE(KLoop)->GetTileShapeB(), MATMUL_MODULE(NLoop)->GetTileShape());
            // scaleA/scaleB
            LocalTensor<ScaleT> scaleA1 = MATMUL_MODULE(CopyCubeInScaleA)->LoadData(
                MATMUL_MODULE(MLoop)->GetInnerIdx(), kOuterIdx,
                MATMUL_MODULE(MLoop)->GetTileShape(), Ceil(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_K_FACTOR));
            LocalTensor<ScaleT> scaleB1 = MATMUL_MODULE(CopyCubeInScaleB)->LoadData(
                kOuterIdx, MATMUL_MODULE(NLoop)->GetInnerIdx(),
                Ceil(MATMUL_MODULE(KLoop)->GetTileShapeB(), MX_K_FACTOR), MATMUL_MODULE(NLoop)->GetTileShape());
            if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
                if (padCount == 0) {
                    BASE_MODULE::PadZeroForABL1(a1, b1);
                }
                MATMUL_MODULE(MatmulCrossCoreSync)->WaitL1Ready();
            }
            // update some params in MxSplitParams which is related to k loop
            bool sL0CInit = false;
            bool sL0CLast = false;
            BASE_MODULE::UpdateComputeParams(enPartialSum, sL0CInit, sL0CLast);
            if (MATMUL_MODULE(BiasScheduler)->IsBias()) {
                BASE_MODULE::SplitBias(bL0Params.axisL0Len);
            }
            bool isATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA();
            bool isBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB();
            bool isScaleATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleA();
            bool isScaleBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeScaleB();
            SplitPrepare(isATranspose, isBTranspose, isScaleATranspose, isScaleBTranspose, aL0Params, bL0Params);
            // allocate L0 buffer
            MATMUL_MODULE(TBufPoolL0)->Allocate();
            LocalTensor<SrcA2T> a2 = SplitA(a1, scaleA1, aL0Params, isATranspose, isScaleATranspose);
            LocalTensor<SrcB2T> b2 = SplitB(b1, scaleB1, bL0Params, isBTranspose, isScaleBTranspose);
            MATMUL_MODULE(TBufPoolL0)->EnQue();
            MATMUL_MODULE(TBufPoolL0)->DeQue();
            // prepare params and compute
            CubeCompute(MATMUL_MODULE(CubeOutBuffer)->GetTensor(), a2, b2, aL0Params.axisL0Len, bL0Params.axisL0Len,
                CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_BASEK_FACTOR), isATranspose, isBTranspose, enPartialSum, sL0CInit, sL0CLast);
 
            MATMUL_MODULE(CopyCubeInA)->ClearLoadData(a1, MATMUL_MODULE(MLoop)->GetInnerIdx(), kOuterIdx);
            MATMUL_MODULE(CopyCubeInB)->ClearLoadData(b1, kOuterIdx, MATMUL_MODULE(NLoop)->GetInnerIdx());
            MATMUL_MODULE(CopyCubeInScaleA)->ClearLoadData(scaleA1, MATMUL_MODULE(MLoop)->GetInnerIdx(), kOuterIdx);
            MATMUL_MODULE(CopyCubeInScaleB)->ClearLoadData(scaleB1, kOuterIdx, MATMUL_MODULE(NLoop)->GetInnerIdx());
        } while (MATMUL_MODULE(KLoop)->OuterNext());
    }
 
private:
    __aicore__ inline void SplitPrepare(const bool isATranspose, const bool isBTranspose,
        const bool isScaleATranspose, const bool isScaleBTranspose,
        MxSplitParams& aL0Params, MxSplitParams& bL0Params)
    {
        UpdateSplitParams(aL0Params, bL0Params);
        UpdateSplitMxParams(aL0Params, bL0Params, isATranspose, isBTranspose);
        MATMUL_MODULE(LoadToA2)->Prepare(isATranspose, aL0Params.kAxisL1Len, aL0Params.axisL1Len);
        MATMUL_MODULE(LoadToB2)->Prepare(isBTranspose, bL0Params.kAxisL1Len);
    }
 
    __aicore__ inline void UpdateSplitParams(MxSplitParams& aL0Params, MxSplitParams& bL0Params)
    {
        // update Split params related to K loop
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            aL0Params.kAxisL1Offset = MATMUL_MODULE(KLoop)->GetOuterIdx() *
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        } else if constexpr (!IsStaticPaddingEnable(MM_CFG)) {
            aL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_BASEK_FACTOR);
        }
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
            bL0Params.kAxisL1Offset = MATMUL_MODULE(KLoop)->GetOuterIdx() *
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        } else if constexpr (!IsStaticPaddingEnable(MM_CFG)) {
            bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeB(), MX_BASEK_FACTOR);
        }
    }

    __aicore__ inline void UpdateSplitMxParams(MxSplitParams& aL0Params, MxSplitParams& bL0Params,
        const bool isATranspose, const bool isBTranspose)
    {
        if (isATranspose) {
            if constexpr(PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
                aL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR);
            } else {
                aL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_BASEK_FACTOR);
            }
            aL0Params.axisL0Len = Ceil(MATMUL_MODULE(MLoop)->GetTileShape(), c0Size_) * c0Size_;
        }
        if (!isBTranspose) {
            if constexpr(PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
                bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR);
            } else if constexpr (!IsStaticPaddingEnable(MM_CFG)) {
                bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeB(), MX_BASEK_FACTOR);
                bL0Params.axisL0Len = Ceil(MATMUL_MODULE(NLoop)->GetTileShape(), c0Size_) * c0Size_;
            }
        }
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::scalePosition)) {
            aL0Params.kAuxMatrixL1Offset = Ceil(MATMUL_MODULE(KLoop)->GetOuterIdx() *
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), MX_K_FACTOR);
        } else {
            aL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
        }
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::scalePosition)) {
            bL0Params.kAuxMatrixL1Offset = Ceil(MATMUL_MODULE(KLoop)->GetOuterIdx() *
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), MX_K_FACTOR);
        } else {
            bL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(KLoop)->GetTileShapeB(), MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
        }
    }

    __aicore__ inline MxSplitParams InitSplitAParams()
    {
        MxSplitParams aL0Params;
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            aL0Params.axisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(), BLOCK_CUBE);
            aL0Params.axisL0Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
            aL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), c0Size_);
        } else {
            aL0Params.axisL1Len = MATMUL_MODULE(MLoop)->GetTileBlockShape() * BLOCK_CUBE;
            aL0Params.axisL0Len = MATMUL_MODULE(MLoop)->GetBaseShape();
        }

        aL0Params.kAxisL1Offset = 0;
        // mx not support GEMV and does not 16 alignment
        aL0Params.axisL0Len = BASE_MODULE::GetFixedMadM(aL0Params.axisL0Len);
        // if input is from L1, update related params
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            if constexpr (IsBasic(MM_CFG)) {
                aL0Params.axisL1Offset = 0;
            } else {
                aL0Params.axisL1Offset =
                    MATMUL_MODULE(MLoop)->GetInnerIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
            }
            aL0Params.axisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM(), BLOCK_CUBE);
            if (MATMUL_MODULE(MatmulShapeInfo)-> IsTransposeA()) {
                aL0Params.kAxisL1Len =
                    CeilAlign(MATMUL_MODULE(MatmulShapeInfo)-> GetSingleCoreK(), BLOCK_CUBE);
            } else {
                aL0Params.kAxisL1Len =
                    CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), c0Size_);
            }
        } else {
            aL0Params.axisL1Offset = 0;
        }
        // for scaleA
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::scalePosition)) {
            aL0Params.kAuxMatrixL1Len = Ceil(
                CeilAlign(MATMUL_MODULE(MatmulShapeInfo)-> GetSingleCoreK(), c0Size_), MX_K_FACTOR);
            aL0Params.auxMatrixL1Offset = MATMUL_MODULE(MLoop)->GetInnerIdx() * 
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        } else {
            aL0Params.auxMatrixL1Offset = 0;
        }
        aL0Params.kAuxMatrixL1Offset = 0;
        return aL0Params;
    }

    __aicore__ inline MxSplitParams InitSplitBParams()
    {
        MxSplitParams bL0Params;
        // if it's constant tiling sence, get params from tiling, else get params from loop
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            bL0Params.axisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN(), BLOCK_CUBE);
            bL0Params.axisL0Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
            bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), c0Size_);
        } else {
            bL0Params.axisL1Len = MATMUL_MODULE(NLoop)->GetTileBlockShape() * BLOCK_CUBE;
            bL0Params.axisL0Len = MATMUL_MODULE(NLoop)->GetBaseShape();
        }
        bL0Params.kAxisL1Offset = 0;
        // if input is from L1, update related params
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
            if constexpr (IsBasic(MM_CFG)) {
                bL0Params.axisL1Offset = 0;
            } else {
                bL0Params.axisL1Offset = MATMUL_MODULE(NLoop)->GetInnerIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
            }
            bL0Params.axisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeInfo)-> GetSingleCoreN(), BLOCK_CUBE);
            if (MATMUL_MODULE(MatmulShapeInfo)-> IsTransposeB()) {
                bL0Params.kAxisL1Len =
                    CeilAlign(MATMUL_MODULE(MatmulShapeInfo)-> GetSingleCoreK(), c0Size_);
            } else {
                bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeInfo)-> GetSingleCoreK(), BLOCK_CUBE);
            }
        } else {
            bL0Params.axisL1Offset = 0;
        }
        // for scaleB
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::scalePosition)) {
            bL0Params.kAuxMatrixL1Len = Ceil(
                CeilAlign(MATMUL_MODULE(MatmulShapeInfo)-> GetSingleCoreK(), c0Size_), MX_K_FACTOR);
            bL0Params.auxMatrixL1Offset = MATMUL_MODULE(NLoop)->GetInnerIdx() * 
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
        } else {
            bL0Params.auxMatrixL1Offset = 0;
        }
        bL0Params.kAuxMatrixL1Offset = 0;
        return bL0Params;
    }
 
    __aicore__ inline LocalTensor<SrcA2T> SplitA(const LocalTensor<SrcAT>& a1, const LocalTensor<ScaleT>& scaleA1,
        const MxSplitParams& aL0Params, const bool isATranspose, const bool isScaleATranspose)
    {
        auto posA = MATMUL_MODULE(MLoop)->GetInnerIdx() * MATMUL_MODULE(KLoop)->GetTotalIter() + MATMUL_MODULE(KLoop)->GetInnerIdx();
        LocalTensor<SrcA2T> a2;
        int32_t kL0Len = CeilAlign(MATMUL_MODULE(KLoop)->GetBaseShape(), MX_BASEK_FACTOR);
        // Split
        if (!(MATMUL_MODULE(TBufPoolL0)->template Hit<TPosition::A2>(posA))) {
            a2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::A2, SrcA2T>();
            MATMUL_MODULE(LoadToA2)->Load(a2, a1, aL0Params.axisL1Len, aL0Params.kAxisL1Len,
                aL0Params.axisL0Len, kL0Len, aL0Params.axisL1Offset,
                aL0Params.kAxisL1Offset, isATranspose, scaleA1, aL0Params.kAuxMatrixL1Len,
                aL0Params.kAuxMatrixL1Offset, aL0Params.auxMatrixL1Offset);
        } else {
            a2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::A2, SrcA2T>();
        }
        return a2;
    }
 
    __aicore__ inline LocalTensor<SrcB2T> SplitB(const LocalTensor<SrcBT>& b1, const LocalTensor<ScaleT>& scaleB1,
        const MxSplitParams& bL0Params, const bool isBTranspose, const bool isScaleATranspose)
    {
        auto posB = MATMUL_MODULE(NLoop)->GetInnerIdx() * MATMUL_MODULE(KLoop)->GetTotalIter() + MATMUL_MODULE(KLoop)->GetInnerIdx();
        LocalTensor<SrcB2T> b2;
        int32_t kL0Len = CeilAlign(MATMUL_MODULE(KLoop)->GetBaseShape(), MX_BASEK_FACTOR);
        if (!(MATMUL_MODULE(TBufPoolL0)->template Hit<TPosition::B2>(posB))) {
            b2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::B2, SrcB2T>();
            MATMUL_MODULE(LoadToB2)->Load(b2, b1, bL0Params.axisL1Len, bL0Params.kAxisL1Len,
                bL0Params.axisL0Len, kL0Len, bL0Params.axisL1Offset,
                bL0Params.kAxisL1Offset, isBTranspose, scaleB1, bL0Params.kAuxMatrixL1Len,
                bL0Params.kAuxMatrixL1Offset, bL0Params.auxMatrixL1Offset);
        } else {
            b2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::B2, SrcB2T>();
        }
        return b2;
    }
 
    __aicore__ inline void CubeCompute(const LocalTensor<L0cT>& cMatrix, const LocalTensor<SrcA2T>& a2,
        const LocalTensor<SrcB2T>& b2, const uint16_t madM, const uint16_t madN, const uint16_t madK,
        const bool isATranspose, const bool isBTranspose, const bool enPartialSum, const bool sL0CInit, const bool sL0CLast)
    {
        uint8_t unitFlag = MATMUL_MODULE(MatmulUnitFlag)->GetUnitFlag(sL0CLast);
        if constexpr (IsMxDisableUnitFlag<A_TYPE, B_TYPE, MM_CFG>) {
            unitFlag = 0;
        }
        bool isBias;
        bool cmatrixInitVal;
        bool cmatrixSource;
        BASE_MODULE::UpdateBiasParams(enPartialSum, sL0CInit, cmatrixSource, cmatrixInitVal, isBias);
        MATMUL_MODULE(MmadCompute)->Compute(cMatrix, a2, b2, madM, madK,
            madN, isATranspose, isBTranspose, unitFlag, cmatrixSource, cmatrixInitVal, isBias);
        MATMUL_MODULE(TBufPoolL0)->Free();
        // clear data in related buffers
        MATMUL_MODULE(BiasScheduler)->Free();
    }

    constexpr static int32_t c0Size_ = AuxGetC0Size<typename A_TYPE::T>();
};
 
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
 
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_NORM_MX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_NORM_MX_H__
#endif