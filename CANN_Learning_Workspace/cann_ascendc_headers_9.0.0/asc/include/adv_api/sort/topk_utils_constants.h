/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/* !
 * \file topk_utils_constants.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TOPK_UTILS_CONSTANTS_H__
#endif

#ifndef LIB_SORT_TOPK_UTILS_CONSTANTS_H
#define LIB_SORT_TOPK_UTILS_CONSTANTS_H

namespace AscendC {
enum class TopKMode {
    TOPK_NORMAL,
    TOPK_NSMALL,
};

enum class TopKAlgo {
    RADIX_SELECT,
    MERGE_SORT
};

enum class TopKOrder {
    UNSET,
    LARGEST,
    SMALLEST
};

struct TopKConfig {
    TopKAlgo algo = TopKAlgo::MERGE_SORT;
    TopKOrder order = TopKOrder::UNSET;
    bool sorted = true;
};

}; // namespace AscendC
#endif // LIB_SORT_TOPK_UTILS_CONSTANTS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TOPK_UTILS_CONSTANTS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TOPK_UTILS_CONSTANTS_H__
#endif