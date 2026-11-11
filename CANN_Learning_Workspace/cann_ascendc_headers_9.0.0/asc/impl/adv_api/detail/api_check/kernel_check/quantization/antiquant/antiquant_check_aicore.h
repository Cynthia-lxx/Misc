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
 * \file antiquant_check_aicore.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/quantization/antiquant/antiquant_check_aicore.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/antiquantize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ANTIQUANT_CHECK_AICORE_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_ANTIQUANT_ANTIQUANT_CHECK_AICORE_H_
#define IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_ANTIQUANT_ANTIQUANT_CHECK_AICORE_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "include/adv_api/quantization/ascend_antiquant_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
template <typename SrcType, typename DstType, bool isTranspose>
class CheckFuncClassAscendAntiQuantChannel {
public:
    __aicore__ inline CheckFuncClassAscendAntiQuantChannel() {};
    __aicore__ inline CheckFuncClassAscendAntiQuantChannel(__gm__ const char *apiName) {};

public:
    __aicore__ inline void VerifyingParameters(const LocalTensor<DstType> &dst, const LocalTensor<SrcType> &src,
        const LocalTensor<DstType> &scale, const LocalTensor<uint8_t> &sharedTmpBuffer, const uint32_t K,
        const AntiQuantShapeInfo& shapeInfo = {}) {};
};

template <typename SrcType, typename DstType, bool isTranspose>
class CheckFuncClassAscendAntiQuantTensor {
public:
    __aicore__ inline CheckFuncClassAscendAntiQuantTensor() {};
    __aicore__ inline CheckFuncClassAscendAntiQuantTensor(__gm__ const char *apiName) {};

public:
    __aicore__ inline void VerifyingParameters(const LocalTensor<DstType> &dst, const LocalTensor<SrcType> &src,
        const DstType scale, const LocalTensor<uint8_t> &sharedTmpBuffer, const uint32_t K,
        const AntiQuantShapeInfo& shapeInfo = {}) {};
};

template <typename SrcType, typename DstType, bool isTranspose>
class CheckFuncClassAscendAntiQuantChannelOffset {
public:
    __aicore__ inline CheckFuncClassAscendAntiQuantChannelOffset() {};
    __aicore__ inline CheckFuncClassAscendAntiQuantChannelOffset(__gm__ const char *apiName) {};

public:
    __aicore__ inline void VerifyingParameters(const LocalTensor<DstType> &dst, const LocalTensor<SrcType> &src,
        const LocalTensor<DstType> &offset, const LocalTensor<DstType> &scale, const LocalTensor<uint8_t> &sharedTmpBuffer,
        const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {}) {};
};

template <typename SrcType, typename DstType, bool isTranspose>
class CheckFuncClassAscendAntiQuantTensorOffset {
public:
    __aicore__ inline CheckFuncClassAscendAntiQuantTensorOffset() {};
    __aicore__ inline CheckFuncClassAscendAntiQuantTensorOffset(__gm__ const char *apiName) {};

public:
    __aicore__ inline void VerifyingParameters(const LocalTensor<DstType> &dst, const LocalTensor<SrcType> &src,
        const DstType offset, const DstType scale, const LocalTensor<uint8_t> &sharedTmpBuffer, const uint32_t K,
        const AntiQuantShapeInfo& shapeInfo = {}) {};
};

}
}
#endif // IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_ANTIQUANT_ANTIQUANT_CHECK_AICORE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ANTIQUANT_CHECK_AICORE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ANTIQUANT_CHECK_AICORE_H__
#endif
