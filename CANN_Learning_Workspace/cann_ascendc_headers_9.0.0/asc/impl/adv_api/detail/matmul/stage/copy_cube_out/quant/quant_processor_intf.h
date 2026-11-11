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
 * \file quant_processor_intf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/stage/copy_cube_out/quant/quant_processor_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_INTF_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_INTF_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_INTF_H

#include "../../../feature_trait/matmul_chip_cap.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, class A_TYPE, class C_TYPE, const auto& MM_CFG, typename = void>
class MatmulQuantProcessor
{
public:
    __aicore__ inline MatmulQuantProcessor() {}
    __aicore__ inline ~MatmulQuantProcessor() {}

    /**
     * @description: Init MatmulQuantProcessor and quant params buf
     * @param: baseN: the quant param len for base block cal
     * @return: void
     */
    __aicore__ inline void Init(const int32_t baseN) {}

    /**
     * @description: Set quant Scalar mode and Scalar params
     * @param: scalar params
     * @return: void
     */
    __aicore__ inline void SetQuantScalar(const uint64_t scalar) {}

    /**
     * @description: Set gm quant VectorMode and vector tensor params
     * @param: vector tensor params
     * @return: void
     */
    __aicore__ inline void SetQuantVector(const GlobalTensor<uint64_t>& tensor) {}

    /**
     * @description: Set local quant VectorMode and vector tensor params
     * @param: vector tensor params
     * @return: void
     */
    __aicore__ inline void SetQuantVector(const LocalTensor<uint64_t>& tensor) {}

    /**
     * @description: if is quant scenario
     * @return: quant scenario or not
     */
    __aicore__ inline bool IsQuantSenario()
    {
        return false;
    }

    /**
     * @description: Get the quant mode
     * @return: quant mode
     */
    __aicore__ inline QuantMode_t GetMatmulQuantMode()
    {
        return QuantMode_t::NoQuant;
    }

    /**
     * @description: Get the Scalar value
     * @return: Scalar Value
     */
    __aicore__ inline uint64_t GetQuantScalarValue()
    {
        return 0;
    }

    /**
     * @description: Free quant param buf
     * @param: tempQuantTensor: The quant params store buf for datacopy/fixpipe interface
     * @param: curNIdx: The quant param block index
     * @param: baseUseN: The quant param block size
     * @return: void
     */
    __aicore__ inline void CopyQuantTensor(LocalTensor<uint64_t>& quantTensor,
        const int32_t curN, const int32_t baseUseN) {}

    /**
     * @description: Update quantTensor by idx
     * @param: idx: The offset in quantTensor
     * @return: void
     */
    __aicore__ inline void UpdateQuantTensor(int32_t idx) {}

    /**
     * @description: Get the flag to district scalar or vector quant mode
     * @return: void
     */
    __aicore__ inline bool IsPerChannelSenario()
    {
        return false;
    }

    /**
     * @description: Free quant param buf for datacopy/fixpipe interface
     * @param: tempQuantTensor: The quant params store buf for datacopy/fixpipe
     * @return: void
     */
    __aicore__ inline void FreeQuantTensor(LocalTensor<uint64_t>& quantTensor) {}

    /**
     * @description: Free quant param buf && free event
     * @return: void
     */
    __aicore__ inline void Destroy() {}

    /**
     * @description: Update datacopy params for datacopy interface
     * @param: enhancedParams: The datacopy params store buf for datacopy
     * @param: curCol: The current column index
     * @return: void
     */
    __aicore__ inline void UpdateDataCopyParamForQuant(DataCopyEnhancedParams& enhancedParams, int curCol)
    {}
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_INTF_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_INTF_H__
#endif