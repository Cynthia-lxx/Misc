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
 * \file reduce_tiling.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_TILING_H__
#endif

#ifndef LIB_REDUCE_REDUCE_TILING_H
#define LIB_REDUCE_REDUCE_TILING_H
#include <cstdint>
#include "graph/types.h"
#include "graph/tensor.h"

namespace AscendC {
/*
 * @brief Reduce pattern
 * R means reduce axis, A means non-reduce axis
 * each A/R represents a dimension
*/
#ifndef ASCC_ENUM_REDUCEPATTERN
#define ASCC_ENUM_REDUCEPATTERN
enum class ReducePattern : uint32_t {
    AR = 0,
    RA = 1,
    R,
    ARA,
    ARAR,
    ARARA,
    ARARAR,
    ARARARA,
    ARARARAR,
    ARARARARA,
    RAR,
    RARA,
    RARAR,
    RARARA,
    RARARAR,
    RARARARA,
};
#endif // ASCC_ENUM_REDUCEPATTERN

/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] srcShape, src tensor shape
 * \param [in] dataType, actual data type of the input
 * \param [in] pattern, reduce pattern
 * \param [in] isSrcInnerPad, whether the last axis of input data is padded
 * \param [in] isReuseSource, whether to reuse the input space of the source operand
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetReduceProdMaxMinTmpSize(const ge::Shape& srcShape, const ge::DataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] srcShape, src tensor shape
 * \param [in] dataType, actual data type of the input
 * \param [in] pattern, reduce pattern
 * \param [in] isSrcInnerPad, whether the last axis of input data is padded
 * \param [in] isReuseSource, whether to reuse the input space of the source operand
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetReduceSumMaxMinTmpSize(const ge::Shape& srcShape, const ge::DataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] srcShape, src tensor shape
 * \param [in] dataType, actual data type of the input
 * \param [in] pattern, reduce pattern
 * \param [in] isSrcInnerPad, whether the last axis of input data is padded
 * \param [in] isReuseSource, whether to reuse the input space of the source operand
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetReduceMeanMaxMinTmpSize(const ge::Shape& srcShape, const ge::DataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] srcShape, src tensor shape
 * \param [in] dataType, actual data type of the input
 * \param [in] pattern, reduce pattern
 * \param [in] isSrcInnerPad, whether the last axis of input data is padded
 * \param [in] isReuseSource, whether to reuse the input space of the source operand
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetReduceMaxMaxMinTmpSize(const ge::Shape& srcShape, const ge::DataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] srcShape, src tensor shape
 * \param [in] dataType, actual data type of the input
 * \param [in] pattern, reduce pattern
 * \param [in] isSrcInnerPad, whether the last axis of input data is padded
 * \param [in] isReuseSource, whether to reuse the input space of the source operand
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetReduceMinMaxMinTmpSize(const ge::Shape& srcShape, const ge::DataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] srcShape, src tensor shape
 * \param [in] dataType, actual data type of the input
 * \param [in] pattern, reduce pattern
 * \param [in] isSrcInnerPad, whether the last axis of input data is padded
 * \param [in] isReuseSource, whether to reuse the input space of the source operand
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetReduceAnyMaxMinTmpSize(const ge::Shape& srcShape, const ge::DataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);

/*!
 * \brief This interface is used to obtain the maximum and minimum temporary space reserved or applied.
 * The developer selects a proper space size based on this range as the tiling parameter.
 *
 * \param [in] srcShape, src tensor shape
 * \param [in] dataType, actual data type of the input
 * \param [in] pattern, reduce pattern
 * \param [in] isSrcInnerPad, whether the last axis of input data is padded
 * \param [in] isReuseSource, whether to reuse the input space of the source operand
 * \param [out] maxValue, maximum temporary space required
 * \param [out] minValue, minimum temporary space required
 */
void GetReduceAllMaxMinTmpSize(const ge::Shape& srcShape, const ge::DataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);
}  // namespace AscendC
#endif // LIB_REDUCE_REDUCE_TILING_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_TILING_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_TILING_H__
#endif