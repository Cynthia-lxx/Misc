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
 * \file sum_tiling.h
 * \brief
 */
#ifndef LIB_REDUCE_SUM_TILING_H
#define LIB_REDUCE_SUM_TILING_H
#include <cstdint>
namespace AscendC {
/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] n, input actual number of calculations in each row of the input data
 * \param [in] typeSize, size of the input data type, in bytes
 * \param [in] isReuseSource, whether to reuse the input space of the source operand
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetSumMaxMinTmpSize(
    const uint32_t n, const uint32_t typeSize, const bool isReuseSource, uint32_t &maxSize, uint32_t &minSize);
}  // namespace AscendC
#endif