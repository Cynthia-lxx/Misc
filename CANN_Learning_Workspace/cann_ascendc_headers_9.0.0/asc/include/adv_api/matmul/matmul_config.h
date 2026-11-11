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
 * \file matmul_config.h
 * \brief
 */
#ifndef LIB_MATMUL_MATMUL_CONFIG_H
#define LIB_MATMUL_MATMUL_CONFIG_H

#include "kernel_tiling/kernel_tiling.h"

constexpr uint32_t SHARED_CO1_BUFFER_SIZE = 64 * 1024;

/**
* @enum class CubeFormat
* @brief physical data layout formats
*/
enum class CubeFormat {
    ND = 0,           // Standard format, row-major
    NZ,               // Colum-major between blocks, row-major within blocks
    ZN,               // row-major between blocks, Colum-major within blocks
    ZZ,               // row-major between blocks, row-major within blocks
    NN,               // Colum-major between blocks, Colum-major within blocks
    ND_ALIGN,         // Aligned ND format, the output is aligned to 32-byte boundaries along the N dimension
    SCALAR,           // A single numerical value with no dimensions
    VECTOR,           // 1D array format for vector data
    ROW_MAJOR = ND,   // Row-major format (equivalent to ND format)
    COLUMN_MAJOR = 8, // Column-major format (DN format)
};

/**
 * @enum class LayoutMode
 * @brief Matrix layout modes
*/
enum class LayoutMode {
    NONE = 0,  // Default value, indicating that BatchMatmul is not used
    BSNGD,     // Reshaped format from original BSH shape
    SBNGD,     // Reshaped format from original SBH shape
    BNGS1S2,   // Typically output of matrix multiplication for the first two layouts
    NORMAL     // General data format with BMNK layout
};

/**
 * @enum class BatchMode
 * @brief Memory scheduling modes for batch processing
 *
 * When the Layout type is NORMAL in a BatchMatmul scenario,set the relationship between the total size of the
 * multi-batch data of BatchMatmul input matrices A/B and the L1 Buffer size
 */
enum class BatchMode {
    NONE = 0,            // No specific batch processing mode
    BATCH_LESS_THAN_L1,  // Total size of multi-batch data < L1 buffer size
    BATCH_LARGE_THAN_L1, // Total size of multi-batch data > L1 buffer size
    SINGLE_LARGE_THAN_L1 // Size of single batch data > L1 buffer size
};

/**
 * @enum BatchOutMode
 * @brief Output modes for batch matrix multiplication
 */
enum BatchOutMode {
    SINGLE_BATCH = 0, // Single batch output
    MULTI_BATCH,      // Multi-batch output
    DYNAMIC,          // Dynamic batch output
};

/**
 * @enum class IterateOrder
 * @brief The loop iteration order for Matmul matrix operations
 */
#ifndef ASCC_STRUCT_ITERATEORDER
#define ASCC_STRUCT_ITERATEORDER
enum class IterateOrder {
    ORDER_M = 0, // Offset along the M-axis first, then along the N-axis
    ORDER_N,     // Offset along the N-axis first, then along the M-axis
    UNDEF,       // Currently invalid
};
#endif
 
 /**
 * @enum class ScheduleType
 * @brief Configure Matmul data transfer mode
 */
enum class ScheduleType {
    INNER_PRODUCT = 0, // k loop, default type
    OUTER_PRODUCT,     // m/n loop, depends on IterateOrder
};

/**
 * @enum IterateMode
 * @brief Optimize the overhead of Matmul computation
 */
enum IterateMode : uint8_t {
    ITERATE_MODE_NORMAL  = 0b00000001, // Only the Iterate interface is called in Matmul computation
    ITERATE_MODE_ALL     = 0b00000010, // Only the IterateAll interface is called in Matmul computation
    ITERATE_MODE_BATCH   = 0b00000100, // Only the IterateBatch interface is called in Matmul computation
    ITERATE_MODE_N_BATCH = 0b00001000, // Only the IterateNBatch interface is called in Matmul computation
    ITERATE_MODE_DEFAULT = 0b11111111, // Default mode with all modes enabled
};


/**
 * @enum DecompressionMode
 * @brief Lut model
 */
enum class DecompressionMode {
    DECOMP_NONE = 0b0000,
    DECOMP_2bitTo8bit = 0b0001,
    DECOMP_3bitTo8bit = 0b0010,
    DECOMP_4bitTo8bit = 0b0011,
    DECOMP_16bitTo64bit = 0b100,
    DECOMP_1bitTo8bit = 0b0101,
    DECOMP_1bitTo4bit = 0b0110,
    DECOMP_2bitTo4bit = 0b0111,
    DECOMP_3bitTo4bit = 0b1000,
};

/**
 * @struct MatmulConfig
 * @brief Core configuration for the Matmul operator
 *
 * Integrates all control parameters for matrix multiplication computation
 */
struct MatmulConfig {
    bool doNorm;          // Enabled by default (true) to activate the Norm template, false to disable the template
    bool doBasicBlock;    // true: enables the BasicBlock template; false: disables the BasicBlock template
    bool doMultiDataLoad; // true: enables the doMultiDataLoad template; false: disables the doMultiDataLoad template
    // basic MNK could only be valid in basic block mode
    uint32_t basicM;      // M dimension size of the basic block
    uint32_t basicN;      // N dimension size of the basic block
    uint32_t basicK;      // K dimension size of the basic block
    bool intrinsicsCheck; // true: enable loop-based data loading, false: disable loop-based data loading
    bool isNBatch;     // true: Enables multi-batch, false: Disables multi-batch (default value)
    bool enVecND2NZ;   // true: Enable ND2NZ with vector instructions, false: Disable ND2NZ with vector instructions
     // only be valid in special basic block mode
    bool doSpecialBasicBlock; // true: Enables the SpecialBasicBlock, false: Disables the SpecialBasicBlock
    uint32_t doMTE2Preload;   // 0: Disabled (default), 1: Enable M-dimension preload, 2: Enable N-dimension preload
    uint32_t singleCoreM;     // Size of the M-axis shape within a single core, in units of elements
    uint32_t singleCoreN;     // Size of the N-axis shape within a single core, in units of elements
    uint32_t singleCoreK;     // Size of the K-axis shape within a single core, in units of elements
    uint32_t stepM;           // The multiple of baseM in the bufferM direction for the left matrix cached in A1
    uint32_t stepN;           // The multiple of baseN in the bufferN direction for the right matrix cached in B1
    uint32_t baseMN;          // Size of baseM * baseN
    uint32_t singleCoreMN;    // Size of singleCoreM * singleCoreN
    bool enUnitFlag = true;   // true: Enables unitflag, false: Disables unitflag
    // AntiQuant Param
    bool isPerTensor;         // B matrix quantization is per-tensor when enabled (for A(half) and B(int8_t) input)
    bool hasAntiQuantOffset;  // Whether to use offset coefficient for B matrix quantization (A: half, B: int8_t)
    bool doIBShareNorm;       // true: Enables IBShare, false: Disables IBShare
    // MDL support stepN == 2
    bool doSpecialMDL;        // true: Enables MDL, false: Disables MDL
    bool enableInit = true;   // true: Enables the Init function
    BatchMode batchMode;      // BatchMatmul (NORMAL layout): Set multi-batch A/B total size vs L1 Buffer size

    // Add for process performance
    bool enableEnd = true;        // true: The End function needs to be called during Matmul computation
    bool enableGetTensorC = true; // true: The GetTensorC function needs to be called during Matmul computation
    bool enableSetOrgShape = true; // true: The SetOrgShape function needs to be called during Matmul computation
    bool enableSetBias = true;     // true: Enable bias computation (default value)
    bool enableSetTail = true;     // true: Call SetTail during Matmul computation (default)
    bool enableQuantVector = true; // true: Call SetQuantVector and SetQuantScalar during Matmul (default)
    bool enableSetDefineData = true; // Whether to set defined data
    uint8_t iterateMode = IterateMode::ITERATE_MODE_DEFAULT; // No limit on calls to Iterate interfaces (default)
    bool enableReuse = true;  // Whether to enable data reuse
    // enable UB reuse(ND2NZ & ND2NZ) for V200
    bool enableUBReuse;   // Whether to enable Unified Buffer reuse
    bool enableL1CacheUB; // Whether to enable L1 Buffer caching for Unified Buffer compute blocks
    // for intra-block l0c add
    bool intraBlockPartSum = false; // false: Disable accumulation of two AIV cores' results in L0C Buffer (default)
    // MDL support M/N db
    IterateOrder iterateOrder { IterateOrder::UNDEF }; // UNDEF: Currently invalid.
    ScheduleType scheduleType;  // Configure the data transfer mode for Matmul
    bool enableDoubleCache;   // When IBShare is enabled, whether to cache two data blocks in L1 Buffer simultaneously
    bool isBiasBatch = true;           // true: Bias includes the Batch dimension (default)
    bool enableStaticPadZeros = false; // false: No auto zero-fill during transfer; user must pad manually (default)
    bool isPartialOutput = false;      // false: Disables the PartialOutput feature
    bool enableMixDualMaster = false;  // Whether to enable MixDualMaster (default: false)
    bool isA2B2Shared = false;         // Whether to enable global management for A2 and B2; false: disabled (default)
    bool isEnableChannelSplit = false; // Whether to enable the channel_split functionality, false: disabled (default)
    bool enableKdimReorderLoad = false; // false: Disable staggered K-axis data loading (default)
    bool isCO1Shared = false; // Whether to enable CO1 memory sharing, false: disabled (default)
    uint32_t sharedCO1BufferSize = SHARED_CO1_BUFFER_SIZE; // Specify the size of the shared CO1 buffer
    BatchOutMode bmmOutMode = BatchOutMode::SINGLE_BATCH;  // BMM output mode
    bool enableL1BankConflictOptimise = false; // Whether to enable L1BankConflictOptimise
    bool enableRelu = false; // Whether to enable Relu
    DecompressionMode decompMode = DecompressionMode::DECOMP_NONE;
};

/**
 * @enum class MatmulConfigMode
 * @brief MatmulConfigMode specifies the MatmulConfig template to be retrieved and modified.
 */
enum class MatmulConfigMode {
    CONFIG_NORM,       // Set MatmulConfig default value to Norm template
    CONFIG_MDL,        // Set MatmulConfig default value to MDL template
    CONFIG_SPECIALMDL, // Set MatmulConfig default value to SpecialMDL template
    CONFIG_IBSHARE     // Set MatmulConfig default value to IBShare template
};

/**
 * @struct MatmulShapeParams
 * @brief tiling shape information
 *
 * Constantized for tiling shape information
 */
struct MatmulShapeParams {
    uint32_t singleCoreM;  // size of M axis shape within a single core, in unit of element
    uint32_t singleCoreN;  // size of N axis shape within a single core, in unit of element
    uint32_t singleCoreK;  // size of K axis shape within a single core, in unit of element
    uint32_t basicM;  // size of M axis shape for Matmul caculation, in unit of element
    uint32_t basicN;  // size of N axis shape for Matmul caculation, in unit of element
    uint32_t basicK;  // size of K axis shape for Matmul caculation, in unit of element
};

/**
 * @struct MatmulQuantParams
 * @brief quant config
 *
 * Scenario of quant: A is float16_t and B is int8_t
 */
struct MatmulQuantParams {
    bool isPerTensor;  // whether B quant is per tensor
    bool hasAntiQuantOffset;  // whether B uses of offset coefficients
};


/**
 * @struct MatmulBatchParams
 * @brief batch matmul config
 *
 * Enable when batchMode is not BatchMode::None
 */
struct MatmulBatchParams {
    bool isNBatch;  // whether invoke IterNBatch to achieve multiple batch inputs and outputs
    BatchMode batchMode;  // relationship between the total size of A/B and the size of L1 Buffer
    bool isBiasBatch = true;  // whether the size of the bias include the batch axis
    BatchOutMode bmmOutMode = BatchOutMode::SINGLE_BATCH;  // whether to cache multiple batch outputs to copy out together
};

/**
 * @struct MatmulFuncParams
 * @brief matmul function config
 *
 * Matmul common feature config
 */
struct MatmulFuncParams {
    bool intrinsicsCheck;  // enable cyclic DataCopy from GM to L1 when the element num of A/B's inner axis >= 65535
    bool enVecND2NZ;  // enable use the Vector instruction to transform ND to NZ
    bool enableDoubleCache;  // enable double data cached in L1 for IBShare
    bool enableL1CacheUB;  // enable use L1 to cache for UB buffer
    uint32_t doMTE2Preload;  // enable the preload function for M/N direction to reduce the MTE2 gap
    IterateOrder iterateOrder;  // the loop iterate order of M or N direction
    ScheduleType scheduleType;  // the type of Matmul data copy
    bool enableReuse = true;  // enable directly pass the calculate data for the dataPtr in the callback funtion
                              //   set by the SetSelfDefineData function
    bool enableUBReuse;  // enable reuse of UB buffer to cache double data for two iterate
    bool isPartialOutput = false;  // enable K axis does not atomic add
    bool isA2B2Shared = false;  // enable all Mamtul instance share the double buffer feature of A2 and B2
    bool isEnableChannelSplit = false;  // enable C matrix split form [x]*[m*n] to [2x]*[m*(n/2)]
                                        //     when C's DataType is float, Position is GM and CubeFormat is NZ
    bool enableKdimReorderLoad = false;  // enable K axis load data by peak-shifting
    bool enableL1BankConflictOptimise = false; // Whether to enable L1BankConflictOptimise
    bool enableRelu = false; // Whether to enable Relu
    DecompressionMode decompMode = DecompressionMode::DECOMP_NONE;
};

/**
 * @struct MatmulBiasParams
 * @brief matmul bias config
 */
struct MatmulBiasParams {
    bool enableSetBias = true;  // enable SetBias function
};

/**
 * @struct MatrixOffset
 * @brief postion of current iterate in the C matrix
 *
 * reserved function.
 */
struct MatrixOffset {
    int32_t offset;  // reserved
    int32_t row;  // reserved
    int32_t col;  // reserved
    int32_t height;  // reserved
    int32_t width;  // reserved
};

extern int blockidx_;

#endif // LIB_MATMUL_MATMUL_CONFIG_H