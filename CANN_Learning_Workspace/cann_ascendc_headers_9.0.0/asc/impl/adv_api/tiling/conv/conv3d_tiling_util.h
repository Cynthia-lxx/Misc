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
 * \file conv3d_tiling_util.h
 * \brief
 */

#ifndef API_ASCENDC_TIKCFW_TILING_CONV3D_TILING_UTIL_H
#define API_ASCENDC_TIKCFW_TILING_CONV3D_TILING_UTIL_H

#include <iostream>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "tiling/platform/platform_ascendc.h"
#include "../../detail/conv/common/conv_common.h"

namespace Conv3dTilingApi {
enum class IterateMNOrder : uint32_t {
    ITER_M_FST = 0,
    ITER_N_FST,
    INVALID
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
    {ConvCommonApi::ConvDtype::CONVDTYPEMAX, "UNKNOWN_DTYPE"}
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
    {ConvCommonApi::ConvFormat::FRACTAL_Z_3D, "FRACTAL_Z_3D"},
    {ConvCommonApi::ConvFormat::MAX, "UNKNOWN_FORMAT"}
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


const uint32_t COUNT_PARAMS_BIAS = 4; // [input, weight, bias, output]
const uint32_t COUNT_PARAMS_NO_BIAS = 3; // [input, weight, output]
/* 
 * int8 dtype: {ConvCommonApi::ConvDtype::INT8, ConvCommonApi::ConvDtype::INT8, ConvCommonApi::ConvDtype::INT32, ConvCommonApi::ConvDtype::FLOAT16}
 */
const std::vector<std::vector<ConvCommonApi::ConvDtype>> SUPPORTED_TYPES_WITH_BIAS = {
    {ConvCommonApi::ConvDtype::BF16, ConvCommonApi::ConvDtype::BF16, ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::BF16},
    {ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT16}
};
/*
 * int8 dtype: {ConvCommonApi::ConvDtype::INT8, ConvCommonApi::ConvDtype::INT8, ConvCommonApi::ConvDtype::FLOAT16}
 */
const std::vector<std::vector<ConvCommonApi::ConvDtype>> SUPPORTED_TYPES_WITHOUT_BIAS = {
    {ConvCommonApi::ConvDtype::BF16, ConvCommonApi::ConvDtype::BF16, ConvCommonApi::ConvDtype::BF16},
    {ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT16}
};

const std::vector<std::vector<ConvCommonApi::ConvDtype>> POINTWISE_SUPPORTED_TYPES_WITH_BIAS = {
    {ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT16},
    {ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT32},
    {ConvCommonApi::ConvDtype::BF16, ConvCommonApi::ConvDtype::BF16, ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::BF16}
};

const std::vector<std::vector<ConvCommonApi::ConvDtype>> POINTWISE_SUPPORTED_TYPES_WITHOUT_BIAS = {
    {ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT16},
    {ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT32},
    {ConvCommonApi::ConvDtype::BF16, ConvCommonApi::ConvDtype::BF16, ConvCommonApi::ConvDtype::BF16}
};

const std::map<platform_ascendc::SocVersion, std::string> SOC_NAME_TAB = {
    {platform_ascendc::SocVersion::ASCEND910, "ASCEND910"},
    {platform_ascendc::SocVersion::ASCEND910B, "ASCEND910B"},
    {platform_ascendc::SocVersion::ASCEND910_93, "ASCEND910_93"},
    {platform_ascendc::SocVersion::ASCEND310P, "ASCEND310P"},
    {platform_ascendc::SocVersion::ASCEND310B, "ASCEND310B"},
    {platform_ascendc::SocVersion::RESERVED_VERSION, "RESERVED_VERSION"}
};

struct ConvType {
    ConvCommonApi::ConvFormat format;
    ConvCommonApi::ConvDtype dtype;
    matmul_tiling::TPosition pos;
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

struct Conv3DOriGroupInfo {
    int64_t groups = -1;
    int64_t cin = -1;
    int64_t cout = -1;
    ConvCommonApi::ConvDtype dtype = ConvCommonApi::ConvDtype::CONVDTYPEMAX;
};

struct Conv3DGroupOptInfo {
    int64_t groupOpt = -1;
    int64_t cinOpt = -1;
    int64_t coutOpt = -1;
};

constexpr uint64_t DOUBLE_BUFFER_NUM = 2;

constexpr uint32_t L1_SIZE = 524288 - 256;
constexpr uint32_t L0A_SIZE = 65536;
constexpr uint32_t L0B_SIZE = 65536;
constexpr uint32_t L0C_SIZE = 131072;
constexpr uint32_t UB_SIZE = 262144;
constexpr uint32_t BT_SIZE = 1024;
constexpr uint32_t FB_SIZE = 2048;

constexpr uint32_t C0_BYTE_SIZE = 32;
constexpr uint32_t MIN_BURST_SIZE = 128;
constexpr uint32_t LOAD3D_MAX_STRIDE_H_W = 63;
constexpr uint32_t LOAD3D_MAX_DILATION_H_W = 255;
constexpr uint32_t LOAD3D_MAX_PAD = 255;
constexpr uint32_t LOAD3D_MAX_FILTER_H_W = 511;
constexpr uint32_t LOAD3D_MAX_DDR2L1_SIZE = 65535;

constexpr uint32_t BLOCK_N0 = 16;
constexpr uint32_t BLOCK_M0 = 16;
constexpr uint32_t FP16_CUBE_UNIT = 16;
constexpr uint32_t FP32_CUBE_UNIT = 8;
constexpr uint32_t INT8_CUBE_UNIT = 32;
constexpr uint32_t MKN_MAX_SIZE = 3;
constexpr uint32_t MKN_M_INDEX = 0;
constexpr uint32_t MKN_K_INDEX = 1;
constexpr uint32_t MKN_N_INDEX = 2;
constexpr uint32_t MKN_VALUE_DEFAULT = 16;

constexpr uint64_t MAX_64_BIT_NUM = 0xFFFFFFFFFFFFFFFFU;
constexpr uint64_t MAX_40_BIT_NUM = 1099511627775;
constexpr uint64_t MAX_32_BIT_NUM = 4294967295;
constexpr uint32_t MAX_16_BIT_NUM = 65535;

constexpr uint32_t CONST_VALUE_2 = 2;
constexpr uint32_t BAND_WIDTH_COEFF = 4;
constexpr uint32_t C0_SIZE = 32;

// load3dv2 postk limit
constexpr uint32_t POSTK_LIMIT = 65535;

// shape limit
constexpr int64_t MAX_ONE_DIM_SIZE = 1000000;
constexpr int64_t MAX_ORI_INPUT_SIZE = 10000000;
constexpr uint64_t MAX_OUT_ONE_DIM_SIZE = 1000000;

struct AscendApiCubeTypeMap {
public:
    struct {
        ConvCommonApi::ConvDtype madType;
        ConvCommonApi::ConvDtype biasType;
    } typeMaps[static_cast<uint8_t>(ConvCommonApi::ConvDtype::CONVDTYPEMAX) + 1] = {
        {ConvCommonApi::ConvDtype::CONVDTYPEMAX, ConvCommonApi::ConvDtype::CONVDTYPEMAX}
    };
    
    ConvCommonApi::ConvDtype ToBiasType(ConvCommonApi::ConvDtype type) const
    {
        return typeMaps[static_cast<uint8_t>(type)].biasType;
    }
    ConvCommonApi::ConvDtype ToMadType(ConvCommonApi::ConvDtype type) const
    {
        return typeMaps[static_cast<uint8_t>(type)].madType;
    }

    AscendApiCubeTypeMap()
    {
        SetMapping(ConvCommonApi::ConvDtype::INT4, ConvCommonApi::ConvDtype::INT32, ConvCommonApi::ConvDtype::INT32);
        SetMapping(ConvCommonApi::ConvDtype::INT8, ConvCommonApi::ConvDtype::INT32, ConvCommonApi::ConvDtype::INT32);
        SetMapping(ConvCommonApi::ConvDtype::UINT8, ConvCommonApi::ConvDtype::INT32, ConvCommonApi::ConvDtype::INT32);
        SetMapping(ConvCommonApi::ConvDtype::FLOAT16, ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT32);
        SetMapping(ConvCommonApi::ConvDtype::BF16, ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT32);
        SetMapping(ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT32, ConvCommonApi::ConvDtype::FLOAT32);
    }

private:
    void SetMapping(ConvCommonApi::ConvDtype key, ConvCommonApi::ConvDtype madType, ConvCommonApi::ConvDtype biasType)
    {
        typeMaps[static_cast<uint8_t>(key)].madType = madType;
        typeMaps[static_cast<uint8_t>(key)].biasType = biasType;
    }
};

struct AscendApiMknMap {
    int32_t GetByIndex(uint32_t idx) const
    {
        if (idx > MKN_MAX_SIZE - 1) {
        return MKN_VALUE_DEFAULT;
        }
        return map[idx];
    }
    int8_t map[MKN_MAX_SIZE];
};
 
struct AscendApiCubeMkn {
    int8_t toIdx[static_cast<uint8_t>(ConvCommonApi::ConvDtype::CONVDTYPEMAX) + 1] = {0};
    AscendApiMknMap maps[3] = {{BLOCK_M0, FP16_CUBE_UNIT, BLOCK_N0}, // fp16
                               {BLOCK_M0, FP32_CUBE_UNIT, BLOCK_N0}, // fp32
                               {BLOCK_M0, INT8_CUBE_UNIT, BLOCK_N0}}; // int8
    uint32_t GetMKN(ConvCommonApi::ConvDtype dType, uint32_t idx) const
    {
        return maps[toIdx[static_cast<uint8_t>(dType)]].GetByIndex(idx);
    }
    AscendApiCubeMkn()
    {
        toIdx[static_cast<uint8_t>(ConvCommonApi::ConvDtype::FLOAT16)] = 0;
        toIdx[static_cast<uint8_t>(ConvCommonApi::ConvDtype::FLOAT32)] = 1;
        toIdx[static_cast<uint8_t>(ConvCommonApi::ConvDtype::BF16)] = 0;
        toIdx[static_cast<uint8_t>(ConvCommonApi::ConvDtype::INT8)] = CONST_VALUE_2;
    }
};

const AscendApiCubeTypeMap CUBE_TYPE_TAB = AscendApiCubeTypeMap();
const AscendApiCubeMkn CUBE_MKN_TAB = AscendApiCubeMkn();

int64_t LCM(int64_t numL, int64_t numR);
uint64_t CeilDiv(uint64_t a, uint64_t b);
uint64_t AlignB(uint64_t a, uint64_t b);
uint64_t Gcd(uint64_t a, uint64_t b);
void CalcCommFactorWithPowerOfTwo(const uint64_t num, const uint64_t numMax, std::vector<uint64_t> &resList);
void CalcCommFactor(const uint64_t num, const uint64_t numMax, std::vector<uint64_t> &resList);
void CalcFactorPointWise(uint64_t numMax, std::vector<uint64_t> &resList);
void VectorElementMultip(std::vector<uint64_t> &range, const uint64_t value);
bool IsArrayEqual(const std::vector<ConvCommonApi::ConvDtype>& arr1, const std::vector<ConvCommonApi::ConvDtype>& arr2, uint32_t size);
uint64_t InferHiL1(uint64_t inputHoL1, uint64_t hi, uint64_t singlekH, uint32_t dilationH, uint32_t strideH);

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool AddWithOverflowCheck(T &res, T a, T b)
{
    T tmpRes = a + b;
    if (tmpRes < a || tmpRes < b) {
        return true;
    }
    if ((a > 0 && b > 0) && tmpRes < 0) {
        return true;
    }
    if ((a < 0 && b < 0) && tmpRes >= 0) {
        return true;
    }
    res = tmpRes;
    return false;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool MulWithOverflowCheck(T &res, T a, T b)
{
    if (a == 0 || b == 0) {
        res = 0;
        return false;
    }
    T tmpRes = a * b;
    if (tmpRes / a != b) {
        return true;
    }
    res = tmpRes;
    return false;
}

// Control the number of parameters passed during invocation to avoid stack overflow
template <typename T, typename... Args, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool MulWithOverflowCheck(T &res, T a, T b, Args... args)
{
    T tmp;
    return MulWithOverflowCheck(tmp, a, b) || MulWithOverflowCheck(res, tmp, args...);
}

} // namespace Conv3dTilingApi

#endif
