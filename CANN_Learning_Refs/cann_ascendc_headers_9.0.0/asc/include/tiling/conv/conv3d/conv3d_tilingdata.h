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
 * \file conv3d_tiling.h
 * \brief
 */
 
#ifndef ADV_API_CONV_CONV3D_CONV3D_TILINGDATA_H
#define ADV_API_CONV_CONV3D_CONV3D_TILINGDATA_H
 
#include "register/tilingdata_base.h"
 
namespace optiling {
    BEGIN_TILING_DATA_DEF(TConv3DApiTiling)
    TILING_DATA_FIELD_DEF(uint64_t, orgDo);
    TILING_DATA_FIELD_DEF(uint32_t, orgCo);
    TILING_DATA_FIELD_DEF(uint64_t, orgHo);
    TILING_DATA_FIELD_DEF(uint64_t, orgWo);
    TILING_DATA_FIELD_DEF(uint64_t, orgDi);
    TILING_DATA_FIELD_DEF(uint32_t, orgCi);
    TILING_DATA_FIELD_DEF(uint64_t, orgHi);
    TILING_DATA_FIELD_DEF(uint64_t, orgWi);
    TILING_DATA_FIELD_DEF(uint32_t, kernelD);
    TILING_DATA_FIELD_DEF(uint32_t, kernelH);
    TILING_DATA_FIELD_DEF(uint32_t, kernelW);
    TILING_DATA_FIELD_DEF(uint64_t, singleCoreDo);
    TILING_DATA_FIELD_DEF(uint32_t, singleCoreCo);
    TILING_DATA_FIELD_DEF(uint64_t, singleCoreM);
    TILING_DATA_FIELD_DEF(uint32_t, singleCoreGroupOpt);
    TILING_DATA_FIELD_DEF(uint32_t, groups);
    TILING_DATA_FIELD_DEF(uint32_t, strideH);
    TILING_DATA_FIELD_DEF(uint32_t, strideW);
    TILING_DATA_FIELD_DEF(uint32_t, strideD);
    TILING_DATA_FIELD_DEF(uint32_t, dilationH);
    TILING_DATA_FIELD_DEF(uint32_t, dilationW);
    TILING_DATA_FIELD_DEF(uint32_t, dilationD);
    TILING_DATA_FIELD_DEF(uint32_t, padHead);
    TILING_DATA_FIELD_DEF(uint32_t, padTail);
    TILING_DATA_FIELD_DEF(uint32_t, padUp);
    TILING_DATA_FIELD_DEF(uint32_t, padDown);
    TILING_DATA_FIELD_DEF(uint32_t, padLeft);
    TILING_DATA_FIELD_DEF(uint32_t, padRight);
    TILING_DATA_FIELD_DEF(uint32_t, mL0);
    TILING_DATA_FIELD_DEF(uint32_t, kL0);
    TILING_DATA_FIELD_DEF(uint32_t, nL0);
    TILING_DATA_FIELD_DEF(uint32_t, kAL1);
    TILING_DATA_FIELD_DEF(uint32_t, kAL1Tail);
    TILING_DATA_FIELD_DEF(uint32_t, kBL1);
    TILING_DATA_FIELD_DEF(uint32_t, kBL1Tail);
    TILING_DATA_FIELD_DEF(uint32_t, nBL1);
    TILING_DATA_FIELD_DEF(uint32_t, mAL1);
    TILING_DATA_FIELD_DEF(uint32_t, kBL1DivK0);
    TILING_DATA_FIELD_DEF(uint32_t, kBL1TailDivK0);
    TILING_DATA_FIELD_DEF(uint32_t, nBL1DivnL0);
    TILING_DATA_FIELD_DEF(uint32_t, mAL1DivmL0);
    TILING_DATA_FIELD_DEF(uint32_t, cin1InAL1);
    TILING_DATA_FIELD_DEF(uint32_t, cin1InAL1Tail);
    TILING_DATA_FIELD_DEF(uint32_t, nL0xk0);
    TILING_DATA_FIELD_DEF(uint64_t, kL0xorgCoAlignN0);
    TILING_DATA_FIELD_DEF(uint64_t, kernelHxkernelW);
    TILING_DATA_FIELD_DEF(uint64_t, cin1xOriHixOriWixk0);
    TILING_DATA_FIELD_DEF(uint64_t, oriHixOriWixk0);
    TILING_DATA_FIELD_DEF(uint64_t, oriWixk0);
    TILING_DATA_FIELD_DEF(uint64_t, orgHixWi);
    TILING_DATA_FIELD_DEF(uint64_t, orgHoxWo);
    TILING_DATA_FIELD_DEF(uint32_t, pBufferFlag);
    TILING_DATA_FIELD_DEF(uint32_t, groupOpt);
    TILING_DATA_FIELD_DEF(uint32_t, cinOpt);
    TILING_DATA_FIELD_DEF(uint32_t, coutOpt);
    TILING_DATA_FIELD_DEF(int8_t, offsetx);
    TILING_DATA_FIELD_DEF(uint8_t, bl1FullLoad);
    TILING_DATA_FIELD_DEF(uint8_t, al1FullLoad);
    TILING_DATA_FIELD_DEF(uint8_t, bl1BypassFlag);
    TILING_DATA_FIELD_DEF(uint8_t, iterateMNOrder);
    TILING_DATA_FIELD_DEF(uint8_t, biasFullLoadFlag);
    TILING_DATA_FIELD_DEF(uint8_t, fixpParamsFullLoadFlag);
    TILING_DATA_FIELD_DEF(uint8_t, hf32Enable);
    TILING_DATA_FIELD_DEF(uint8_t, hf32TransMode);
    TILING_DATA_FIELD_DEF(uint8_t, resvered1);
    TILING_DATA_FIELD_DEF(uint16_t, resvered2);
    TILING_DATA_FIELD_DEF(uint32_t, resvered3);
 
    END_TILING_DATA_DEF;
    REGISTER_TILING_DATA_CLASS(TConv3DApiTilingOpApi, TConv3DApiTiling);
}
#endif // ADV_API_CONV_CONV3D_CONV3D_TILINGDATA_H