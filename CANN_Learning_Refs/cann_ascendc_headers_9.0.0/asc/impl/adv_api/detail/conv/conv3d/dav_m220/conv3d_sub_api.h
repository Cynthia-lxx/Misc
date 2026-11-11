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
 * \file conv3d_sub_api.h
 * \brief
 */

#ifndef API_CONV3D_SUB_API_H
#define API_CONV3D_SUB_API_H

#include "conv3d_mte1_sub_api.h"

namespace Conv3dApiFunc {

template <class Intf, typename DataTypeT>
class LoadChannelWiseL1Tools {
public:
    __aicore__ inline LoadChannelWiseL1Tools()
    {}

    __aicore__ inline void SetParams(Intf *self)
    {
        self_ = self;
    }

    __aicore__ inline void SetN(uint64_t n)
    {
        currentNL0_ = n;
    }

    __aicore__ inline void LoadChannelWiseL1(
        const AscendC::LocalTensor<DataTypeT> &tensorL1, const AscendC::GlobalTensor<DataTypeT> &tensorGm)
    {
        PreProcess();
        uint64_t srcDValue =
            self_->ctx.biasFullLoadFlag ? ConvApi::AlignB(self_->ctx.singleCoreCo, ConvApi::BLOCK_L0_N) : ConvApi::AlignB(currentNL0_, ConvApi::BLOCK_L0_N);
        if (currentNL0_ != srcDValue) {
            // Non 16 aligned scene with 0, tail block
            AscendC::InitConstValueParams<DataTypeT> initConstValueParams;
            initConstValueParams.repeatTimes = 1;
            initConstValueParams.blockNum = srcDValue * sizeof(DataTypeT) / ConvApi::AL1_BLOCK_SIZE;
            initConstValueParams.dstGap = 0;
            initConstValueParams.initValue = 0;
            AscendC::InitConstValue<DataTypeT>(tensorL1, initConstValueParams);
            KERNEL_LOG(KERNEL_DEBUG, "[LoadChannelWiseL1] initConstValueParams.blockNum %d.\n",
                initConstValueParams.blockNum);
        }
        if (currentNL0_ <= ConvApi::MAX_UINT16) {
            SetNd2NzParams(currentNL0_, srcDValue);
            AscendC::DataCopy<DataTypeT>(tensorL1, tensorGm[tensorGmOffset], nd2NzParams);
        } else {
            biasBlockNum = AscendC::ONE_BLK_SIZE / self_->ctx.sizeOfBias;
            limitLen = (ConvApi::MAX_UINT16 / biasBlockNum) * biasBlockNum;
            uint16_t num = currentNL0_ / limitLen;

            KERNEL_LOG(KERNEL_DEBUG, "[LoadChannelWiseL1] num %d limitLen %d tensorGmOffset %d.\n",
                num, limitLen, tensorGmOffset);
            SetNd2NzParams(limitLen, limitLen);
            nd2NzParams.ndNum = num;
            nd2NzParams.srcNdMatrixStride = limitLen;
            nd2NzParams.dstNzMatrixStride = limitLen;
            AscendC::DataCopy<DataTypeT>(tensorL1, tensorGm[tensorGmOffset], nd2NzParams);

            uint16_t tail = currentNL0_ % limitLen;
            if (tail) {
                uint64_t offset = num * limitLen;
                tensorGmOffset += offset;
                KERNEL_LOG(KERNEL_DEBUG, "[LoadChannelWiseL1] offset %d tensorGmOffset %d tail %d.\n",
                            offset, tensorGmOffset, tail);
                SetNd2NzParams(tail, tail);
                AscendC::DataCopy<DataTypeT>(tensorL1[offset], tensorGm[tensorGmOffset], nd2NzParams);
            }
        }
    }

private:
    __aicore__ inline void PreProcess()
    {
        tensorGmOffset =
            self_->ctx.nBL1Iter * self_->ctx.conv3dTiling->nBL1 + self_->ctx.nBL0Iter * self_->ctx.conv3dTiling->nL0;
        currentNL0_ = self_->ctx.biasFullLoadFlag ? self_->ctx.singleCoreCo : currentNL0_;
        KERNEL_LOG(KERNEL_DEBUG, "tensorGmOffset %d currentNL0_ %d \n", tensorGmOffset, currentNL0_);
    }

    __aicore__ inline void SetNd2NzParams(uint64_t dValue, uint64_t srcDValue)
    {
        nd2NzParams.ndNum = 1;
        nd2NzParams.nValue = 1;
        nd2NzParams.dValue = dValue;
        nd2NzParams.srcNdMatrixStride = 0;
        nd2NzParams.srcDValue = srcDValue;
        nd2NzParams.dstNzC0Stride = 1;
        nd2NzParams.dstNzNStride = 1;
        nd2NzParams.dstNzMatrixStride = 1;
        KERNEL_LOG(KERNEL_DEBUG, "[LoadChannelWiseL1] nd2NzParams.dValue %d nd2NzParams.srcDValue %d.\n",
            nd2NzParams.dValue,
            nd2NzParams.srcDValue);
    }

private:
    Intf *self_ = nullptr;
    uint64_t tensorGmOffset;
    uint64_t currentNL0_;
    uint64_t biasBlockNum = 0;
    uint64_t limitLen = 0;
    AscendC::Nd2NzParams nd2NzParams;
};

template <class Intf>
class LoadBiasBtTools {
public:
    __aicore__ inline LoadBiasBtTools()
    {}

    __aicore__ inline void SetParams(Intf *self)
    {
        self_ = self;
        tilingNBL1_ = self_->ctx.conv3dTiling->nBL1;
    }

    __aicore__ inline void SetN(uint64_t n)
    {
        currentNL0_ = n;
    }

    __aicore__ inline void LoadBiasBt()
    {
        uint32_t offset = 0;
        if (self_->ctx.conv3dTiling->biasFullLoadFlag) {
            offset = self_->ctx.nBL1Iter * tilingNBL1_ + self_->ctx.nBL0Iter * self_->ctx.conv3dTiling->nL0;
        }
        AscendC::DataCopyParams dataCopyParams;
        dataCopyParams.blockCount = 1;
        dataCopyParams.blockLen = ConvApi::CeilDIV(currentNL0_ * sizeof(typename Intf::BiasT), ConvApi::BT_BLOCK_SIZE);
        AscendC::DataCopy<typename Intf::L0cT, typename Intf::BiasT>(
            self_->ctx.biasBT, self_->ctx.biasL1[offset], dataCopyParams);
    }

private:
    Intf *self_ = nullptr;
    uint64_t tilingNBL1_;
    uint64_t currentNL0_ = 0;
};

template <class Intf>
class MMadTools {
public:
    __aicore__ inline MMadTools()
    {}

    __aicore__ inline void SetParams(Intf *self)
    {
        self_ = self;
    }

    __aicore__ inline void SetMN(uint64_t m, uint64_t n)
    {
        currentML0_ = m;
        currentNL0_ = n;
    }

    template <bool isFirst = false, bool isLast = false>
    __aicore__ inline void Mad()
    {
        AscendC::MmadParams mmadParams;
        mmadParams.m = currentML0_;
        mmadParams.n = currentNL0_;
        if constexpr (!isLast) {
            mmadParams.k = self_->ctx.singleCoreKL0;
        } else {
            mmadParams.k = self_->ctx.kL0Tail;
        }

        if (!self_->ctx.enableBias) {
            if constexpr (isFirst) {
                mmadParams.cmatrixInitVal = true;
                mmadParams.cmatrixSource = false;
                mmadParams.isBias = false;
            } else {
                mmadParams.cmatrixInitVal = false;
                mmadParams.cmatrixSource = false;
                mmadParams.isBias = false;
            }
        } else {
            if constexpr (isFirst) {
                mmadParams.cmatrixInitVal = false;
                mmadParams.cmatrixSource = true;
                mmadParams.isBias = true;
            } else {
                mmadParams.cmatrixInitVal = false;
                mmadParams.cmatrixSource = false;
                mmadParams.isBias = false;
            }
        }
        KERNEL_LOG(KERNEL_DEBUG, 
            "[Mad] mmadParams.cmatrixInitVal %d, mmadParams.cmatrixSource %d, mmadParams.isBias %d, mmadParams.k %d, "
            "mmadParams.n %d, mmadParams.m %d.\n",
            mmadParams.cmatrixInitVal,
            mmadParams.cmatrixSource,
            mmadParams.isBias,
            mmadParams.k,
            mmadParams.n,
            mmadParams.m);
        AscendC::Mmad<typename Intf::L0cT, typename Intf::InputT, typename Intf::WeightT>(
            self_->ctx.cl0, self_->ctx.al0, self_->ctx.bl0, mmadParams);
    }

private:
    Intf *self_ = nullptr;
    uint64_t currentML0_ = 0;
    uint64_t currentNL0_ = 0;
};

template <class Intf>
class CopyOutTools {
public:
    __aicore__ inline CopyOutTools()
    {}

    __aicore__ inline void SetParams(Intf *self)
    {
        self_ = self;
        tilingNBL1_ = self_->ctx.conv3dTiling->nBL1;
        tilingMAL1_ = self_->ctx.conv3dTiling->mAL1;
        valueHoWo_ = self_->ctx.orgHo * self_->ctx.orgWo;
    }

    __aicore__ inline void SetMN(uint64_t m, uint64_t n)
    {
        currentML0_ = m;
        currentNL0_ = n;
    }

    __aicore__ inline void SetFixpipeIntriParams(AscendC::FixpipeParamsV220 &intriParams)
    {
        if (self_->ctx.nBL1Iter == self_->ctx.maxNBL1Iter && self_->ctx.nBL0Iter == self_->ctx.maxNL0Iter) {
            intriParams.nSize = currentNL0_;
        } else {
            intriParams.nSize = self_->ctx.conv3dTiling->nL0;
        }

        if (self_->ctx.mAL1Iter == self_->ctx.maxMAL1Iter && self_->ctx.mAL0Iter == self_->ctx.maxML0Iter) {
            intriParams.mSize = self_->ctx.singleCoreM - self_->ctx.mAL1Iter * self_->ctx.conv3dTiling->mAL1 -
                                self_->ctx.mAL0Iter * self_->ctx.conv3dTiling->mL0;
            intriParams.srcStride = ConvApi::AlignB(self_->ctx.mAL0Tail, ConvApi::BLOCK_L0_M);
        } else {
            intriParams.mSize = self_->ctx.conv3dTiling->mL0;
            intriParams.srcStride = self_->ctx.conv3dTiling->mL0;
        }

        intriParams.dstStride = valueHoWo_;
        intriParams.quantPre = GetQuantPre();
        intriParams.reluEn = false;

        if constexpr (AscendC::IsSameType<typename Intf::L0cT, float>::value &&
                      AscendC::IsSameType<typename Intf::OutputT, float>::value) {
            intriParams.isChannelSplit = true;
        }

        KERNEL_LOG(KERNEL_DEBUG, "[CopyOut] intriParams.nSize %d, intriParams.mSize %d, intriParams.srcStride %d, "
            "intriParams.dstStride %d, intriParams.quantPre %d, intriParams.reluEn %d.\n",
            intriParams.nSize,
            intriParams.mSize,
            intriParams.srcStride,
            intriParams.dstStride,
            intriParams.quantPre,
            intriParams.reluEn);
    }

    __aicore__ inline void CopyOut(const AscendC::GlobalTensor<typename Intf::OutputT> &output)
    {
        AscendC::FixpipeParamsV220 intriParams;
        SetFixpipeIntriParams(intriParams);
        uint64_t offset = CalcFixpipeOffset();
        KERNEL_LOG(KERNEL_DEBUG, "[CopyOut] offset %d.\n", offset);
        AscendC::Fixpipe<typename Intf::OutputT, typename Intf::L0cT, AscendC::CFG_NZ>(output[offset], self_->ctx.cl0, intriParams);
    }

private:
    __aicore__ inline uint64_t CalcFixpipeOffset()
    {
        uint64_t offsetCout = tilingNBL1_ * self_->ctx.nBL1Iter + self_->ctx.conv3dTiling->nL0 * self_->ctx.nBL0Iter;
        uint64_t offsetM = tilingMAL1_ * self_->ctx.mAL1Iter + self_->ctx.conv3dTiling->mL0 * self_->ctx.mAL0Iter;
        // Currently, only one dout is produced each time
        uint64_t offsetDout = self_->ctx.dOutIter;
        if constexpr (Intf::groupConvType) {
            return offsetDout * ConvApi::AlignB(self_->ctx.conv3dTiling->orgCo, self_->ctx.cin0) * valueHoWo_ +
                   self_->ctx.groupOptIter * self_->ctx.orgCoAlignK0 * valueHoWo_ + offsetCout * valueHoWo_ +
                   offsetM * self_->ctx.cin0;
        } else {
            return offsetDout * self_->ctx.orgCoAlignK0 * valueHoWo_ + offsetCout * valueHoWo_ +
                   offsetM * self_->ctx.cin0;
        }
    }

    __aicore__ inline QuantMode_t GetQuantPre()
    {
        if constexpr (AscendC::IsSameType<typename Intf::L0cT, float>::value &&
                      AscendC::IsSameType<typename Intf::OutputT, float>::value) {
            return QuantMode_t::NoQuant;
        }

        if constexpr (AscendC::IsSameType<typename Intf::L0cT, int32_t>::value &&
                      AscendC::IsSameType<typename Intf::OutputT, half>::value) {
            return QuantMode_t::VDEQF16;
        }

        if constexpr (AscendC::IsSameType<typename Intf::L0cT, float>::value &&
                      AscendC::IsSameType<typename Intf::OutputT, bfloat16_t>::value) {
            return QuantMode_t::F322BF16;
        }

        return QuantMode_t::F322F16;
    }

private:
    Intf *self_ = nullptr;
    uint64_t tilingNBL1_ = 0;
    uint64_t tilingMAL1_ = 0;
    uint64_t valueHoWo_ = 0;
    uint64_t currentML0_ = 0;
    uint64_t currentNL0_ = 0;
};

};  // namespace Conv3dApiFunc

#endif  // __API_CONV3D_SUB_API_H__
