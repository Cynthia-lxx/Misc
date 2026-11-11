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
 * \file conv3d_bp_tiling_base.h
 * \brief
 */
#ifndef CONV3D_BP_TILING_BASE_H
#define CONV3D_BP_TILING_BASE_H

#include <map>
#include "tiling/platform/platform_ascendc.h"

#include "../../../impl/adv_api/detail/conv/common/conv_common.h"

namespace ConvBackpropApi {
enum OpType : size_t {
  kConv3DBackpropFilter,
  kConv3DBackpropInput,
  kConv3DTranspose
};

struct PlatformInfo {
    platform_ascendc::SocVersion socVersion = platform_ascendc::SocVersion::RESERVED_VERSION;
    uint64_t l1Size = 0;
    uint64_t l0CSize = 0;
    uint64_t ubSize = 0;
    uint64_t l0ASize = 0;
    uint64_t l0BSize = 0;
    uint64_t btSize = 0;
    uint64_t fbSize = 0;
};

const std::map<ConvCommonApi::ConvDtype, uint32_t> g_dtypeSizeTab = {
    {ConvCommonApi::ConvDtype::FLOAT16, 2},
    {ConvCommonApi::ConvDtype::FLOAT32, 4},
    {ConvCommonApi::ConvDtype::BF16, 2},
    {ConvCommonApi::ConvDtype::INT8, 1},
    {ConvCommonApi::ConvDtype::UINT8, 1},
    {ConvCommonApi::ConvDtype::INT64, 8},
    {ConvCommonApi::ConvDtype::UINT64, 8},
    {ConvCommonApi::ConvDtype::INT32, 4}
};

const std::map<ConvCommonApi::ConvFormat, std::string> g_formatToStr = {
    {ConvCommonApi::ConvFormat::NCHW, "NCHW"},
    {ConvCommonApi::ConvFormat::NHWC, "NHWC"},
    {ConvCommonApi::ConvFormat::HWCN, "HWCN"},
    {ConvCommonApi::ConvFormat::DHWNC, "DHWNC"},
    {ConvCommonApi::ConvFormat::DHWCN, "DHWCN"},
    {ConvCommonApi::ConvFormat::NDHWC, "NDHWC"},
    {ConvCommonApi::ConvFormat::NCDHW, "NCDHW"},
    {ConvCommonApi::ConvFormat::NC1HWC0, "NC1HWC0"},
    {ConvCommonApi::ConvFormat::ND, "ND"},
    {ConvCommonApi::ConvFormat::NDC1HWC0, "NDC1HWC0"},
    {ConvCommonApi::ConvFormat::FRACTAL_Z_3D, "FRACTAL_Z_3D"}
};

const std::map<ConvCommonApi::ConvDtype, std::string> g_dtypeToStr = {
    {ConvCommonApi::ConvDtype::FLOAT16, "float16"},
    {ConvCommonApi::ConvDtype::FLOAT32, "float32"},
    {ConvCommonApi::ConvDtype::BF16, "bfloat16"},
    {ConvCommonApi::ConvDtype::INT4, "int4"},
    {ConvCommonApi::ConvDtype::INT8, "int8"},
    {ConvCommonApi::ConvDtype::UINT8, "uint8"},
    {ConvCommonApi::ConvDtype::INT64, "int64"},
    {ConvCommonApi::ConvDtype::UINT64, "uint64"},
    {ConvCommonApi::ConvDtype::INT32, "int32"},
};

struct ConvBpType {
    ConvCommonApi::ConvFormat format;
    ConvCommonApi::ConvDtype dtype;
    ConvCommonApi::TPosition pos;
};

// shapeInfo
struct Conv3DBackPropShape {
    int64_t orgN = -1;
    int64_t orgCi = -1;
    int64_t orgDi = -1;
    int64_t orgHi = -1;
    int64_t orgWi = -1;

    int64_t orgkD = -1;
    int64_t orgkH = -1;
    int64_t orgkW = -1;

    int64_t orgCo = -1;
    int64_t orgDo = -1;
    int64_t orgHo = -1;
    int64_t orgWo = -1;
};

// attrInfo
struct Conv3DBpAttr {
    int64_t groups = 1;

    int64_t padFront = 0;
    int64_t padBack = 0;
    int64_t padUp = 0;
    int64_t padDown = 0;
    int64_t padLeft = 0;
    int64_t padRight = 0;

    // backpropPad参数暂仅dx使用
    int64_t backpropPadHead = 0;
    int64_t backpropPadTail = 0;
    int64_t backpropPadUp = 0;
    int64_t backpropPadDown = 0;
    int64_t backpropPadLeft = 0;
    int64_t backpropPadRight = 0;

    int64_t strideD = 1;
    int64_t strideH = 1;
    int64_t strideW = 1;

    int64_t dilationD = 1;
    int64_t dilationH = 1;
    int64_t dilationW = 1;

    int64_t outputPadD = 0;
    int64_t outputPadH = 0;
    int64_t outputPadW = 0;

    uint8_t hf32Enable = 0;
};

}  // namespace optiling
#endif  // CONV3D_BP_TILING_BASE_H