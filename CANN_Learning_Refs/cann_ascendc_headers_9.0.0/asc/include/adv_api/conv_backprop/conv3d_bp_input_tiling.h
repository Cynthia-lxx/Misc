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
 * \file conv3d_bp_input_tiling.h
 * \brief
 */
#ifndef CONV3D_BP_INPUT_TILING_H
#define CONV3D_BP_INPUT_TILING_H

#include "conv3d_bp_input_tiling_base.h"
#include "conv3d_bp_input_tilingdata.h"

namespace ConvBackpropApi {
class Conv3DBpInputTiling : public Conv3DBpInputTilingBase {
public:
    Conv3DBpInputTiling() {};
    explicit Conv3DBpInputTiling(const platform_ascendc::PlatformAscendC &ascendcPlatform)
        : Conv3DBpInputTilingBase(ascendcPlatform) {};
    explicit Conv3DBpInputTiling(const PlatformInfo& platform) : Conv3DBpInputTilingBase(platform) {};
    ~Conv3DBpInputTiling() override = default;
    int64_t GetTiling(optiling::Conv3DBackpropInputTilingData &tiling) override;
    int64_t GetTiling(AscendC::tiling::Conv3DBackpropInputTilingData &tiling) override;
protected:
    int64_t Compute() override;
    bool CheckInputParam() override;
    void SetFinalTiling(optiling::Conv3DBackpropInputTilingData& tiling) override;
    void SetFinalTiling(AscendC::tiling::Conv3DBackpropInputTilingData& tiling) override;   
    void PrintTilingData() const override;
    void SetBasicBlockAttrsTiling();
    int32_t CalFmapH(const uint32_t& mL1Size) const;
    void AlignCout1(uint32_t &cout1A, uint32_t &cout1B, bool adaptFP32) const;
    bool CalModifyBackpropPadHW();
    bool CalModifyBackpropPadD();
    void SetBackpropPadInfo();
    void SetInitOutput();

    void CalStepMNK();
    void LadderMatchStepKWithFullLoad(uint32_t& stepKa, const uint32_t& stepKb);
    void LadderMatchStepMNK(uint32_t& stepKa, uint32_t& stepKb);
    void EqualL1MatchStepMNK(uint32_t& stepKa, uint32_t& stepKb);
    bool MultiCoreSplitMN();
    bool IsStepL1Valid(const uint32_t& stepKa, const uint32_t& stepKb);
    void InitBaseMNK();
    void AdjustBaseMNK(const uint32_t l0abPingPong, const uint32_t l0cPingPong, uint32_t& baseM, uint32_t& baseN, uint32_t& baseK);
    void SetSingleCoreInfo();
    void LegalProtection();
    bool IsL2Efficient(const uint64_t singleCoreM, const uint64_t singleCoreN, const uint64_t singleCoreK, const uint64_t transdataWorkSpace);
    void ShrinkBasicBlock();

    MatMulDxInfo mmInfo_;
    uint64_t lenHkWkC0_ = 1;
    BasicBlockDxTilingParams tilingParams;

    uint8_t loadB2Condition_ = 0;
    bool enableKernelSplit_ = false;
    int32_t initOutputFlag = 0;

    int32_t dtypeByte_ = 2;
    int32_t blockSize_ = 16;
private:
    bool CheckCalPads();
    bool CheckAttrs();
    bool CheckPadRange();
    bool CheckOutputHeight();
    bool CheckTransposeOutputtingRange();
    bool InferShape();
};
}  // namespace ConvBackpropApi
#endif  // CONV3D_BP_INPUT_TILING_H