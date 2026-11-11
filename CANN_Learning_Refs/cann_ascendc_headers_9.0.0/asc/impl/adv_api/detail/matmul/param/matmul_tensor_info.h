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
 * \file matmul_tensor_info.h
 * \brief matmul input tensor manager
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/param/matmul_tensor_info.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_TENSOR_INFO_H__
#endif

#ifndef IMPL_MATMUL_PARAM_MATMUL_TENSOR_INFO_H
#define IMPL_MATMUL_PARAM_MATMUL_TENSOR_INFO_H

#include "../utils/matmul_module.h"

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, const auto &MM_CFG, class INPUT_TYPE, typename = void>
class MatmulTensorInfo {
    using SrcT = typename INPUT_TYPE::T;

    MATMUL_USE_MODULE(MatmulShapeInfo);
public:
    __aicore__ inline MatmulTensorInfo() = default;
    __aicore__ inline ~MatmulTensorInfo() = default;
    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline GlobalTensor<SrcT> GetGlobalTensor() const
    {
        GlobalTensor<SrcT> globalMatrix;
        globalMatrix.SetGlobalBuffer(globalMatrix_);
        return globalMatrix;
    }

    __aicore__ inline LocalTensor<SrcT> GetLocalTensor() const
    {
        LocalTensor<SrcT> localMatrix;
        localMatrix.SetAddr(localMatrix_.address_);
        return localMatrix;
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline void SetGlobalTensor(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose)
    {
        globalMatrix_ = globalMatrix.address_;
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
            CheckMatrixA(isTranspose);
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeA(isTranspose);
        } else {
            CheckMatrixB(isTranspose);
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeB(isTranspose);
        }
    }

    __aicore__ inline void SetLocalTensor(const LocalTensor<SrcT>& localMatrix, bool isTranspose)
    {
        localMatrix_.address_ = localMatrix.address_;
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
            CheckMatrixA(isTranspose);
            CheckMatrixAFromLocalMemory();
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeA(isTranspose);
        } else {
            CheckMatrixB(isTranspose);
            CheckMatrixBFromLocalMemory();
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeB(isTranspose);
        }
    }

    template <bool isTrans = false>
    __aicore__ inline int GetBaseUseHeight() const
    {
        if constexpr (isTrans) {
            return MATMUL_CONST_PARAM_VAR.baseUseK_;
        } else {
            return MATMUL_CONST_PARAM_VAR.baseUseM_;
        }
    }

private:
    __aicore__ inline void CheckMatrixA(bool isTransposeA)
    {
        ASCENDC_ASSERT((isTransposeA <= INPUT_TYPE::isTrans), {
            KERNEL_LOG(KERNEL_ERROR, "It is not allowed to set matrix A transpose when matmul A transpose is not defined.");
        });
        if constexpr (INPUT_TYPE::format == CubeFormat::VECTOR) {
            ASCENDC_ASSERT(!isTransposeA, { KERNEL_LOG(KERNEL_ERROR,
                "In GEMV mode, matrix A should not be transposed.");});
        }
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
        if constexpr (IsSameType<SrcT, int4b_t>::value) {
            ASCENDC_ASSERT(!isTransposeA, { KERNEL_LOG(KERNEL_ERROR,
                "When matrix A DType is int4, matrix A should not be transposed");});
        }
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
        if constexpr (IsSameType<SrcT, int8_t>::value) {
            ASCENDC_ASSERT(!isTransposeA, { KERNEL_LOG(KERNEL_ERROR,
                "When matrix A DType is int8, matrix A should not be transposed");});
        }
#endif
    }

    __aicore__ inline void CheckMatrixAFromLocalMemory()
    {
        // A/B does not come from GM with IBShare is not supported
        if constexpr (DoMatmulIBShareNorm(MM_CFG) && INPUT_TYPE::ibShare) {
            ASCENDC_ASSERT((false), {
                KERNEL_LOG(KERNEL_ERROR, "It is not allowed to set matrix A whose src::pos is L1 when matmul A is ibShare.");
            });
        }
    }

    __aicore__ inline void CheckMatrixB(bool isTransposeB)
    {
        ASCENDC_ASSERT((isTransposeB <= INPUT_TYPE::isTrans), {
            KERNEL_LOG(KERNEL_ERROR, "It is not allowed to set matrix B transpose when matmul B transpose is not defined.");
        });
    }

    __aicore__ inline void CheckMatrixBFromLocalMemory()
    {
        // A/B does not come from GM with IBShare is not supported
        if constexpr (DoMatmulIBShareNorm(MM_CFG) && INPUT_TYPE::ibShare) {
            ASCENDC_ASSERT((false), {
                KERNEL_LOG(KERNEL_ERROR, "It is not allowed to set matrix B whose src::pos is L1 when matmul B is ibShare.");
            });
        }
    }

    LocalTensor<TensorTrait<SrcT>> localMatrix_;
    __gm__ SrcT* globalMatrix_;
};

template <typename IMPL, const auto &MM_CFG, class INPUT_TYPE>
class MatmulTensorInfo<IMPL, MM_CFG, INPUT_TYPE, enable_if_t<IsIntrablock<MM_CFG> && !HasScalePosition<INPUT_TYPE>::value>> {
    using SrcT = typename INPUT_TYPE::T;

    MATMUL_USE_MODULE(MatmulShapeInfo);
public:
    __aicore__ inline MatmulTensorInfo() = default;
    __aicore__ inline ~MatmulTensorInfo() = default;
    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline GlobalTensor<SrcT> GetGlobalTensor() const
    {
        GlobalTensor<SrcT> globalMatrix;
        if constexpr (IS_INTRA_BLOCK) {
            globalMatrix.SetGlobalBuffer(intrablockGlobalMatrix_);
        } else {
            globalMatrix.SetGlobalBuffer(globalMatrix_);
        }
        return globalMatrix;
    }

    __aicore__ inline LocalTensor<SrcT> GetLocalTensor() const
    {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Intrablock only support inputs from GM."); });
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline void SetGlobalTensor(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose)
    {
        if constexpr (IS_INTRA_BLOCK) {
            intrablockGlobalMatrix_ = globalMatrix.address_;
        } else {
            globalMatrix_ = globalMatrix.address_;
        }
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::A) {
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeA(isTranspose);
        } else {
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeB(isTranspose);
        }
    }

    __aicore__ inline void SetLocalTensor(const LocalTensor<SrcT>& localMatrix, bool isTranspose)
    {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Intrablock only support inputs from GM."); });
    }

private:
    __gm__ SrcT* globalMatrix_;
    __gm__ SrcT* intrablockGlobalMatrix_;
};

template <const auto& MM_CFG, typename INPUT_TYPE>
constexpr bool IsSparseMatmul = (INPUT_TYPE::TAG == InputTypeTag::B) &&
    HasSparseIndex<INPUT_TYPE>() && DoMatmulMDL(MM_CFG);

template <typename IMPL, const auto &MM_CFG, class INPUT_TYPE>
class MatmulTensorInfo<IMPL, MM_CFG, INPUT_TYPE, enable_if_t<IsSparseMatmul<MM_CFG, INPUT_TYPE> && !HasScalePosition<INPUT_TYPE>::value>> {
    using SrcT = typename INPUT_TYPE::T;

    MATMUL_USE_MODULE(MatmulShapeInfo);
public:
    __aicore__ inline MatmulTensorInfo() = default;
    __aicore__ inline ~MatmulTensorInfo() = default;
    __aicore__ inline LocalTensor<SrcT> GetLocalTensor() const
    {
        LocalTensor<SrcT> localMatrix;
        localMatrix.SetAddr(localMatrix_.address_);
        return localMatrix;
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline GlobalTensor<SrcT> GetGlobalTensor() const
    {
        GlobalTensor<SrcT> globalMatrix;
        globalMatrix.SetGlobalBuffer(globalMatrix_);
        return globalMatrix;
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline void SetGlobalTensor(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose)
    {
        globalMatrix_ = globalMatrix.address_;
        MATMUL_MODULE(MatmulShapeInfo)->SetTransposeB(isTranspose);
    }

    __aicore__ inline void SetLocalTensor(const LocalTensor<SrcT>& localMatrix, bool isTranspose)
    {
        localMatrix_.address_ = localMatrix.address_;
        MATMUL_MODULE(MatmulShapeInfo)->SetTransposeB(isTranspose);
    }

    __aicore__ inline void SetGlobalSparseIndex(const GlobalTensor<uint8_t>& indexGlobal)
    {
        indexGlobal_ = indexGlobal;
    }

    __aicore__ inline void SetLocalSparseIndex(const LocalTensor<uint8_t>& indexLocal)
    {
        indexLocal_ = indexLocal;
    }

    __aicore__ inline GlobalTensor<uint8_t> GetGlobalSparseIndex()
    {
        return indexGlobal_;
    }

    __aicore__ inline LocalTensor<uint8_t> GetLocalSparseIndex()
    {
        return indexLocal_;
    }

    template <bool IS_TRANS = false>
    __aicore__ inline int GetBaseUseHeight() const
    {
        if constexpr (IS_TRANS) {
            return MATMUL_CONST_PARAM_VAR.baseUseN_;
        } else {
            return MATMUL_CONST_PARAM_VAR.baseUseK_;
        }
    }

private:
    LocalTensor<TensorTrait<SrcT>> localMatrix_;
    __gm__ SrcT* globalMatrix_;
    GlobalTensor<uint8_t> indexGlobal_;
    LocalTensor<uint8_t> indexLocal_;
};

template <typename IMPL, const auto &MM_CFG, class INPUT_TYPE>
class MatmulTensorInfo<IMPL, MM_CFG, INPUT_TYPE, enable_if_t<HasScalePosition<INPUT_TYPE>::value &&
    (INPUT_TYPE::TAG == InputTypeTag::scaleA || INPUT_TYPE::TAG == InputTypeTag::scaleB)>> {
    using SrcT = fp8_e8m0_t;
    MATMUL_USE_MODULE(MatmulShapeInfo);
public:
    __aicore__ inline MatmulTensorInfo() = default;
    __aicore__ inline ~MatmulTensorInfo() = default;

    __aicore__ inline LocalTensor<SrcT> GetLocalTensor() const
    {
        LocalTensor<SrcT> localMatrix;
        localMatrix.SetAddr(localMatrix_.address_);
        return localMatrix;
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline GlobalTensor<SrcT> GetGlobalTensor() const
    {
        GlobalTensor<SrcT> globalMatrix;
        globalMatrix.SetGlobalBuffer(globalMatrix_);
        return globalMatrix;
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline void SetGlobalTensor(const GlobalTensor<SrcT>& globalMatrix, bool isTranspose)
    {
        globalMatrix_ = globalMatrix.address_;
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::scaleA) {
            CheckMatrixScaleA(isTranspose);
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeScaleA(isTranspose);
        } else {
            CheckMatrixScaleB(isTranspose);
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeScaleB(isTranspose);
        }
    }

    __aicore__ inline void SetLocalTensor(const LocalTensor<SrcT>& localMatrix, bool isTranspose)
    {
        localMatrix_.address_ = localMatrix.address_;
        if constexpr (INPUT_TYPE::TAG == InputTypeTag::scaleA) {
            CheckMatrixScaleA(isTranspose);
            CheckMatrixScaleAFromLocalMemory();
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeScaleA(isTranspose);
        } else {
            CheckMatrixScaleB(isTranspose);
            CheckMatrixScaleBFromLocalMemory();
            MATMUL_MODULE(MatmulShapeInfo)->SetTransposeScaleB(isTranspose);
        }
    }

    template <bool isTrans = false>
    __aicore__ inline int GetBaseUseHeight() const
    {
        if constexpr (!isTrans) {
            return MATMUL_CONST_PARAM_VAR.baseUseM_;
        } else {
            return MATMUL_CONST_PARAM_VAR.baseUseK_;
        }
    }

private:
    __aicore__ inline void CheckMatrixScaleA(bool isTransposeScaleA)
    {
        ASCENDC_ASSERT((isTransposeScaleA <= INPUT_TYPE::isScaleTrans), {
            KERNEL_LOG(KERNEL_ERROR, "It is not allowed to set matrix scaleA transpose when matmul scaleA transpose is not defined.");
        });
        // The following are the limitations for using mx gemv.
        ASCENDC_ASSERT(!(INPUT_TYPE::format == CubeFormat::VECTOR ^ INPUT_TYPE::scaleFormat == CubeFormat::VECTOR), {
            KERNEL_LOG(KERNEL_ERROR, "In GEMV mode, the Format for matrix A and scale A must be a vector.");
        });
        ASCENDC_ASSERT((INPUT_TYPE::scaleFormat != CubeFormat::VECTOR || !isTransposeScaleA), {
            KERNEL_LOG(KERNEL_ERROR, "In GEMV mode, matrix scale A do not support transposition.");
        });
        ASCENDC_ASSERT((INPUT_TYPE::scaleFormat != CubeFormat::VECTOR || 
                        (INPUT_TYPE::pos == TPosition::GM && INPUT_TYPE::scalePosition == TPosition::GM)), {
            KERNEL_LOG(KERNEL_ERROR, "In GEMV mode, the TPosition of matrix A and scale A must be in GM.");
        });
    }

    __aicore__ inline void CheckMatrixScaleAFromLocalMemory() {}

    __aicore__ inline void CheckMatrixScaleB(bool isTransposeScaleB)
    {
        ASCENDC_ASSERT((isTransposeScaleB <= INPUT_TYPE::isScaleTrans), {
            KERNEL_LOG(KERNEL_ERROR, "It is not allowed to set matrix scaleB transpose when matmul scaleB transpose is not defined.");
        });
    }

    __aicore__ inline void CheckMatrixScaleBFromLocalMemory() {}

    LocalTensor<TensorTrait<SrcT>> localMatrix_;
    __gm__ SrcT* globalMatrix_;
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_TENSOR_INFO_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_TENSOR_INFO_H__
#endif // IMPL_MATMUL_PARAM_MATMUL_TENSOR_INFO_H