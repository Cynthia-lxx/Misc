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
 * \file conv3d_bp_filter_tiling_base.h
 * \brief
 */

#ifndef CONV3D_BP_FILTER_TILING_BASE_H
#define CONV3D_BP_FILTER_TILING_BASE_H

#include "conv3d_bp_tiling_base.h"
#include "conv3d_bp_filter_tilingdata.h"

namespace ConvBackpropApi {

// shapeCalc
struct Conv3DBpCalcShape {
    int64_t Ci1 = 1;
    int64_t Co1 = 1;
    int64_t channelSize = 16;
    int64_t real_g = 1;
    int64_t cin1_g = 1;
    int64_t cout1_g = 1;
};

// descInfo
struct Conv3DBpDesc {
    ConvBpType fMapType  = {ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::GM};
    ConvBpType weightType  = {ConvCommonApi::ConvFormat::FRACTAL_Z_3D, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::GM};
    ConvBpType outBackpropType = {ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::CO1};
};

struct TilingValueDw
{
    int64_t singleCoreBatch;
    int64_t singleCoreGroup;
    int64_t singleCoreDk;
    int64_t singleCoreCout;
    int64_t singleCoreCin;
    int64_t singleCoreHo;
    int64_t al0Pbuffer;
    int64_t bl0Pbuffer;
    int64_t cl0Pbuffer;
    int64_t al1Pbuffer;
    int64_t bl1Pbuffer;
    int64_t baseM;
    int64_t baseK;
    int64_t baseN;
    int64_t stepM;
    int64_t stepN;
    int64_t stepKa;
    int64_t stepKb;
    int64_t iterateOrder;
    int64_t bl1Bound;
};

struct BasicBlockTilingParams
{
    int64_t usedCoreNum = 1;
    int64_t totalCnt = 0;
    int64_t blockBaseM = 128;
    int64_t blockBaseN = 128;
    int64_t blockBaseK = 128;
    int64_t singleCoreM = 128;
    int64_t singleCoreN = 128;
    int64_t singleCoreK = 128;
    int64_t depthA1 = 1;
    int64_t depthB1 = 1;
    int64_t stepKa = 1;
    int64_t stepKb = 1;
    int64_t stepM = 1;
    int64_t stepN = 1;
    int64_t dbL1A = 1;
    int64_t dbL1B = 1;
    int64_t dbL0C = 1;
    int64_t iterateOrder = 0;
    int64_t coreBindDirection = 1;
    int64_t coreBindOrder = 1;
};

struct MatMulInfo
{
    uint64_t mValue = 0;
    uint64_t kValue = 0;
    uint64_t nValue = 0;
};

class Conv3dBpFilterTilingBase {
public:
    Conv3dBpFilterTilingBase();
    explicit Conv3dBpFilterTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform);
    explicit Conv3dBpFilterTilingBase(const PlatformInfo& platform);
    virtual ~Conv3dBpFilterTilingBase() = default;
    virtual int64_t GetTiling(optiling::Conv3DBackpropFilterTilingData& tiling) = 0;
    virtual int64_t GetTiling(AscendC::tiling::Conv3DBackpropFilterTilingData& tiling) = 0;

    void SetWeightShape(int64_t cout, int64_t cin, int64_t d, int64_t h, int64_t w);
    void SetInputShape(int64_t n, int64_t c, int64_t d, int64_t h, int64_t w);
    void SetGradOutputShape(int64_t n, int64_t c, int64_t d, int64_t h, int64_t w);

    void SetWeightType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype);
    void SetInputType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype);
    void SetGradOutputType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype);

    void SetPadding(int64_t padFront, int64_t padBack, int64_t padUp, int64_t padDown,
                    int64_t padLeft, int64_t padRight);
    void SetStride(int64_t strideD, int64_t strideH, int64_t strideW);
    void SetDilation(int64_t dilationD, int64_t dilationH, int64_t dilationW);
protected:
    virtual int64_t Compute() = 0;
    virtual void SetFinalTiling(optiling::Conv3DBackpropFilterTilingData& tiling);
    virtual void SetFinalTiling(AscendC::tiling::Conv3DBackpropFilterTilingData& tiling);
    virtual void PrintTilingData() const;
    bool CheckInputParam();
    bool ShapeInitCalc();

    PlatformInfo platformInfo;
    Conv3DBackPropShape shapeInfo;
    Conv3DBpAttr attrInfo;
    Conv3DBpCalcShape shapeCalc;
    Conv3DBpDesc descInfo;

    BasicBlockTilingParams blockTiling_;
    MatMulInfo mmInfo_;

    uint32_t coreNum_ = 1;
    uint32_t dtypeByte_ = 2;
private:
    bool CheckInputAttr();
    bool CheckInputShape();
    bool CheckInputFormat();
    bool CheckLoad3DLimits();
    bool CheckInstructionLimits();

    void SetGroup(int64_t groups);
    void SetHF32(bool hf32Enable);
    bool hf32Enable_ = false;
};
} // ConvBackpropApi

#endif // CONV3D_BP_FILTER_TILING_BASE_H