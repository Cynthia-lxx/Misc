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
 * \file kernel_operator_gemm_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/basic_api/kernel_operator_gemm_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_gemm_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_GEMM_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_GEMM_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_GEMM_INTERFACE_IMPL_H
#include "kernel_tensor.h"
#include "kernel_operator_gemm_base_impl.h"
#include "kernel_operator_data_copy_intf_impl.h"
#include "kernel_struct_data_copy.h"

namespace AscendC {
// T should be left matrix dtype
template <typename T> 
[[deprecated("NOTICE: GetGemmTiling has been deprecated and will be removed in the next version. "
        "Please do not use it!")]]
__aicore__ inline GemmTiling GetGemmTiling(uint32_t m, uint32_t k, uint32_t n)
{
    uint32_t c0 = 0;
    uint32_t dSize = 1;
    if (Std::is_same<T, uint8_t>::value || Std::is_same<T, int8_t>::value) {
        c0 = 32;
        dSize = 1;
    } else {
        c0 = 16;
        dSize = 2;
    }
    GemmTiling tilling;
    tilling.c0Size = c0;
    tilling.dtypeSize = dSize;
    tilling.mNum = m;
    tilling.nNum = n;
    tilling.kNum = k;
    tilling.roundM = DivCeil(m, tilling.blockSize) * tilling.blockSize; // blockSize = 16 * 16
    tilling.roundN = DivCeil(n, tilling.blockSize) * tilling.blockSize;
    tilling.roundK = DivCeil(k, tilling.c0Size) * tilling.c0Size; // c0Size = 16 || c0Size = 32
    uint32_t k0a = TOTAL_L0A_SIZE / 2 / (tilling.roundM * dSize);
    uint32_t k0b = TOTAL_L0B_SIZE / 2 / (tilling.roundN * dSize);
    uint32_t k0 = k0a > k0b ? k0b : k0a;
    k0 = k0 > k ? k : k0;

    tilling.kTileBlock = k0 / tilling.c0Size;
    if (tilling.kTileBlock == 0) {
        tilling.kTileBlock = 1;
    }
    tilling.loopMode = LoopMode::MODE_NM;

    tilling.mBlockNum = DivCeil(m, tilling.blockSize);
    tilling.nBlockNum = DivCeil(n, tilling.blockSize);
    tilling.kBlockNum = DivCeil(k, tilling.c0Size);

    CalculateGemmTiling(tilling);

    return tilling;
}

/*
 * @ingroup Gemm
 * @brief Multiply two matrices
 * @param [out] dst output LocalTensor
 * @param [in] src0 input GlobalTensor
 * @param [in] src1 input GlobalTensor
 * @param [in] m Number of rows of src0
 * @param [in] n Number of rows of src1
 * @param [in] k Number of columns of src1
 * @param [in] tilling.blockSize size of block
 * @param [in] tilling.mNum args of m
 * @param [in] tilling.nNum args of n
 * @param [in] tilling.kNum args of k
 * @param [in] tilling.roundM/N/K Rounding parameter
 * @param [in] tilling.c0Size The byte length of a block
 * @param [in] tilling.dtypeSize Byte length of the incoming data type
 * @param [in] tilling.m/n/kBlockNum Number of blocks of m/n/k axis
 * @param [in] tilling.m/n/kIterNum Number of traversal dimensions
 * @param [in] tilling.m/k/nTileBlock Number of M/N/K axis cutting blocks
 * @param [in] tilling.m/n/kHasTailNumber of tail blocks of M/K/N axis
 * @param [in] tilling.kHasTileEle Judge whether the tail block exists
 * @param [in] tilling.KtailEle K-axis tail block element
 * @param [in] tilling.kThreadNum K-axis passes
 * @param [in] partialsum judge whether the calculation result is moved out
 * @param [in] initValue Initialization parameters
 */
template <typename T, typename U, typename S>
[[deprecated("NOTICE: Gemm has been deprecated and will be removed in the next version. "
        "Please do not use it!")]]
__aicore__ inline __inout_pipe__(V) void Gemm(const LocalTensor<T>& dst, const LocalTensor<U>& src0,
    const LocalTensor<S>& src1, const uint32_t m, const uint32_t k, const uint32_t n, GemmTiling tilling,
    bool partialsum, int32_t initValue)
{
#if ASCENDC_CPU_DEBUG
    bool flag = CheckParams(dst, src0, src1, m, k, n, tilling);
    if (!flag) {
        return;
    }
#endif

#if (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    DataCopyParams dataCopyParams;
    dataCopyParams.blockCount = 1;
    DataCopyEnhancedParams enhancedParams;
    enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX;
#endif

    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    LocalTensor<T> l0c;
    if (dstScope == Hardware::L0C) {
        l0c = dst[0];
    } else {
#if (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
        TBuffAddr tbufc;
        tbufc.logicPos = static_cast<uint8_t>(TPosition::C2);
        l0c.SetAddr(tbufc);
        l0c.InitBuffer(0, TOTAL_L0C_SIZE / sizeof(PrimT<T>));

        dataCopyParams.blockLen = dst.GetSize() * sizeof(PrimT<T>) / 1024;
        DataCopy(l0c, dst, dataCopyParams, enhancedParams);
#endif
    }

    if (tilling.loopMode == LoopMode::MODE_NM) {
        GemmExecNm(l0c, src0, src1, tilling, initValue);
    } else if (tilling.loopMode == LoopMode::MODE_MN) {
        GemmExecMn(l0c, src0, src1, tilling, initValue);
    } else {
        // other mode are not supported
    }

#if (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    if (dstScope == Hardware::UB) {
        pipe_barrier(PIPE_ALL);
        dataCopyParams.blockLen = tilling.roundM * tilling.roundN * sizeof(PrimT<T>) / 1024;
        DataCopy(dst, l0c, dataCopyParams, enhancedParams);
    }
#endif
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_GEMM_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_GEMM_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_GEMM_INTF_IMPL_H__
#endif
