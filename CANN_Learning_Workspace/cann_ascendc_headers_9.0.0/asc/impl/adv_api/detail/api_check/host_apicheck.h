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
 * \file host_apicheck.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/host_apicheck.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/tiling_api.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HOST_APICHECK_H__
#endif

#ifndef IMPL_HOST_APICHECK_H
#define IMPL_HOST_APICHECK_H

#include <set>
#include <string>
#include <vector>
#include "../../detail/host_log.h"
#include "graph/tensor.h"
#include "tiling/platform/platform_ascendc.h"

namespace AscendC {
namespace HighLevelApiCheck {
template <const char* func>
inline void SrcShapeSizeVerifyingParameters(const size_t shapeSize, const uint32_t typeSize)
{
    (void)typeSize;
    ASCENDC_HOST_ASSERT(shapeSize > 0u, return, "[%s] Input shape size should be greater than 0!", func);
}

template <const char* func>
inline void ShapeLastAxisAlignVerifyingParameters(const ge::Shape& shape, const uint32_t dataTypeSize,
    const uint32_t blockSize)
{
    std::vector<int64_t> shapeDims = shape.GetDims();
    auto lastAxisDataLength = shapeDims.back() * dataTypeSize;
    ASCENDC_HOST_ASSERT((shapeDims.back() > 0u),, "[%s] The shape LastAxis size should be greater than 0!", func);
    ASCENDC_HOST_ASSERT((lastAxisDataLength % blockSize == 0u),,
        "[%s] The shape LastAxis data size must be 32-byte aligned!", func);
}

template <const char* func>
inline void LocalWorkSpaceSizeVerifyingParameters(const uint32_t localWorkSpaceSize)
{
    ASCENDC_HOST_ASSERT((localWorkSpaceSize > 0u), return, "[%s] The localWorkSpaceSize should be greater than 0!",
        func);
}

template <const char* func>
inline void TypeSizeVerifyingParameters(const uint32_t typeSize, const std::set<uint32_t>& supportTypeSize)
{
    std::string supportTypeSizeStr = "";
    for (const uint32_t& size : supportTypeSize) {
        supportTypeSizeStr.append(" " + std::to_string(size));
    }
    ASCENDC_HOST_ASSERT((supportTypeSize.find(typeSize) != supportTypeSize.end()),,
        "[%s] Type size %u is unsupported! Support typesize is %s.", func, typeSize, supportTypeSizeStr.c_str());
}

template <const char* func>
inline void IsReuseSourceVerifyingParameters(const bool isReuseSource)
{
    if (isReuseSource) {
        TILING_LOG_WARNING("[%s] Current isReuseSource is true, which is not effective!", func);
    }
}
} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_HOST_APICHECK_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HOST_APICHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HOST_APICHECK_H__
#endif
