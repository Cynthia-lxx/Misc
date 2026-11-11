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
 * \file conv3d_tiling_base.h
 * \brief
 */

#ifndef ADV_API_CONV_CONV3D_CONV3D_TILING_BASE_H
#define ADV_API_CONV_CONV3D_CONV3D_TILING_BASE_H

#include "conv3d_tilingdata.h"
#include "../../../../impl/adv_api/tiling/conv/conv3d_tiling_util.h"
#include "kernel_tiling/kernel_tiling.h"

namespace Conv3dTilingApi {
/**
* @struct Conv3DL1Tiling
* @brief Structure for conv3d l1 tiling type configuration
*/
struct Conv3DL1Tiling {
    uint64_t kAL1 = 0;                ///< K dimension tile size for input in L1
    uint64_t kBL1 = 0;                ///< K dimension tile size for weight in L1
    uint64_t mAL1Value = 0;           ///< M dimension tile size for input in L1
    uint64_t nBL1Value = 0;           ///< N dimension tile size for weight in L1
    uint64_t mAL1DivmL0 = 0;          ///< Ratio of L1 M tile to L0 M tile
    uint64_t nBL1DivnL0 = 0;          ///< Ratio of L1 N tile to L0 N tile
    uint64_t cin1InAL1 = 0;           ///< C_in dimension tile size in L1
    uint64_t kAL1Tail = 0;            ///< Tail handling for K dimension in L1
    uint64_t cin1InAL1Tail = 0;       ///< Tail handling for C_in dimension in L1
    uint64_t kBL1DivK0 = 0;           ///< Ratio of L1 K tile to cube K0
    uint64_t kBL1Tail = 0;            ///< Tail handling for K dimension in weight L1
    uint64_t kBL1TailDivK0 = 0;       ///< Tail ratio for K dimension in weight L1
    IterateMNOrder iterateMNOrder = IterateMNOrder::INVALID;  ///< Iteration order for M and N dimensions
    bool isWeightBypass = false;      ///< Flag indicating if weight bypass is enabled
    bool biasFullLoadFlag = false;    ///< Flag for full bias loading optimization
    bool fixpParamsFullLoadFlag = false;  ///< Flag for full fixed-point parameters loading
    bool al1FullLoad = false;         ///< Flag for full input loading to L1
    bool bl1FullLoad = false;         ///< Flag for full weight loading to L1
};
/**
* @struct Conv3DL0Tiling
* @brief Structure for conv3d l0 tiling type configuration
*/
struct Conv3DL0Tiling {
    uint64_t mL0 = 0;                 ///< M dimension tile size in L0
    uint64_t kL0 = 0;                 ///< K dimension tile size in L0
    uint64_t nL0 = 0;                 ///< N dimension tile size in L0
    uint64_t nL0xk0 = 0;              ///< Product of N dimension tile and cube K0
    uint64_t kL0xorgCoAlignN0 = 0;    ///< K dimension tile aligned with output channels
};
/**
* @struct Conv3DInputshape
* @brief Structure for conv3d input shape configuration
*/
struct Conv3DInputshape {
    int64_t orgBatch = 1;             ///< Original batch size
    int64_t orgkH = -1;               ///< Original kernel height
    int64_t orgkW = -1;               ///< Original kernel width
    int64_t orgkD = -1;               ///< Original kernel depth
    int64_t orgCo = -1;               ///< Original output channels
    int64_t coutOpt = -1;             ///< Optimized output channels
    int64_t orgCi = -1;               ///< Original input channels
    int64_t cinOpt = -1;              ///< Optimized input channels
    int64_t orgDi = -1;               ///< Original input depth
    int64_t orgHi = -1;               ///< Original input height
    int64_t orgWi = -1;               ///< Original input width
    int64_t singlekH = -1;            ///< Single kernel height for tiling
    int64_t singlekW = -1;            ///< Single kernel width for tiling
    int64_t singlekD = -1;            ///< Single kernel depth for tiling
    int64_t singleBatch = 1;          ///< Single batch size for tiling
    int64_t singleCi = -1;            ///< Single input channels for tiling
    int64_t singleCo = -1;            ///< Single output channels for tiling
    int64_t singleDo = -1;            ///< Single output depth for tiling
    int64_t singleM = -1;             ///< Single M dimension for tiling
    int64_t singleHo = -1;            ///< Single output height for tiling
    int64_t singleWo = -1;            ///< Single output width for tiling
    int64_t singleCoreGroupOpt = -1;  ///< Single core group optimization factor
};
/**
* @struct Conv3DInputAttr
* @brief Structure for conv3d input attr configuration
*/
struct Conv3DInputAttr {
    int64_t groups = 1;               ///< Number of groups for grouped convolution
    int64_t groupOpt = 1;             ///< Optimized group factor
    int64_t padHead = 0;              ///< Padding at the beginning of depth dimension
    int64_t padTail = 0;              ///< Padding at the end of depth dimension
    int64_t padUp = 0;                ///< Padding at the top of height dimension
    int64_t padDown = 0;              ///< Padding at the bottom of height dimension
    int64_t padLeft = 0;              ///< Padding at the left of width dimension
    int64_t padRight = 0;             ///< Padding at the right of width dimension
    int64_t strideH = 1;              ///< Stride in height dimension
    int64_t strideW = 1;              ///< Stride in width dimension
    int64_t strideD = 1;              ///< Stride in depth dimension
    int64_t dilationH = 1;            ///< Dilation in height dimension
    int64_t dilationW = 1;            ///< Dilation in width dimension
    int64_t dilationD = 1;            ///< Dilation in depth dimension
    int8_t offsetx = 0;               ///< Offset for quantization
    uint8_t hf32Enable = 0;           ///< Flag for half-precision float32 optimization
    uint8_t hf32TransMode = 0;        ///< Transformation mode for half-precision float32
};
/**
* @struct Conv3DCalcShape
* @brief Structure for conv3d input calculate shape configuration
*/
struct Conv3DCalcShape {
    uint64_t singleCi1 = 0;           ///< Calculated single input channels
    uint64_t singleCo1 = 0;           ///< Calculated single output channels
    uint64_t singleM1 = 0;            ///< Calculated single M dimension
    uint64_t orgHo = 0;               ///< Original output height
    uint64_t orgWo = 0;               ///< Original output width
    uint64_t orgDo = 0;               ///< Original output depth
};
/**
* @struct Conv3DDesc
* @brief Structure for conv3d tiling type configuration
*/
struct Conv3DDesc {
    ConvType fMapType = {ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::GM};  ///< Input feature map type
    ConvType weightType = {ConvCommonApi::ConvFormat::FRACTAL_Z_3D, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::GM};  ///< Weight tensor type
    ConvType biasType = {ConvCommonApi::ConvFormat::ND, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::GM};  ///< Bias tensor type
    ConvType outputType = {ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::TPosition::CO1};  ///< Output tensor type
    ConvType quantScaleType = {ConvCommonApi::ConvFormat::ND, ConvCommonApi::ConvDtype::INT64, ConvCommonApi::TPosition::GM};  ///< Quantization scale type
};
/**
* @struct DoubleBufferValue
* @brief Structure for conv3d tiling double buffer configuration
*/
struct DoubleBufferValue {
    uint8_t pbAL1 = 1;                ///< Double buffer count for input in L1
    uint8_t pbBL1 = 1;                ///< Double buffer count for weight in L1
    uint8_t pbAL0 = 2;                ///< Double buffer count for input in L0
    uint8_t pbBL0 = 2;                ///< Double buffer count for weight in L0
    uint8_t pbCL0 = 1;                ///< Double buffer count for output in L0
    uint64_t pBufferFlag = 0;         ///< Buffer optimization flags
};
/**
* @struct CubeInfo
* @brief Structure for conv3d cube calculate configuration
*/
struct CubeInfo {
    uint32_t m0 = 0;                  ///< M dimension tile size for cube operations
    uint32_t k0 = 0;                  ///< K dimension tile size for cube operations
    uint32_t n0 = 0;                  ///< N dimension tile size for cube operations
    ConvCommonApi::ConvDtype madType = ConvCommonApi::ConvDtype::CONVDTYPEMAX;  ///< Matrix multiply-add data type
    ConvCommonApi::ConvDtype biasType = ConvCommonApi::ConvDtype::CONVDTYPEMAX;  ///< Bias data type
    uint32_t minBurstNum = 0;         ///< Minimum burst number for memory access
};

class Conv3dTilingBase {
public:
    explicit Conv3dTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform);
    explicit Conv3dTilingBase(const PlatformInfo& platform);
    virtual ~Conv3dTilingBase() = default;
    /**
    * @brief Interface to get tiling information
    * @param [in] tiling: reference to store the tiling information
    * @note the tiling of this function is in namespace optiling
    */
    virtual int64_t GetTiling(optiling::TConv3DApiTiling& tiling) = 0;
    /**
    * @brief Interface to get tiling information
    * @param [in] tiling: reference to store the tiling information
    * @note the tiling of this function is in global namespace
    */
    virtual int64_t GetTiling(AscendC::tiling::TConv3DApiTiling& tiling) = 0;
    /**
    * @brief Set the original weight shape dimensions
    * @param [in] orgCo: the original cout shape of weight
    * @param [in] orgKd: the original kerneld shape of weight
    * @param [in] orgKh: the original kernelh shape of weight
    * @param [in] orgKw: the original kernelw shape of weight
    */
    void SetOrgWeightShape(int64_t orgCo, int64_t orgKd, int64_t orgKh, int64_t orgKw);
    /**
    * @brief Set the original input feature map shape dimensions
    * @param [in] orgCi: the original channel in shape of input
    * @param [in] orgDi: the original depth shape of input
    * @param [in] orgHi: the original height shape of input
    * @param [in] orgWi: the original width shape of input
    */
    void SetOrgInputShape(int64_t orgCi, int64_t orgDi, int64_t orgHi, int64_t orgWi);

    /**
    * @brief Set the single weight shape dimensions
    * @param [in] singleCi: the channel in shape of single weight
    * @param [in] singleKd: the kernel depth shape of single weight
    * @param [in] singleKh: the kernel height shape of single weight
    * @param [in] singleKw: the kernel width shape of single weight
    */
    void SetSingleWeightShape(int64_t singleCi, int64_t singleKd, int64_t singleKh, int64_t singleKw);

    /**
    * @brief Set the single output shape dimensions
    * @param [in] singleCo: the channel out shape of single output
    * @param [in] singleDo: the depth shape of single output
    * @param [in] singleM: the height out mul width out dimension shape of single output
    */
    void SetSingleOutputShape(int64_t singleCo, int64_t singleDo, int64_t singleM);
    /**
    * @brief Set the weight tensor type configuration
    * @param [in] pos: the tensor position identifier
    * @param [in] format: the data format of weight tensor
    * @param [in] dtype: the data type of weight tensor
    */
    void SetWeightType(const ConvCommonApi::TPosition pos, const ConvCommonApi::ConvFormat format, const ConvCommonApi::ConvDtype dtype);

    /**
    * @brief Set the input tensor type configuration
    * @param [in] pos: the tensor position identifier
    * @param [in] format: the data format of input tensor
    * @param [in] dtype: the data type of input tensor
    */
    void SetInputType(const ConvCommonApi::TPosition pos, const ConvCommonApi::ConvFormat format, const ConvCommonApi::ConvDtype dtype);

    /**
    * @brief Set the bias tensor type configuration
    * @param [in] pos: the tensor position identifier
    * @param [in] format: the data format of bias tensor
    * @param [in] dtype: the data type of bias tensor
    */
    void SetBiasType(const ConvCommonApi::TPosition pos, const ConvCommonApi::ConvFormat format, const ConvCommonApi::ConvDtype dtype);

    /**
    * @brief Set the output tensor type configuration
    * @param [in] pos: the tensor position identifier
    * @param [in] format: the data format of output tensor
    * @param [in] dtype: the data type of output tensor
    */
    void SetOutputType(const ConvCommonApi::TPosition pos, const ConvCommonApi::ConvFormat format, const ConvCommonApi::ConvDtype dtype);

    /**
    * @brief Set the conv3d padding parameters for all dimensions
    * @param [in] padHead: the padding size at the head (depth start)
    * @param [in] padTail: the padding size at the tail (depth end)
    * @param [in] padUp: the padding size at the top (height start)
    * @param [in] padDown: the padding size at the bottom (height end)
    * @param [in] padLeft: the padding size at the left (width start)
    * @param [in] padRight: the padding size at the right (width end)
    */
    void SetPadding(int64_t padHead, int64_t padTail, int64_t padUp, int64_t padDown,
                    int64_t padLeft, int64_t padRight);
    /**
    * @brief Set the conv3d dilation parameters
    * @param [in] dilationD: the dilation value along depth dimension
    * @param [in] dilationH: the dilation value along height dimension
    * @param [in] dilationW: the dilation value along width dimension
    */
    void SetDilation(int64_t dilationD, int64_t dilationH, int64_t dilationW);

    /**
    * @brief Set the conv3d stride parameters
    * @param [in] strideD: the stride value along depth dimension
    * @param [in] strideH: the stride value along height dimension
    * @param [in] strideW: the stride value along width dimension
    */
    void SetStride(int64_t strideD, int64_t strideH, int64_t strideW);

    /**
    * @brief Set the conv3d group configuration
    * @param [in] groups: the number of groups for grouped convolution
    */
    void SetGroups(int64_t groups);

    Conv3DDesc descInfo;
    Conv3DInputshape shapeInfo;
    Conv3DCalcShape shapeCalc;
    Conv3DInputAttr attrInfo;
    CubeInfo cubeInfo;
    Conv3DL1Tiling l1TilingInfo;
    Conv3DL0Tiling l0TilingInfo;
    DoubleBufferValue dbValue;
    PlatformInfo platformInfo;

    bool hasBias = false;
    bool hasQuantScale = false;

    bool hf32Enable_ = false;
    bool hf32TransMode_ = false;

protected:
    virtual int64_t Compute() = 0;
    void SetFinalTilingBasicInfo(optiling::TConv3DApiTiling& tiling);
    void SetFinalTilingBasicInfo(AscendC::tiling::TConv3DApiTiling& tiling);
    void SetFinalTilingDecisionInfo(optiling::TConv3DApiTiling& tiling);
    void SetFinalTilingDecisionInfo(AscendC::tiling::TConv3DApiTiling& tiling);
    void SetFinalTiling(optiling::TConv3DApiTiling& tiling);
    void SetFinalTiling(AscendC::tiling::TConv3DApiTiling& tiling);
    void PrintTilingDataBasicInfo() const;
    void PrintTilingDataDecision() const;
    void PrintTilingData() const;
    bool CheckInputParam();
    bool CheckSocVersion();
    void GetCubeInfo();
    bool ShapeInitCalc();
    bool CheckParamsOverflow();

private:
    bool CheckInputAttr();
    bool CheckOrgInputInfo();
    bool CheckSingleInputInfo();
    bool CheckInputConstraint();
    bool CheckInputShape();
    bool CheckInputFormat();
    bool CheckParamsDtype();
    bool CheckLoad3DLimits();
    bool CheckLoadL1SizeLimits();
    bool CheckInstructionLimits();
    bool CheckHF32();
    bool CheckPaddedInput();
    void SetSingleOutputShape(int64_t singleCo, int64_t singleDo, int64_t singleHo, int64_t singleWo);
    void SetHF32(bool hf32Enable, bool hf32TransMode);
};
} // namespace Conv3dTilingApi

#endif // ADV_API_CONV_CONV3D_CONV3D_TILING_BASE_H