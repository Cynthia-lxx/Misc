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
 * \file confusion_transpose_tilingdata.h
 * \brief
 */

#ifndef LIB_TRANSPOSE_CONFUSION_TRANSPOSE_TILINGDATA_H
#define LIB_TRANSPOSE_CONFUSION_TRANSPOSE_TILINGDATA_H
#include "register/tilingdata_base.h"

namespace optiling {
BEGIN_TILING_DATA_DEF(ConfusionTransposeTiling)
    TILING_DATA_FIELD_DEF(uint32_t, param0);
    TILING_DATA_FIELD_DEF(uint32_t, param1);
    TILING_DATA_FIELD_DEF(uint32_t, param2);
    TILING_DATA_FIELD_DEF(uint32_t, param3);
    TILING_DATA_FIELD_DEF(uint32_t, param4);
    TILING_DATA_FIELD_DEF(uint32_t, param5);
    TILING_DATA_FIELD_DEF(uint32_t, param6);
    TILING_DATA_FIELD_DEF(uint32_t, param7);
    TILING_DATA_FIELD_DEF(uint32_t, param8);
    TILING_DATA_FIELD_DEF(uint32_t, param9);
    TILING_DATA_FIELD_DEF(uint32_t, param10);
    TILING_DATA_FIELD_DEF(uint32_t, param11);
    TILING_DATA_FIELD_DEF(uint32_t, param12);
    TILING_DATA_FIELD_DEF(uint32_t, param13);
    TILING_DATA_FIELD_DEF(uint32_t, param14);
    TILING_DATA_FIELD_DEF(uint32_t, param15);
    TILING_DATA_FIELD_DEF(uint32_t, param16);
    TILING_DATA_FIELD_DEF(uint32_t, param17);
END_TILING_DATA_DEF;
}
#endif // LIB_TRANSPOSE_CONFUSION_TRANSPOSE_TILINGDATA_H