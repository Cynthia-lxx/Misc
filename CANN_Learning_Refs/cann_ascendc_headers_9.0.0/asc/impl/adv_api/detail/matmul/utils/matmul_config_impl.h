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
 * \file matmul_config_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/utils/matmul_config_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_CONFIG_IMPL_H__
#endif

#ifndef IMPL_MATMUL_UTILS_MATMUL_CONFIG_IMPL_H
#define IMPL_MATMUL_UTILS_MATMUL_CONFIG_IMPL_H

#include "include/adv_api/matmul/matmul_config.h"

template <typename ArgType>
__aicore__ inline constexpr void GetMMConfigImpl(MatmulConfig& cfg, ArgType arg) {
    if constexpr (AscendC::IsSameType<ArgType, MatmulShapeParams>::value) {
        cfg.singleCoreM = arg.singleCoreM;
        cfg.singleCoreN = arg.singleCoreN;
        cfg.singleCoreK = arg.singleCoreK;
        cfg.basicM = arg.basicM;
        cfg.basicN = arg.basicN;
        cfg.basicK = arg.basicK;
    } else if constexpr (AscendC::IsSameType<ArgType, MatmulQuantParams>::value) {
        cfg.isPerTensor = arg.isPerTensor;
        cfg.hasAntiQuantOffset = arg.hasAntiQuantOffset;
    } else if constexpr (AscendC::IsSameType<ArgType, MatmulBatchParams>::value) {
        cfg.isNBatch = arg.isNBatch;
        cfg.batchMode = arg.batchMode;
        cfg.isBiasBatch = arg.isBiasBatch;
        cfg.bmmOutMode = arg.bmmOutMode;
    } else if constexpr (AscendC::IsSameType<ArgType, MatmulFuncParams>::value) {
        cfg.intrinsicsCheck = arg.intrinsicsCheck;
        cfg.enVecND2NZ = arg.enVecND2NZ;
        cfg.enableDoubleCache = arg.enableDoubleCache;
        cfg.enableL1CacheUB = arg.enableL1CacheUB;
        cfg.doMTE2Preload = arg.doMTE2Preload;
        cfg.iterateOrder = arg.iterateOrder;
        cfg.scheduleType = arg.scheduleType;
        cfg.enableReuse = arg.enableReuse;
        cfg.enableUBReuse = arg.enableUBReuse;
        cfg.isPartialOutput = arg.isPartialOutput;
        cfg.isA2B2Shared = arg.isA2B2Shared;
        cfg.isEnableChannelSplit = arg.isEnableChannelSplit;
        cfg.enableKdimReorderLoad = arg.enableKdimReorderLoad;
        cfg.enableL1BankConflictOptimise = arg.enableL1BankConflictOptimise;
        cfg.enableRelu = arg.enableRelu;
        cfg.decompMode = arg.decompMode;
    } else if constexpr (AscendC::IsSameType<ArgType, MatmulBiasParams>::value) {
        cfg.enableSetBias = arg.enableSetBias;
    }
}

template <typename T, typename... ArgTypes>
__aicore__ inline constexpr void GetMMConfigImpl(MatmulConfig& cfg, T arg, ArgTypes&&... args) {
    GetMMConfigImpl(cfg, arg);
    GetMMConfigImpl(cfg, args...);
}

#endif // _MATMUL_CONFIG_IMPL_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_CONFIG_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_CONFIG_IMPL_H__
#endif