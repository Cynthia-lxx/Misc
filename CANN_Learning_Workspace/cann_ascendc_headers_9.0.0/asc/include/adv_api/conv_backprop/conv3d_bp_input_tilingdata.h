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
 * \file conv3d_bp_input_tilingdata.h
 * \brief
 */
#ifndef CONV3D_BP_INPUT_TILINGDATA_H
#define CONV3D_BP_INPUT_TILINGDATA_H

#include "register/tilingdata_base.h"

namespace optiling {

BEGIN_TILING_DATA_DEF(TConv3DBackpropInputTiling)
TILING_DATA_FIELD_DEF(uint32_t, batch);
TILING_DATA_FIELD_DEF(uint32_t, cin);
TILING_DATA_FIELD_DEF(uint32_t, cout);
TILING_DATA_FIELD_DEF(uint32_t, cout1);
TILING_DATA_FIELD_DEF(uint32_t, cin1);
TILING_DATA_FIELD_DEF(uint32_t, cout1G);
TILING_DATA_FIELD_DEF(uint32_t, cin1G);
TILING_DATA_FIELD_DEF(uint32_t, c0);
TILING_DATA_FIELD_DEF(uint32_t, c0Bits);
TILING_DATA_FIELD_DEF(uint32_t, dout);
TILING_DATA_FIELD_DEF(uint32_t, ho);
TILING_DATA_FIELD_DEF(uint32_t, wo);
TILING_DATA_FIELD_DEF(uint32_t, di);
TILING_DATA_FIELD_DEF(uint32_t, hi);
TILING_DATA_FIELD_DEF(uint32_t, wi);
TILING_DATA_FIELD_DEF(uint32_t, dk);
TILING_DATA_FIELD_DEF(uint32_t, hk);
TILING_DATA_FIELD_DEF(uint32_t, wk);
TILING_DATA_FIELD_DEF(uint32_t, group);
TILING_DATA_FIELD_DEF(uint32_t, strideD);
TILING_DATA_FIELD_DEF(uint32_t, strideH);
TILING_DATA_FIELD_DEF(uint32_t, strideW);
TILING_DATA_FIELD_DEF(uint32_t, padFront);
TILING_DATA_FIELD_DEF(uint32_t, padBack);
TILING_DATA_FIELD_DEF(uint32_t, padUp);
TILING_DATA_FIELD_DEF(uint32_t, padDown);
TILING_DATA_FIELD_DEF(uint32_t, padLeft);
TILING_DATA_FIELD_DEF(uint32_t, padRight);
TILING_DATA_FIELD_DEF(uint32_t, backpropPadTail);
TILING_DATA_FIELD_DEF(uint32_t, backpropPadUp);
TILING_DATA_FIELD_DEF(uint32_t, backpropPadDown);
TILING_DATA_FIELD_DEF(uint32_t, backpropPadLeft);
TILING_DATA_FIELD_DEF(uint32_t, backpropPadRight);
TILING_DATA_FIELD_DEF(uint32_t, dilationD);
TILING_DATA_FIELD_DEF(uint32_t, dilationH);
TILING_DATA_FIELD_DEF(uint32_t, dilationW);
TILING_DATA_FIELD_DEF(uint32_t, al0Pbuffer);
TILING_DATA_FIELD_DEF(uint32_t, bl0Pbuffer);
TILING_DATA_FIELD_DEF(uint32_t, cl0Pbuffer);
TILING_DATA_FIELD_DEF(uint32_t, al1Pbuffer);
TILING_DATA_FIELD_DEF(uint32_t, bl1Pbuffer);
TILING_DATA_FIELD_DEF(uint32_t, singleCoreGroup);
TILING_DATA_FIELD_DEF(uint32_t, singleCoreCout);
TILING_DATA_FIELD_DEF(uint32_t, singleCoreCout1);
TILING_DATA_FIELD_DEF(uint32_t, singleCoreCin1);
TILING_DATA_FIELD_DEF(uint32_t, singleCoreDin);
TILING_DATA_FIELD_DEF(uint32_t, singleCoreHo);
TILING_DATA_FIELD_DEF(uint32_t, baseM);
TILING_DATA_FIELD_DEF(uint32_t, baseK);
TILING_DATA_FIELD_DEF(uint32_t, baseN);
TILING_DATA_FIELD_DEF(uint32_t, baseD);
TILING_DATA_FIELD_DEF(uint32_t, baseBatch);
TILING_DATA_FIELD_DEF(uint32_t, baseGroup);
TILING_DATA_FIELD_DEF(uint32_t, stepM);
TILING_DATA_FIELD_DEF(uint32_t, stepN);
TILING_DATA_FIELD_DEF(uint32_t, stepKa);
TILING_DATA_FIELD_DEF(uint32_t, stepKb);
TILING_DATA_FIELD_DEF(uint32_t, stepBatch);
TILING_DATA_FIELD_DEF(uint32_t, stepGroup);
TILING_DATA_FIELD_DEF(uint32_t, iterateOrder);
TILING_DATA_FIELD_DEF(int32_t, hf32Flag);
TILING_DATA_FIELD_DEF(int32_t, initOutputFlag);
TILING_DATA_FIELD_DEF(int32_t, reserved);
TILING_DATA_FIELD_DEF(uint64_t, singleCoreBatch);
TILING_DATA_FIELD_DEF(uint64_t, singleCoreM);
TILING_DATA_FIELD_DEF(uint64_t, singleCoreCin);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(TConv3DBackpropInputTilingOpApi, TConv3DBackpropInputTiling);

BEGIN_TILING_DATA_DEF(Conv3DBackpropInputTilingData)
TILING_DATA_FIELD_DEF_STRUCT(TConv3DBackpropInputTiling, conv3DDxTiling);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(Conv3DBackpropInputTilingDataOpApi, Conv3DBackpropInputTilingData)
}  // namespace optiling
#endif  // CONV3D_BP_INPUT_TILINGDATA_H