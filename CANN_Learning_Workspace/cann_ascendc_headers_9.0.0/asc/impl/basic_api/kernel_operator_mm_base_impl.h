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
 * \file kernel_operator_mm_base_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/basic_api/kernel_operator_mm_base_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_mm_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_BASE_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MM_BASE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MM_BASE_IMPL_H
#include "kernel_tensor.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_mm_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_mm_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_mm_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_mm_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_mm_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_c310/kernel_operator_mm_impl.h"
#endif
#include "kernel_operator_mm_check.h"
#include "kernel_operator_mm_load2d_impl.h"
#include "kernel_struct_mm.h"
namespace AscendC {
struct IsResetLoad3dConfig {
    __aicore__ constexpr IsResetLoad3dConfig(const bool isSetFMatrixIn, const bool isSetPaddingIn)
    {
        isSetFMatrix = isSetFMatrixIn;
        isSetPadding = isSetPaddingIn;
    }
    bool isSetFMatrix = true;
    bool isSetPadding = true;
};

constexpr IsResetLoad3dConfig IS_RESER_LOAD3D_DEFAULT_CONFIG = {true, true};

/* **************************************************************************************************
 * LoadData 3dv1                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.padList padding list
 * @param [in] loadDataParams.l1H operand height
 * @param [in] loadDataParams.l1W operand width
 * @param [in] loadDataParams.c1Inde The starting point of the tensor C1 dimension
 * @param [in] loadDataParams.fetchFilterW The starting position of the w dimension on the convolution kernel
 * @param [in] loadDataParams.fetchFilterH The starting position of the H dimension on the convolution kernel
 * @param [in] loadDataParams.leftTopW Start point of the W dimension on the source operand
 * @param [in] loadDataParams.leftTopH Start point of the H dimension on the source operand
 * @param [in] loadDataParams.strideW W dimension stride
 * @param [in] loadDataParams.strideH H dimension stride
 * @param [in] loadDataParams.filterW Convolution kernel width
 * @param [in] loadDataParams.filterH Convolution kernel height
 * @param [in] loadDataParams.dilationFilterW Convolution kernel width expansion coefficient
 * @param [in] loadDataParams.dilationFilterH Convolution kernel height expansion coefficient
 * @param [in] loadDataParams.jumpStride repeat stride
 * @param [in] loadDataParams.repeatMode repeat mode
 * @param [in] loadDataParams.repeatTime repeat times
 * @param [in] loadDataParams.cSize judge whether to turn on optimization
 * @param [in] loadDataParams.padValue Value of Pad filling value
 */

template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG,
    typename U = PrimT<T>, typename std::enable_if<IsSameType<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadDataImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LoadData3DParamsV1<U>& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncLoadData3dv1(dst, src, loadDataParams, "LoadData with LoadData3DParamsV1")) {
        ASCENDC_REPORT_CHECK_ERROR("LoadData with LoadData3DParamsV1", KernelFuncType::NONE_MODE);
    }
#endif
    ASCENDC_ASSERT((SupportType<PrimT<T>, uint8_t, int8_t, half>()),
        {KERNEL_LOG(KERNEL_ERROR, "Failed to check dtype in "
        "LoadData with LoadData3DParamsV1, current api support dtype combination is src and dst both: uint8_t / int8_t "
        "/ half.");});

    if constexpr (defaultConfig.isSetFMatrix) {
        Load3DSetFMatrixCal(loadDataParams.l1H, loadDataParams.l1W, loadDataParams.padList);
    }
    if constexpr (defaultConfig.isSetPadding) {
        Load3DSetPaddingCal(loadDataParams.padValue);
    }

    CheckTensorPos<T>(src, Hardware::L1, "src", "A1 / B1", "LoadData with LoadData3DParamsV1");
    CheckTensorAlign<T>(src, ONE_BLK_SIZE, "src", "LoadData with LoadData3DParamsV1");
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadData with LoadData3DParamsV1"); // 512B align
        LoadData3DV1L12L0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(),
                              (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadData with LoadData3DParamsV1"); // 512B align
        LoadData3DV1L12L0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(),
                              (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::UB) {
        CheckTensorAlign<T>(dst, ONE_BLK_SIZE, "dst", "LoadData with LoadData3DParamsV1");
        LoadData3DV1L12UBCal((__ubuf__ PrimT<T>*)dst.GetPhyAddr(),
                             (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else {
        ASCENDC_CHECK_TPOSITION((false), "dst", "A2 / B2 / UB", "LoadData with LoadData3DParamsV1",
            ConstDefiner::Instance().logicNameMap.at(static_cast<uint8_t>(dst.GetPosition())));
    }
}

/* **************************************************************************************************
 * LoadData 3dv2                                             *
 * enhanced from v1, suitable for aicore > 200                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.padList padding list
 * @param [in] loadDataParams.l1H operand height
 * @param [in] loadDataParams.l1W operand width
 * @param [in] loadDataParams.channelSize number of channels
 * @param [in] loadDataParams.kExtension Transmission length of K dimension
 * @param [in] loadDataParams.mExtension Transmission length of M dimension
 * @param [in] loadDataParams.kStartPt Start point of K dimension
 * @param [in] loadDataParams.mStartPt Start point of M dimension
 * @param [in] loadDataParams.strideW W dimension stride
 * @param [in] loadDataParams.strideH H dimension stride
 * @param [in] loadDataParams.filterW Convolution kernel width
 * @param [in] loadDataParams.filterH Convolution kernel height
 * @param [in] loadDataParams.dilationFilterW Convolution kernel width expansion coefficient
 * @param [in] loadDataParams.dilationFilterH Convolution kernel height expansion coefficient
 * @param [in] loadDataParams.enTranspose judge whether to enable the transpose function
 * @param [in] loadDataParams.enSmallK Whether to enable the small k feature
 * @param [in] loadDataParams.padValue Value of Pad filling value
 */
template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG,
    typename U = PrimT<T>, typename std::enable_if<IsSameType<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadDataImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LoadData3DParamsV2<U>& loadDataParams)
{
    ASCENDC_ASSERT(CheckFuncLoadData3dv2(dst, src, loadDataParams, "LoadData with LoadData3DParamsV2"), {
        ASCENDC_REPORT_CHECK_ERROR("LoadData with LoadData3DParamsV2", KernelFuncType::NONE_MODE);
    });
    if constexpr (defaultConfig.isSetFMatrix) {
        Load3DSetFMatrixCal(loadDataParams.l1H, loadDataParams.l1W, loadDataParams.padList);
    }
    if constexpr (defaultConfig.isSetPadding) {
        Load3DSetPaddingCal(loadDataParams.padValue);
    }

    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
#if __NPU_ARCH__ == 2002
    ASCENDC_ASSERT((SupportType<PrimT<T>, uint8_t, int8_t, half, int4b_t>()),
        {KERNEL_LOG(KERNEL_ERROR, "Failed to check dtype in "
        "LoadData with LoadData3DParamsV2, current api support dtype combination is src and dst both: uint8_t / int8_t "
        "/ half / int4b_t.");});
#elif __NPU_ARCH__ == 2201
    if (dstScope == Hardware::L0A) {
        ASCENDC_ASSERT((SupportType<PrimT<T>, uint8_t, int8_t, half, bfloat16_t, float, uint32_t, int32_t, int4b_t>()),
            {KERNEL_LOG(KERNEL_ERROR, "Failed to check dtype in LoadData with LoadData3DParamsV2 when dst position is "
            "A2, current api support dtype combination is src and dst both: uint8_t / int8_t / half / bfloat16_t / "
            "float / uint32_t / int32_t / int4b_t.");});
    } else if (dstScope == Hardware::L0B) {
        ASCENDC_ASSERT((SupportType<PrimT<T>, half, bfloat16_t, float, uint32_t, int32_t>()), {KERNEL_LOG(KERNEL_ERROR,
            "Failed to check dtype in LoadData with LoadData3DParamsV2 when dst position is B2, current api support "
            "dtype combination is src and dst both: half / bfloat16_t / float / uint32_t / int32_t.");});
    }
#elif __NPU_ARCH__ == 3510
    ASCENDC_ASSERT(loadDataParams.kExtension * sizeof(T) % ONE_BLK_SIZE == 0, {
        KERNEL_LOG(KERNEL_ERROR, "kExtension * sizeof(T) must be a multiple of 32");});
    ASCENDC_ASSERT(loadDataParams.mExtension % 16 == 0, {
        KERNEL_LOG(KERNEL_ERROR, "mExtension should be a multiple of 16");});
    ASCENDC_ASSERT(loadDataParams.kStartPt * sizeof(T) % ONE_BLK_SIZE == 0, {
        KERNEL_LOG(KERNEL_ERROR, "kStartPt * sizeof(T) must be a multiple of 32");});
    ASCENDC_ASSERT(loadDataParams.mStartPt % 16 == 0, {
        KERNEL_LOG(KERNEL_ERROR, "mStartPt should be a multiple of 16");});
#elif __NPU_ARCH__ == 3102
    if (dstScope == Hardware::L0A) {
        ASCENDC_ASSERT((SupportType<PrimT<T>, uint8_t, int8_t, half, uint16_t, int16_t, int4b_t>()),
            {KERNEL_LOG(KERNEL_ERROR,
            "Failed to check dtype in LoadData with LoadData3DParamsV2 when dst position is A2, current api support "
            "dtype combination is src and dst both: uint8_t / int8_t / half / uint16_t / int16_t / int4b_t.");});
    } else {
        ASCENDC_ASSERT((SupportType<PrimT<T>, half, int16_t, uint16_t>()),
        {KERNEL_LOG(KERNEL_ERROR, "Failed to check dtype "
            "in LoadData with LoadData3DParamsV2 when dst position is B2, current api support dtype combination is src "
            "and dst both: half / int16_t / uint16_t.");});
    }
#endif

    CheckTensorPos<T>(src, Hardware::L1, "src", "A1 / B1", "LoadData with LoadData3DParamsV2");
    if (dstScope == Hardware::L0A) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadData with LoadData3DParamsV2");
        LoadData3DV2L12L0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(),
                              (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadData with LoadData3DParamsV2");
        LoadData3DV2L12L0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(),
                              (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::UB) {
        CheckTensorAlign<T>(dst, ONE_BLK_SIZE, "dst", "LoadData with LoadData3DParamsV2");
        LoadData3DV2L12UBCal((__ubuf__ PrimT<T>*)dst.GetPhyAddr(),
                             (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else {
        ASCENDC_CHECK_TPOSITION((false), "dst", "A2 / B2 / UB", "LoadData with LoadData3DParamsV2",
            ConstDefiner::Instance().logicNameMap.at(static_cast<uint8_t>(dst.GetPosition())));
    }
}

#if ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) ||            \
     (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
// cce compiler process load3d bfloat16_t using B8, so use the half dtype instead
template <const IsResetLoad3dConfig& defaultConfig>
[[deprecated("NOTICE: LoadData<IsResetLoad3dConfig> has been deprecated and will be removed in the next version."
             " Please do not use it!")]]
__aicore__ inline void LoadData(const LocalTensor<bfloat16_t>& dst, const LocalTensor<bfloat16_t>& src,
    const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    LoadDataImpl<bfloat16_t, defaultConfig>(dst, src, loadDataParams);
}
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG,
    typename U = PrimT<T>, typename std::enable_if<IsSameType<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadDataWithStrideImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LoadData3DParamsV2<U>& loadDataParams)
{
    ASCENDC_ASSERT(CheckFuncLoadData3dv2(dst, src, loadDataParams, "LoadDataWithStride with LoadData3DParamsV2"), {
        ASCENDC_REPORT_CHECK_ERROR("LoadDataWithStride with LoadData3DParamsV2", KernelFuncType::NONE_MODE);
    });
    if constexpr (defaultConfig.isSetFMatrix) {
        Load3DSetFMatrixCal(loadDataParams.l1H, loadDataParams.l1W, loadDataParams.padList);
    }
    if constexpr (defaultConfig.isSetPadding) {
        Load3DSetPaddingCal(loadDataParams.padValue);
    }

    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    ASCENDC_ASSERT(loadDataParams.kExtension * sizeof(T) % ONE_BLK_SIZE == 0, {
        KERNEL_LOG(KERNEL_ERROR, "kExtension * sizeof(T) must be a multiple of 32");});
    ASCENDC_ASSERT(loadDataParams.mExtension % 16 == 0, {
        KERNEL_LOG(KERNEL_ERROR, "mExtension should be a multiple of 16");});
    ASCENDC_ASSERT(loadDataParams.kStartPt * sizeof(T) % ONE_BLK_SIZE == 0, {
        KERNEL_LOG(KERNEL_ERROR, "kStartPt * sizeof(T) must be a multiple of 32");});
    ASCENDC_ASSERT(loadDataParams.mStartPt % 16 == 0, {
        KERNEL_LOG(KERNEL_ERROR, "mStartPt should be a multiple of 16");});

    CheckTensorPos<T>(src, Hardware::L1, "src", "A1 / B1", "LoadDataWithStride with LoadData3DParamsV2");
    if (dstScope == Hardware::L0A) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadDataWithStride with LoadData3DParamsV2");
        LoadData3DV2L12L0AWithStrideCal((__ca__ PrimT<T>*)dst.GetPhyAddr(),
                            (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadDataWithStride with LoadData3DParamsV2");
        LoadData3DV2L12L0BWithStrideCal((__cb__ PrimT<T>*)dst.GetPhyAddr(),
                            (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::UB) {
        CheckTensorAlign<T>(dst, ONE_BLK_SIZE, "dst", "LoadDataWithStride with LoadData3DParamsV2");
        LoadData3DV2L12UBWithStrideCal((__ubuf__ PrimT<T>*)dst.GetPhyAddr(),
                            (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else {
        ASCENDC_CHECK_TPOSITION((false), "dst", "A2 / B2 / UB", "LoadDataWithStride with LoadData3DParamsV2",
            ConstDefiner::Instance().logicNameMap.at(static_cast<uint8_t>(dst.GetPosition())));
    }
}

// cce compiler process load3d bfloat16_t using B8, so use the half dtype instead
template <const IsResetLoad3dConfig& defaultConfig>
[[deprecated("NOTICE: LoadDataWithStride<IsResetLoad3dConfig> has been deprecated and will be removed in the next version."
            " Please do not use it!")]]
__aicore__ inline void LoadDataWithStride(const LocalTensor<bfloat16_t>& dst, const LocalTensor<bfloat16_t>& src,
    const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    LoadDataWithStrideImpl<bfloat16_t, defaultConfig>(dst, src, loadDataParams);
}
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
template <typename T, typename U>
__aicore__ inline __inout_pipe__(MTE2) void LoadDataImpl(const LocalTensor<T>& dst, const GlobalTensor<U>& src,
    const LoadData2DParamsV2& loadDataParams, const Nd2NzParamsV2& nd2nzParams)
{
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L1) {
        LoadData2DGM2L1Cal((__cbuf__ T *)dst.GetPhyAddr(), (__gm__ U *)src.GetPhyAddr(), loadDataParams, nd2nzParams);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "dst only support A1/B1"); });
    }
}
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <TPosition Dst, TPosition Src, typename T>
__aicore__ inline void LoadDataImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const Load3DBitModeParam& loadDataParams)
{
    CheckTensorAlign<T>(src, ONE_BLK_SIZE, "src", "LoadData with LoadData3DParams");
    CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadData with LoadData3DParams");

    if constexpr (Src != TPosition::A1 && Src != TPosition::A2) {
        ASCENDC_CHECK_TPOSITION(false, "src", "A1 / B1",
            "LoadData with LoadDataBitModeParams",
            ConstDefiner::Instance().logicNameMap.at(static_cast<uint8_t>(src.GetPosition())));
    };
    if constexpr (Dst == TPosition::A2) {
        LoadData3DV2L12L0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(),
                            (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if constexpr (Dst == TPosition::B2) {
        LoadData3DV2L12L0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(),
                            (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else {
        ASCENDC_CHECK_TPOSITION(false, "dst", "A2 / B2",
            "LoadData with LoadData3DParams",
            ConstDefiner::Instance().logicNameMap.at(static_cast<uint8_t>(dst.GetPosition())));
    }
}
#endif
/* **************************************************************************************************
 * LoadData 3dv2Pro                                             *
 * enhanced from v1, suitable for aicore > 200                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.channelSize number of channels
 * @param [in] loadDataParams.GetKExtension() Transmission length of K dimension
 * @param [in] loadDataParams.GetMExtension() Transmission length of M dimension
 * @param [in] loadDataParams.GetKStartPt() Start point of K dimension
 * @param [in] loadDataParams.GetMStartPt() Start point of M dimension
 * @param [in] loadDataParams.GetStrideW() W dimension stride
 * @param [in] loadDataParams.GetStrideH() H dimension stride
 * @param [in] loadDataParams.GetFilterW() Convolution kernel width
 * @param [in] loadDataParams.GetFilterH() Convolution kernel height
 * @param [in] loadDataParams.GetDilationFilterW() Convolution kernel width expansion coefficient
 * @param [in] loadDataParams.GetDilationFilterH() Convolution kernel height expansion coefficient
 * @param [in] loadDataParams.enTranspose judge whether to enable the transpose function
 * @param [in] loadDataParams.enSmallK Whether to enable the small k feature
 */
template <typename T>
__aicore__ inline void LoadDataImpl(const LocalTensor<T>& dst, const LocalTensor<T>& src,
    const LoadData3DParamsV2Pro& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncLoadData3dv2Pro(dst, src, loadDataParams, "LoadData with LoadData3DParamsV2Pro")) {
        ASCENDC_REPORT_CHECK_ERROR("LoadData with LoadData3DParamsV2Pro", KernelFuncType::NONE_MODE);
    }
#endif
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A) {
        LoadData3DV2L12L0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(),
                              (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        LoadData3DV2L12L0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(),
                              (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::UB) {
        LoadData3DV2L12UBCal((__ubuf__ PrimT<T>*)dst.GetPhyAddr(),
                             (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else {
        ASCENDC_CHECK_TPOSITION((false), "dst", "A1 / A2 / UB", "LoadData with LoadData3DParamsV2Pro",
            ConstDefiner::Instance().logicNameMap.at(static_cast<uint8_t>(dst.GetPosition())));
    }
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003))
// cce compiler process load3d bfloat16_t using B8, so use the half dtype instead
template <>
__aicore__ inline void LoadDataImpl(const LocalTensor<bfloat16_t>& dst, const LocalTensor<bfloat16_t>& src,
    const LoadData3DParamsV2Pro& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT(CheckFuncLoadData3dv2Pro(dst, src, loadDataParams, "loaddata3dv2Pro"), {
        KERNEL_LOG(KERNEL_ERROR, "check loaddata3dv2Pro instr failed");
    });
#endif

    const Hardware dstScope = GetPhyType((QuePosition)dst.GetPosition());
    // compiler process bfloat16_t load3dv2 is using B8 type, so cast to half which is using B16 type
    if (dstScope == Hardware::L0A) {
        LoadData3DV2L12L0ACal((__ca__ half*)dst.GetPhyAddr(),
            (__cbuf__ half*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        LoadData3DV2L12L0BCal((__cb__ half*)dst.GetPhyAddr(),
            (__cbuf__ half*)src.GetPhyAddr(), loadDataParams);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "dst only support A2/B2"); });
    }
}
#endif

/* **************************************************************************************************
 * Mmad                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Mmad
 * @brief Matrix multiplication and addition
 * @param [out] dst output LocalTensor
 * @param [in] fm input LocalTensor
 * @param [in] filter input LocalTensor
 * @param [in] mmadParams.m Left matrix row number
 * @param [in] mmadParams.n right matrix column number
 * @param [in] mmadParams.k Left matrix column number m
 * @param [in] mmadParams.unitFlag whether enable unit flag
 * @param [in] mmadParams.kDirectionAlign is the indicator for alignment in L0A/L0B in the K direction
 * @param [in] mmadParams.cmatrixSource indicates the C matrix source, 1: the C matrix is in bias table buffer, 0: the C
 * matrix is in L0C
 * @param [in] mmadParams.cmatrixInitVal indicates the initial matrix, 1: the number in C matrix is 0, 0：use the real
 * number in C matrix
 */

template <typename T, typename U, typename S>
__aicore__ inline void MmadImpl(const LocalTensor<T>& dst, const LocalTensor<U>& fm,
    const LocalTensor<S>& filter, const MmadParams& mmadParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckMmadParams(dst, fm, filter, mmadParams, "Mmad")) {
        ASCENDC_REPORT_CHECK_ERROR("Mmad", KernelFuncType::NONE_MODE);
    }
    CheckMmadAlign(dst, fm, filter);
#endif
    MmadCal((__cc__ PrimT<T>*)dst.GetPhyAddr(), (__ca__ PrimT<U>*)fm.GetPhyAddr(),
        (__cb__ PrimT<S>*)filter.GetPhyAddr(), mmadParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadImpl(const LocalTensor<T>& dst, const LocalTensor<U>& fm,
    const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadParams& mmadParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckMmadParams(dst, fm, filter, bias, mmadParams, "Mmad with bias")) {
        ASCENDC_REPORT_CHECK_ERROR("Mmad with bias", KernelFuncType::NONE_MODE);
    }
    CheckMmadAlign(dst, fm, filter);
    CheckTensorAlign<V>(bias, 128, "bias", "Mmad");
#if __NPU_ARCH__ == 2201
    ASCENDC_ASSERT((SupportType<Tuple<PrimT<T>, PrimT<U>, PrimT<S>, PrimT<V>>,
        Tuple<int32_t, int8_t, int8_t, int32_t>,
        Tuple<float, half, half, float>, Tuple<float, float, float, float>,
        Tuple<float, bfloat16_t, bfloat16_t, float>>()),
        {KERNEL_LOG(KERNEL_ERROR, "Failed to check dtype in Mmad, current api support dtype combination is "
        "Dst: int32_t, src0: int8_t, src1: int8_t, Bias: int32_t; Dst: float, src0: half, src1: half, Bias: float; "
        "Dst: float, src0: float, src1: float, Bias: float; "
        "Dst: float, src0: bfloat16_t, src1: bfloat16_t, Bias: float");});
#endif
#endif
    const Hardware biasScope = GetPhyType((TPosition)bias.GetPosition());
    bool cmatrixSource = false;
    if (biasScope == Hardware::BIAS) {
        cmatrixSource = true;
    } else if (biasScope == Hardware::L0C) {
        cmatrixSource = false;
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR,
            "Failed to check bias tensor position in Mmad, supported positions are CO1 or C2"); });
    }
    MmadCal((__cc__ PrimT<T>*)dst.GetPhyAddr(), (__ca__ PrimT<U>*)fm.GetPhyAddr(),
        (__cb__ PrimT<S>*)filter.GetPhyAddr(), (uint64_t)bias.GetPhyAddr(), mmadParams, cmatrixSource);
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
template <typename T, typename U, typename S>
__aicore__ inline void MmadImpl(const LocalTensor<T>& dst, const LocalTensor<U>& fm,
    const LocalTensor<S>& filter, const MmadBitModeParams& mmadParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckMmadParams(dst, fm, filter, mmadParams.GetConfig0(), "Mmad")) {
        ASCENDC_REPORT_CHECK_ERROR("Mmad", KernelFuncType::NONE_MODE);
    }
    CheckMmadAlign(dst, fm, filter);
#endif
    MmadCal((__cc__ PrimT<T>*)dst.GetPhyAddr(), (__ca__ PrimT<U>*)fm.GetPhyAddr(),
        (__cb__ PrimT<S>*)filter.GetPhyAddr(), mmadParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadImpl(const LocalTensor<T>& dst, const LocalTensor<U>& fm,
    const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadBitModeParams& mmadParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckMmadParams(dst, fm, filter, bias, mmadParams.GetConfig0(), "Mmad with bias")) {
        ASCENDC_REPORT_CHECK_ERROR("Mmad with bias", KernelFuncType::NONE_MODE);
    }
    CheckMmadAlign(dst, fm, filter);
    CheckTensorAlign<V>(bias, 128, "bias", "Mmad");
#endif
    const Hardware biasScope = GetPhyType((TPosition)bias.GetPosition());
    bool cmatrixSource = false;
    if (biasScope == Hardware::BIAS) {
        cmatrixSource = true;
    } else if (biasScope == Hardware::L0C) {
        cmatrixSource = false;
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR,
            "Failed to check bias tensor position in Mmad, supported positions are CO1 or C2"); });
    }
    MmadCal((__cc__ PrimT<T>*)dst.GetPhyAddr(), (__ca__ PrimT<U>*)fm.GetPhyAddr(),
        (__cb__ PrimT<S>*)filter.GetPhyAddr(), (uint64_t)bias.GetPhyAddr(), mmadParams);
}
#endif

#if __NPU_ARCH__ == 2201
template <typename T = int32_t, typename U = int8_t,
    typename std::enable_if<IsSameType<PrimT<T>, int32_t>::value, bool>::type = true,
    typename std::enable_if<IsSameType<PrimT<U>, int8_t>::value, bool>::type = true>
__aicore__ inline void MmadSpImpl(const LocalTensor<T>& dst, const LocalTensor<U>& fm,
    const LocalTensor<U>& filter, const MmadParams& mmadParams)
{
    CheckTensorPos<T>(dst, Hardware::L0C, "dst", "CO1", "MmadWithSparse");
    CheckTensorPos<U>(fm, Hardware::L0A, "fm", "A2", "MmadWithSparse");
    CheckTensorPos<U>(filter, Hardware::L0B, "filter", "B2", "MmadWithSparse");
    CheckTensorAlign<T>(dst, 1024, "dst", "MmadWithSparse");             // 1024B aligned
    CheckTensorAlign<U>(fm, VALUE_512, "fm", "MmadWithSparse");           // 512B aligned
    CheckTensorAlign<U>(filter, VALUE_512, "filter", "MmadWithSparse");   // 512B aligned
    ASCENDC_CHECK_VALUE_RANGE(mmadParams.m, 0, UINT12_MAX, "m", "MmadWithSparse");
    ASCENDC_CHECK_VALUE_RANGE(mmadParams.n, 0, UINT12_MAX, "n", "MmadWithSparse");
    ASCENDC_CHECK_VALUE_RANGE(mmadParams.k, 0, UINT12_MAX, "k", "MmadWithSparse");
    MmadSpCal((__cc__ int32_t*)dst.GetPhyAddr(), (__ca__ int8_t*)fm.GetPhyAddr(),
        (__cb__ int8_t*)filter.GetPhyAddr(), mmadParams);
}

template <typename T = int8_t, typename U = uint8_t,
    typename std::enable_if<IsSameType<PrimT<T>, int8_t>::value, bool>::type = true,
    typename std::enable_if<IsSameType<PrimT<U>, uint8_t>::value, bool>::type = true>
__aicore__ inline void LoadDataWithSparseImpl(const LocalTensor<T> &dst, const LocalTensor<T> &src,
    const LocalTensor<U> &idx, const LoadData2dParams &loadDataParam)
{
    CheckTensorPos<T>(dst, Hardware::L0B, "dst", "B2", "LoadDataWithSparse");
    CheckTensorPos<T>(src, Hardware::L1, "src", "B1", "LoadDataWithSparse");
    CheckTensorPos<U>(idx, Hardware::L1, "idx", "B1", "LoadDataWithSparse");
    CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadDataWithSparse");        // 512B align
    CheckTensorAlign<T>(src, ONE_BLK_SIZE, "src", "LoadDataWithSparse");     // 32B align
    CheckTensorAlign<U>(idx, ONE_BLK_SIZE, "idx", "LoadDataWithSparse");    // 32B align
    LoadDataWithSparseCal(dst, src, idx, loadDataParam);
}
#endif

#if __NPU_ARCH__ == 2002
template <typename T = int8_t, typename std::enable_if<IsSameType<PrimT<T>, int8_t>::value, bool>::type = true>
__aicore__ inline void LoadUnzipIndexImpl(const GlobalTensor<T>& src, uint32_t numOfIndexTabEntry)
{
    LoadUnzipIndexCal(src, numOfIndexTabEntry);
}
#endif

/* **************************************************************************************************
 * BroadCastVecToMM                                             *
 * ************************************************************************************************* */
template <typename T, typename U>
__aicore__ inline __inout_pipe__(V) void BroadCastVecToMMImpl(const LocalTensor<T> &dst,
    const LocalTensor<U> &src, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap,
    const uint8_t dstGap)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncBroadCastToMM(dst, src, blockCount, blockLen, srcGap, dstGap, "BroadCastVecToMM")) {
        ASCENDC_REPORT_CHECK_ERROR("BroadCastVecToMM", KernelFuncType::NONE_MODE);
    }
#endif
    BroadCastVecToMMCal((__cc__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<U>*)src.GetPhyAddr(),
        blockCount, blockLen, srcGap, dstGap);
}

/* **************************************************************************************************
 * SetLoadDataPaddingValue                                             *                                            *
 * ************************************************************************************************* */
/*
 * @ingroup SetLoadDataPaddingValue
 * @brief setting loadData pad value
 * @param [in]padValue padding value
 */
template <typename T>
__aicore__ inline void Load3DSetPaddingImpl(const T padValue)
{
    Load3DSetPaddingCal(padValue);
}

/* **************************************************************************************************
 * Fill                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Fill
 * @brief L0A/L0B value initializing
 * @param [out] dst output LocalTensor
 * @param [in] InitConstValueParams.repeatTimes repeat times
 * @param [in] InitConstValueParams.repeatTimes blockNum block number
 * @param [in] InitConstValueParams.dstGap interval between the previous tail and the next block head
 * @param [in] InitConstValueParams.initValue initialize Value
 */
template <typename T, typename U = PrimT<T>,
    typename std::enable_if<IsSameType<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void FillImpl(const LocalTensor<T> &dst,
    const InitConstValueParams<U> &initConstValueParams)
{
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "Fill when TPosition is A2");
        InitL0ANzMatrixCal((__ca__ PrimT<T>*)dst.GetPhyAddr(), initConstValueParams);
    } else if (dstScope == Hardware::L0B) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "Fill when TPosition is B2");
        InitL0BNzMatrixCal((__cb__ PrimT<T>*)dst.GetPhyAddr(), initConstValueParams);
    } else if (dstScope == Hardware::L1) {
        CheckTensorAlign<T>(dst, ONE_BLK_SIZE, "dst", "Fill when TPosition is A1 / B1");
        InitL1BufferCal((__cbuf__ PrimT<T>*)dst.GetPhyAddr(), initConstValueParams);
    } else {
        ASCENDC_CHECK_TPOSITION(false, "dst", "A1 / B1 / A2 / B2", "Fill",
            ConstDefiner::Instance().logicNameMap.at(static_cast<uint8_t>(dst.GetPosition())));
    }
}

/* **************************************************************************************************
 * InitConstValue                                             *
 * ************************************************************************************************* */
/*
 * @ingroup InitConstValue
 * @brief L0A/L0B value initializing
 * @param [out] dst output LocalTensor
 * @param [in] InitConstValueParams.repeatTimes repeat times
 * @param [in] InitConstValueParams.repeatTimes blockNum block number
 * @param [in] InitConstValueParams.dstGap interval between the previous tail and the next block head
 * @param [in] InitConstValueParams.initValue initialize Value
 */
template <typename T, typename U = PrimT<T>,
    typename std::enable_if<IsSameType<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void InitConstValueImpl(const LocalTensor<T> &dst,
    const InitConstValueParams<U> &initConstValueParams)
{
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "InitConstValue when TPosition is A2");
        InitL0ANzMatrixCal((__ca__ PrimT<T>*)dst.GetPhyAddr(), initConstValueParams);
    } else if (dstScope == Hardware::L0B) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "InitConstValue when TPosition is B2");
        InitL0BNzMatrixCal((__cb__ PrimT<T>*)dst.GetPhyAddr(), initConstValueParams);
    } else if (dstScope == Hardware::L1) {
        CheckTensorAlign<T>(dst, ONE_BLK_SIZE, "dst", "InitConstValue when TPosition is A1 / B1");
        InitL1BufferCal((__cbuf__ PrimT<T>*)dst.GetPhyAddr(), initConstValueParams);
    } else {
        ASCENDC_CHECK_TPOSITION(false, "dst", "A1 / B1 / A2 / B2", "InitConstValue",
            ConstDefiner::Instance().logicNameMap.at(static_cast<uint8_t>(dst.GetPosition())));
    }
}

/* **************************************************************************************************
 * SetFmatrix                                             *
 * ************************************************************************************************* */
/*
 * @ingroup SetFmatrix
 * @brief setting fmatrix
 * @param [in]l1H operand height
 * @param [in]l1W operand width
 * @param [in]padList padding list
 * @param [in]fmatrixMode set fmatrix_a or fmatrix_b
 */
__aicore__ inline void SetFmatrixImpl(uint16_t l1H, uint16_t l1W, const uint8_t padList[4],
    const FmatrixMode &fmatrixMode)
{
    if (fmatrixMode == FmatrixMode::FMATRIX_LEFT) {
        Load3DSetFMatrixCal(l1H, l1W, padList);
    } else if (fmatrixMode == FmatrixMode::FMATRIX_RIGHT) {
        Load3DSetFMatrixBCal(l1H, l1W, padList);
    }
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
__aicore__ inline void SetFmatrixImpl(const SetFMatrixBitModeParams &param, const FmatrixMode &fmatrixMode)
{
    if (fmatrixMode == FmatrixMode::FMATRIX_LEFT) {
        Load3DSetFMatrixCal(param.GetConfig0());
    } else if (fmatrixMode == FmatrixMode::FMATRIX_RIGHT) {
        Load3DSetFMatrixBCal(param.GetConfig0());
    }
}
#endif

/* **************************************************************************************************
 * SetLoadDataBoundary                                             *
 * ************************************************************************************************* */
/*
 * @ingroup SetFmatrix
 * @brief setting loaddata boundary
 * @param [in]boundaryValue
 */
__aicore__ inline void SetLoadDataBoundaryImpl(uint32_t boundaryValue)
{
    SetLoadDataBoundaryCal(boundaryValue);
}

/* **************************************************************************************************
 * SetLoadDataRepeat                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataRepeatImpl(const LoadDataRepeatParam& repeatParams)
{
    SetLoadDataRepeatCal(repeatParams);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
__aicore__ inline void SetLoadDataRepeatWithStrideImpl(const LoadDataRepeatParamWithStride& repeatParams)
{
    SetLoadDataRepeatWithStrideCal(repeatParams);
}
#endif

/* **************************************************************************************************
 * LoadDataUnzipImpl                                             *
 * ************************************************************************************************* */
/*
 * @ingroup LoadDataUnzip
 * @brief loadData and unzip
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 */
template <typename T>
__aicore__ inline void LoadDataUnzipImpl(const LocalTensor<T>& dst, const GlobalTensor<T>& src)
{
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
#if ASCENDC_CPU_DEBUG
    if (dstScope == Hardware::L1) {
        CheckTensorAlign<T>(dst,  ONE_BLK_SIZE, "dst", "LoadDataUnzip in A1 / B1"); // 32B align
    } else if (dstScope == Hardware::L0A || dstScope == Hardware::L0B) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadDataUnzip in B2");               // 512B align
    }
    if constexpr(!SupportType<PrimT<T>, int8_t>()) {
        ASCENDC_ASSERT(false, {KERNEL_LOG(KERNEL_ERROR, "Failed to check dtype in LoadDataUnzip, current api support "
            "dtype combination is dst: int8_t.");});
    }
#endif
    if (dstScope == Hardware::L1) {
        LoadDataUnzipToL1Cal((__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr());
    } else if (dstScope == Hardware::L0A) {
        LoadDataUnzipToL0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr());
    } else if (dstScope == Hardware::L0B) {
        LoadDataUnzipToL0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr());
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Failed to check dst tensor position in LoadDataUnzip, "
            "supported positions are A1 / B1 / B2"); });
    }
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_BASE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_BASE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_BASE_IMPL_H__
#endif