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
 * \file scheduler_mdl_mx_base.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/scheduler/base/scheduler_mdl_mx_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_MX_BASE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_MDL_BASE_MX_H
#define IMPL_MATMUL_SCHEDULER_MDL_BASE_MX_H
 
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
    PolicyType POLICY_TYPE = PolicyType::MATMUL_DEFAULT, typename = void>
class MxMatmulSchedulerBase
    : public MatmulMDLSchedulerCommon<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>
{
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(CopyCubeInScaleA);
    MATMUL_USE_MODULE(CopyCubeInScaleB);
    MATMUL_USE_MODULE(CopyCubeInA);
    MATMUL_USE_MODULE(CopyCubeInB);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(LoadToA2);
    MATMUL_USE_MODULE(LoadToB2);
    MATMUL_USE_MODULE(TBufPoolL0);
    MATMUL_USE_MODULE(MatmulUnitFlag);
    MATMUL_USE_MODULE(MmadCompute);
    MATMUL_USE_MODULE(BiasScheduler);
    MATMUL_USE_MODULE(CubeOutBuffer);

    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using ScaleT = fp8_e8m0_t;
    using BiasT = typename BIAS_TYPE::T;
    using SrcA2T = typename GetL0DataType<typename A_TYPE::T, true>::Type;
    using SrcB2T = typename GetL0DataType<typename B_TYPE::T, true>::Type;
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;

public:
    using BASE_MODULE = AscendC::Impl::Detail::MatmulMDLSchedulerCommon<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>;

    __aicore__ inline bool MoveNext()
    {
        if (unlikely(BASE_MODULE::isFirstIter_)) {
            if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
                MATMUL_MODULE(KLoop)->InnerStart();
            }
            return BASE_MODULE::MoveOnFirstIterate();
        } else {
            if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
                if (MATMUL_MODULE(KLoop)->InnerNext()) {
                    return true;
                }
                MATMUL_MODULE(KLoop)->InnerStart();
            }
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::UNDEF) {
                if (likely(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() == static_cast<int>(IterateOrder::ORDER_M))) {
                    return MoveOnIterateOrderM();
                } else {
                    ASCENDC_ASSERT((MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                        static_cast<int>(IterateOrder::ORDER_N)), {
                        KERNEL_LOG(KERNEL_ERROR, "iterateOrder is %d , which should be ORDER_N",
                        MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder());
                    });
                    return MoveOnIterateOrderN();
                }
            } else if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_N) {
                return MoveOnIterateOrderN();
            } else {
                return MoveOnIterateOrderM();
            }
        }
    }
 
    __aicore__ inline bool MoveOnIterateOrderNInner()
    {
        if (!MATMUL_MODULE(NLoop)->OuterNext()) {
            ClearLoadDataA();
            MATMUL_MODULE(NLoop)->OuterStart();
            MATMUL_MODULE(NLoop)->InnerStart();
            if (!MATMUL_MODULE(MLoop)->OuterNext()) {
                return false;
            }
            MATMUL_MODULE(MLoop)->InnerStart();
        }
        return true;
    }

    __aicore__ inline bool MoveOnIterateOrderN()
    {
        // when M inner loop is finished, and restart M inner loop
        if (!MATMUL_MODULE(MLoop)->InnerNext()) {
            MATMUL_MODULE(MLoop)->InnerStart();
            // when N inner loop is finished, clear right matrix's data in L1 buffer
            if (!MATMUL_MODULE(NLoop)->InnerNext()) {
                ClearLoadDataB();
                if (!MoveOnIterateOrderNInner()) {
                    return false;
                }
            }
        }
        return true;
    }

    __aicore__ inline bool MoveOnIterateOrderMInner()
    {
        if (!MATMUL_MODULE(MLoop)->OuterNext()) {
            ClearLoadDataB();
            MATMUL_MODULE(MLoop)->OuterStart();
            MATMUL_MODULE(MLoop)->InnerStart();
            if (!MATMUL_MODULE(NLoop)->OuterNext()) {
                return false;
            }
            MATMUL_MODULE(NLoop)->InnerStart();
        }
        return true;
    }

    __aicore__ inline bool MoveOnIterateOrderM()
    {
        // when N inner loop is finished, and restart N inner loop
        if (!MATMUL_MODULE(NLoop)->InnerNext()) {
            MATMUL_MODULE(NLoop)->InnerStart();
            // when M inner loop is finished, clear left matrix's data in L1 buffer
            if (!MATMUL_MODULE(MLoop)->InnerNext()) {
                ClearLoadDataA();
                // when M outer loop is finished, clear right matrix's data in L1 buffer,
                // and restart M outer and inner loop
                if (!MoveOnIterateOrderMInner()) {
                    return false;
                }
            }
        }
        return true;
    }

    __aicore__ inline void ClearLoadDataA()
    {
        if constexpr (!PhyPosIsL1OrUB(A_TYPE::pos)) {
            if ((MATMUL_MODULE(KLoop)->IsAKL1FullLoad() && !MATMUL_MODULE(MLoop)->IsAML1FullLoad()) ||
                (MATMUL_MODULE(MLoop)->IsLastOuterIter() && MATMUL_MODULE(NLoop)->IsLastOuterIter())) {
                MATMUL_MODULE(CopyCubeInA)->ClearLoadData();
            }
        }
        if constexpr (!(PhyMxScalePosIsL1<A_TYPE>() || PhyMxScalePosIsUB<A_TYPE>())) {
            if (MATMUL_MODULE(KLoop)->IsScaleAKL1FullLoad()) {
                if (MATMUL_MODULE(MLoop)->GetOuterScaleMIdx() != MATMUL_MODULE(MLoop)->GetNextOuterScaleMIdx()) {
                    MATMUL_MODULE(CopyCubeInScaleA)->ClearLoadData();
                }
            }
        }
    }

    __aicore__ inline void ClearLoadDataB()
    {
        if constexpr (!PhyPosIsL1OrUB(B_TYPE::pos)) {
            if ((MATMUL_MODULE(KLoop)->IsBKL1FullLoad() && !MATMUL_MODULE(NLoop)->IsBNL1FullLoad()) ||
                (MATMUL_MODULE(MLoop)->IsLastOuterIter() && MATMUL_MODULE(NLoop)->IsLastOuterIter())) {
                MATMUL_MODULE(CopyCubeInB)->ClearLoadData();
            }
        }
        if constexpr (!(PhyMxScalePosIsL1<B_TYPE>() || PhyMxScalePosIsUB<B_TYPE>())) {
            if (MATMUL_MODULE(KLoop)->IsScaleBKL1FullLoad()) {
                if (MATMUL_MODULE(NLoop)->GetOuterScaleNIdx() != MATMUL_MODULE(NLoop)->GetNextOuterScaleNIdx()) {
                    MATMUL_MODULE(CopyCubeInScaleB)->ClearLoadData();
                }
            }
        }
    }

    __aicore__ inline int32_t GetBL1OffsetFromGM()
    {
        int32_t tilingBaseN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
        int32_t stepN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN();
        if (!MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB()) {
            if (tilingBaseN % BASE_MODULE::c0Size_ == 0 || stepN == 1) {
                return (MATMUL_MODULE(NLoop)->GetInnerIdx() - MATMUL_MODULE(NLoop)->GetOuterIdx() * stepN) * tilingBaseN;
            } else {
                return (MATMUL_MODULE(NLoop)->GetInnerIdx() -
                    MATMUL_MODULE(NLoop)->GetOuterIdx() * stepN) * CeilAlign(tilingBaseN, BASE_MODULE::c0Size_);
            }
        }
        return (MATMUL_MODULE(NLoop)->GetInnerIdx() -
                MATMUL_MODULE(NLoop)->GetOuterIdx() * stepN) * tilingBaseN;
    }
    __aicore__ inline int32_t GetAL1OffsetFromGM()
    {
        int32_t tilingBaseM = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        int32_t stepM = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM();
        if (MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA()) {
            if (tilingBaseM % BASE_MODULE::c0Size_ == 0 || stepM == 1) {
                return (MATMUL_MODULE(MLoop)->GetInnerIdx() - MATMUL_MODULE(MLoop)->GetOuterIdx() * stepM) * tilingBaseM;
            } else {
                return (MATMUL_MODULE(MLoop)->GetInnerIdx() -
                    MATMUL_MODULE(MLoop)->GetOuterIdx() * stepM) * CeilAlign(tilingBaseM, BASE_MODULE::c0Size_);
            }
        }
        return (MATMUL_MODULE(MLoop)->GetInnerIdx() -
                MATMUL_MODULE(MLoop)->GetOuterIdx() * stepM) * tilingBaseM;
    }

protected:
    __aicore__ inline MxSplitParams InitSplitAParams()
    {
        MxSplitParams aL0Params;
        const auto &tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        int32_t tilingBaseM = tiling.GetBaseM();
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            aL0Params.axisL1Offset = MATMUL_MODULE(MLoop)->GetInnerIdx() * tilingBaseM;
            // ds && 82 mdl support multi singleshape in l1
            aL0Params.axisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgM() != -1 ?
            MATMUL_MODULE(MatmulShapeInfo)->GetOrgM() : MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM();
            aL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() != -1 ?
            MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() : MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
        } else if constexpr (IsStaticPaddingEnable(MM_CFG)){
            aL0Params.axisL1Len = tiling.GetStepM() * tilingBaseM;
            aL0Params.kAxisL1Len = tiling.GetStepKa() * tiling.GetBaseK();
            aL0Params.axisL1Offset = (MATMUL_MODULE(MLoop)->GetInnerIdx() - MATMUL_MODULE(MLoop)->GetOuterIdx() *
                tiling.GetStepM()) * tilingBaseM;
        } else {
            aL0Params.axisL1Len = MATMUL_MODULE(MLoop)->GetTileBlockShape() * BLOCK_CUBE;
            aL0Params.axisL1Offset = GetAL1OffsetFromGM();
        }

        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::scalePosition)) {
            aL0Params.auxMatrixL1Offset = MATMUL_MODULE(MLoop)->GetInnerIdx() * tilingBaseM;
            aL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_K_FACTOR);
        } else {
            int32_t mInnerIdx = MATMUL_MODULE(MLoop)->GetInnerIdx();
            int32_t stepScaleM = tiling.GetStepM() * MATMUL_MODULE(MLoop)->GetScaleFactorM();
            aL0Params.auxMatrixL1Offset = (mInnerIdx - mInnerIdx / stepScaleM * stepScaleM) * tilingBaseM;
        }
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            aL0Params.axisL0Len = tilingBaseM;
        } else {
            aL0Params.axisL0Len = MATMUL_MODULE(MLoop)->GetBaseShape();
        }
        if constexpr ((A_TYPE::format == CubeFormat::VECTOR) || (A_TYPE::format == CubeFormat::SCALAR)) {
            aL0Params.axisL0Len = 1;
        } else {
            if (!MatmulFeatureTrait<MM_CFG>::IsSupportDisableGemvMode() && aL0Params.axisL0Len == 1) {
                aL0Params.axisL0Len = BLOCK_CUBE;
            }
        }
        return aL0Params;
    }

    __aicore__ inline MxSplitParams InitSplitBParams()
    {
        MxSplitParams bL0Params;
        int32_t tilingBaseN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
        bL0Params.axisL0Len = MATMUL_MODULE(NLoop)->GetBaseShape();
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            bL0Params.axisL1Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() * tilingBaseN;
            bL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb() *
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
            bL0Params.axisL0Len = tilingBaseN;
            bL0Params.axisL1Offset = GetBL1OffsetFromGM();
            bL0Params.kAxisL1Offset = 0;
        } else {
            if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
                bL0Params.axisL1Offset = MATMUL_MODULE(NLoop)->GetInnerIdx() * tilingBaseN;
                // ds && 82 mdl support multi singleshape in l1
                bL0Params.axisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgN() != -1 ?
                MATMUL_MODULE(MatmulShapeInfo)->GetOrgN() : MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
                bL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() != -1 ?
                MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() : MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            } else {
                bL0Params.axisL1Len = MATMUL_MODULE(NLoop)->GetTileBlockShape() * BLOCK_CUBE;
                bL0Params.axisL1Offset = GetBL1OffsetFromGM();
            }
        }

        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::scalePosition)) {
            bL0Params.auxMatrixL1Offset = MATMUL_MODULE(NLoop)->GetInnerIdx() * tilingBaseN;
            bL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_K_FACTOR);
        } else {
            int32_t nInnerIdx = MATMUL_MODULE(NLoop)->GetInnerIdx();
            int32_t stepScaleN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() * MATMUL_MODULE(NLoop)->GetScaleFactorN();
            bL0Params.auxMatrixL1Offset = (nInnerIdx - nInnerIdx / stepScaleN * stepScaleN) * tilingBaseN;
        }
        return bL0Params;
    }

    __aicore__ inline void UpdateTransParams(MxSplitParams& aL0Params, MxSplitParams& bL0Params,
        const bool isATranspose, const bool isBTranspose)
    {
        if (isATranspose) {
            aL0Params.axisL1Len = CeilAlign(MATMUL_MODULE(MLoop)->GetTileShape(), BASE_MODULE::c0Size_);
            aL0Params.axisL0Len = CeilAlign(MATMUL_MODULE(MLoop)->GetBaseShape(), BASE_MODULE::c0Size_);
            if constexpr(PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
                aL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR);
            } else {
                aL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeA(), MX_BASEK_FACTOR);
            }
            if constexpr(IsScaleTransWithInlv<A_TYPE>) {
                aL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
            }
        }
        if (!isBTranspose) {
            bL0Params.axisL1Len = CeilAlign(MATMUL_MODULE(NLoop)->GetTileShape(), BASE_MODULE::c0Size_);
            if constexpr (!IsStaticPaddingEnable(MM_CFG)) {
                bL0Params.axisL0Len = CeilAlign(MATMUL_MODULE(NLoop)->GetBaseShape(), BASE_MODULE::c0Size_);
            }
            if constexpr(PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
                bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR);
            } else if constexpr (!IsStaticPaddingEnable(MM_CFG)) {
                bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(KLoop)->GetTileShapeB(), MX_BASEK_FACTOR);
            }
            if constexpr(IsScaleTransWithInlv<B_TYPE>) {
                bL0Params.kAuxMatrixL1Len = Ceil(MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK(), MX_BASEK_FACTOR) * MX_EVEN_FACTOR;
            }
        }
    }

    __aicore__ inline void CubeCompute(const LocalTensor<L0cT>& cMatrix, const LocalTensor<SrcA2T>& a2,
        const LocalTensor<SrcB2T>& b2, const uint16_t madM, const uint16_t madN, const uint16_t madK,
        const bool isATranspose, const bool isBTranspose, const bool sL0CInit, const bool sL0CLast)
    {
        int32_t kInnerStartIdx;
        if (BASE_MODULE::IsMDLKFullLoad()) {
            kInnerStartIdx = 0;
        } else {
            kInnerStartIdx = MATMUL_MODULE(KLoop)->GetInnerStartIdx();
        }
        auto unitFlag = MATMUL_MODULE(MatmulUnitFlag)->GetUnitFlag(sL0CLast && (MATMUL_MODULE(KLoop)->GetInnerIdx() ==
            kInnerStartIdx + MATMUL_MODULE(KLoop)->GetInnerIter() - 1));
        if constexpr (IsMxDisableUnitFlag<A_TYPE, B_TYPE, MM_CFG>) {
            unitFlag = 0;
        }
        bool cmatrixSource;
        bool cmatrixInitVal;
        BASE_MODULE::UpdateMmadComputeParams(kInnerStartIdx, sL0CInit, cmatrixSource, cmatrixInitVal);
        MATMUL_MODULE(MmadCompute)->Compute(cMatrix, a2, b2,
            madM, madK, madN, isATranspose, isBTranspose, unitFlag, cmatrixSource, cmatrixInitVal);
    }

    __aicore__ inline LocalTensor<SrcA2T> SplitA(const LocalTensor<SrcAT>& a1, LocalTensor<ScaleT>& scaleA1,
        const MxSplitParams& aL0Params, const bool isATranspose)
    {
        auto posA = MATMUL_MODULE(MLoop)->GetInnerIdx() * MATMUL_MODULE(KLoop)->GetTotalIter() +
            MATMUL_MODULE(KLoop)->GetInnerIdx();
        int32_t kL0Len = CeilAlign(MATMUL_MODULE(KLoop)->GetBaseShape(), MX_BASEK_FACTOR);
        if (!(MATMUL_MODULE(TBufPoolL0)->template Hit<TPosition::A2>(posA))) {
            LocalTensor<SrcA2T> a2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::A2, SrcA2T>();
            MATMUL_MODULE(LoadToA2)->Load(a2, a1, aL0Params.axisL1Len, aL0Params.kAxisL1Len,
                aL0Params.axisL0Len, kL0Len, aL0Params.axisL1Offset, aL0Params.kAxisL1Offset, isATranspose,
                scaleA1, aL0Params.kAuxMatrixL1Len, aL0Params.kAuxMatrixL1Offset, aL0Params.auxMatrixL1Offset);
            return a2;
        } else {
            return MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::A2, SrcA2T>();
        }
    }

    __aicore__ inline LocalTensor<SrcB2T> SplitB(const LocalTensor<SrcBT>& b1, LocalTensor<ScaleT>& scaleB1,
        const MxSplitParams& bL0Params, const bool isBTranspose)
    {
        auto posB = MATMUL_MODULE(NLoop)->GetInnerIdx() * MATMUL_MODULE(KLoop)->GetTotalIter() +
            MATMUL_MODULE(KLoop)->GetInnerIdx();
        int32_t kL0Len = CeilAlign(MATMUL_MODULE(KLoop)->GetBaseShape(), MX_BASEK_FACTOR);
        if (!(MATMUL_MODULE(TBufPoolL0)->template Hit<TPosition::B2>(posB))) {
            LocalTensor<SrcB2T> b2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::B2, SrcB2T>();
            MATMUL_MODULE(LoadToB2)->Load(b2, b1, bL0Params.axisL1Len, bL0Params.kAxisL1Len,
                bL0Params.axisL0Len, kL0Len, bL0Params.axisL1Offset, bL0Params.kAxisL1Offset, isBTranspose,
                scaleB1, bL0Params.kAuxMatrixL1Len, bL0Params.kAuxMatrixL1Offset, bL0Params.auxMatrixL1Offset);
            return b2;
        } else {
            return MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::B2, SrcB2T>();
        }
    }

    __aicore__ inline void ComputeKDB(const LocalTensor<SrcAT>& a1, const LocalTensor<SrcBT>& b1, LocalTensor<ScaleT>& scaleA1, LocalTensor<ScaleT>& scaleB1,
        const MxSplitParams& aL0Params, const MxSplitParams& bL0Params,
        const bool isATranspose, const bool isBTranspose, const bool sL0CInit, const bool sL0CLast)
    {
        MATMUL_MODULE(TBufPoolL0)->Allocate();
        LocalTensor<SrcA2T> a2 = SplitA(a1, scaleA1, aL0Params, isATranspose);
        LocalTensor<SrcB2T> b2 = SplitB(b1, scaleB1, bL0Params, isBTranspose);
        MATMUL_MODULE(TBufPoolL0)->EnQue();
        MATMUL_MODULE(TBufPoolL0)->DeQue();
        CubeCompute(MATMUL_MODULE(CubeOutBuffer)->GetTensor(), a2, b2, aL0Params.axisL0Len,
            bL0Params.axisL0Len, CeilAlign(MATMUL_MODULE(KLoop)->GetBaseShape(), MX_BASEK_FACTOR), isATranspose, isBTranspose, sL0CInit, sL0CLast);
        MATMUL_MODULE(TBufPoolL0)->Free();
        MATMUL_MODULE(BiasScheduler)->Free();
    }

    __aicore__ inline bool IsPreloadDeque()
    {
        if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() == 1) {
            return (MATMUL_MODULE(MLoop)->GetInnerIdx() %
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() == 0);
        } else if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() == 1){
            return (MATMUL_MODULE(NLoop)->GetInnerIdx() %
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() == 0);
        } else {
            return (MATMUL_MODULE(MLoop)->GetInnerIdx() %
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() == 0) &&
                (MATMUL_MODULE(NLoop)->GetInnerIdx() %
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() == 0);
        }
    }
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_MX_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_MX_BASE_H__
#endif