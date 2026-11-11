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
 * \file bmm_tiling.h
 * \brief
 */
#ifndef LIB_MATMUL_BMM_TILING_H
#define LIB_MATMUL_BMM_TILING_H

#include "matmul_tiling_base.h"
#include "matmul_tilingdata.h"
#include "kernel_tiling/kernel_tiling.h"
#include "tiling/platform/platform_ascendc.h"

namespace matmul_tiling {
/**
 * @class MultiCoreMatmulTiling
 * @brief matmul tiling for multi core
 *
 * Provide a set of Matmul Tiling APIs for multi-core computing scenarios,
 * enabling users to easily obtain the Tiling parameters required for Matmul kernel calculations.
 * Users only need to pass information such as the Position, Format, and Dtype of matrices A/B/C,
 * and by calling the API interface, they can get the relevant parameters from the
 * TCubeTiling structure in the Matmul Init API.
*/
class MultiCoreMatmulTiling : public MatmulApiTilingBase {
public:
    /**
     * @brief no-parameter constructor.
     */
    MultiCoreMatmulTiling() {};
    /**
     * @brief Use the PlatformAscendC class to pass information.
     * @param ascendcPlatform : hardware platform information.
     */
    explicit MultiCoreMatmulTiling(const platform_ascendc::PlatformAscendC &ascendcPlatform)
        : MatmulApiTilingBase(ascendcPlatform){};
    /**
     * @brief Use the PlatformInfo class to pass information.
     * Input hardware version and memory size provided by each hardware unit in the AI Core.
     * @param platform : platform information.
     */
    explicit MultiCoreMatmulTiling(const PlatformInfo &platform)
        : MatmulApiTilingBase(platform) {};
    /**
     * @brief Set the allowed block dim.
     * @param dim : core number.
     */
    int32_t SetDim(int32_t dim);
    /**
     * @brief Set the dimensions m, n, and k for the Matmul computation,
        which can represent either the original full matrix or a submatrix, measured in elements.
        The matrix multiplication of these dimensions can be performed using a single core or multiple cores.
     * @param m : Set the M-direction size calculated by Matmul. The unit is element.
     * @param n : Set the N-direction size calculated by Matmul. The unit is element.
     * @param k : Set the K-direction size calculated by Matmul. The unit is element.
     */
    int32_t SetShape(int32_t m, int32_t n, int32_t k) override;  // Set the size of the original input
    /**
     * @brief Set the shape for single-core computation of Matmul as singleMIn, singleNIn, and singleKIn,
        with units in elements.
     * @param singleMIn : The size of singleMIn is set in units of elements, with a default value of -1.
        A value of -1 indicates that no specific singleMIn is set, and this value is calculated by the tiling
        function itself.
     * @param singleNIn : The size of singleNIn is set in units of elements, with a default value of -1.
        A value of -1 indicates that no specific singleNIn is set, and this value is calculated by the tiling
        function itself.
     * @param singleKIn : The size of singleKIn is set in units of elements, with a default value of -1.
        A value of -1 indicates that no specific singleKIn is set, and this value is calculated by the tiling
        function itself.
     */
    virtual int32_t SetSingleShape(int32_t singleMIn = -1, int32_t singleNIn = -1, int32_t singleKIn = -1);
    /**
     * @brief Get Tiling parameters.
     * @param tiling : The Tiling structure stores the final tiling results. The TCubeTiling structure with the
        optiling namespace, which define the Matmul TilingData on the Host side.
     */
    int64_t GetTiling(optiling::TCubeTiling &tiling) override;
    /**
     * @brief Get Tiling parameters.
     * @param tiling : The Tiling structure stores the final tiling results. The TCubeTiling structure without the
        optiling namespace, Matmul TilingData defined on the Kernel side.
     */
    int64_t GetTiling(AscendC::tiling::TCubeTiling &tiling) override;
    /**
     * @brief Set the maximum and minimum values for singleCoreM/singleCoreN/singleCoreK.
     * @param maxM : Set the maximum value of singleCoreM to -1, which indicates that no specific maximum value
        for singleCoreM is set. This value is calculated by the Tiling function itself.
     * @param maxN : Set the maximum value of singleCoreN to -1, which indicates that no specific maximum value
        for singleCoreN is set. This value is calculated by the Tiling function itself.
     * @param maxK : Set the maximum value of singleCoreK to -1, which indicates that no specific maximum value
        for singleCoreK is set. This value is calculated by the Tiling function itself.
     * @param minM : Set the minimum value of singleCoreM to -1, which indicates that no specific minimum value
        for singleCoreM is set. This value is calculated by the Tiling function itself.
     * @param minN : Set the minimum value of singleCoreN to -1, which indicates that no specific minimum value
        for singleCoreN is set. This value is calculated by the Tiling function itself.
     * @param minK : Set the minimum value of singleCoreK to -1, which indicates that no specific minimum value
        for singleCoreK is set. This value is calculated by the Tiling function itself.
     */
    virtual int32_t SetSingleRange(int32_t maxM = -1, int32_t maxN = -1, int32_t maxK = -1,
        int32_t minM = -1, int32_t minN = -1, int32_t minK = -1)
    {
        this->maxSingleM = maxM;
        this->maxSingleN = maxN;
        this->maxSingleK = maxK;
        this->minSingleM = minM;
        this->minSingleN = minN;
        this->minSingleK = minK;
        return 0;
    }
    /**
     * @brief When performing multi-core splitting, set the alignment value for singleCoreM/singleCoreN/singleCoreK.
        for example, setting the alignment value for singleCoreM to 64(in units of elements) ensures that the split
        singleCoreM is a multiple of 64.
     * @param alignM : The alignment value for singleCoreM. If -1 or 0 is passed, it indicates that the specified
        alignment value for singleCoreM is not set, and this value will be calculated by the Tiling function itself.
     * @param alignN : The alignment value for singleCoreN. If -1 or 0 is passed, it indicates that the specified
        alignment value for singleCoreN is not set, and this value will be calculated by the Tiling function itself.
     * @param alignK : The alignment value for singleCoreK. If -1 or 0 is passed, it indicates that the specified
        alignment value for singleCoreK is not set, and this value will be calculated by the Tiling function itself.
     */
    int32_t SetAlignSplit(int32_t alignM, int32_t alignN, int32_t alignK);
    /**
     * @brief Get the calculated singleCoreM/singleCoreN/singleCoreK.
     * @param shapeM : Get the singleCoreM value calculated form multi-core tiling.
     * @param shapeN : Get the singleCoreN value calculated form multi-core tiling.
     * @param shapeK : Get the singleCoreK value calculated form multi-core tiling.
     */
    int32_t GetSingleShape(int32_t &shapeM, int32_t &shapeN, int32_t &shapeK);
    /**
     * @brief Get the NumBlocks used after multi core tiling. It is carried by users to the kernel to control
        the service logic in the kernel.
     * @param dim : Get the number of cores required for computation, dim = mDim * nDim.
     * @param mDim : Determine the number of core required for the M direction during computation.
     * @param nDim : Determine the number of core required for the N direction during computation.
     */
    int32_t GetCoreNum(int32_t &dim, int32_t &mDim, int32_t &nDim);
    /**
     * @brief In a multi-core scenario, this interface enables the split of the K-axis. if this interface
        is not called, the K-axis will not be split by default. It should be used before calling the
        GetTiling interface.
     * @param flag : whether to enable the K-axis cutting.
     */
    void EnableMultiCoreSplitK(bool flag)
    {
        enableSplitK_ = flag;
    }
    /**
     * @brief Controls whether to enable K-axis splitting in multi-core scenarios
     *
     * This interface is used to set whether to split the K-axis in multi-core computing scenarios.
     * If this interface is not called, the system disables K-axis splitting by default.
     * Note: This interface has the same functionality as EnableMultiCoreSplitK, and it's recommended to use the latter.
     *
     * @param flag Boolean parameter:
     *             - true: Enable K-axis splitting
     *             - false: Disable K-axis splitting (default behavior)
     *
     * @note This interface must be used before calling the GetTiling interface to ensure that the K-axis splitting
     *       configuration is correctly applied to the tensor tiling process.
     */
    void SetSplitK(bool flag)
    {
        EnableMultiCoreSplitK(flag);
    }

    /**
     * @brief Whether can enable L1BankConflictOptimise
     * @return return true if can, else return false
     */
    bool EnableL1BankConflictOptimise();

protected:
    int64_t Compute() override;
};

/**
 * @class BatchMatmulTiling
 *
 * @brief Class for creating a BatchMatmul Tiling object
 *
 * Used to create and manage Tiling objects for batch matrix multiplication operations.
 */
class BatchMatmulTiling : public MatmulApiTilingBase {
public:
    BatchMatmulTiling() {};
    explicit BatchMatmulTiling(const platform_ascendc::PlatformAscendC &ascendcPlatform)
        : MatmulApiTilingBase(ascendcPlatform){};
    /**
     * @brief Retrieves the NumBlocks used after multi-core splitting
     *
     * This interface is used to obtain the number of cores distributed in different directions
     * after multi-core splitting, which constitutes the NumBlocks for computation.
     *
     * @param[out] dim Total number of cores required for computation, where dim = mDim * nDim
     * @param[out] mDim Number of cores required in the M direction for computation
     * @param[out] nDim Number of cores required in the N direction for computation
     * @param[out] batchCoreM Number of cores required in the batch M direction for computation
     * @param[out] batchCoreN Number of cores required in the batch N direction for computation
     *
     * @return 0 if the core numbers are successfully retrieved; -1 if the retrieval fails
     */
    int32_t GetCoreNum(int32_t &dim, int32_t &mDim, int32_t &nDim, int32_t &batchCoreM, int32_t &batchCoreN);
    /**
     * @brief Retrieves the tiling parameters
     *
     * This interface is used to obtain the tiling parameters and store them in the TCubeTiling structure.
     *
     * @param[out] tiling The TCubeTiling structure that stores the final tiling results.
     *                    - optiling::TCubeTiling: The TCubeTiling structure with the optiling namespace,
     *                      which is the Matmul TilingData defined on the Host side.
     *
     * @return int64_t Return value indicating the result of tiling calculation:
     *                 - Any value other than -1: Tiling calculation succeeded, and the tiling structure can be used.
     *                 - -1: Tiling calculation failed, and the tiling result cannot be used.
     */
    int64_t GetTiling(optiling::TCubeTiling &tiling) override;
    /**
     * @brief Retrieves the tiling parameters
     *
     * This interface is used to obtain the tiling parameters and store them in the TCubeTiling structure.
     *
     * @param[out] tiling The TCubeTiling structure that stores the final tiling results.
     *                    - TCubeTiling: The TCubeTiling structure without the optiling namespace,
     *                      which is the Matmul TilingData defined on the Kernel side.
     *
     * @return int64_t Return value indicating the result of tiling calculation:
     *                 - Any value other than -1: Tiling calculation succeeded, and the tiling structure can be used.
     *                 - -1: Tiling calculation failed, and the tiling result cannot be used.
     */
    int64_t GetTiling(AscendC::tiling::TCubeTiling &tiling) override;
protected:
    int64_t Compute() override;
private:
    int32_t SetBatch(int32_t batchMIn = 1, int32_t batchNIn = 1);
    // Set the batch axis tiling mode.
    int32_t SetSingleBatch(int32_t singleMIn = -1, int32_t singleNIn = -1);
};
} // namespace matmul_tiling

extern "C" {
/**
 * @brief Get L1/Unified/L0C buffer sizes for multi-core Matmul
 *
 * Retrieves buffer sizes using TCubeTiling from MultiCoreMatmulTiling after GetTiling.
 *
 * @param[in] tiling Input `optiling::TCubeTiling` structure (from MultiCoreMatmulTiling object)
 * @param[out] bufSize Output buffer sizes (ubSize, l1Size, l0cSize)
 *
 * @return 0 if the core numbers are successfully retrieved; -1 if the retrieval fails
 */
int32_t MultiCoreMatmulGetTmpBufSize(optiling::TCubeTiling &tiling, matmul_tiling::SysTilingTempBufSize &bufSize);
/**
 * @brief Get L1/Unified/L0C buffer sizes for BatchMatmul
 *
 * Retrieves buffer sizes using TCubeTiling from BatchMatmulTiling after GetTiling.
 *
 * @param[in] tiling Input `optiling::TCubeTiling` structure (from BatchMatmulTiling object)
 * @param[out] bufSize Output buffer sizes (ubSize, l1Size, l0cSize)
 *
 * @return 0 if the core numbers are successfully retrieved; -1 if the retrieval fails
 */
int32_t BatchMatmulGetTmpBufSize(optiling::TCubeTiling &tiling, matmul_tiling::SysTilingTempBufSize &bufSize);
/**
 * @brief Get L1/Unified/L0C buffer sizes for BatchMatmul
 *
 * Retrieves buffer sizes using TCubeTiling from BatchMatmulTiling after GetTiling.
 *
 * @param[in] tiling Input `TCubeTiling` structure (from BatchMatmulTiling object)
 * @param[out] bufSize Output buffer sizes (ubSize, l1Size, l0cSize)
 *
 * @return 0 if the core numbers are successfully retrieved; -1 if the retrieval fails
 */
int32_t MultiCoreMatmulGetTmpBufSizeV2(AscendC::tiling::TCubeTiling &tiling, matmul_tiling::SysTilingTempBufSize &bufSize);
/**
 * @brief Get L1/Unified/L0C buffer sizes for BatchMatmul
 *
 * Retrieves buffer sizes using TCubeTiling from BatchMatmulTiling after GetTiling.
 *
 * @param[in] tiling Input `TCubeTiling` structure (from BatchMatmulTiling object)
 * @param[out] bufSize Output buffer sizes (ubSize, l1Size, l0cSize)
 *
 * @return 0 if the core numbers are successfully retrieved; -1 if the retrieval fails
 */
int32_t BatchMatmulGetTmpBufSizeV2(AscendC::tiling::TCubeTiling &tiling, matmul_tiling::SysTilingTempBufSize &bufSize);
};

#endif // LIB_MATMUL_BMM_TILING_H