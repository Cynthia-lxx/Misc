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
 * \file transdata_tiling.h
 * \brief
 */
#ifndef LIB_TRANSPOSE_TRANSDATA_TILING_H
#define LIB_TRANSPOSE_TRANSDATA_TILING_H
#include <cstdint>
#include "graph/tensor.h"
#include "graph/types.h"
#include "tiling/platform/platform_ascendc.h"

namespace AscendC {
/*
 * @brief DataFormat
*/
#ifndef ASCC_ENUM_DATAFORMAT
#define ASCC_ENUM_DATAFORMAT
enum class DataFormat : uint8_t {
    ND = 0,
    NZ,
    NCHW,
    NC1HWC0,
    NHWC,
    NCDHW,
    NDC1HWC0,
    FRACTAL_Z_3D,
};
#endif // ASCC_ENUM_DATAFORMAT

#ifndef ASCC_PARAM_TRANSDATACONFIG
#define ASCC_PARAM_TRANSDATACONFIG
struct TransDataConfig {
    DataFormat srcFormat;
    DataFormat dstFormat;
};
#endif // ASCC_PARAM_TRANSDATACONFIG

/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] platform, targeted platform information
 * \param [in] srcShape, src tensor shape
 * \param [in] dstShape, src tensor shape
 * \param [in] dataType, actual data type of the input
 * \param [in] config, transdata config
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 * \return whether get the max/min value successfully
 */
bool GetTransDataMaxMinTmpSize(const platform_ascendc::PlatformAscendC& platform, const ge::Shape& srcShape,
    const ge::Shape& dstShape, const ge::DataType dataType, const TransDataConfig& config, uint32_t& maxValue,
    uint32_t& minValue);
} // AscendC
#endif // LIB_TRANSPOSE_TRANSDATA_TILING_H