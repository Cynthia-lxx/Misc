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
 * \file conv3d_api_impl.h
 * \brief
 */
#ifndef API_CONV3D_API_IMPL_H
#define API_CONV3D_API_IMPL_H


#include "kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "kernel_utils.h"
#include "conv3d_util.h"
#include "include/adv_api/conv/conv3d/conv3d_config.h"
#include "conv3d_common_func.h"
#include "../common/conv_forward_framework_util.h"
#include "../common/conv_forward_common_func.h"

#define QUE_DEPTH_SIZE_TWO 2

namespace Conv3dApi {
template <typename Intf, class Config_, bool OutputOrder = true>
struct Conv3dApiImpl {
public:
    using Config = Config_;
    using ConvParam = typename Config_::ConvParam;
    constexpr static uint32_t ImplType = Config::implType;

    CONV_DEFINE_MEMBER(ConvApi, ConvParam, outputOrder, true, bool);
    CONV_DEFINE_MEMBER(ConvApi, ConvParam, l0pingpong, static_cast<int8_t>(ConvL0PingPong::ALL_CLOSE), int);
    CONV_DEFINE_MEMBER(ConvApi, ConvParam, bl1bypass, static_cast<int8_t>(ConvBL1ByPass::BYPASS_ON), int);
    CONV_DEFINE_MEMBER(ConvApi, ConvParam, groupConvType, static_cast<int8_t>(GroupConvType::NoGroup_Conv), int);

public:
    __aicore__ inline Conv3dApiImpl(){};

    CONV_REG_IMPL(Config, Conv3dApiFunc, Init);
    CONV_REG_IMPL(Config, ConvApiFunc, SetInput);
    CONV_REG_IMPL(Config, ConvApiFunc, SetWeight);
    CONV_REG_IMPL(Config, ConvApiFunc, SetBias);
    CONV_REG_IMPL(Config, Conv3dApiFunc, SetOrgInputShape);
    CONV_REG_IMPL(Config, Conv3dApiFunc, SetOrgWeightShape);
    CONV_REG_IMPL(Config, Conv3dApiFunc, SetOrgOutputShape);
    CONV_REG_IMPL(Config, Conv3dApiFunc, SetSingleInputShape);
    CONV_REG_IMPL(Config, Conv3dApiFunc, SetSingleOutputShape);
    CONV_REG_IMPL(Config, Conv3dApiFunc, SetInputStartPosition);
    CONV_REG_IMPL(Config, Conv3dApiFunc, SetGroupOptInfo);
    CONV_REG_IMPL(Config, Conv3dApiFunc, Iterate);
    CONV_REG_IMPL(Config, Conv3dApiFunc, IterateAll);
    CONV_REG_IMPL(Config, Conv3dApiFunc, GetTensorC);
    CONV_REG_IMPL(Config, ConvApiFunc, End);

    struct ContextData : public Config::ContextData {
        __aicore__ inline ContextData(){};

        const struct TConv3DApiTiling *__restrict conv3dTiling;

        AscendC::TPipe* pipe = nullptr;

        using LoadAL1Tools = typename AscendC::Conditional<Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            Conv3dApiFunc::LoadAL1WithPointWiseTools<Intf>, Conv3dApiFunc::LoadAL1Tools<Intf>>::type;

        using LoadBL1ToolsTmp = typename AscendC::Conditional<Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            Conv3dApiFunc::LoadBL1WithPointWiseTools<Intf>, Conv3dApiFunc::LoadBL1Tools<Intf>>::type;
        using LoadBL1Tools =
            typename AscendC::Conditional<groupConvType, Conv3dApiFunc::LoadBL1WithGroupOptTools<Intf>, LoadBL1ToolsTmp>::type;

        using MMadTools = typename AscendC::Conditional<Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            Conv3dApiFunc::MMadWithPointWiseTools<Intf>, Conv3dApiFunc::MMadTools<Intf>>::type;

        using LoadBL0ToolsTmp = typename AscendC::Conditional<Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            Conv3dApiFunc::LoadBL0WithPointWiseTools<Intf>, Conv3dApiFunc::LoadBL0Tools<Intf>>::type;
        using LoadBL0Tools =
            typename AscendC::Conditional<groupConvType, Conv3dApiFunc::LoadBL0WithGroupOptTools<Intf>, LoadBL0ToolsTmp>::type;

        using LoadAL0Tools = typename AscendC::Conditional<
            Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            Conv3dApiFunc::LoadAL0WithPointWiseTools<Intf>, Conv3dApiFunc::LoadAL0Tools<Intf>>::type;
        using LoadChannelWiseL1Tools = typename AscendC::Conditional<
            Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            Conv3dApiFunc::LoadBiasL1WithPointWiseTools<Intf, typename Config::BiasT>,
            Conv3dApiFunc::LoadChannelWiseL1Tools<Intf, typename Config::BiasT>>::type;
        using LoadBiasL0Tools = typename AscendC::Conditional<
            Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            Conv3dApiFunc::LoadBiasL0WithBroadcastTools<Intf>, Conv3dApiFunc::LoadBiasBtTools<Intf>>::type;
        using CopyOutTools = typename AscendC::Conditional<
            Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            Conv3dApiFunc::CopyOutWithPointWiseTools<Intf>, Conv3dApiFunc::CopyOutTools<Intf>>::type;

        LoadAL1Tools loadAl1Ins;
        LoadBL1Tools loadBL1Ins;
        MMadTools madIns;
        LoadBL0Tools loadBL0Ins;
        LoadAL0Tools loadAL0Ins;
        LoadChannelWiseL1Tools loadBiasL1Ins;
        LoadBiasL0Tools loadBiasBTIns; // for pointwise situation, using l0a and l0b instead of bt.
        CopyOutTools copyOutIns;

        // GM Tensor
        AscendC::GlobalTensor<typename Config::SrcAT> agm;
        AscendC::GlobalTensor<typename Config::SrcBT> bgm;
        AscendC::GlobalTensor<typename Config::BiasT> biasgm;

        // LocalTensor
        AscendC::LocalTensor<typename Config::SrcAT> al1 =
            AscendC::LocalTensor<typename Config::SrcAT>(AscendC::TPosition::A1, 0, 0);
        AscendC::LocalTensor<typename Config::SrcBT> bl1 =
            AscendC::LocalTensor<typename Config::SrcBT>(AscendC::TPosition::B1, 0, 0);
        AscendC::LocalTensor<typename Config::BiasT> biasL1 =
            AscendC::LocalTensor<typename Config::BiasT>(AscendC::TPosition::A1, 0, 0);
        AscendC::LocalTensor<typename Config::L0cT> biasBT =
            AscendC::LocalTensor<typename Config::L0cT>(AscendC::TPosition::C2, 0, 0);
        AscendC::LocalTensor<typename Config::SrcAT> al0 =
            AscendC::LocalTensor<typename Config::SrcAT>(AscendC::TPosition::A2, 0, 0);
        AscendC::LocalTensor<typename Config::SrcBT> bl0 =
            AscendC::LocalTensor<typename Config::SrcBT>(AscendC::TPosition::B2, 0, 0);
        AscendC::LocalTensor<typename Config::SrcAT> al0Ping =
            AscendC::LocalTensor<typename Config::SrcAT>(AscendC::TPosition::A2, 0, 0);
        AscendC::LocalTensor<typename Config::SrcAT> al0Pong =
            AscendC::LocalTensor<typename Config::SrcAT>(AscendC::TPosition::A2, 0, 0);
        AscendC::LocalTensor<typename Config::SrcBT> bl0Ping =
            AscendC::LocalTensor<typename Config::SrcBT>(AscendC::TPosition::B2, 0, 0);
        AscendC::LocalTensor<typename Config::SrcBT> bl0Pong =
            AscendC::LocalTensor<typename Config::SrcBT>(AscendC::TPosition::B2, 0, 0);
        AscendC::LocalTensor<typename Config::L0cT> cl0 =
            AscendC::LocalTensor<typename Config::L0cT>(AscendC::TPosition::CO1, 0, 0);

        // Queue
        AscendC::TQue<AscendC::QuePosition::A1, QUE_DEPTH_SIZE_TWO> queueAL1;      // AL1
        AscendC::TQue<AscendC::QuePosition::B1, QUE_DEPTH_SIZE_TWO> queueBL1;      // BL1
        AscendC::TQue<AscendC::QuePosition::A1, 1> queueBiasL1;   // BiasL1
        AscendC::TQue<AscendC::TPosition::C2, 1> queueBiasBT;     // BT
        AscendC::TQue<AscendC::QuePosition::CO1, 1> queueCL0;  // CL0
        // Buffers
        using L0aBufType = typename AscendC::Conditional<
            Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            AscendC::TBuf<AscendC::TPosition::B2>, AscendC::TBuf<AscendC::TPosition::A2>>::type;
        using L0bBufType = typename AscendC::Conditional<
            Config::formatA == ConvCommonApi::ConvFormat::NCDHW,
            AscendC::TBuf<AscendC::TPosition::A2>, AscendC::TBuf<AscendC::TPosition::B2>>::type;

        L0aBufType l0aBuf;
        L0bBufType l0bBuf;
        AscendC::LocalTensor<typename Config::L0cT> al0BiasB =
            AscendC::LocalTensor<typename Config::L0cT>(AscendC::TPosition::A2, 0, 0);
        AscendC::LocalTensor<typename Config::L0cT> bl0BiasB =
            AscendC::LocalTensor<typename Config::L0cT>(AscendC::TPosition::B2, 0, 0);

        uint8_t enableBias = false;
        uint8_t isFirstIterate = true;
        uint8_t loadAL1Flag = true;
        uint8_t loadBL1Flag = true;
        uint8_t loadAL0Flag = true;
        uint8_t loadBL0Flag = true; 
        uint8_t kAL1fullload = false;
        uint8_t kBL1fullload = false;
        uint8_t biasFullLoadFlag = false;
        uint8_t mL0IsDivisibleByWo = false;

        uint8_t freeAL1TensorFlag = false;
        uint8_t freeBL1TensorFlag = false;
        uint8_t isGroupOptDimTail = false;
        
        uint64_t kAL1Iter = 0;
        uint64_t kBL1Iter = 0;
        uint64_t mAL1Iter = 0;
        uint64_t nBL1Iter = 0;
        uint64_t dOutIter = 0;
        uint64_t kIter = 0;
        uint64_t kAL0Iter = 0;
        uint64_t kBL0Iter = 0;  
        uint64_t mAL0Iter = 0;  
        uint64_t nBL0Iter = 0;
        uint64_t groupOptIter = 0;

        uint64_t maxKAL1Iter = 0;
        uint64_t maxMAL1Iter = 0;
        uint64_t maxNBL1Iter = 0;
        uint64_t maxKBL1Iter = 0;
        uint64_t maxNL0Iter = 0;
        uint64_t maxML0Iter = 0;
        uint64_t maxKL0Iter = 0;
        uint64_t maxDOutIter = 0;
        uint64_t maxGroupOptIter = 0;

        uint64_t ddr2l1LoopN = 0;
        uint64_t l12l0LoopN = 0;
        uint64_t ddr2l1LoopD = 0;
        uint64_t ddr2l1LoopM = 0;
        uint64_t l12l0LoopM = 0;
        uint64_t ddr2l0LoopK = 0;

        // conv3d shape info
        uint64_t orgCi = 0;
        uint64_t orgCo = 0;
        uint64_t orgDi = 0;
        uint64_t orgDo = 0;
        uint64_t orgHi = 0;
        uint64_t orgWi = 0;
        uint64_t orgHo = 0;
        uint64_t orgWo = 0;
        uint64_t kernelD = 0;
        uint64_t kernelH = 0;
        uint64_t kernelW = 0;
        uint64_t strideD = 0;
        uint64_t strideH = 0;
        uint64_t strideW = 0;
        uint64_t dilationD = 0;
        uint64_t dilationH = 0;
        uint64_t dilationW = 0;
        uint64_t padHead = 0;
        uint64_t padTail = 0;
        uint64_t padUp = 0;
        uint64_t padDown = 0;
        uint64_t padLeft = 0;
        uint64_t padRight = 0;
        uint64_t singleCoreCin = 0;
        uint64_t singleCoreCo = 0;
        uint64_t singleCoreM = 0;
        uint64_t singleCoreDo = 0;

        uint64_t dilatedKernelH = 0;
        uint64_t dilatedKernelW = 0;
        uint64_t dilatedKernelD = 0;
        uint64_t cin0 = 0;
        uint64_t cin1 = 0;
        uint64_t cout1 = 0;

        uint64_t kernelHxkernelW = 0;
        uint64_t kernelHxkernelWxkernelD = 0;

        uint64_t kL0Tail = 0;
        uint64_t mAL1Tail = 0;
        uint64_t mAL0Tail = 0;
        uint64_t nL0Tail = 0;
        uint64_t nBL1Tail = 0;
        uint64_t multiKAL1 = 1;
        uint64_t multiKBL1 = 1;

        uint64_t mStartPos = 0;
        uint64_t diStartPos = 0;

        uint64_t orgCoAlignK0 = 0;
        uint64_t orgCoAlignN0 = 0;
        uint64_t nBL1TailAlign = 0;
        uint64_t sizeOfInput = 0;
        uint64_t sizeOfWeight = 0;
        uint64_t sizeOfBias = 0;
        uint64_t sizeOfL0c = 0;

        // GroupOpt
        uint64_t groupKAL1 = 0;
        uint64_t groupKBL1 = 0;
        uint64_t groupKAL1Tail = 0;
        uint64_t groupKBL1Tail = 0;
        uint64_t singleCoreKL0 = 0;
        uint64_t preCorePerGroupSumCout = 0;
        uint64_t singleCoreGroupOpt = 0;
        uint64_t singleCoreCinTail = 0;
        uint64_t singleCoreCoutTail = 0;

        uint8_t preloadAL1DbFlag = false;
        uint8_t preloadABL1DbFlag = false;
    };

    struct ImplDataType {
        __aicore__ inline ImplDataType(){};
    };
};

}  // namespace Conv3dApi

#endif
