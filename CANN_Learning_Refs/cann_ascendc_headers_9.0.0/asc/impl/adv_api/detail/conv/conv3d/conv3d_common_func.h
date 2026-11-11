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
 * \file conv3d_common_func.h
 * \brief
 */

#ifndef API_CONV3D_COMMON_FUNC_H
#define API_CONV3D_COMMON_FUNC_H

#include "conv3d_iterate_impl.h"

namespace Conv3dApiFunc {

CONV_DECLARE_REG_IMPL(Init);
CONV_DECLARE_REG_IMPL(SetOrgInputShape);
CONV_DECLARE_REG_IMPL(SetOrgWeightShape);
CONV_DECLARE_REG_IMPL(SetOrgOutputShape);
CONV_DECLARE_REG_IMPL(SetSingleInputShape);
CONV_DECLARE_REG_IMPL(SetSingleOutputShape);
CONV_DECLARE_REG_IMPL(SetInputStartPosition);
CONV_DECLARE_REG_IMPL(SetGroupOptInfo);
CONV_DECLARE_REG_IMPL(Iterate);
CONV_DECLARE_REG_IMPL(GetTensorC);
CONV_DECLARE_REG_IMPL(IterateAll);

using TypeFalse = const struct {
    __uint128_t _[1024];
};

template <class Intf, uint32_t ImplType>
struct GetTensorC {
    template <bool sync = true>
    static __aicore__ inline bool call(
        Intf *self, const AscendC::GlobalTensor<typename Intf::OutputT> &output, bool enSequentialWrite = false)
    {
        self->ctx.copyOutIns.CopyOut(output);
        self->ctx.queueCL0.FreeTensor(self->ctx.cl0);
        if (self->ctx.enableBias) {
            if (!self->ctx.biasFullLoadFlag) {
                self->ctx.queueBiasL1.FreeTensor(self->ctx.biasL1);
            }
            if constexpr (Intf::formatType != ConvCommonApi::ConvFormat::NCDHW) {
                self->ctx.queueBiasBT.FreeTensor(self->ctx.biasBT);
            }
        }
        KERNEL_LOG(KERNEL_DEBUG, "[GetTensorC] GetTensorC Success! \n\n");
        return false;
    }
};

template <class Intf, uint32_t ImplType>
struct IterateAll {
    template <bool sync = true>
    static __aicore__ inline bool call(
        Intf *self, const AscendC::GlobalTensor<typename Intf::OutputT> &output, bool enPartialSum = false)
    {
        self->ctx.loadBiasL1Ins.SetParams(self);
        self->ctx.loadBL1Ins.SetParams(self);
        self->ctx.loadAl1Ins.SetParams(self);
        self->ctx.loadBL0Ins.SetParams(self);
        self->ctx.madIns.SetParams(self);
        self->ctx.copyOutIns.SetParams(self);
        self->ctx.loadBiasBTIns.SetParams(self);
        if constexpr (Intf::formatType == ConvCommonApi::ConvFormat::NCDHW) {
            self->ctx.loadAL0Ins.SetParams(self);
        } else {
            self->ctx.loadAL0Ins.SetParams(self, &self->ctx.loadAl1Ins);
        }
        if constexpr (Intf::groupConvType) {
            IterateAllWithGroups(self, output, enPartialSum);
        } else {
            IterateAllBase(self, output, enPartialSum);
        }
        return false;
    }

    static __aicore__ void inline IterateAllBase(
        Intf *self, const AscendC::GlobalTensor<typename Intf::OutputT> &output, bool enPartialSum = false) 
    {
        if (self->ctx.biasFullLoadFlag && self->ctx.enableBias) {
            self->ctx.biasL1 = self->ctx.queueBiasL1.template AllocTensor<typename Intf::BiasT>();
            self->ctx.loadBiasL1Ins.LoadChannelWiseL1(self->ctx.biasL1, self->ctx.biasgm);
            self->ctx.queueBiasL1.EnQue(self->ctx.biasL1);
            self->ctx.biasL1 = self->ctx.queueBiasL1.template DeQue<typename Intf::BiasT>();
        }
        while (Iterate<Intf, ImplType>::call(self, enPartialSum)) {
            GetTensorC<Intf, ImplType>::call(self, output);
            if constexpr (Intf::formatType != ConvCommonApi::ConvFormat::NCDHW) {
                if (self->ctx.enableBias) {
                    self->ctx.queueBiasBT.FreeAllEvent();
                }
            }
        }
        if (self->ctx.biasFullLoadFlag && self->ctx.enableBias) {
            self->ctx.queueBiasL1.FreeTensor(self->ctx.biasL1);
        }
        self->ctx.isFirstIterate = true;
        self->ctx.nBL0Iter = 0;
        self->ctx.nBL1Iter = 0;
    }

    static __aicore__ void inline ReCalculationKTilingWithGroups(
        Intf *self, uint64_t &updateKAL1, uint64_t &updateKBL1, uint64_t &updateKL0)
    {
        // Update kaL1/kbL1/kL0 when singleCoreCin changes.
        uint64_t curKAL1Kd = Conv3dApi::GetCurrentKD(
            self->ctx.conv3dTiling->kAL1, ConvApi::AlignB(self->ctx.orgCi, self->ctx.cin0), self->ctx.kernelHxkernelW);
        uint64_t curKBL1Kd = Conv3dApi::GetCurrentKD(
            self->ctx.conv3dTiling->kBL1, ConvApi::AlignB(self->ctx.orgCi, self->ctx.cin0), self->ctx.kernelHxkernelW);
        uint64_t curCinxKhxKw = ConvApi::AlignB(self->ctx.singleCoreCin, self->ctx.cin0) * self->ctx.kernelHxkernelW;
        updateKAL1 = curCinxKhxKw > self->ctx.conv3dTiling->kAL1 ? 0 : curCinxKhxKw;
        updateKBL1 = curCinxKhxKw > self->ctx.conv3dTiling->kBL1 ? 0 : curCinxKhxKw;
        if (curKAL1Kd > 1) {
            // The kAL1/kBL1 is calculated by multiplying the new cin by the kd of the previous tiling decision.
            updateKAL1 = curKAL1Kd * curCinxKhxKw;
        }
        if (updateKAL1 == 0) {
            // To ensure that kAL1/kBL1 is the factor of cin1, 1 is used as kAL1, which can be optimized in the future.
            updateKAL1 =
                curCinxKhxKw % self->ctx.conv3dTiling->kAL1 == 0 ? 0 : self->ctx.cin0 * self->ctx.kernelHxkernelW;
        }
        if (curKBL1Kd > 1) {
            // The kAL1/kBL1 is calculated by multiplying the new cin by the kd of the previous tiling decision.
            updateKBL1 = curKBL1Kd * curCinxKhxKw;
        }
        if (updateKBL1 == 0) {
            // To ensure that kAL1/kBL1 is the factor of cin1, 1 is used as kAL1, which can be optimized in the future.
            updateKBL1 =
                curCinxKhxKw % self->ctx.conv3dTiling->kBL1 == 0 ? 0 : self->ctx.cin0 * self->ctx.kernelHxkernelW;
        }
        if (updateKAL1 % self->ctx.conv3dTiling->kL0 != 0 || updateKBL1 % self->ctx.conv3dTiling->kL0 != 0) {
            // To ensure that kL0 is the factor of kAL1/kBL1, cin0 is used as kL0, which can be optimized in the future.
            updateKL0 = self->ctx.cin0;
        }
    }

    static __aicore__ void inline PreProcessGroupOptDimTail(Intf *self)
    {
        if (!self->ctx.isGroupOptDimTail) {
            return;
        }

        if (self->ctx.singleCoreCinTail != 0) {
            KERNEL_LOG(KERNEL_DEBUG, "[IterateAllWithGroups] singleCoreCin %d update to %d \n",
                self->ctx.singleCoreCin,
                self->ctx.singleCoreCinTail);
            self->ctx.singleCoreCin = self->ctx.singleCoreCinTail;
            uint64_t updateKAL1 = 0;
            uint64_t updateKBL1 = 0;
            uint64_t updateKL0 = 0;
            ReCalculationKTilingWithGroups(self, updateKAL1, updateKBL1, updateKL0);
            InitKDirectionBaseValue<Intf>(self, updateKAL1, updateKBL1, updateKL0);
            self->ctx.preloadAL1DbFlag = false;
            self->ctx.preloadABL1DbFlag = false;
            KERNEL_LOG(KERNEL_DEBUG, "[IterateAllWithGroups] updateKAL1 %d updateKBL1 %d updateKL0 %d \n",
                updateKAL1,
                updateKBL1,
                updateKL0);
        }
        if (self->ctx.singleCoreCoutTail != 0) {
            KERNEL_LOG(KERNEL_DEBUG, "[IterateAllWithGroups] singleCoreCo %d update to %d \n",
                self->ctx.singleCoreCo,
                self->ctx.singleCoreCoutTail);
            self->ctx.singleCoreCo = self->ctx.singleCoreCoutTail;
            InitCoutDirectionBaseValue<Intf>(self);
        }
    }

    static __aicore__ void inline PostProcessGroupOptDimTail(Intf *self, const uint64_t &tmpSingleCoreCo,
        const uint8_t &tmpPreloadAL1DbFlag, const uint8_t &tmpPreloadABL1DbFlag)
    {
        if (!self->ctx.isGroupOptDimTail) {
            return;
        }

        if (self->ctx.singleCoreCin != self->ctx.conv3dTiling->cinOpt) {
            self->ctx.singleCoreCin = self->ctx.conv3dTiling->cinOpt;
            InitKDirectionBaseValue<Intf>(self);
            self->ctx.preloadAL1DbFlag = tmpPreloadAL1DbFlag;
            self->ctx.preloadABL1DbFlag = tmpPreloadABL1DbFlag;
        }
        if (self->ctx.singleCoreCo != tmpSingleCoreCo) {
            self->ctx.singleCoreCo = tmpSingleCoreCo;
            InitCoutDirectionBaseValue<Intf>(self);
        }
        self->ctx.isGroupOptDimTail = false;
    }

    static __aicore__ void inline IterateAllWithGroups(
        Intf *self, const AscendC::GlobalTensor<typename Intf::OutputT> &output, bool enPartialSum = false)
    {
        uint64_t weightOneGroupOptSize =
            self->ctx.conv3dTiling->cinOpt * self->ctx.kernelHxkernelWxkernelD * self->ctx.conv3dTiling->coutOpt;
        while (self->ctx.groupOptIter < self->ctx.maxGroupOptIter - 1) {
            IterateAllBase(self, output, enPartialSum);
            self->SetWeight(self->ctx.bgm[weightOneGroupOptSize]);
            if (self->ctx.enableBias) {
                self->SetBias(self->ctx.biasgm[self->ctx.conv3dTiling->coutOpt]);
            }
            self->ctx.groupOptIter++;
        }
        uint64_t tmpSingleCoreCo = self->ctx.singleCoreCo;
        uint8_t tmpPreloadAL1DbFlag = self->ctx.preloadAL1DbFlag;
        uint8_t tmpPreloadABL1DbFlag = self->ctx.preloadABL1DbFlag;
        PreProcessGroupOptDimTail(self);
        IterateAllBase(self, output, enPartialSum);
        PostProcessGroupOptDimTail(self, tmpSingleCoreCo, tmpPreloadAL1DbFlag, tmpPreloadABL1DbFlag);
        self->ctx.groupOptIter = 0;
    }
};

}  // namespace Conv3dApiFunc

#endif
