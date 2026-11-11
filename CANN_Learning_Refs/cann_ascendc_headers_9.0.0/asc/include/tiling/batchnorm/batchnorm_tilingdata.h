/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef LIB_NORMALIZATION_BATCHNORM_TILINGDATA_H
#define LIB_NORMALIZATION_BATCHNORM_TILINGDATA_H
#include "register/tilingdata_base.h"

namespace optiling {
BEGIN_TILING_DATA_DEF(BatchNormTiling)
    TILING_DATA_FIELD_DEF(uint32_t, originalBLength);
    TILING_DATA_FIELD_DEF(uint32_t, meanVarSize);
    TILING_DATA_FIELD_DEF(uint32_t, meanTmpTensorPos);
    TILING_DATA_FIELD_DEF(uint32_t, varianceTmpTensorPos);
    TILING_DATA_FIELD_DEF(uint32_t, tmpBufSize);
    TILING_DATA_FIELD_DEF(uint32_t, oneTmpSize);
    TILING_DATA_FIELD_DEF(uint32_t, firstTmpStartPos);
    TILING_DATA_FIELD_DEF(uint32_t, secondTmpStartPos);
    TILING_DATA_FIELD_DEF(uint32_t, thirdTmpStartPos);
    TILING_DATA_FIELD_DEF(uint32_t, loopRound);
    TILING_DATA_FIELD_DEF(uint32_t, inputTailSize);
    TILING_DATA_FIELD_DEF(uint32_t, inputTailPos);
    TILING_DATA_FIELD_DEF(uint32_t, meanVarTailSize);
    TILING_DATA_FIELD_DEF(uint32_t, meanVarTailPos);
    TILING_DATA_FIELD_DEF(uint32_t, bshCurLength);
    TILING_DATA_FIELD_DEF(uint32_t, shCurLength);
    TILING_DATA_FIELD_DEF(float, firstDimValueBack);
    TILING_DATA_FIELD_DEF(uint32_t, castHalfRepStride);
    TILING_DATA_FIELD_DEF(uint32_t, shCurLengthBlockNum);
    TILING_DATA_FIELD_DEF(uint32_t, castHalfOutRepStride);
END_TILING_DATA_DEF;
}
#endif // LIB_NORMALIZATION_BATCHNORM_TILINGDATA_H