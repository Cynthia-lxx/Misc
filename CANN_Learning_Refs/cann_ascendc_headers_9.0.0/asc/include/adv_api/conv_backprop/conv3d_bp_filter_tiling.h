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
 * \file conv3d_bp_filter_tiling.h
 * \brief
 */

#ifndef CONV3D_BP_FILTER_TILING_H
#define CONV3D_BP_FILTER_TILING_H

#include "conv3d_bp_filter_tiling_base.h"
#include "conv3d_bp_filter_tilingdata.h"

namespace ConvBackpropApi {
class Conv3dBpFilterTiling : public Conv3dBpFilterTilingBase {
public:
    Conv3dBpFilterTiling() {};
    explicit Conv3dBpFilterTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        : Conv3dBpFilterTilingBase(ascendcPlatform) {};
    explicit Conv3dBpFilterTiling(const PlatformInfo& platform) : Conv3dBpFilterTilingBase(platform) {};
    ~Conv3dBpFilterTiling() override = default;
    int64_t GetTiling(optiling::Conv3DBackpropFilterTilingData& tiling) override;
    int64_t GetTiling(AscendC::tiling::Conv3DBackpropFilterTilingData& tiling) override;

protected:
    int64_t Compute() override;
    void PrintTilingData() const override;
    void SetFinalTiling(optiling::Conv3DBackpropFilterTilingData& tiling) override;
    void SetFinalTiling(AscendC::tiling::Conv3DBackpropFilterTilingData& tiling) override;
    void SetFinalBasickBlockTiling(optiling::Conv3DBackpropFilterTilingData& tiling);
    void SetFinalBasickBlockTiling(AscendC::tiling::Conv3DBackpropFilterTilingData& tiling);
    void InitTilingValue();
    void ReCalDilation();

    void InitBaseMNK();
    void UpdateStepMNK();
    void UpdateSingleCoreInfo();
    void MultiCoreSplitK();
    void MultiCoreSplitMN();
    void SetBasicBlockAttrsTiling();
    void ShrinkBaseBlock();
    void ShrinkBlockBaseMN();
    bool ShrinkBlockBaseK();
    uint32_t CalculateBl1Cin1CopyLen(uint32_t newBaseN);
    uint64_t CalculateL1SizeGap();
    bool IsCurBlockL1L0Invalid();
    uint64_t CalBL1Bound() const;
    TilingValueDw tilingParams;
    bool seperateDk_ = true;
};

} // namespace ConvBackpropApi

#endif // CONV3D_BP_FILTER_TILING_H