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
 * \file conv3d_bp_input_impl_base.h
 * \brief
 */

#ifndef CONV3D_BP_INPUT_IMPL_BASE_H
#define CONV3D_BP_INPUT_IMPL_BASE_H

#include "include/adv_api/conv_backprop/common/conv3d_bp_config_base.h"
#include "conv3d_bp_input_func.h"
#include "../common/conv3d_bp_util.h"
#include "kernel_basic_intf.h"
#include "kernel_utils.h"

namespace ConvBackpropApi {
template <typename Intf, class Config_>
struct ConvBpInputImpl {
public:
    using Config = Config_;

public:
    __aicore__ inline ConvBpInputImpl() {}

    DECLARE_IMPL(ConvBackpropInputFunc, Config_, ConvBackpropInputFunc, Init, Intf);
    DECLARE_IMPL(ConvBackpropInputFunc, Config_, ConvBackpropInputFunc, SetWeight, Intf);
    DECLARE_IMPL(ConvBackpropInputFunc, Config_, ConvBackpropInputFunc, SetOutBackprop, Intf);
    DECLARE_IMPL(ConvBackpropInputFunc, Config_, ConvBackpropInputFunc, SetSingleShape, Intf);
    DECLARE_IMPL(ConvBackpropInputFunc, Config_, ConvBackpropInputFunc, SetStartPosition, Intf);
    DECLARE_SYNC_IMPL(ConvBackpropInputFunc, Config_, ConvBackpropInputFunc, Iterate, Intf);
    DECLARE_SYNC_IMPL(ConvBackpropInputFunc, Config_, ConvBackpropInputFunc, IterateAll, Intf);
    DECLARE_SYNC_IMPL(ConvBackpropInputFunc, Config_, ConvBackpropInputFunc, GetTensorC, Intf);
    DECLARE_IMPL(ConvBackpropInputFunc, Config_, ConvBackpropInputFunc, End, Intf);
    struct ContextData : public Config::ContextData {
         __aicore__ inline ContextData(){};
        DEFINE_STUCT_FIELD(AscendC::TPipe, pipe_);
        DEFINE_STUCT_FIELD(const TConv3DBackpropInputTiling *__restrict, tiling_);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, c1Ping_, AscendC::TPosition::CO1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, c1Pong_, AscendC::TPosition::CO1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, a1Ping_, AscendC::TPosition::A1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, a1Pong_, AscendC::TPosition::A1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, b1Ping_, AscendC::TPosition::B1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TQue, b1Pong_, AscendC::TPosition::B1, 1);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TBuf, l0aBuf_, AscendC::TPosition::A2);
        DEFINE_STUCT_TEMPLATE_FIELD(AscendC::TBuf, l0bBuf_, AscendC::TPosition::B2);
        DEFINE_STUCT_FIELD(uint8_t, isFirstIter_);
        DEFINE_STUCT_FIELD(uint8_t, needComputeFlag_);
        DEFINE_STUCT_FIELD(uint8_t, needComputeKFlag_);        
        DEFINE_STUCT_FIELD(uint8_t, l0aPingPongFlag_);
        DEFINE_STUCT_FIELD(uint8_t, useL0PingPong_);
        DEFINE_STUCT_FIELD(uint8_t, usingCacheC1Ping_);
        DEFINE_STUCT_FIELD(bool, isB1FullLoadFlag_);
        DEFINE_STUCT_FIELD(bool, isLoadB1_);
        DEFINE_STUCT_FIELD(bool, isFreeB1_);
        DEFINE_STUCT_FIELD(uint32_t, curStepM_);
        DEFINE_STUCT_FIELD(uint32_t, curStepN_);
        DEFINE_STUCT_FIELD(uint32_t, curNL0Idx_);
        DEFINE_STUCT_FIELD(uint32_t, curNL1Idx_);
        DEFINE_STUCT_FIELD(uint32_t, curML0Idx_);
        DEFINE_STUCT_FIELD(uint32_t, curML1Idx_);
        DEFINE_STUCT_FIELD(uint32_t, curDinIdx_);
        DEFINE_STUCT_FIELD(uint32_t, curPingCoutIdx_);
        DEFINE_STUCT_FIELD(uint32_t, curPongCoutIdx_);
        DEFINE_STUCT_FIELD(uint32_t, channelSize_);
        DEFINE_STUCT_FIELD(uint32_t, curCin1Size_);
        DEFINE_STUCT_FIELD(uint32_t, curHoSize_);
        DEFINE_STUCT_FIELD(uint32_t, kIterStepKaTail);
        DEFINE_STUCT_FIELD(uint32_t, kIterStepKbTail);
        DEFINE_STUCT_FIELD(uint32_t, stepKaTail);
        DEFINE_STUCT_FIELD(uint32_t, stepKbTail);
        DEFINE_STUCT_FIELD(int32_t, curHoIdx_);
        DEFINE_STUCT_FIELD(uint32_t, idxC1in_);
        DEFINE_STUCT_FIELD(uint32_t, baseB1Offset_);
        DEFINE_STUCT_FIELD(uint32_t, blockBaseN_);
        DEFINE_STUCT_FIELD(uint32_t, baseUseM_);
        DEFINE_STUCT_FIELD(uint32_t, baseUseN_);
        DEFINE_STUCT_FIELD(uint32_t, baseUseK_);
        DEFINE_STUCT_FIELD(uint32_t, curLoadKal1_);
        DEFINE_STUCT_FIELD(uint32_t, curLoadKbl1_);
        DEFINE_STUCT_FIELD(uint32_t, baseUseAlignN_);
        DEFINE_STUCT_FIELD(uint32_t, baseMN_);
        DEFINE_STUCT_FIELD(uint32_t, HkWk_);
        DEFINE_STUCT_FIELD(uint32_t, HkWkC0_);
        DEFINE_STUCT_FIELD(uint32_t, DkHkWkC0_);
        DEFINE_STUCT_FIELD(uint32_t, singleShapeHin_);
        DEFINE_STUCT_FIELD(uint32_t, singleShapeCin1_);
        DEFINE_STUCT_FIELD(uint32_t, singleShapeCout1_);
        DEFINE_STUCT_FIELD(uint32_t, singleShapeCin_);
        DEFINE_STUCT_FIELD(uint32_t, singleShapeDin_);
        DEFINE_STUCT_FIELD(uint32_t, curDinStartIdx_);
        DEFINE_STUCT_FIELD(int32_t, curHoStartIdx_);
        DEFINE_STUCT_FIELD(uint32_t, alignedCout_);
        DEFINE_STUCT_FIELD(uint32_t, alignedCout1_);
        DEFINE_STUCT_FIELD(uint32_t, splitHk_);
        DEFINE_STUCT_FIELD(uint32_t, splitWk_);
        DEFINE_STUCT_FIELD(uint32_t, splitHkWk_);
        DEFINE_STUCT_FIELD(uint32_t, splitHkWkC0_);
        DEFINE_STUCT_FIELD(uint32_t, splitHi_);
        DEFINE_STUCT_FIELD(uint32_t, splitWi_);
        DEFINE_STUCT_FIELD(uint32_t, splitSingleShapeM_);
        DEFINE_STUCT_FIELD(uint32_t, stepKaRound_);
        DEFINE_STUCT_FIELD(uint32_t, stepKbRound_);
        DEFINE_STUCT_FIELD(uint32_t, nIter_);
        DEFINE_STUCT_FIELD(uint32_t, tailN_);
        DEFINE_STUCT_FIELD(uint64_t, singleShapeM_);
        DEFINE_STUCT_FIELD(uint64_t, mIter_);
        DEFINE_STUCT_FIELD(uint64_t, kIter_);
        DEFINE_STUCT_FIELD(uint64_t, tailM_);
        DEFINE_STUCT_FIELD(uint64_t, tailK_);
        DEFINE_STUCT_FIELD(uint64_t, hwI_);
        DEFINE_STUCT_FIELD(uint64_t, hwO_);

        DEFINE_STUCT_FIELD(AscendC::MmadParams, mmad_);
        DEFINE_STUCT_FIELD(AscendC::LoadData2DParams, load2d_);

        using LoadData3DParamsV2SrcT = AscendC::LoadData3DParamsV2<typename Intf::SrcT>;
        DEFINE_STUCT_FIELD(LoadData3DParamsV2SrcT, load3d_);
        using srcLocalTensor = AscendC::LocalTensor<typename Intf::SrcT>;
        DEFINE_STUCT_FIELD(srcLocalTensor, cacheA1BufPing_);
        DEFINE_STUCT_FIELD(srcLocalTensor, cacheA1BufPong_);
        DEFINE_STUCT_FIELD(srcLocalTensor, cacheB1BufPing_);
        DEFINE_STUCT_FIELD(srcLocalTensor, cacheB1BufPong_);
        using GlobalTensor = AscendC::GlobalTensor<typename Intf::SrcT>;
        DEFINE_STUCT_FIELD(GlobalTensor, outBackPropGlobal_);
        DEFINE_STUCT_FIELD(GlobalTensor, fmapGlobal_);
        DEFINE_STUCT_FIELD(GlobalTensor, weightGlobal_);
    };
};

}  // namespace ConvBackpropApi

#endif
