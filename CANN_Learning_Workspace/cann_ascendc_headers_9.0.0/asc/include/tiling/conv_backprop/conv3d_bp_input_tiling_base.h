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
 * \file conv3d_bp_input_tiling_base.h
 * \brief
 */
#ifndef CONV3D_BP_INPUT_TILING_BASE_H
#define CONV3D_BP_INPUT_TILING_BASE_H

#include "conv3d_bp_tiling_base.h"
#include "conv3d_bp_input_tilingdata.h"

namespace ConvBackpropApi {

struct Conv3DDxBpCalcShape {
    int64_t Ci1 = 1;
    int64_t Co1 = 1;
    int64_t realG = 1;
    int64_t cout1G = 1;
    int64_t cin1G = 1;
};

struct Conv3DBpDxDesc {
    ConvBpType outBackpropType = {ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::GM};
    ConvBpType weightType = {ConvCommonApi::ConvFormat::FRACTAL_Z_3D, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::GM};
    ConvBpType fMapType = {ConvCommonApi::ConvFormat::NCDHW, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::CO1};
};

struct BasicBlockDxTilingParams
{
    uint64_t coreNum = 1;
    uint64_t singleCoreM = 0;
    uint32_t singleCoreCout = 0;
    uint32_t singleCoreCout1 = 0;
    uint64_t singleCoreCin = 0;
    uint32_t singleCoreCin1 = 0;
    uint32_t al0Pbuffer = 1;
    uint32_t bl0Pbuffer = 1;
    uint32_t cl0Pbuffer = 1;
    uint32_t al1Pbuffer = 1;
    uint32_t bl1Pbuffer = 1;
    uint32_t baseM = 1;
    uint32_t baseK = 1;
    uint32_t baseN = 1;
    uint32_t stepM = 1;
    uint32_t stepN = 1;
    uint32_t stepKa = 0;
    uint32_t stepKb = 0;
    uint32_t iterateOrder = 0;
};

struct MatMulDxInfo
{
    uint64_t mValue = 1;
    uint64_t kValue = 1;
    uint64_t nValue = 1;
};

class Conv3DBpInputTilingBase {
public:
    Conv3DBpInputTilingBase();
    explicit Conv3DBpInputTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform);
    explicit Conv3DBpInputTilingBase(const PlatformInfo& platform);
    virtual ~Conv3DBpInputTilingBase() = default;
    virtual int64_t GetTiling(optiling::Conv3DBackpropInputTilingData& tiling) = 0;
    virtual int64_t GetTiling(AscendC::tiling::Conv3DBackpropInputTilingData& tiling) = 0;

    bool SetWeightShape(int64_t cout, int64_t cin, int64_t d, int64_t h, int64_t w);
    bool SetInputShape(int64_t n, int64_t c, int64_t d, int64_t h, int64_t w);
    bool SetGradOutputShape(int64_t n, int64_t c, int64_t d, int64_t h, int64_t w);

    void SetWeightType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype);
    void SetInputType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype);
    void SetGradOutputType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype);
    void SetPadding(int64_t padFront, int64_t padBack, int64_t padUp, int64_t padDown,
        int64_t padLeft, int64_t padRight);
    void SetStride(int64_t strideD, int64_t strideH, int64_t strideW);
    void SetDilation(int64_t dilationD, int64_t dilationH, int64_t dilationW);
    bool SetOutputPadding(int64_t outputPadD, int64_t outputPadH, int64_t outputPadW);
protected:
    virtual int64_t Compute() = 0;
    virtual void SetFinalTiling(optiling::Conv3DBackpropInputTilingData& tiling);
    virtual void SetFinalTiling(AscendC::tiling::Conv3DBackpropInputTilingData& tiling);
    virtual void PrintTilingData() const;
    virtual bool CheckInputParam();
    bool ShapeInitCalc();

    PlatformInfo platformInfo;
    Conv3DBackPropShape shapeInfo;
    Conv3DBpDxDesc descInfo;
    Conv3DBpAttr attrInfo;
    Conv3DDxBpCalcShape shapeCalc;
    uint32_t blockDim_ = 1;
    uint32_t coreNum_ = 1;
    OpType opType_ = OpType::kConv3DTranspose;
private:
    bool CheckInputAttr();
    bool CheckInputShape();
    bool CheckInputFormat();

    void SetHF32(uint8_t hf32Enable);
    void SetGroup(int64_t groups);
    void SetBackpropPadding(int64_t backpropPadUp, int64_t backpropPadDown,
        int64_t backpropPadLeft, int64_t backpropPadRight);
};
}  // namespace optiling
#endif  // CONV3D_BP_INPUT_TILING_BASE_H