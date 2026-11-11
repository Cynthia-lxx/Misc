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
 * \file matmul.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_H__
#endif

#ifndef LIB_MATMUL_MATMUL_H
#define LIB_MATMUL_MATMUL_H

#include <type_traits>
#include "include/adv_api/matmul/constant_tiling.h"
#include "include/adv_api/matmul/tiling.h"
#include "../../../impl/adv_api/detail/matmul/policy/matmul_policy.h"
#include "../../../impl/adv_api/detail/matmul/utils/matmul_call_back.h"
#include "../../../impl/adv_api/detail/matmul/utils/matmul_module.h"

namespace AscendC {

/**
 * @struct MatmulApiConfig
 * @brief Matmul external configuration
 */
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
struct MatmulApiConfig {
    using AType = A_TYPE;       ///< MatmulType of A matrix
    using BType = B_TYPE;       ///< MatmulType of B matrix
    using CType = C_TYPE;       ///< MatmulType of C matrix
    using BiasType = BIAS_TYPE; ///< MatmulType of Bias
    constexpr static MatmulConfig Config = ToMatmulConfig(MM_CFG);  ///< MatmulConfig
};

/**
 * @class MatmulImpl
 * @brief Matmul implementation of user-defined matmul object
 */
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM,
class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy), typename = void>
class MatmulImpl
{
public:
    using AType = A_TYPE;       ///< MatmulType of A matrix
    using BType = B_TYPE;       ///< MatmulType of B matrix
    using CType = C_TYPE;       ///< MatmulType of C matrix
    using BiasType = BIAS_TYPE; ///< MatmulType of Bias
private:
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;   ///< data type of L0c
    using SrcT = typename A_TYPE::T;        ///< data type of input data
    using SrcAT = typename A_TYPE::T;       ///< data type of A matrix
    using SrcBT = typename B_TYPE::T;       ///< data type of B matrix
    using DstT = typename C_TYPE::T;        ///< data type of output data
    using BiasT = typename BIAS_TYPE::T;    ///< data type of bias

public:
    __aicore__ inline MatmulImpl() {}
    /**
     * @brief Initialize tiling data in Matmul object and allocate resources according to tiling parameters
     * @param [in] cubeTiling: matmul tiling
     * @param [in] tpipe: TPipe object
     */
    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe = nullptr) {}
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
    /**
     * @brief Initialize tiling data in Matmul object and allocate resources according to tiling parameters
     * @param [in] gmCubeTiling: matmul tiling in GM
     * @param [in] tpipe: TPipe object
     */
    __aicore__ inline void Init(const __gm__ TCubeTiling* gmCubeTiling, TPipe* tpipe = nullptr) {}
#endif
    /**
     * @brief Set full original shape M N K of the number of elements
     * @param [in] orgM: size of original A matrix M-axis shape
     * @param [in] orgN: size of original B matrix N-axis shape
     * @param [in] orgK: size of original A/B matrix K-axis shape, only when Ka equal to Kb
     */
    __aicore__ inline void SetOrgShape(int orgM, int orgN, int orgK) {}
    /**
     * @brief Set full original shape M N K of the number of elements
     * @param [in] orgM: size of original A matrix M-axis shape
     * @param [in] orgN: size of original B matrix N-axis shape
     * @param [in] orgKa: size of original A matrix K-axis shape
     * @param [in] orgKb: size of original B matrix K-axis shape
     * @param [in] orgKc: size of C matrix N-axis shape, only when B matrix's N and C matrix's N are different
     */
    __aicore__ inline void SetOrgShape(int orgM, int orgN, int orgKa, int orgKb, int orgKc = 0) {}
    /**
     * @brief Set single core shape M N K of the number of elements
     * @param [in] singleM: size of M-axis shape within a single core
     * @param [in] singleN: size of N-axis shape within a single core
     * @param [in] singleK: size of K-axis shape within a single core
     */
    __aicore__ inline void SetSingleShape(int singleM, int singleN, int singleK) {}
    /**
     * @brief Without changing tiling, reconfigure singleCoreM, singleCoreN, singleCoreK for this computation
     * @param [in] tailM: size of M-axis shape within a single core
     * @param [in] tailN: size of N-axis shape within a single core
     * @param [in] tailK: size of K-axis shape within a single core
     */
    __aicore__ inline void SetTail(int tailM = -1, int tailN = -1, int tailK = -1) {}
    /**
     * @brief Set A matrix
     * @param [in] gm: A matrix in GlobalTensor
     * @param [in] isTransposeA: whether A matrix needs to be transposed
     */
    __aicore__ inline void SetTensorA(const GlobalTensor<SrcAT>& gm, bool isTransposeA = false) {}
    /**
     * @brief Set B matrix
     * @param [in] gm: B matrix in GlobalTensor
     * @param [in] isTransposeA: whether B matrix needs to be transposed
     */
    __aicore__ inline void SetTensorB(const GlobalTensor<SrcBT>& gm, bool isTransposeB = false) {}
    /**
     * @brief Set bias matrix
     * @param [in] gm: bias matrix in GlobalTensor
     */
    __aicore__ inline void SetBias(const GlobalTensor<BiasT>& biasGlobal) {}
    /**
     * @brief When using MatmulCallBackFunc, set the required computation data or the storage address of data on GM
     * @tparam [in] T: dataPtr data type, default is uint64_t
     * @param [in] dataPtr: the required computation data or the storage address of data on GM
     * @note must be called before SetTensorA and SetTensorB
     */
    template <class T>
    __aicore__ inline void SetSelfDefineData(T dataPtr) {}
    /**
     * @brief When using MatmulCallBackFunc, set the tiling address used by callback function
     * @param [in] tilingPtr: the tiling address
     * @note only need to be called once
     */
    __aicore__ inline void SetUserDefInfo(const uint64_t tilingPtr) {}
    /**
     * @brief Set the index matrix generated during the densification process of a sparse matrix
     * @param [in] indexGlobal: the first address of the index matrix in Global Memory
     */
    __aicore__ inline void SetSparseIndex(const GlobalTensor<uint8_t>& indexGlobal);
    /**
     * @brief Set the quantization scale for anti-quantization when A matrix's data type is half and B matrix's data type is int8
     * @param [in] offsetScalar: quantization scale for addition
     * @param [in] scaleScalar: quantization scale for multiplication
     */
    __aicore__ inline void SetAntiQuantScalar(const SrcT offsetScalar, const SrcT scaleScalar) {}
    /**
     * @brief Set the quantization vector for anti-quantization when A matrix's data type is half and B matrix's data type is int8
     * @param [in] offsetTensor: quantization vector for addition
     * @param [in] scaleTensor: quantization vector for multiplication
     */
    __aicore__ inline void SetAntiQuantVector(const LocalTensor<SrcT> &offsetTensor,
        const LocalTensor<SrcT> &scaleTensor) {}
    /**
     * @brief Set the quantization scale
     * @param [in] quantScalar: quantization scale
     */
    __aicore__ inline void SetQuantScalar(const uint64_t quantScalar) {}
    /**
     * @brief Set the quantization vector
     * @param [in] quantTensor: quantization vector in global memery
     */
    __aicore__ inline void SetQuantVector(const GlobalTensor<uint64_t>& quantTensor) {}
    /**
     * @brief Set the quantization vector
     * @param [in] quantTensor: quantization vector in local memery
     */
    __aicore__ inline void SetQuantVector(const LocalTensor<uint64_t>& quantTensor) {}
    /**
     * @brief Set A matrix
     * @param [in] leftMatrix: A matrix in LocalTensor
     * @param [in] isTransposeA: whether A matrix needs to be transposed
     */
    __aicore__ inline void SetTensorA(const LocalTensor<SrcAT>& leftMatrix, bool isTransposeA = false) {}
    /**
     * @brief Set B matrix
     * @param [in] rightMatrix: B matrix in LocalTensor
     * @param [in] isTransposeB: whether B matrix needs to be transposed
     */
    __aicore__ inline void SetTensorB(const LocalTensor<SrcBT>& rightMatrix, bool isTransposeB = false) {}
    /**
     * @brief Set A matrix
     * @param [in] aScalar: values set in A matrix
     * @note scalar data will be expanded into a tensor of shape [1, K]
     */
    __aicore__ inline void SetTensorA(SrcAT aScalar) {}
    /**
     * @brief Set B matrix
     * @param [in] bScalar: values set in B matrix
     * @note scalar data will be expanded into a tensor of shape [1, K]
     */
    __aicore__ inline void SetTensorB(SrcBT bScalar) {}
    /**
     * @brief Set bias matrix
     * @param [in] inputBias: bias matrix in LocalTensor
     */
    __aicore__ inline void SetBias(const LocalTensor<BiasT>& inputBias) {}
    /**
     * @brief Reset the batch number for Batch Matmul without chaging tiling
     * @param [in] batchA: batch number of A matrix
     * @param [in] batchB: batch number of B matrix
     */
    __aicore__ inline void SetBatchNum(int32_t batchA, int32_t batchB) {}
    /**
     * @brief Clear bias flag, bias will not be involved in the computation
     */
    __aicore__ inline void DisableBias() {}
    /**
     * @brief Clear bias flag, bias will not be involved in the computation
     * @note recommend to use DisableBias
     */
    __aicore__ inline void ClearBias() {}
    /**
     * @brief Calculate a C matrix of size baseM * baseN
     * @tparam [in] sync: set to synchronous or asynchronous mode
     * @param [in] enPartialSum: whether to accumulate the result of Iterate into CO1 data
     */
    template <bool sync = true> __aicore__ inline bool Iterate(bool enPartialSum = false)
    {
        return false;
    }
    /**
     * @brief Calculate a C matrix of size baseM * baseN
     * @tparam [in] sync: set to synchronous or asynchronous mode
     * @param [in] enPartialSum: whether to accumulate the result of Iterate into CO1 data
     * @param [in] localCmatrix: the LocalTensor memory on CO1 applied for by user, used to store the results of Iterate
     */
    template <bool sync = true, typename T> __aicore__ inline bool Iterate(bool enPartialSum,
        const LocalTensor<T>& localCmatrix)
    {
        return false;
    }
    /**
     * @brief Calculate a C matrix of size singleCoreM * singleCoreN
     * @tparam [in] sync: set to synchronous or asynchronous mode
     * @param [in] gm: C matrix in GlobalTensor
     * @param [in] enAtomic: whether to enable atomic operations
     * @param [in] enSequentialWrite: whether to enable sequential write mode
     * @param [in] waitIterateAll: whether to wait for IterateAll to complete by WaitIterateAll when in asynchronous mode
     * @param [in] fakeMsg: whether to enable fake message when in IBShare or IntraBlockPartSum mode
     */
    template <bool sync = true>
    __aicore__ inline void IterateAll(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0,
        bool enSequentialWrite = false, bool waitIterateAll = false, bool fakeMsg = false) {}
    /**
     * @brief Calculate a C matrix of size singleCoreM * singleCoreN
     * @tparam [in] sync: set to synchronous or asynchronous mode
     * @param [in] ubCmatrix: C matrix in LocalTensor
     * @param [in] enAtomic: whether to enable atomic operations
     */
    template <bool sync = true>
    __aicore__ inline void IterateAll(const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0) {}

    /**
     * @brief Calculate multiple C matrices of size singleCoreM * singleCoreN
     * @param [in] gm: C matrix in GlobalTensor
     * @param [in] enPartialSum: whether to accumulate the result of Iterate into CO1 data
     * @param [in] enAtomic: whether to enable atomic operations
     * @param [in] enSequentialWrite: whether to enable sequential write mode
     * @param [in] matrixStrideA: offset between the starting address of adjacent nd matrix in A matrix, in terms of elements
     * @param [in] matrixStrideB: offset between the starting address of adjacent nd matrix in B matrix, in terms of elements
     * @param [in] matrixStrideC: reserved parameter
     */
    __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm,
        bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0,
        const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0) {}
        /**
     * @brief Calculate multiple C matrices of size singleCoreM * singleCoreN
     * @param [in] ubCmatrix: C matrix in LocalTensor
     * @param [in] enPartialSum: whether to accumulate the result of Iterate into CO1 data
     * @param [in] enAtomic: whether to enable atomic operations
     * @param [in] enSequentialWrite: whether to enable sequential write mode
     * @param [in] matrixStrideA: offset between the starting address of adjacent nd matrix in A matrix, in terms of elements
     * @param [in] matrixStrideB: offset between the starting address of adjacent nd matrix in B matrix, in terms of elements
     * @param [in] matrixStrideC: reserved parameter
     */
    __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix,
        bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0,
        const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0) {}

    /**
     * @brief After Iterate, get one or two C matrix slices
     * @tparam [in] sync: set to synchronous or asynchronous mode
     * @param [in] co2Local: get C matrix to VECIN, data format only supports NZ
     * @param [in] enAtomic: whether to enable atomic operations
     * @param [in] enSequentialWrite: whether to enable sequential write mode
     */
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const LocalTensor<DstT>& co2Local, uint8_t enAtomic = 0,
        bool enSequentialWrite = false) {}
    /**
     * @brief After Iterate, get one or two C matrix slices
     * @tparam [in] sync: set to synchronous or asynchronous mode
     * @param [in] gm: get C matrix to GM, data format supports ND or NZ
     * @param [in] enAtomic: whether to enable atomic operations
     * @param [in] enSequentialWrite: whether to enable sequential write mode
     */
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0,
        bool enSequentialWrite = false) {}
    /**
     * @brief After Iterate, get one or two C matrix slices
     * @tparam [in] sync: set to synchronous or asynchronous mode
     * @param [in] gm: get C matrix to GM, data format only supports NZ
     * @param [in] co2Local: get C matrix to VECIN, data format only supports NZ
     * @param [in] enAtomic: whether to enable atomic operations
     * @param [in] enSequentialWrite: whether to enable sequential write mode
     */
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT> &gm, const LocalTensor<DstT> &co2Local,
        uint8_t enAtomic = 0, bool enSequentialWrite = false) {}
    /**
     * @brief Get the position of the current fragment in the entire C matrix
     * @tparam [in] isTurnOnDebug: reserved parameter
     * @note reserved function
     */
    template <bool isTurnOnDebug = true>
    __aicore__ inline MatrixOffset GetOffsetC()
    {
        return {};
    }
    /**
     * @brief Release Matmul computation resources to prevent confilicts among multiple Matmul objects
     * @note call End once when switching computations between multiple Matmul objects
     */
    __aicore__ inline void End() {}
    /**
     * @brief Set whether to enable HF32 mode
     * @param [in] enableHF32: whether to enable HF32 mode
     * @param [in] transMode: when enable HF32 mode, set ROUND mode used when converting float to hf32
     * @note  enable HF32 mode can improve performance but it may also result in a loss of precision
     */
    __aicore__ inline void SetHF32(bool enableHF32 = false, int32_t transMode = 0) {}
    /**
     * @brief Set sub-block index
     * @param [in] subBlockIdx: sub-block index
     */
    __aicore__ inline void SetSubBlockIdx(uint8_t subBlockIdx) {}
    /**
     * @brief Get sub-block index
     */
    __aicore__ inline uint8_t GetSubBlockIdx()
    {
        return 0;
    }
    /**
     * @brief Allocate a temporary buffer for caching computation reselts
     * @param [in] addr: workspace on GM provided by user, GM address type
     * @param [in] size: number of elements
     */
    template <class T> __aicore__ inline void SetWorkspace(__gm__ const T* addr, int size) {}
    /**
     * @brief Allocate a temporary buffer for caching computation reselts
     * @param [in] addr: workspace on GM provided by user, GlobalTensor type
     * @note recommend to use this function
     */
    template <class T> __aicore__ inline void SetWorkspace(GlobalTensor<T>& addr) {}

    /**
     * @brief Set starting physical address of additional VECCALC space
     * @param [in] tmpBuffer: temporary space
     * @note when Matmul requires additional VECCALC space and user wants to reuse this additional space,
     *       the space must be pre-reserved and a LocalTensor must be allocated
     */
    __aicore__ inline void SetLocalWorkspace(const LocalTensor<uint8_t>& tmpBuffer) {}
    using CallBack = MM_CB;
};

} // namespace AscendC
// Compatible with the previously used matmul namespace
namespace matmul = AscendC;
#include "../../../impl/adv_api/detail/matmul/matmul_impl_base.h"
#include "../../../impl/adv_api/detail/matmul/matmul_impl.h"
#include "../../../impl/adv_api/detail/matmul/batch_matmul_impl.h"
#include "../../../impl/adv_api/detail/matmul/mx_matmul_impl.h"
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATMUL_H__
#endif