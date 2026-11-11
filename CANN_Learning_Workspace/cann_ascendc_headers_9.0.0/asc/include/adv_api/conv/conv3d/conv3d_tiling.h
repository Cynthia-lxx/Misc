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

#ifndef ADV_API_CONV_CONV3D_CONV3D_TILING_H
#define ADV_API_CONV_CONV3D_CONV3D_TILING_H

#include "conv3d_tiling_base.h"
#include "conv3d_tilingdata.h"

namespace Conv3dTilingApi {
class Conv3dTiling : public Conv3dTilingBase {
public:
    /**
     * @brief Constructor with AscendC platform context
     * @param ascendcPlatform Reference to AscendC platform instance
     */
    explicit Conv3dTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        : Conv3dTilingBase(ascendcPlatform) {};

    /**
     * @brief Constructor with platform information
     * @param platform Platform information structure
     */
    explicit Conv3dTiling(const PlatformInfo& platform) : Conv3dTilingBase(platform) {};
    ~Conv3dTiling() override = default;

    /**
     * @brief Get tiling configuration for 3D convolution
     * @param tiling Reference to tiling structure to be filled
     * @return Status code indicating success or failure
     */
    int64_t GetTiling(optiling::TConv3DApiTiling &tiling) override;

    /**
     * @brief Get tiling configuration for 3D convolution (AscendC version)
     * @param tiling Reference to AscendC tiling structure to be filled
     * @return Status code indicating success or failure
     */
    int64_t GetTiling(AscendC::tiling::TConv3DApiTiling& tiling) override;

protected:
    int64_t Compute() override;
};
} // namespace Conv3dTilingApi

#endif // ADV_API_CONV_CONV3D_CONV3D_TILING_H