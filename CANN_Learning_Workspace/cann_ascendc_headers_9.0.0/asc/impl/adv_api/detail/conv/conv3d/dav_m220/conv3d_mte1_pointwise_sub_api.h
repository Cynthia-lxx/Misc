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
 * \file conv3d_mte1_pointwise_sub_api.h
 * \brief
 */

#ifndef API_CONV3D_MTE1_POINTWISE_SUB_API_H
#define API_CONV3D_MTE1_POINTWISE_SUB_API_H

#include "conv3d_mte2_pointwise_sub_api.h"

namespace Conv3dApiFunc {

// 实际使用L0A
template <class Intf>
class LoadAL0WithPointWiseTools {
public:
    __aicore__ inline LoadAL0WithPointWiseTools()
    {}

    __aicore__ inline void SetParams(Intf *self)
    {
        self_ = self;
        alignMAL1Tail = ConvApi::AlignB(self_->ctx.mAL1Tail, ConvApi::BLOCK_L0_M);
        alignKAL1Tail = ConvApi::AlignB(self_->ctx.conv3dTiling->kAL1Tail, ConvApi::DATA_COPY_OP_LEN);
        alignKL0Tail = ConvApi::AlignB(self_->ctx.kL0Tail, ConvApi::DATA_COPY_OP_LEN);
        fractalElements = ConvApi::FRACTAL_SIZE / self_->ctx.sizeOfInput;
    }

    __aicore__ inline void SetM(uint64_t m)
    {
        // m是对齐到了BLOCK_L0_N
        currentML0_ = m;
        numML0Block_ = currentML0_ / ConvApi::BLOCK_L0_N;
        currentMAL1 = IsMTail() ? alignMAL1Tail : self_->ctx.conv3dTiling->mAL1;
    }

    __aicore__ inline void LoadAL0()
    {
        currentKL0 = self_->ctx.kIter == self_->ctx.maxKL0Iter ? alignKL0Tail
                                                                : self_->ctx.conv3dTiling->kL0;
        if constexpr (AscendC::IsSameType<typename Intf::InputT, float>::value) {
            LoadAL0ByLoad2dTranspose();
        } else {
            // for bf16 and fp16
            LoadAL0ByLoad2d();
        }
    }

private:

    __aicore__ inline void LoadAL0ByLoad2dTranspose()
    {
        load2dSrcOffset = self_->ctx.kAL0Iter * self_->ctx.conv3dTiling->kL0 * currentMAL1 +
                            self_->ctx.mAL0Iter * self_->ctx.conv3dTiling->mL0 * ConvApi::DATA_COPY_OP_LEN;
        KERNEL_LOG(KERNEL_DEBUG, "[LoadAL0WithPointWise Load2dTranspose] load2dSrcOffset: %u currentKL0: %u\n",
            load2dSrcOffset, currentKL0);

        AscendC::LoadData2dTransposeParams loadData2dTransposeParams;
        uint64_t numKL0Block = currentKL0 / ConvApi::DATA_COPY_OP_LEN;
        SetLoadData2dTransposeParams(loadData2dTransposeParams, numML0Block_);
        uint64_t tmp1 = ConvApi::DATA_COPY_OP_LEN * currentML0_;
        uint64_t tmp2 = ConvApi::DATA_COPY_OP_LEN * currentMAL1;
        for (uint32_t copy_part = 0; copy_part < numKL0Block; ++copy_part) {
            AscendC::LoadDataWithTranspose<typename Intf::InputT>(self_->ctx.al0[copy_part * tmp1],
                    self_->ctx.al1[load2dSrcOffset + copy_part * tmp2],
                    loadData2dTransposeParams);
        }
    }

    __aicore__ inline void LoadAL0ByLoad2d()
    {
        // currentKAL1需要对齐到k0_, kAL1Tail在InitKDirectionBaseValue中已经对齐过�?
        currentKAL1 = self_->ctx.kAL1Iter == self_->ctx.maxKAL1Iter ? 
                    alignKAL1Tail : self_->ctx.conv3dTiling->kAL1;
        
        load2dSrcOffset = self_->ctx.mAL0Iter * self_->ctx.conv3dTiling->mL0 * currentKAL1 +
                            self_->ctx.kAL0Iter * self_->ctx.conv3dTiling->kL0 * ConvApi::BLOCK_L0_N;

        uint64_t numKL0Block = currentKL0 / self_->ctx.cin0;
        AscendC::LoadData2DParams loadData2dParams;
        loadData2dParams.repeatTimes = numKL0Block;
        loadData2dParams.srcStride = 1;
        loadData2dParams.ifTranspose = true;
        loadData2dParams.dstGap = numML0Block_ - 1;
        KERNEL_LOG(KERNEL_DEBUG, "[LoadAL0WithPointWise LoadAL0ByLoad2d] load2dSrcOffset: %u currentKAL1: %u currentKL0: %u\n",
            load2dSrcOffset, currentKAL1, currentKL0);
        
        uint64_t continueOffset = currentKAL1 * ConvApi::BLOCK_L0_N;
        for (uint32_t copy_part = 0; copy_part < numML0Block_; ++copy_part) {
            AscendC::LoadData<typename Intf::InputT>(self_->ctx.al0[copy_part * fractalElements],
                    self_->ctx.al1[load2dSrcOffset + copy_part * continueOffset],
                    loadData2dParams);
        }
    }
    
    __aicore__ inline bool IsMTail()
    {
        return self_->ctx.mAL1Iter == self_->ctx.maxMAL1Iter;
    }

    __aicore__ inline void SetLoadData2dTransposeParams(AscendC::LoadData2dTransposeParams &loadData2dTransposeParams, 
                                                        const uint64_t &repeatTimes)
    {
        // 从小z大N变成小n大Z
        loadData2dTransposeParams.repeatTimes = repeatTimes;
        loadData2dTransposeParams.srcStride = 1;
        if constexpr (AscendC::IsSameType<typename Intf::L0cT, float>::value &&
                      AscendC::IsSameType<typename Intf::OutputT, float>::value) {
            loadData2dTransposeParams.dstFracGap = numML0Block_ - 1;
            loadData2dTransposeParams.dstGap = 0;
        } else {
            loadData2dTransposeParams.dstGap = 0;
        }
        KERNEL_LOG(KERNEL_DEBUG, "[LoadAL0WithPointWise] LoadData2dTransposeParams repeatTimes %d dstFracGap %d dstGap %d.\n",
            loadData2dTransposeParams.repeatTimes, loadData2dTransposeParams.dstFracGap,
            loadData2dTransposeParams.dstGap);
    }

private:
    Intf *self_ = nullptr;
    uint64_t currentML0_ = 0;
    uint64_t currentKL0 = 0;
    uint64_t numML0Block_ = 0;
    uint64_t load2dSrcOffset = 0;
    uint64_t alignMAL1Tail = 0;
    uint64_t alignKAL1Tail = 0;
    uint64_t alignKL0Tail = 0;
    uint64_t currentMAL1 = 0;
    uint64_t currentKAL1 = 0;
    uint64_t fractalElements = 0;
};

// 实际使用L0B
template <class Intf>
class LoadBL0WithPointWiseTools {
public:
    __aicore__ inline LoadBL0WithPointWiseTools()
    {}

    __aicore__ inline void SetParams(Intf *self)
    {
        self_ = self;
        alignNBL1Tail = ConvApi::AlignB(self_->ctx.nBL1Tail, ConvApi::BLOCK_L0_N);
        alignKL0Tail = ConvApi::AlignB(self_->ctx.kL0Tail, ConvApi::DATA_COPY_OP_LEN);
        k0_ = AscendC::ONE_BLK_SIZE / self_->ctx.sizeOfWeight;
        fractalElements = ConvApi::FRACTAL_SIZE / self_->ctx.sizeOfWeight;
    }

    __aicore__ inline void SetN(uint64_t n)
    {
        // should align to 16
        currentNL0_ = n;
    }

    __aicore__ inline void LoadBL0()
    {
        currentNBL1 = self_->ctx.nBL1Iter == self_->ctx.maxNBL1Iter ? 
                alignNBL1Tail : self_->ctx.conv3dTiling->nBL1;
        currentKL0 = self_->ctx.kIter == self_->ctx.maxKL0Iter ? alignKL0Tail
                                                                : self_->ctx.conv3dTiling->kL0;

        uint64_t load2dSrcOffset = self_->ctx.kBL0Iter * self_->ctx.conv3dTiling->kL0 * currentNBL1 + 
                          self_->ctx.nBL0Iter * self_->ctx.conv3dTiling->nL0 * k0_;
        KERNEL_LOG(KERNEL_DEBUG, "[LoadBL0WithPointWise] load2dSrcOffset: %u currentNBL1: %u currentKL0: %u\n",
                        load2dSrcOffset, currentNBL1, currentKL0);
        numKL0Block = currentKL0 / k0_;
        uint64_t numNL0Block = currentNL0_ / ConvApi::BLOCK_L0_M;

        AscendC::LoadData2DParams loadData2dParams;
        SetLoadData2DParams(loadData2dParams, numNL0Block);
        uint64_t continueOffset = currentNBL1 * k0_;
        for (uint32_t copy_part = 0; copy_part < numKL0Block; ++copy_part) {
            AscendC::LoadData<typename Intf::WeightT>(self_->ctx.bl0[copy_part * fractalElements],
                    self_->ctx.bl1[load2dSrcOffset + copy_part * continueOffset],
                    loadData2dParams);
        }
    }

private:
    __aicore__ inline void SetLoadData2DParams(AscendC::LoadData2DParams &loadData2dParams, const uint64_t &repeatTimes)
    {
        // 从小z大N变成小z大Z
        loadData2dParams.repeatTimes = repeatTimes;
        loadData2dParams.srcStride = 1;
        loadData2dParams.dstGap = numKL0Block - 1;
        KERNEL_LOG(KERNEL_DEBUG, "[LoadBL0WithPointWise] loadData2dParams.repeatTimes %d.\n",
            loadData2dParams.repeatTimes);
    }

private:
    Intf *self_ = nullptr;
    uint64_t k0_ = 0;
    uint64_t currentNL0_ = 0;
    uint64_t currentKL0 = 0;
    uint64_t numKL0Block = 0;
    uint64_t currentNBL1 = 0;
    uint64_t alignNBL1Tail = 0;
    uint64_t alignKL0Tail = 0;
    uint64_t fractalElements = 0;
};

};  // namespace Conv3dApiFunc

#endif  // __API_CONV3D_POINTWISE_SUB_API_H__
