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
 * \file conv3d_tiling_algorithm.h
 * \brief
 */

#ifndef TILING_CONV_CONV3D_TILING_ALGORITHM_H
#define TILING_CONV_CONV3D_TILING_ALGORITHM_H

#include <cstdint>
#include "include/adv_api/conv/conv3d/conv3d_tiling_base.h"

namespace Conv3dTilingApi {

enum class L1TilingMode : uint32_t {
    FULL_LOAD_BL1 = 0,
    FULL_LOAD_AL1,
    ALL_FULL_LOAD,
    NONE_FULL_LOAD,
    INVALID
};

struct L1TilingFlag {
    L1TilingMode abL1Mode = L1TilingMode::INVALID;
    IterateMNOrder iterateMNOrder = IterateMNOrder::INVALID;
    bool kAL1CanFullLoadFlag = false;
    bool kBL1CanFullLoadFlag = false;
    bool kABL1CanFullLoadFlag = false;
    bool isBiasFullLoad = false;
    bool isWeightBypass = false;
};

struct DoubleBufferTilingValue {
    uint8_t pbAL1 = 2;
    uint8_t pbBL1 = 1;
    uint8_t pbAL0 = 2;
    uint8_t pbBL0 = 2;
    uint8_t pbCL0 = 1;
    uint64_t pBufferFlag = 0;
};

struct L1TilingIdx {
    uint64_t kAL1Idx = 0;
    uint64_t kBL1Idx = 0;
    uint64_t mAL1Idx = 0;
    uint64_t nBL1Idx = 0;

    void SetIdx(uint64_t kAl1, uint64_t kBl1, uint64_t mAL1, uint64_t nBL1)
    {
        kAL1Idx = kAl1;
        kBL1Idx = kBl1;
        mAL1Idx = mAL1;
        nBL1Idx = nBL1;
    }
};

struct L0TilingRange {
    std::vector<uint64_t> mL0Range;
    std::vector<uint64_t> kL0Range;
    std::vector<uint64_t> nL0Range;
};

struct L0TilingParams {
    uint64_t mL0 = 0;
    uint64_t kL0 = 0;
    uint64_t nL0 = 0;
    uint64_t orgCoAlignN0 = 0;
};

struct L0TilingIdx {
    uint64_t mL0Idx = 0;
    uint64_t nL0Idx = 0;
    uint64_t kL0Idx = 0;
};

struct L1TilingRange {
    std::vector<uint64_t> mAL1ValueRange;
    std::vector<uint64_t> nBL1ValueRange;
    std::vector<uint64_t> kAL1Range;
    std::vector<uint64_t> kBL1Range;
};

struct L1TilingParams {
    uint64_t kAL1 = 0;
    uint64_t kAL1Tail = 0;
    uint64_t kBL1 = 0;
    uint64_t kBL1Tail = 0;
    uint64_t mAL1Value = 0;
    uint64_t nBL1Value = 0;
};

struct L1TilingCalc {
    uint64_t ci0HkWk = 0;
    uint64_t alignCinKhKwKd = 0;
    uint64_t inputFullLoadL1Size = 0;
    uint64_t weightFullLoadL1Size = 0;
    uint64_t inputKL1FullLoadSize = 0;
    uint64_t weightKL1FullLoadSize = 0;
    uint64_t inputMinLoadL1Size = 0;
    uint64_t weightMinLoadL1Size = 0;
    uint64_t biasMinLoadL1Size = 0;
};

class Conv3dTilingAlgorithm {
public:
    explicit Conv3dTilingAlgorithm(Conv3dTilingBase *tilingIns);
    virtual ~Conv3dTilingAlgorithm()
    {
        tilingIns_ = nullptr;
    }
    int64_t Process();

protected:
    bool FixL0PingpongDecision();
    bool CheckL0Buffer(uint64_t currmL0, uint64_t currkL0, uint64_t currnL0);

private:
    // weight bypass
    virtual bool CoreL1TilingMinWeightBypass() const;
    virtual bool NoneKABL1FullLoadWeightBypass() const;

    virtual uint64_t L1NoFullLoadInputSize() const;
    // L1 Tiling
    int64_t GetL1Tiling();
    int64_t PreProcessingL1Tiling();
    virtual bool CheckL1Buffer() const;
    virtual int64_t InitCalcL1Params();
    uint64_t InferHiL1(uint64_t inputHoL1, uint64_t hi) const;
    virtual void GetL1TilingRange();
    void InitL1Tiling();
    void InitABL1TilingMode();
    void CoreL1TilingDecision();
    void InputL1FullLoadIter();
    void WeightL1FullLoadIter();
    void InitKL1LoadFlag();
    void L1NoFullLoadIter();
    void KAL1FullLoadIter();
    void KBL1FullLoadIter();
    uint64_t KABL1FullLoadIterN();
    uint64_t KABL1FullLoadIterM();
    void NoneKABL1FullLoadIter();
    void BiasL1TilingDecision();
    virtual void GetKL0TilingDecision();
    virtual void WeightBypassDecision();
    void UpdateL1DoubleBuffer();
    virtual void SetKAL1KBL1TailRes();
    void SetL1TilingRes();

     // L0 tiling
    int64_t GetL0Tiling();
    void InitPingPong();
    void GetL0TilingRange();
    void L0TilingDecision();
    uint64_t CalcAL0Size(uint64_t currmL0, uint64_t currkL0) const;
    uint64_t CalcBL0Size(uint64_t currkL0, uint64_t currnL0) const;
    uint64_t CalcCL0Size(uint64_t currmL0, uint64_t currnL0) const;
    virtual uint64_t CalcL1SizeForL0Tiling(uint64_t currmL0, uint64_t currnL0) const;
    virtual uint64_t CalcBTSize(uint64_t currnL0) const;
    void CheckL0CDoubleBuffer();

    // set db
    void SetPBufferFlag();

protected:
    Conv3dTilingBase* tilingIns_ = nullptr;

    L0TilingRange l0TilingRange;
    L0TilingParams l0TilingParams;
    L0TilingIdx l0TilingIdx;

    L1TilingRange l1TilingRange;
    L1TilingParams l1TilingParams;
    L1TilingCalc l1TilingCalc;
    L1TilingIdx l1TilingIdx;
    L1TilingFlag l1TilingFlag;
    DoubleBufferTilingValue doubleBufferValue;
    std::map<L1TilingMode, L1TilingIdx> l1TilingInitMap;

    uint64_t fMapDTypeSize = 0;
    uint64_t weightDTypeSize = 0;
    uint64_t biasDTypeSize = 0;
};
} // namespace Conv3dTilingApi

#endif
