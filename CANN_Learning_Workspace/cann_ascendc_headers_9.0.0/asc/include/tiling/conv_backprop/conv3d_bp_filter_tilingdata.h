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
 * \file conv3d_bp_filter_tilingdata.h
 * \brief
 */
#ifndef CONV3D_BP_FILTER_TILINGDATA_H
#define CONV3D_BP_FILTER_TILINGDATA_H

#include "register/tilingdata_base.h"

namespace optiling {
BEGIN_TILING_DATA_DEF(TConv3DBpFilterTiling)
    TILING_DATA_FIELD_DEF(uint32_t, batch);
    TILING_DATA_FIELD_DEF(uint32_t, cin);
    TILING_DATA_FIELD_DEF(uint32_t, cout);
    TILING_DATA_FIELD_DEF(uint32_t, cin1G);
    TILING_DATA_FIELD_DEF(uint32_t, cout1G);
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
    TILING_DATA_FIELD_DEF(uint32_t, dilationD);
    TILING_DATA_FIELD_DEF(uint32_t, dilationH);
    TILING_DATA_FIELD_DEF(uint32_t, dilationW);
    TILING_DATA_FIELD_DEF(uint32_t, channelSize);
    TILING_DATA_FIELD_DEF(uint32_t, al0Pbuffer);
    TILING_DATA_FIELD_DEF(uint32_t, bl0Pbuffer);
    TILING_DATA_FIELD_DEF(uint32_t, cl0Pbuffer);
    TILING_DATA_FIELD_DEF(uint32_t, al1Pbuffer);
    TILING_DATA_FIELD_DEF(uint32_t, bl1Pbuffer);
    TILING_DATA_FIELD_DEF(uint32_t, baseM);
    TILING_DATA_FIELD_DEF(uint32_t, baseK);
    TILING_DATA_FIELD_DEF(uint32_t, baseN);
    TILING_DATA_FIELD_DEF(uint32_t, m0);
    TILING_DATA_FIELD_DEF(uint32_t, k0);
    TILING_DATA_FIELD_DEF(uint32_t, n0);
    TILING_DATA_FIELD_DEF(uint32_t, stepM);
    TILING_DATA_FIELD_DEF(uint32_t, stepN);
    TILING_DATA_FIELD_DEF(uint32_t, stepKa);
    TILING_DATA_FIELD_DEF(uint32_t, stepKb);
    TILING_DATA_FIELD_DEF(uint32_t, iterateOrder);
    TILING_DATA_FIELD_DEF(uint32_t, bl1Bound);
    TILING_DATA_FIELD_DEF(uint32_t, hf32Flag);
    TILING_DATA_FIELD_DEF(uint32_t, singleCoreDk);
    TILING_DATA_FIELD_DEF(uint32_t, singleCoreGroup);
    TILING_DATA_FIELD_DEF(uint32_t, singleCoreCout);
    TILING_DATA_FIELD_DEF(uint32_t, singleCoreHo);
    TILING_DATA_FIELD_DEF(uint64_t, singleCoreBatch);
    TILING_DATA_FIELD_DEF(uint64_t, singleCoreCin);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(TConv3DBpFilterTilingOpApi, TConv3DBpFilterTiling);

BEGIN_TILING_DATA_DEF(Conv3DBpFilterParams)
    TILING_DATA_FIELD_DEF(uint32_t, totalL1Size);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(Conv3DBpFilterParamsOpApi, Conv3DBpFilterParams)

BEGIN_TILING_DATA_DEF(TConv3DBpFilterBasicBlockTiling)
    TILING_DATA_FIELD_DEF(uint32_t, singleCoreM);
    TILING_DATA_FIELD_DEF(uint32_t, singleCoreN);
    TILING_DATA_FIELD_DEF(uint32_t, singleCoreK);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(TConv3DBpFilterBasicBlockTilingOpApi, TConv3DBpFilterBasicBlockTiling)

BEGIN_TILING_DATA_DEF(Conv3DBackpropFilterTilingData)
    TILING_DATA_FIELD_DEF_STRUCT(Conv3DBpFilterParams, params);
    TILING_DATA_FIELD_DEF_STRUCT(TConv3DBpFilterTiling, dwTiling);
    TILING_DATA_FIELD_DEF_STRUCT(TConv3DBpFilterBasicBlockTiling, basicBlockTiling);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(Conv3DBackpropFilterTilingDataOpApi, Conv3DBackpropFilterTilingData)
}
#endif // CONV3D_BP_FILTER_TILINGDATA_H