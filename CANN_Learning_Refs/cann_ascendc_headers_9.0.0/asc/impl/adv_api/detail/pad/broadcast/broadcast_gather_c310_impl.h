/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/* !
 * \file broadcast_gather_c310_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/pad/broadcast/broadcast_gather_c310_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/broadcast.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_GATHER_C310_IMPL_H__
#endif

#ifndef IMPL_PAD_BROADCAST_BROADCAST_GATHER_C310_IMPL_H
#define IMPL_PAD_BROADCAST_BROADCAST_GATHER_C310_IMPL_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"

namespace AscendC {
template <typename T>
__simd_vf__ inline void VfGenIndex(__ubuf__ T *indexUb, uint32_t sizeI0, uint32_t sizeI1, uint32_t sizeI2, uint32_t strideI0,
    uint32_t strideI1, uint32_t strideI2, uint32_t offset)
{
    Reg::RegTensor<T> v0;
    Reg::RegTensor<T> v1;
    Reg::RegTensor<T> v2;

    Reg::RegTensor<T> voffset;
    Reg::RegTensor<T> vr0;

    Reg::RegTensor<T> vd0;
    Reg::RegTensor<T> vd1;
    Reg::RegTensor<T> vd2;

    Reg::RegTensor<T> vi0;
    Reg::RegTensor<T> vi1;
    Reg::RegTensor<T> vi2;

    Reg::RegTensor<T> vs0;
    Reg::RegTensor<T> vs1;
    Reg::RegTensor<T> vs2;

    Reg::MaskReg p0;

    p0 = Reg::CreateMask<T>();
    Reg::Arange(v0, 0);

    Reg::Duplicate(v1, (T)sizeI2, p0);
    Reg::Div(vd0, v0, v1, p0);
    Reg::Mul(v2, vd0, v1, p0);
    Reg::Sub(vi2, v0, v2, p0);

    Reg::Duplicate(v1, (T)sizeI1, p0);
    Reg::Div(vd1, vd0, v1, p0);
    Reg::Mul(v2, vd1, v1, p0);
    Reg::Sub(vi1, vd0, v2, p0);

    Reg::Duplicate(v1, (T)sizeI0, p0);
    Reg::Div(vd2, vd1, v1, p0);
    Reg::Mul(v2, vd2, v1, p0);
    Reg::Sub(vi0, vd1, v2, p0);

    Reg::Duplicate(vs0, (T)strideI0, p0);
    Reg::Duplicate(vs1, (T)strideI1, p0);
    Reg::Duplicate(vs2, (T)strideI2, p0);

    Reg::Mul(vr0, vs2, vi2, p0);
    Reg::MulAddDst(vr0, vs1, vi1, p0);
    Reg::MulAddDst(vr0, vs0, vi0, p0);

    Reg::Duplicate(voffset, (T)offset, p0);
    Reg::Add(vr0, vr0, voffset, p0);

    Reg::StoreAlign(indexUb, vr0, p0);
}

template <typename T>
__simd_vf__ inline void VfGenIndexForFourDim(__ubuf__ T *indexUb, uint32_t sizeI0, uint32_t sizeI1, uint32_t sizeI2, uint32_t sizeI3,
    uint32_t strideI0, uint32_t strideI1, uint32_t strideI2, uint32_t strideI3, uint32_t offset)
{
    Reg::RegTensor<T> v0;
    Reg::RegTensor<T> v1;
    Reg::RegTensor<T> v2;

    Reg::RegTensor<T> voffset;
    Reg::RegTensor<T> vr0;

    Reg::RegTensor<T> vd0;
    Reg::RegTensor<T> vd1;
    Reg::RegTensor<T> vd2;
    Reg::RegTensor<T> vd3;

    Reg::RegTensor<T> vi0;
    Reg::RegTensor<T> vi1;
    Reg::RegTensor<T> vi2;
    Reg::RegTensor<T> vi3;

    Reg::RegTensor<T> vs0;
    Reg::RegTensor<T> vs1;
    Reg::RegTensor<T> vs2;
    Reg::RegTensor<T> vs3;

    Reg::MaskReg p0;

    p0 = Reg::CreateMask<T>();
    Reg::Arange(v0, 0);

    Reg::Duplicate(v1, (T)sizeI3, p0);
    Reg::Div(vd0, v0, v1, p0);
    Reg::Mul(v2, vd0, v1, p0);
    Reg::Sub(vi3, v0, v2, p0);

    Reg::Duplicate(v1, (T)sizeI2, p0);
    Reg::Div(vd1, vd0, v1, p0);
    Reg::Mul(v2, vd1, v1, p0);
    Reg::Sub(vi2, vd0, v2, p0);

    Reg::Duplicate(v1, (T)sizeI1, p0);
    Reg::Div(vd2, vd1, v1, p0);
    Reg::Mul(v2, vd2, v1, p0);
    Reg::Sub(vi1, vd1, v2, p0);

    Reg::Duplicate(v1, (T)sizeI0, p0);
    Reg::Div(vd3, vd2, v1, p0);
    Reg::Mul(v2, vd3, v1, p0);
    Reg::Sub(vi0, vd2, v2, p0);

    Reg::Duplicate(vs0, (T)strideI0, p0);
    Reg::Duplicate(vs1, (T)strideI1, p0);
    Reg::Duplicate(vs2, (T)strideI2, p0);
    Reg::Duplicate(vs3, (T)strideI3, p0);

    Reg::Mul(vr0, vs3, vi3, p0);
    Reg::MulAddDst(vr0, vs2, vi2, p0);
    Reg::MulAddDst(vr0, vs1, vi1, p0);
    Reg::MulAddDst(vr0, vs0, vi0, p0);

    Reg::Duplicate(voffset, (T)offset, p0);
    Reg::Add(vr0, vr0, voffset, p0);

    Reg::StoreAlign(indexUb, vr0, p0);
}

template <typename T>
__simd_vf__ inline void VfGatherBrc(__ubuf__ T *dstUb, __ubuf__ T *srcUb, __ubuf__ T *indexUb, uint16_t size0, uint16_t size1,
    uint16_t size2, uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2, uint32_t main, uint32_t tail)
{
    Reg::UnalignReg u0;
    Reg::RegTensor<T> vindex0;
    Reg::RegTensor<T> vindex;
    Reg::RegTensor<T> vstride0;
    Reg::RegTensor<T> vstride1;
    Reg::RegTensor<T> vstride2;
    Reg::RegTensor<T> vbase0;
    Reg::RegTensor<T> vbase1;
    Reg::RegTensor<T> vbase2;
    Reg::RegTensor<T> voffset0;
    Reg::RegTensor<T> voffset1;
    Reg::RegTensor<T> voffset2;

    Reg::RegTensor<T> vd0;
    Reg::RegTensor<T> vd1;

    Reg::MaskReg pa;
    Reg::MaskReg p0;
    Reg::MaskReg p1;
    pa = Reg::CreateMask<T>();
    uint32_t main1 = main;
    uint32_t tail1 = tail;
    p0 = Reg::UpdateMask<T>(main);
    p1 = Reg::UpdateMask<T>(tail);
    Reg::Duplicate(vstride0, (T)srcStride0, pa);
    Reg::Duplicate(vstride1, (T)srcStride1, pa);
    Reg::Duplicate(vstride2, (T)srcStride2, pa);
    Reg::LoadAlign(vindex0, indexUb);
    for (uint16_t i0 = 0; i0 < size0; ++i0) {
        Reg::Muls(voffset0, vstride0, (T)i0, p0);
        Reg::Add(vbase0, voffset0, vindex0, p0);
        for (uint16_t i1 = 0; i1 < size1; ++i1) {
            Reg::Muls(voffset1, vstride1, (T)i1, p0);
            Reg::Add(vbase1, vbase0, voffset1, p0);
            for (uint16_t i2 = 0; i2 < size2; ++i2) {
                Reg::Muls(voffset2, vstride2, (T)i2, p0);
                Reg::Add(vindex, vbase1, voffset2, p0);
                Reg::Gather(vd0, srcUb, vindex, p0);
                Reg::StoreUnAlign(dstUb, vd0, u0, main1);
            }
            Reg::Muls(voffset2, vstride2, (T)size2, p1);
            Reg::Add(vindex, vbase1, voffset2, p1);
            Reg::Gather(vd1, srcUb, vindex, p1);
            Reg::StoreUnAlign(dstUb, vd1, u0, tail1);
        }
    }
    Reg::StoreUnAlignPost(dstUb, u0, 0);
}

template <typename T>
__simd_vf__ inline void VfGatherBrcForFourDim(__ubuf__ T *dstUb, __ubuf__ T *srcUb, __ubuf__ T *indexUb, uint16_t size0, uint16_t size1,
    uint16_t size2, uint16_t size3, uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2, uint16_t srcStride3, uint32_t main,
    uint32_t tail)
{
    Reg::UnalignReg u0;
    Reg::RegTensor<T> vindex0;
    Reg::RegTensor<T> vindex;
    Reg::RegTensor<T> vstride0;
    Reg::RegTensor<T> vstride1;
    Reg::RegTensor<T> vstride2;
    Reg::RegTensor<T> vstride3;
    Reg::RegTensor<T> vbase0;
    Reg::RegTensor<T> vbase1;
    Reg::RegTensor<T> vbase2;
    Reg::RegTensor<T> voffset0;
    Reg::RegTensor<T> voffset1;
    Reg::RegTensor<T> voffset2;
    Reg::RegTensor<T> voffset3;

    Reg::RegTensor<T> vd0;
    Reg::RegTensor<T> vd1;

    Reg::MaskReg pa;
    Reg::MaskReg p0;
    Reg::MaskReg p1;
    pa = Reg::CreateMask<T>();
    uint32_t main1 = main;
    uint32_t tail1 = tail;
    p0 = Reg::UpdateMask<T>(main);
    p1 = Reg::UpdateMask<T>(tail);
    Reg::Duplicate(vstride0, (T)srcStride0, pa);
    Reg::Duplicate(vstride1, (T)srcStride1, pa);
    Reg::Duplicate(vstride2, (T)srcStride2, pa);
    Reg::Duplicate(vstride3, (T)srcStride3, pa);
    Reg::LoadAlign(vindex0, indexUb);
    for (uint16_t i0 = 0; i0 < size0; ++i0) {
        Reg::Muls(voffset0, vstride0, (T)i0, p0);
        Reg::Add(vbase0, voffset0, vindex0, p0);
        for (uint16_t i1 = 0; i1 < size1; ++i1) {
            Reg::Muls(voffset1, vstride1, (T)i1, p0);
            Reg::Add(vbase1, vbase0, voffset1, p0);
            for (uint16_t i2 = 0; i2 < size2; ++i2) {
                Reg::Muls(voffset2, vstride2, (T)i2, p0);
                Reg::Add(vbase2, vbase1, voffset2, p0);
                for (uint16_t i3 = 0; i3 < size3; ++i3) {
                    Reg::Muls(voffset3, vstride3, (T)i3, p0);
                    Reg::Add(vindex, vbase2, voffset3, p0);
                    Reg::Gather(vd0, srcUb, vindex, p0);
                    Reg::StoreUnAlign(dstUb, vd0, u0, main1);
                }
                Reg::Muls(voffset3, vstride3, (T)size3, p1);
                Reg::Add(vindex, vbase2, voffset3, p1);
                Reg::Gather(vd1, srcUb, vindex, p1);
                Reg::StoreUnAlign(dstUb, vd1, u0, tail1);
            }
        }
    }
    Reg::StoreUnAlignPost(dstUb, u0, 0);
}

template <typename T> __aicore__ inline void GenGatherIndex(__ubuf__ T *indexUb, uint32_t *size, uint32_t *srcStride)
{
    constexpr uint32_t VF_LEN = GetVecLen() / sizeof(T);
    uint32_t sizeI[3];
    uint32_t srcStrideI[3];
    srcStrideI[0] = srcStride[0];
    srcStrideI[1] = srcStride[1];
    srcStrideI[2] = srcStride[2];

    if (size[2] * size[1] * size[0] < VF_LEN) {
        sizeI[0] = size[0];
        sizeI[1] = size[1];
        sizeI[2] = size[2];
    } else if (size[2] * size[1] < VF_LEN) {
        sizeI[0] = VF_LEN / (size[2] * size[1]);
        sizeI[1] = size[1];
        sizeI[2] = size[2];
    } else if (size[2] < VF_LEN) {
        sizeI[0] = 1;
        sizeI[1] = VF_LEN / size[2];
        sizeI[2] = size[2];
    } else {
        sizeI[0] = 1;
        sizeI[1] = 1;
        sizeI[2] = VF_LEN;
    }

    VfGenIndex<T>(indexUb, sizeI[0], sizeI[1], sizeI[2], srcStrideI[0], srcStrideI[1],
        srcStrideI[2], 0);
}

template <typename T> __aicore__ inline void GenGatherIndexForFourDim(__ubuf__ T *indexUb, uint32_t *size, uint32_t *srcStride)
{
    constexpr uint32_t VF_LEN = GetVecLen() / sizeof(T);
    uint32_t sizeI[4];
    uint32_t srcStrideI[4];
    srcStrideI[0] = srcStride[0];
    srcStrideI[1] = srcStride[1];
    srcStrideI[2] = srcStride[2];
    srcStrideI[3] = srcStride[3];

    if (size[3] * size[2] * size[1] * size[0] < VF_LEN) {
        sizeI[0] = size[0];
        sizeI[1] = size[1];
        sizeI[2] = size[2];
        sizeI[3] = size[3];
    } else if (size[3] * size[2] * size[1] < VF_LEN) {
        sizeI[0] = VF_LEN / (size[3] * size[2] * size[1]);
        sizeI[1] = size[1];
        sizeI[2] = size[2];
        sizeI[3] = size[3];
    } else if (size[3] * size[2] < VF_LEN) {
        sizeI[0] = 1;
        sizeI[1] = VF_LEN / (size[3] * size[2]);
        sizeI[2] = size[2];
        sizeI[3] = size[3];
    } else if (size[3] < VF_LEN) {
        sizeI[0] = 1;
        sizeI[1] = 1;
        sizeI[2] = VF_LEN / size[3];
        sizeI[3] = size[3];
    } else {
        sizeI[0] = 1;
        sizeI[1] = 1;
        sizeI[2] = 1;
        sizeI[3] = VF_LEN;
    }

    VfGenIndexForFourDim<T>(indexUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], srcStrideI[0], srcStrideI[1],
        srcStrideI[2], srcStrideI[3], 0);
}

template <typename T>
__aicore__ inline void GatherWrapper(__ubuf__ T *dstUb, __ubuf__ T *srcUb, uint32_t *size, uint32_t *srcStride)
{
    constexpr uint32_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t sizeI[3];
    uint16_t srcStrideI[3];
    uint32_t main;
    uint32_t tail;
    uint32_t vlTile0;
    uint32_t vlTile1;
    uint32_t vlTile2;

    if (size[2] * size[1] < VF_LEN) {
        vlTile2 = size[2];
        vlTile1 = size[1];
        vlTile0 = VF_LEN / (vlTile2 * vlTile1);
        sizeI[0] = 1;
        sizeI[1] = 1;
        sizeI[2] = static_cast<uint16_t>(size[0] / vlTile0);
        srcStrideI[0] = 0;
        srcStrideI[1] = 0;
        srcStrideI[2] = static_cast<uint16_t>(srcStride[0] * vlTile0);
        main = vlTile2 * vlTile1 * vlTile0;
        tail = size[2] * size[1] * size[0] - sizeI[2] * main;
    } else if (size[2] < VF_LEN) {
        vlTile2 = size[2];
        vlTile1 = VF_LEN / (vlTile2);
        sizeI[0] = 1;
        sizeI[1] = size[0];
        sizeI[2] = size[1] / vlTile1;
        srcStrideI[0] = 0;
        srcStrideI[1] = static_cast<uint16_t>(srcStride[0]);
        srcStrideI[2] = static_cast<uint16_t>(srcStride[1] * vlTile1);
        main = vlTile2 * vlTile1;
        tail = size[2] * size[1] - sizeI[2] * main;
    } else {
        vlTile2 = VF_LEN;
        sizeI[0] = size[0];
        sizeI[1] = size[1];
        sizeI[2] = size[2] / vlTile2;
        srcStrideI[0] = static_cast<uint16_t>(srcStride[0]);
        srcStrideI[1] = static_cast<uint16_t>(srcStride[1]);
        srcStrideI[2] = static_cast<uint16_t>(srcStride[2] * vlTile2);
        main = vlTile2;
        tail = size[2] - sizeI[2] * main;
    }
    LocalTensor<T> indexUb;
    PopStackBuffer<T, TPosition::LCM>(indexUb);
    if constexpr (sizeof(T) == sizeof(uint32_t)) {
        GenGatherIndex((__ubuf__ int32_t *)indexUb.GetPhyAddr(), size, srcStride);
        VfGatherBrc<uint32_t>((__ubuf__ uint32_t *)dstUb, (__ubuf__ uint32_t *)srcUb,
            (__ubuf__ uint32_t *)indexUb.GetPhyAddr(), sizeI[0], sizeI[1], sizeI[2], srcStrideI[0], srcStrideI[1],
            srcStrideI[2], main, tail);
    } else {
        GenGatherIndex((__ubuf__ int16_t *)indexUb.GetPhyAddr(), size, srcStride);
        VfGatherBrc<uint16_t>((__ubuf__ uint16_t *)dstUb, (__ubuf__ uint16_t *)srcUb,
            (__ubuf__ uint16_t *)indexUb.GetPhyAddr(), sizeI[0], sizeI[1], sizeI[2], srcStrideI[0], srcStrideI[1],
            srcStrideI[2], main, tail);
    }
}

template <typename T>
__aicore__ inline void GatherWrapperForFourDim(__ubuf__ T *dstUb, __ubuf__ T *srcUb, uint32_t *size, uint32_t *srcStride)
{
    constexpr uint32_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t sizeI[4];
    uint16_t srcStrideI[4];
    uint32_t main;
    uint32_t tail;
    uint32_t vlTile0;
    uint32_t vlTile1;
    uint32_t vlTile2;
    uint32_t vlTile3;

    if (size[3] * size[2] * size[1] < VF_LEN) {
        vlTile3 = size[3];
        vlTile2 = size[2];
        vlTile1 = size[1];
        vlTile0 = VF_LEN / (vlTile3 * vlTile2 * vlTile1);
        sizeI[0] = 1;
        sizeI[1] = 1;
        sizeI[2] = 1;
        sizeI[3] = static_cast<uint16_t>(size[0] / vlTile0);
        srcStrideI[0] = 0;
        srcStrideI[1] = 0;
        srcStrideI[2] = 0;
        srcStrideI[3] = static_cast<uint16_t>(srcStride[0] * vlTile0);
        main = vlTile3 * vlTile2 * vlTile1 * vlTile0;
        tail = size[3] * size[2] * size[1] * size[0] - sizeI[3] * main;
    } else if (size[3] * size[2] < VF_LEN) {
        vlTile3 = size[3];
        vlTile2 = size[2];
        vlTile1 = VF_LEN / (vlTile2 * vlTile3);
        sizeI[0] = 1;
        sizeI[1] = 1;
        sizeI[2] = size[0];
        sizeI[3] = static_cast<uint16_t>(size[1] / vlTile1);
        srcStrideI[0] = 0;
        srcStrideI[1] = 0;
        srcStrideI[2] = static_cast<uint16_t>(srcStride[0]);
        srcStrideI[3] = static_cast<uint16_t>(srcStride[1] * vlTile1);
        main = vlTile3 * vlTile2 * vlTile1;
        tail = size[3] * size[2] * size[1] - sizeI[3] * main;
    } else if (size[3] < VF_LEN) {
        vlTile3 = size[3];
        vlTile2 = VF_LEN / vlTile3;
        sizeI[0] = 1;
        sizeI[1] = size[0];;
        sizeI[2] = size[1];
        sizeI[3] = static_cast<uint16_t>(size[2] / vlTile2);
        srcStrideI[0] = 0;
        srcStrideI[1] = static_cast<uint16_t>(srcStride[0]);
        srcStrideI[2] = static_cast<uint16_t>(srcStride[1]);
        srcStrideI[3] = static_cast<uint16_t>(srcStride[2] * vlTile2);
        main = vlTile3 * vlTile2;
        tail = size[3] * size[2] - sizeI[3] * main;
    } else {
        vlTile3 = VF_LEN;
        sizeI[0] = size[0];
        sizeI[1] = size[1];
        sizeI[2] = size[2];
        sizeI[3] = static_cast<uint16_t>(size[3] / vlTile3);
        srcStrideI[0] = static_cast<uint16_t>(srcStride[0]);
        srcStrideI[1] = static_cast<uint16_t>(srcStride[1]);
        srcStrideI[2] = static_cast<uint16_t>(srcStride[2]);
        srcStrideI[3] = static_cast<uint16_t>(srcStride[3] * vlTile3);
        main = vlTile3;
        tail = size[3] - sizeI[3] * main;
    }
    LocalTensor<T> indexUb;
    PopStackBuffer<T, TPosition::LCM>(indexUb);
    if constexpr (sizeof(T) == sizeof(uint32_t)) {
        GenGatherIndexForFourDim((__ubuf__ int32_t *)indexUb.GetPhyAddr(), size, srcStride);
        VfGatherBrcForFourDim<uint32_t>((__ubuf__ uint32_t *)dstUb, (__ubuf__ uint32_t *)srcUb,
            (__ubuf__ uint32_t *)indexUb.GetPhyAddr(), sizeI[0], sizeI[1], sizeI[2], sizeI[3], srcStrideI[0],
            srcStrideI[1], srcStrideI[2], srcStrideI[3], main, tail);
    } else {
        GenGatherIndexForFourDim((__ubuf__ int16_t *)indexUb.GetPhyAddr(), size, srcStride);
        VfGatherBrcForFourDim<uint16_t>((__ubuf__ uint16_t *)dstUb, (__ubuf__ uint16_t *)srcUb,
            (__ubuf__ uint16_t *)indexUb.GetPhyAddr(), sizeI[0], sizeI[1], sizeI[2], sizeI[3], srcStrideI[0],
            srcStrideI[1], srcStrideI[2], srcStrideI[3], main, tail);
    }
}
} // namespace AscendC
#endif // IMPL_PAD_BROADCAST_BROADCAST_GATHER_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_GATHER_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_GATHER_C310_IMPL_H__
#endif
