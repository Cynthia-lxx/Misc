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
 * \file kernel_operator_dump_tensor_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/basic_api/dav_c310/kernel_operator_dump_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H

#include "kernel_tpipe_impl.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_data_copy_impl.h"
#include "kernel_pop_stack_buffer.h"
#include "kernel_operator_print_impl.h"
#include "impl/utils/debug/asc_aicore_dump_impl.h"
#include "impl/utils/debug/asc_aicore_time_impl.h"

namespace AscendC {
template <typename T>
__aicore__ constexpr inline Internal::DumpTensorDataType GetTensorDataType();

template <typename T>
constexpr __aicore__ inline uint32_t GetDataType()
{
    return static_cast<uint32_t>(GetTensorDataType<T>());
}

template <typename T>
constexpr __aicore__ inline bool IsSpecialDtype()
{
    if constexpr (SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t, fp8_e5m2_t, fp8_e4m3fn_t, hifloat8_t>())  {
        return true;
    }
    return false;
}

template <typename T>
constexpr __aicore__ inline uint32_t GetSpecialDtypeBitSize()
{
    if constexpr (SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        return ConstantsInternal::ASCENDC_B4_BIT_NUM;
    }
    if constexpr (SupportType<T, fp8_e5m2_t, fp8_e4m3fn_t, hifloat8_t>()) {
        return ONE_BYTE_BIT_SIZE;
    }
    return ONE_BYTE_BIT_SIZE;
}

__aicore__ inline uint8_t GetRelevantDumpCubeCoreIdx()
{
    if ASCEND_IS_AIV {
        return GetBlockIdxImpl() / 2 + AIV_CORE_NUM;
    } else {
        // return itself
        return GetBlockIdxImpl() + AIV_CORE_NUM;
    }
}

__aicore__ inline void InitDumpImpl(bool mixFlag, uint32_t gmLen)
{
    uint32_t totalBlockNum = get_block_num();
    if (g_dumpWorkspaceReserved == nullptr) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "init dump get nullptr system workspace ptr"); });
        return;
    }
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved);

    if (mixFlag == true) {
        totalBlockNum = get_block_num() * (1 + MIX_NUM);
    }
    uint32_t blockDumpSize = DUMP_UINTSIZE; // DUMP_UINTSIZE is 1M

    uint32_t numBlocks = GetDumpBlockIdx();
    if (numBlocks >= DUMP_CORE_COUNT) {
        return;
    }
    uint32_t blkInfoLen = sizeof(BlockInfo) + sizeof(DumpMeta);
    uint64_t blockInfoStart = dumpWorkspaceStart + numBlocks * DUMP_UINTSIZE;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_LEN_POS) = blockDumpSize;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_CORE_POS) = numBlocks;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_BLOCKNUM_POS) = totalBlockNum;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_DUMPOFFSET_POS) = blockDumpSize - blkInfoLen;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_MAGIC_POS) = BLOCK_INFO_MAGIC_NUM;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_RSV_POS) = 0;
    *((__gm__ uint64_t*)((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_DUMP_ADDR)) = blockInfoStart + blkInfoLen;
    dcci((__gm__ uint64_t*)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);

    // add DUMP_META info
    blockInfoStart = blockInfoStart + sizeof(BlockInfo);
    *(__gm__ uint32_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_TYPE_POS) =
        static_cast<uint32_t>(DumpType::DUMP_META);
    *(__gm__ uint32_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_LEN_POS) = 8;
    *(__gm__ uint16_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_BLOCK_DIM_POS) =
        static_cast<uint16_t>(get_block_num());
    *(__gm__ uint8_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_CORE_TYPE_POS) =
        static_cast<uint8_t>(g_coreType);
    *(__gm__ uint8_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_TASK_RATION) =
        static_cast<uint8_t>(mixFlag);
    *((__gm__ uint32_t*)blockInfoStart + DUMP_META_RSV_POS) = 0;
    dcci((__gm__ uint64_t*)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}
__aicore__ inline DataCopyParams GetDataCopyParamImpl(uint32_t offset)
{
    DataCopyParams repeatParams;
    repeatParams.blockCount = 1;
    repeatParams.blockLen = (offset + ONE_BLK_SIZE - 1)/ ONE_BLK_SIZE;
    repeatParams.srcStride = 0;
    repeatParams.dstStride = 0;
    return repeatParams;
}

template <typename T>
__aicore__ inline uint32_t CheckValidPosition(const LocalTensor<T>& tensor)
{
    // set the head struct value
    uint32_t position = 0;
    if ((Hardware)GetPhyType((TPosition)tensor.GetPosition()) == Hardware::UB) {
        position = static_cast<uint32_t>(AscendC::Hardware::UB);
        return position;
    } else if ((Hardware)GetPhyType((TPosition)tensor.GetPosition()) == Hardware::L1) {
        position = static_cast<uint32_t>(AscendC::Hardware::L1);
        return position;
    } else if ((Hardware)GetPhyType((TPosition)tensor.GetPosition()) == Hardware::L0C) {
        position = static_cast<uint32_t>(AscendC::Hardware::L0C);
        return position;
    } else {
        return 0;
    }
}

__aicore__ inline void UpdateBlockInfo(__gm__ BlockInfo* ptr, uint32_t offset)
{
    ptr->dumpOffset -= offset;
    ptr->dumpAddr += offset;
    dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

__aicore__ inline void WriteDumpShapeInfo(const ShapeInfo &shapeInfo)
{
    uint8_t core = GetDumpBlockIdx();
    if (core >= DUMP_CORE_COUNT) {
        return;
    }
    uint32_t valueSize = sizeof(DumpShapeMessageHead);
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved);
    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    uint32_t tlvSize = valueSize + DUMP_SHAPE_MESSAGE_TL_LEN;
    if (ptr->dumpOffset < tlvSize) {
        ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "remained space is %d, which is not enough for write data %u", ptr->dumpOffset,
            tlvSize); });
        *((__gm__ uint32_t*)ptr + BLOCK_INFO_RSV_POS) = DUMP_EXC_FLAG;
        dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
        return;
    }
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_SHAPE_MESSAGE_HEAD_TYPE_POS) = static_cast<uint32_t>(DumpType::DUMP_SHAPE);
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_SHAPE_MESSAGE_HEAD_LEN_POS) = valueSize;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_SHAPE_MESSAGE_HEAD_DIM_POS) = shapeInfo.shapeDim;
    for (uint32_t idx = 0; idx < shapeInfo.shapeDim && idx < K_MAX_SHAPE_DIM; idx++) {
        *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_SHAPE_MESSAGE_HEAD_SHAPE_START_POS + idx) = shapeInfo.shape[idx];
    }
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_SHAPE_MESSAGE_HEAD_RSV_POS) = 0;
    // update block info
    UpdateBlockInfo(ptr, tlvSize);
}

__aicore__ inline void WriteRingBufShapeInfo(const ShapeInfo &shapeInfo);

__aicore__ inline void DumpShapeImpl(const ShapeInfo &shapeInfo)
{
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        WriteRingBufShapeInfo(shapeInfo);
    } else {
        WriteDumpShapeInfo(shapeInfo);
    }
}

__aicore__ inline __ssbuf__ L12UBDumpCtrlMessage* GetDumpCtrlMsg()
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    return reinterpret_cast<__ssbuf__ L12UBDumpCtrlMessage*>(ConstDefiner::Instance().cpuSSbuf);
#else
    return reinterpret_cast<__ssbuf__ L12UBDumpCtrlMessage*>(get_imm(0));
#endif
}

__aicore__ inline void EnableL1Dump()
{
    __ssbuf__ L12UBDumpCtrlMessage* ptr = GetDumpCtrlMsg();
    ptr->enDumpFlag = BLOCK_INFO_MAGIC_NUM;
    ptr->sig = 0;
    ptr->len = 0;
}

__aicore__ inline void DisableL1Dump()
{
    __ssbuf__ L12UBDumpCtrlMessage* ptr = GetDumpCtrlMsg();
    ptr->enDumpFlag = 0;
    ptr->sig = 0;
    ptr->len = 0;
}

__aicore__ inline bool IsL1DumpEnable()
{
    __ssbuf__ L12UBDumpCtrlMessage* ptr = GetDumpCtrlMsg();
    return ptr->enDumpFlag == BLOCK_INFO_MAGIC_NUM;
}

__aicore__ inline void SendL12UBDumpCtrlMessage(uint32_t sig, uint32_t len)
{
    __ssbuf__ L12UBDumpCtrlMessage* ptr = GetDumpCtrlMsg();
    ptr->sig = sig;
    ptr->len = len;
}

__aicore__ inline uint32_t GetLoopCount(uint32_t offset, uint32_t unitLen)
{
    uint32_t loopCount = 0;
    if (offset % unitLen != 0) {
        loopCount = 1 + offset / unitLen;
    } else {
        loopCount = offset / unitLen;
    }
    return loopCount;
}

template <typename T>
__aicore__ inline void DumpTensorL12GMImpl(const LocalTensor<T>& tensor, uint32_t offset)
{
    using PrimType = PrimT<T>;
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved);
    uint8_t core = GetDumpBlockIdx();
    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    uint32_t needCopyLen = offset;
    uint32_t copyCount = GetLoopCount(offset, L1_DUMP_UB_SIZE);
    for (uint32_t idx = 0; idx < copyCount; idx++) {
        uint32_t copyLen = L1_DUMP_UB_SIZE;
        if (needCopyLen < L1_DUMP_UB_SIZE) {
            copyLen = needCopyLen;
        }
        SendL12UBDumpCtrlMessage(ConstantsInternal::L1_2_UB_DUMP_CTRL_SIGNAL_DUMP, copyLen);
        PipeBarrier<PIPE_ALL>();
        DataCopyParams repeatParams = GetDataCopyParamImpl(copyLen);
    #if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        auto ubAddr = reinterpret_cast<__ubuf__ PrimType*>(ConstDefiner::Instance().cpuUB);
    #else
        auto ubAddr = reinterpret_cast<__ubuf__ PrimType*>(get_imm(0));
    #endif
        // L1 to UB
        DataCopyL12UBImpl(ubAddr,
            reinterpret_cast<__cbuf__ PrimType*>((__cbuf__ uint8_t*)tensor.GetPhyAddr() + idx * L1_DUMP_UB_SIZE), repeatParams);
        PipeBarrier<PIPE_ALL>(); // set_intra_block前需要保证所有流水已经执行完毕
        set_intra_block(PIPE_MTE3, ConstantsInternal::DUMP_INTRA_BLOCK_ID); // notify vector copy UB2GM
        // 等待AIV上一次数据搬运完成
        wait_intra_block(PIPE_MTE3, ConstantsInternal::DUMP_INTRA_BLOCK_ID);
        needCopyLen -= copyLen;
    }
}

template <typename T>
__aicore__ inline void DumpTensorL0C2GMImpl(const LocalTensor<T>& tensor, __gm__ BlockInfo* ptr, uint32_t dumpSize)
{
    // L0C to GM
    uint16_t align = (dumpSize % DEFAULT_BLOCK_SIZE == 0) ? 0 : 1;
    uint16_t countBlks = align + dumpSize / DEFAULT_BLOCK_SIZE;

    uint16_t burstLen = static_cast<uint16_t>(SRC_BURST_LEN_SIZE_ELE * SRC_BURST_LEN_SIZE_ELE
        * sizeof(float) / ONE_BLK_SIZE);
    uint16_t n = countBlks * BLOCK_CUBE;
    uint16_t m = (burstLen * ONE_BLK_SIZE / B32_BYTE_SIZE) / BLOCK_CUBE;
    bool nz2dnEn = true;
    copy_matrix_cc_to_gm((__gm__ float *)(ptr->dumpAddr), (__cc__ float *)(tensor.GetPhyAddr()),
        0, n, m, m * BLOCK_CUBE, m, 0,
        0, 0, static_cast<uint64_t>(QuantMode_t::NoQuant),
        static_cast<uint8_t>(false), false, false,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0, false, false, true,
        false, false, false);
}

template <typename T>
__aicore__ inline void DumpTensorLocal2GMEntityImpl(const LocalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize)
{
    using PrimType = PrimT<T>;
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved);
    uint8_t core = GetDumpBlockIdx();
    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    uint32_t position = CheckValidPosition(tensor);
    // set the head struct value
    if (position == 0) {
        ASCENDC_ASSERT((false),
                   { KERNEL_LOG(KERNEL_ERROR, "dump tensor only support dump tensor from local to gm"); });
        return;
    }
    const Hardware srcHWPos = GetPhyType((TPosition)tensor.GetPosition());
    if (srcHWPos == Hardware::L1 && !IsL1DumpEnable()) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "data moving from L1 to Gm is not supported on current device"); });
        return;
    }
    PrimType data;
    if (core >= DUMP_CORE_COUNT) {
        return;
    }
    uint32_t offset;
    uint32_t padOffset;
    if (IsSpecialDtype<PrimType>()) {
        // calculate total tensor size, overflow will not happen for special dtype
        offset = dumpSize * ONE_BYTE_BIT_SIZE / GetSpecialDtypeBitSize<PrimType>();
    } else {
        offset = dumpSize * sizeof(PrimType);
    }
    padOffset = AlignUp(offset, ONE_BLK_SIZE);

    if (ptr->dumpOffset < (padOffset + sizeof(DumpMessageHead))) {
        ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "remained space is %d, which is not enough for write data %u", ptr->dumpOffset,
            (padOffset + sizeof(DumpMessageHead))); });
        *((__gm__ uint32_t*)ptr + BLOCK_INFO_RSV_POS) = DUMP_EXC_FLAG;
        dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
        return;
    }
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_TYPE_POS) = static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_LEN_POS) = padOffset + DUMP_MSG_HEAD_SIZE;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_ADDR_POS) =
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(tensor.GetPhyAddr()));
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DATA_TYPE_POS) = GetDataType<PrimType>();
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DESC_POS) = desc;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_BUFFERID_POS) = 0;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_POSITION_POS) = position;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DUMP_SIZE_POS) = dumpSize;
    // update block info
    UpdateBlockInfo(ptr, sizeof(DumpMessageHead));
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t*)ptr->dumpAddr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (srcHWPos == Hardware::UB) {
        DataCopyParams repeatParams = GetDataCopyParamImpl(padOffset);
        DataCopyUB2GMImpl((__gm__ PrimType*)(ptr->dumpAddr), (__ubuf__ PrimType*)tensor.GetPhyAddr(), repeatParams); // UB to GM
        PipeBarrier<PIPE_ALL>();
        UpdateBlockInfo(ptr, padOffset);
    } else if (srcHWPos == Hardware::L1) {
        DumpTensorL12GMImpl(tensor, padOffset);
        PipeBarrier<PIPE_ALL>();
    } else if (srcHWPos == Hardware::L0C) {
        if ASCEND_IS_NOT_AIC {
            return;
        }
        DumpTensorL0C2GMImpl(tensor, ptr, dumpSize);
        PipeBarrier<PIPE_ALL>();
        UpdateBlockInfo(ptr, padOffset);
    }
}

template <typename T>
__aicore__ inline void DumpTensorLocal2GMImpl(const LocalTensor<T>& src, uint32_t desc,
                                                                uint32_t dumpSize)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        const Hardware position = GetPhyType(static_cast<TPosition>(src.GetPosition()));
        if (position == Hardware::UB) {
            __asc_aicore::asc_dump_ubuf((__ubuf__ T*)src.GetPhyAddr(), desc, dumpSize);
        } else if (position == Hardware::L1) {
            return; // L1 to GM dump not support
        } else if (position == Hardware::L0C) {
            if ASCEND_IS_NOT_AIC {
                return;
            }
            __asc_aicore::asc_dump_cbuf((__cc__ T*)src.GetPhyAddr(), desc, dumpSize);
        } else {
            ASCENDC_ASSERT((false),
                    { KERNEL_LOG(KERNEL_ERROR, "dump tensor only support dump tensor from local to gm"); });
            return;
        }
    } else {
        DumpTensorLocal2GMEntityImpl(src, desc, dumpSize);
    }
    set_ctrl(ctrlValue);
}

template <typename T>
__aicore__ inline void InitTmpTensor(LocalTensor<T>& tmpLocal, uint8_t quePos)
{
    TBuffAddr localTmpTBuf;
    localTmpTBuf.logicPos = quePos;
    tmpLocal.SetAddr(localTmpTBuf);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    tmpLocal.address_.absAddr = reinterpret_cast<uint8_t *>(ConstDefiner::Instance().cpuUB);
#else
    tmpLocal.address_.bufferAddr = get_imm(0);
#endif
    tmpLocal.address_.dataLen = ONE_DUMP_BACKUP_SIZE;
}

__aicore__ inline bool CheckDumpValid(uint32_t padOffset)
{
    uint8_t core = GetDumpBlockIdx();
    if (core >= DUMP_CORE_COUNT) {
        return false;
    }
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved);

    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    if (ptr->dumpOffset < (padOffset + sizeof(DumpMessageHead) + ONE_DUMP_BACKUP_SIZE)) {
        KERNEL_LOG(KERNEL_ERROR, "remained space is %d, which is not enough for write data %lu",
            ptr->dumpOffset, (padOffset + sizeof(DumpMessageHead) + ONE_DUMP_BACKUP_SIZE));
        *((__gm__ uint32_t*)ptr + BLOCK_INFO_RSV_POS) = DUMP_EXC_FLAG;
        dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
        return false;
    }

    return true;
}
template <typename T>
__aicore__ inline void DumpBlockInfoImpl(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize, uint32_t padOffset)
{
    using PrimType = PrimT<T>;
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved);
    uint32_t position =  static_cast<uint32_t>(AscendC::Hardware::GM);
    T data;

    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * GetDumpBlockIdx());
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_TYPE_POS) = static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_LEN_POS) = padOffset + DUMP_MSG_HEAD_SIZE;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_ADDR_POS) =
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(tensor.GetPhyAddr()));
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DATA_TYPE_POS) = GetDataType<PrimType>();
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DESC_POS) = desc;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_BUFFERID_POS) = 0;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_POSITION_POS) = position;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DUMP_SIZE_POS) = dumpSize;

    UpdateBlockInfo(ptr, sizeof(DumpMessageHead));
}
template <typename T>
__aicore__ inline void DumpAivGMTailImpl(LocalTensor<T>& tmpLocal, uint32_t alignSize, uint64_t tmpAddr,
                                      uint64_t gmAddr, uint32_t padOffset)
{
    using PrimType = PrimT<T>;
    DataCopyParams tailParams = GetDataCopyParamImpl((alignSize + ONE_BLK_SIZE - 1) & (~(ONE_BLK_SIZE - 1)));
    DataCopyGM2UBImpl((__ubuf__ PrimType*)tmpLocal.GetPhyAddr(),
                          (__gm__ PrimType*)(tmpAddr + padOffset - alignSize), tailParams);
    PipeBarrier<PIPE_ALL>();
    DataCopyUB2GMImpl((__gm__ PrimType*)gmAddr, (__ubuf__ PrimType*)tmpLocal.GetPhyAddr(), tailParams);
    PipeBarrier<PIPE_ALL>();
}
template <typename T>
__aicore__ inline void DumpTensorGM2GMEntityImpl(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize)
{
    using PrimType = PrimT<T>;
    uint32_t position =  static_cast<uint32_t>(AscendC::Hardware::GM);
    PrimType data;
    uint32_t offset;
    uint32_t padOffset;
    if (IsSpecialDtype<PrimType>()) {
        // calculate total tensor size , overflow will not happen
        offset = dumpSize * ONE_BYTE_BIT_SIZE / GetSpecialDtypeBitSize<PrimType>();
    } else {
        offset = dumpSize * sizeof(PrimType);
    }
    padOffset = AlignUp(offset, ONE_BLK_SIZE);
    if (!CheckDumpValid(padOffset)) {
        return;
    }
    DumpBlockInfoImpl(tensor, desc, dumpSize, padOffset);
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved);
    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * GetDumpBlockIdx());
    dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    uint64_t gmAddr = ptr->dumpAddr;
    if (g_coreType == AIV) {
        DataCopyParams backupParams = GetDataCopyParamImpl(ONE_DUMP_BACKUP_SIZE); // 1K unit
        LocalTensor<T> tmpLocal;
        uint64_t gmBackAddr = dumpWorkspaceStart + DUMP_UINTSIZE * (GetDumpBlockIdx() + 1) - ONE_DUMP_BACKUP_SIZE;

        // 1、alloc 1k UB 2、 backup static GM addr 3、loop copy 4、recover
        PipeBarrier<PIPE_ALL>();
        // BackUp
        InitTmpTensor(tmpLocal, static_cast<uint8_t>(TPosition::VECIN));
        DataCopyUB2GMImpl((__gm__ PrimType*)(gmBackAddr), (__ubuf__ PrimType*)tmpLocal.GetPhyAddr(), backupParams);
        PipeBarrier<PIPE_ALL>();
        dcci((__gm__ uint64_t*)gmBackAddr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
        uint32_t alignSize = padOffset % ONE_DUMP_BACKUP_SIZE;
        uint64_t tmpAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(tensor.GetPhyAddr()));
        // Loop copy data
        for (int i = 0; i < padOffset / ONE_DUMP_BACKUP_SIZE; i++) {
            DataCopyGM2UBImpl((__ubuf__ PrimType*)tmpLocal.GetPhyAddr(),
                                (__gm__ PrimType*)(tmpAddr + ONE_DUMP_BACKUP_SIZE * i), backupParams);
            PipeBarrier<PIPE_ALL>();
            DataCopyUB2GMImpl((__gm__ PrimType*)gmAddr, (__ubuf__ PrimType*)tmpLocal.GetPhyAddr(), backupParams);
            gmAddr += ONE_DUMP_BACKUP_SIZE;
            PipeBarrier<PIPE_ALL>();
        }
        if (alignSize != 0) {
            DumpAivGMTailImpl(tmpLocal, alignSize, tmpAddr, gmAddr, padOffset);
        }
        // Recovery
        DataCopyGM2UBImpl((__ubuf__ PrimType*)tmpLocal.GetPhyAddr(), (__gm__ PrimType*)gmBackAddr, backupParams);
    } else if (g_coreType == AIC) {
        // global tensor using Scalar write
        for (uint32_t i = 0; i < dumpSize; i++) {
            *((__gm__ PrimType*)gmAddr + i) = *((__gm__ PrimType*)tensor.GetPhyAddr(i));
        }
        dcci((__gm__ uint64_t*)gmAddr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    }
    PipeBarrier<PIPE_ALL>();
    UpdateBlockInfo(ptr, padOffset);
}

template <typename T>
__aicore__ inline void DumpTensorGM2GMImpl(const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        __asc_aicore::asc_dump_gm((__gm__ T*)src.GetPhyAddr(), desc, dumpSize);
    } else {
        DumpTensorGM2GMEntityImpl(src, desc, dumpSize);
    }
    set_ctrl(ctrlValue);
}

__aicore__ inline void WriteTimeStampInfo(uint32_t descId)
{
#ifdef ASCENDC_TIME_STAMP_ON
    __gm__ BlockInfo *blockInfo = GetBlockInfo();
    uint64_t dumpAddr = blockInfo->dumpAddr;
    // // WriteTLHead
    *((__gm__ uint32_t *)dumpAddr) = static_cast<uint32_t>(DumpType::DUMP_TIME_STAMP);
    *((__gm__ uint32_t *)dumpAddr + DUMP_TIME_STAMP_LEN_POS) = DUMP_TIME_STAMP_LEN;
    // write value
    *((__gm__ uint32_t *)dumpAddr + DUMP_TIME_STAMP_ID_POS) = descId;
    *((__gm__ uint64_t *)((__gm__ uint32_t *)dumpAddr + DUMP_TIME_STAMP_CYCLE_POS)) = static_cast<uint64_t>(GetSystemCycle());
    *((__gm__ uint64_t *)((__gm__ uint32_t *)dumpAddr + DUMP_TIME_STAMP_PTR_POS)) = static_cast<uint64_t>(get_pc());
    // update block addr
    *((__gm__ uint64_t *)((__gm__ uint32_t *)blockInfo + BLOCK_INFO_DUMP_ADDR)) = dumpAddr + DUMP_TIME_STAMP_TOTAL_LEN;
    *((__gm__ uint32_t*)blockInfo + BLOCK_INFO_DUMPOFFSET_POS) = blockInfo->dumpOffset - DUMP_TIME_STAMP_TOTAL_LEN;
    dcci((__gm__ uint64_t*)blockInfo, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
#endif
}

__aicore__ inline void DumpTimeStampImpl(uint32_t descId)
{
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        __asc_aicore::asc_time_stamp(descId);
    } else {
        WriteTimeStampInfo(descId);
    }
}

__aicore__ inline void AscendCTimeStamp(uint32_t descId, uint64_t pcPtr = 0)
{
#ifdef ASCENDC_TIME_STAMP_ON  // 打点开关宏
    DumpTimeStampImpl(descId);
#endif
}

__aicore__ inline void InitDump(bool mixFlag, uint32_t gmLen)
{
#if defined(ASCENDC_DUMP) || defined(ASCENDC_ACC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
    g_dumpWorkspaceReserved = GetSysWorkSpacePtr();
    if (g_dumpWorkspaceReserved == nullptr) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "init dump get nullptr system workspace ptr"); });
        return;
    }
    if (reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) < DUMP_WORKSPACE_SIZE) {
        KERNEL_LOG(KERNEL_ERROR, "DumpWorkSpace addr is %lu, which must be greater than %u",
            reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved), DUMP_WORKSPACE_SIZE);
        return;
    }
    g_dumpWorkspaceReserved -= DUMP_WORKSPACE_SIZE;
    InitDumpImpl(mixFlag, gmLen);
#else
    return;
#endif
}
__aicore__ inline void InitDump(bool mixFlag, GM_ADDR dumpStartAddr, uint32_t gmLen)
{
#if defined(ASCENDC_DUMP) || defined(ASCENDC_ACC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
    g_dumpWorkspaceReserved = dumpStartAddr;
    if (g_dumpWorkspaceReserved == nullptr) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "init dump get nullptr system workspace ptr"); });
        return;
    }
    InitDumpImpl(mixFlag, gmLen);
#else
    return;
#endif
}

__aicore__ inline void DumpL1TensorTransferByUB()
{
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved);
    uint8_t core = GetRelevantDumpCubeCoreIdx();
    __gm__ BlockInfo* gmAddr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    if ASCEND_IS_AIC {
        return;
    }

    // only need one vector unit to transfer dump data
    if (GetBlockIdxImpl() % 2 != 0) { // 2: used to judge odd or even block idx
        return;
    }
    EnableL1Dump();
    while (true) {
        if (!IsL1DumpEnable()) {
            break;
        }
        // 监听是否接收到dump数据信号
        if (GetDumpCtrlMsg()->sig == ConstantsInternal::L1_2_UB_DUMP_CTRL_SIGNAL_DUMP) {
            // 等待AIC搬运完成
            wait_intra_block(PIPE_MTE3, ConstantsInternal::DUMP_INTRA_BLOCK_ID);
            // 刷dcci, 更新最新gm内容
            dcci((__gm__ uint64_t*)gmAddr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
            // 重新获取len信息 避免信息老化
            uint32_t copyLen = GetDumpCtrlMsg()->len;
            DataCopyParams repeatParams = GetDataCopyParamImpl(copyLen);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
            auto ubAddr = reinterpret_cast<__ubuf__ uint8_t*>(ConstDefiner::Instance().cpuUB);
#else
            auto ubAddr = reinterpret_cast<__ubuf__ uint8_t*>(get_imm(0));
#endif
            // UB to GM
            PipeBarrier<PIPE_ALL>();
            DataCopyUB2GMImpl((__gm__ uint8_t*)(gmAddr->dumpAddr), ubAddr, repeatParams);
            // update block info
            PipeBarrier<PIPE_ALL>();
            UpdateBlockInfo(gmAddr, copyLen);
            dcci((__gm__ uint64_t*)(gmAddr->dumpAddr), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
            dcci((__gm__ uint64_t*)(gmAddr), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
            // 清除dump标志位，避免aiv误进入下一次循环，卡在wait_intra_block
            SendL12UBDumpCtrlMessage(0, 0);
            PipeBarrier<PIPE_ALL>();
            set_intra_block(PIPE_MTE3, ConstantsInternal::DUMP_INTRA_BLOCK_ID); // notify cube core copy again
        }
    }
}

__aicore__ inline void FinalizeL1TensorDump()
{
    DisableL1Dump();
}

template <typename T>
__aicore__ constexpr inline Internal::DumpTensorDataType GetTensorDataType()
{
    if constexpr (IsSameType<T, bool>::value) {
        return Internal::DumpTensorDataType::ACL_BOOL;
    } else if (IsSameType<T, uint8_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT8;
    } else if (IsSameType<T, int8_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT8;
    } else if (IsSameType<T, int16_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT16;
    } else if (IsSameType<T, uint16_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT16;
    } else if (IsSameType<T, int32_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT32;
    } else if (IsSameType<T, uint32_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT32;
    } else if (IsSameType<T, uint64_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT64;
    } else if (IsSameType<T, int64_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT64;
    } else if (IsSameType<T, float>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT;
    } else if (IsSameType<T, half>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT16;
    } else if (IsSameType<T, bfloat16_t>::value) {
        return Internal::DumpTensorDataType::ACL_BF16;
    } else if (IsSameType<T, hifloat8_t>::value) {
        return Internal::DumpTensorDataType::ACL_HIFLOAT8;
    } else if (IsSameType<T, fp8_e5m2_t>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT8_E5M2;
    } else if (IsSameType<T, fp8_e4m3fn_t>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT8_E4M3FN;
    } else if (IsSameType<T, fp8_e8m0_t>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT8_E8M0;
    } else if (IsSameType<T, fp4x2_e2m1_t>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT4_E2M1;
    } else if (IsSameType<T, fp4x2_e1m2_t>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT4_E1M2;
    } else {
        return Internal::DumpTensorDataType::ACL_MAX;
    }
}

__aicore__ inline void WriteRingBufShapeInfo(const ShapeInfo &shapeInfo)
{
    __gm__ BlockRingBufInfo* blockRingBufInfo = GetBlockRingBufInfo();
    if (blockRingBufInfo == nullptr) {
        return;
    }
    uint32_t tlvLen = sizeof(DumpShapeTlvInfo);
    if (!CheckAndWaitRingBufSpace(blockRingBufInfo, tlvLen)) {
        return;
    }
    __gm__ DumpShapeTlvInfo* shapeTlv =
        reinterpret_cast<__gm__ DumpShapeTlvInfo*>(GetRingBufTlv(blockRingBufInfo));
    shapeTlv->type = static_cast<uint32_t>(DumpType::DUMP_SHAPE);
    shapeTlv->length = tlvLen - sizeof(uint32_t[2]);
    shapeTlv->dim = shapeInfo.shapeDim;
    for (uint32_t i = 0; i < K_MAX_SHAPE_DIM; ++i) {
        shapeTlv->shape[i] = shapeInfo.shape[i];
    }
    shapeTlv->resv = static_cast<uint32_t>(0U);

    dcci((__gm__ uint64_t*)shapeTlv, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);

    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    UpdateWriteInfo(writeInfo, tlvLen);
}

}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
