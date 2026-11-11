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
#pragma message("impl/basic_api/dav_m300/kernel_operator_dump_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H

#include "kernel_tpipe_impl.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_data_copy_impl.h"
#include "kernel_operator_sys_var_intf.h"
#include "kernel_pop_stack_buffer.h"
#include "kernel_struct_fixpipe.h"
#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_fixpipe_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_fixpipe_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_fixpipe_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_fixpipe_impl.h"
#endif

namespace AscendC {
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_dumpWorkspaceReserved;

template <typename T> __aicore__ inline uint32_t GetDataType(T data)
{
    uint32_t type;

    if (IsSameType<T, bool>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_BOOL);
    } else if (IsSameType<T, uint8_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT8);
    } else if (IsSameType<T, int8_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT8);
    } else if (IsSameType<T, int16_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT16);
    } else if (IsSameType<T, uint16_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT16);
    } else if (IsSameType<T, int32_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT32);
    } else if (IsSameType<T, uint32_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT32);
    } else if (IsSameType<T, uint64_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT64);
    } else if (IsSameType<T, int64_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT64);
    } else if (IsSameType<T, float>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT);
    } else if (IsSameType<T, half>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT16);
    } else {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_MAX);
    }
}

__aicore__ inline uint8_t GetDumpBlockIdx()
{
    return GetBlockIdxImpl();
}

__aicore__ inline void InitDumpImpl(bool mixFlag, uint32_t gmLen)
{
    (void)mixFlag;
    uint32_t totalBlockNum;
    uint64_t firstTimeStamp = static_cast<uint64_t>(GetSystemCycle());
    if (g_dumpWorkspaceReserved == nullptr) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "init dump get nullptr system workspace ptr"); });
        return;
    }
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
    totalBlockNum = get_block_num();
    uint32_t blockDumpSize = DUMP_UINTSIZE; // DUMP_UINTSIZE is 1M

    uint32_t numBlocks = GetDumpBlockIdx();
    if (numBlocks >= DUMP_CORE_COUNT) {
        return;
    }
#ifdef ASCENDC_TIME_STAMP_ON
    uint32_t blkInfoLen = sizeof(BlockInfo) + sizeof(DumpMeta) + sizeof(DumpTimeStamp);
#else
    uint32_t blkInfoLen = sizeof(BlockInfo) + sizeof(DumpMeta);
#endif
    uint64_t blockInfoStart = dumpWorkspaceStart + numBlocks * DUMP_UINTSIZE;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_LEN_POS) = blockDumpSize;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_CORE_POS) = numBlocks;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_BLOCKNUM_POS) = totalBlockNum;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_DUMPOFFSET_POS) = blockDumpSize - blkInfoLen;
    *((__gm__ uint32_t*)blockInfoStart + BLOCK_INFO_MAGIC_POS) = 0x5aa5bccd;
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
        static_cast<uint8_t>(1);
    *((__gm__ uint32_t*)blockInfoStart + DUMP_META_RSV_POS) = 0;
#ifdef ASCENDC_TIME_STAMP_ON
    blockInfoStart = blockInfoStart + sizeof(DumpMeta);
    // WriteTLHead
    *((__gm__ uint32_t *)blockInfoStart) = static_cast<uint32_t>(DumpType::DUMP_TIME_STAMP);
    *((__gm__ uint32_t *)blockInfoStart + DUMP_TIME_STAMP_LEN_POS) = DUMP_TIME_STAMP_LEN;
    // write value
    *((__gm__ uint32_t *)blockInfoStart + DUMP_TIME_STAMP_ID_POS) = 0;
    *((__gm__ uint64_t *)((__gm__ uint32_t *)blockInfoStart + DUMP_TIME_STAMP_CYCLE_POS)) = firstTimeStamp;
    *((__gm__ uint64_t *)((__gm__ uint32_t *)blockInfoStart + DUMP_TIME_STAMP_PTR_POS)) = 0;
#endif
    dcci((__gm__ uint64_t*)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}
__aicore__ inline DataCopyParams GetDataCopyParamImpl(uint32_t offset)
{
    DataCopyParams repeatParams;
    repeatParams.blockCount = 1;
    repeatParams.blockLen = offset / ONE_BLK_SIZE;
    repeatParams.srcStride = 0;
    repeatParams.dstStride = 0;
    return repeatParams;
}

template <typename T>
__aicore__ inline void DumpTensorL0C2GMImpl(const LocalTensor<T>& src, __gm__ BlockInfo* ptr, uint32_t dumpSize)
{
    // L0C to GM
    uint16_t align = (dumpSize % DEFAULT_BLOCK_SIZE == 0) ? 0 : 1;
    uint16_t countBlks = align + dumpSize / DEFAULT_BLOCK_SIZE;

    uint16_t burstLen = static_cast<uint16_t>(SRC_BURST_LEN_SIZE_ELE * SRC_BURST_LEN_SIZE_ELE
        * sizeof(float) / ONE_BLK_SIZE);
    uint16_t n = countBlks * BLOCK_CUBE;
    uint16_t m = (burstLen * ONE_BLK_SIZE / B32_BYTE_SIZE) / BLOCK_CUBE;

    copy_matrix_cc_to_gm((__gm__ float *)(ptr->dumpAddr), (__cc__ float *)(src.GetPhyAddr()),
        0, n, m, 0, 0, 0,
        0, 0, static_cast<uint64_t>(QuantMode_t::NoQuant),
        static_cast<uint8_t>(false), false, false,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0, false, false, true,
        false, false, false);
}

template <typename T>
__aicore__ inline uint32_t CheckValidPosition(const LocalTensor<T>& src)
{
    // set the head struct value
    uint32_t position = 0;
    if ((Hardware)GetPhyType((TPosition)src.GetPosition()) == Hardware::UB) {
        position = static_cast<uint32_t>(AscendC::Hardware::UB);
        return position;
    } else if ((Hardware)GetPhyType((TPosition)src.GetPosition()) == Hardware::L1) {
        position = static_cast<uint32_t>(AscendC::Hardware::L1);
        return position;
    } else if ((Hardware)GetPhyType((TPosition)src.GetPosition()) == Hardware::L0C) {
        position = static_cast<uint32_t>(AscendC::Hardware::L0C);
        return position;
    } else {
        return false;
    }
}

__aicore__ inline void DumpShapeImpl(const ShapeInfo &shapeInfo)
{
    uint8_t core = GetDumpBlockIdx();
    if (core >= DUMP_CORE_COUNT) {
        return;
    }
    uint32_t valueSize = sizeof(DumpShapeMessageHead);
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    uint32_t tlvSize = valueSize + DUMP_SHAPE_MESSAGE_TL_LEN;
    if (ptr->dumpOffset < tlvSize) {
        ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "dumpOffset is %d, which must less than remained space %d", ptr->dumpOffset,
            valueSize); });
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
    ptr->dumpAddr += tlvSize;
    ptr->dumpOffset -= tlvSize;
    dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

template <typename T>
__aicore__ inline void DumpTensorLocal2GMEntityImpl(const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    uint32_t position = CheckValidPosition(src);
    // set the head struct value
    if (position == 0) {
        ASCENDC_ASSERT((false),
                   { KERNEL_LOG(KERNEL_ERROR, "dump tensor only support dump tensor from local to gm"); });
        return;
    }

    T data;
    uint8_t core = GetDumpBlockIdx();
    if (core >= DUMP_CORE_COUNT) {
        return;
    }
    uint32_t offset = dumpSize * sizeof(T);
    uint32_t padOffset = AlignUp(offset, ONE_BLK_SIZE);
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;

    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    if (ptr->dumpOffset < (padOffset + sizeof(DumpMessageHead))) {
        ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "dumpOffset is %d, which must less than remained space %d", ptr->dumpOffset,
            (padOffset + sizeof(DumpMessageHead))); });
        *((__gm__ uint32_t*)ptr + BLOCK_INFO_RSV_POS) = DUMP_EXC_FLAG;
        dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
        return;
    }

    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_TYPE_POS) = static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_LEN_POS) = padOffset + DUMP_MSG_HEAD_SIZE;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_ADDR_POS) =
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr()));
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DATA_TYPE_POS) = GetDataType(data);
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DESC_POS) = desc;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_BUFFERID_POS) = 0;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_POSITION_POS) = position;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DUMP_SIZE_POS) = dumpSize;
    // update block info
    ptr->dumpAddr += sizeof(DumpMessageHead);
    ptr->dumpOffset -= sizeof(DumpMessageHead);
    dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    DataCopyParams repeatParams = GetDataCopyParamImpl(padOffset);
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());

    PipeBarrier<PIPE_ALL>();
    if (srcHWPos == Hardware::UB) {
        DataCopyUB2GMImpl((__gm__ T*)(ptr->dumpAddr), (__ubuf__ T*)src.GetPhyAddr(), repeatParams); // UB to GM
    } else if (srcHWPos == Hardware::L1) {
        DataCopyL12GMImpl((__gm__ T*)(ptr->dumpAddr), (__cbuf__ T*)src.GetPhyAddr(), repeatParams); // L1 to GM
    } else if (srcHWPos == Hardware::L0C) {
        // L0C to GM
        if ASCEND_IS_NOT_AIC {
            return;
        }
        DumpTensorL0C2GMImpl(src, ptr, dumpSize);
    }
    PipeBarrier<PIPE_ALL>();
    ptr->dumpOffset -= padOffset;
    ptr->dumpAddr += padOffset;
    dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

template <typename T>
__aicore__ inline void DumpTensorLocal2GMImpl(const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    DumpTensorLocal2GMEntityImpl(src, desc, dumpSize);
    set_ctrl(ctrlValue);
}

__aicore__ inline uint32_t GetLoopCount(uint32_t offset)
{
    uint32_t loopCount = 0;
    if (offset % ONE_DUMP_BACKUP_SIZE != 0) {
        loopCount = 1 + offset / ONE_DUMP_BACKUP_SIZE;
    } else {
        loopCount = offset / ONE_DUMP_BACKUP_SIZE;
    }
    return loopCount;
}

template <typename T>
__aicore__ inline void InitTmpTensor(LocalTensor<T>& tmp, uint8_t quePos)
{
    TBuffAddr tbufTmpLocal;
    tbufTmpLocal.logicPos = quePos;
    tmp.SetAddr(tbufTmpLocal);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    tmp.address_.absAddr = reinterpret_cast<uint8_t *>(ConstDefiner::Instance().cpuUB);
#else
    tmp.address_.bufferAddr = get_imm(0);
#endif
    tmp.address_.dataLen = ONE_DUMP_BACKUP_SIZE;
}
__aicore__ inline bool CheckDumpValid(uint32_t offset)
{
    if (offset % ONE_BLK_SIZE != 0) {
        KERNEL_LOG(KERNEL_ERROR, "dump size is %d, which must be 32B aligned", offset);
        return false;
    }
    uint8_t core = GetDumpBlockIdx();
    if (core >= DUMP_CORE_COUNT) {
        return false;
    }
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
    if (reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) < DUMP_WORKSPACE_SIZE) {
        KERNEL_LOG(KERNEL_ERROR, "DumpWorkSpace addr is %lu, which must be larger than 75M", reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved));
        return false;
    }
    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    if (ptr->dumpOffset < (offset + sizeof(DumpMessageHead) + ONE_DUMP_BACKUP_SIZE)) {
        KERNEL_LOG(KERNEL_ERROR, "dumpOffset is %d, which must less than remained space %ld", ptr->dumpOffset,
            (offset + sizeof(DumpMessageHead) + ONE_DUMP_BACKUP_SIZE));
        *((__gm__ uint32_t*)ptr + BLOCK_INFO_RSV_POS) = DUMP_EXC_FLAG;
        dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
        return false;
    }

    return true;
}
template <typename T>
__aicore__ inline void DumpBlockInfoImpl(const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize, uint32_t padOffset)
{
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
    uint32_t position =  static_cast<uint32_t>(AscendC::Hardware::GM);
    T data;

    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * GetDumpBlockIdx());
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_TYPE_POS) = static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_LEN_POS) = padOffset + DUMP_MSG_HEAD_SIZE;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_ADDR_POS) =
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr()));
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DATA_TYPE_POS) = GetDataType(data);
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DESC_POS) = desc;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_BUFFERID_POS) = 0;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_POSITION_POS) = position;
    *((__gm__ uint32_t*)ptr->dumpAddr + DUMP_MESSAGE_HEAD_DUMP_SIZE_POS) = dumpSize;

    ptr->dumpAddr += sizeof(DumpMessageHead);
    ptr->dumpOffset -= sizeof(DumpMessageHead);
    dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}
template <typename T>
__aicore__ inline void DumpGMTailImpl(LocalTensor<T>& tmp, uint32_t alignSize, uint64_t tmpAddr,
                                      uint64_t gmAddr, uint32_t offset)
{
    DataCopyParams tailParams = GetDataCopyParamImpl((alignSize + ONE_BLK_SIZE - 1) & (~(ONE_BLK_SIZE - 1)));
    DataCopyGM2L1Impl((__cbuf__ T*)tmp.GetPhyAddr(),
                          (__gm__ T*)(tmpAddr + offset - alignSize), tailParams);
    PipeBarrier<PIPE_ALL>();
    DataCopyL12GMImpl((__gm__ T*)gmAddr, (__cbuf__ T*)tmp.GetPhyAddr(), tailParams);
    PipeBarrier<PIPE_ALL>();
}
template <typename T>
__aicore__ inline void DumpTensorGM2GMEntityImpl(const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    uint32_t position =  static_cast<uint32_t>(AscendC::Hardware::GM);
    T data;
    uint32_t offset = dumpSize * sizeof(T);
    uint32_t padOffset = AlignUp(offset, ONE_BLK_SIZE);
    if (!CheckDumpValid(padOffset)) {
        return;
    }
    DumpBlockInfoImpl(src, desc, dumpSize, padOffset);
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
    __gm__ BlockInfo* ptr = (__gm__ BlockInfo*)(dumpWorkspaceStart + DUMP_UINTSIZE * GetDumpBlockIdx());
    DataCopyParams backupParams = GetDataCopyParamImpl(ONE_DUMP_BACKUP_SIZE); // 1K unit
    LocalTensor<T> tmp;
    uint64_t gmBackAddr = dumpWorkspaceStart + DUMP_UINTSIZE * (GetDumpBlockIdx() + 1) - ONE_DUMP_BACKUP_SIZE;

    // 1、alloc 1k UB 2、 backup static GM addr 3、loop copy 4、recover
    PipeBarrier<PIPE_ALL>();
    // BACKUP
    InitTmpTensor(tmp, static_cast<uint8_t>(TPosition::A1));
    DataCopyL12GMImpl((__gm__ T*)(gmBackAddr), (__cbuf__ T*)tmp.GetPhyAddr(), backupParams);
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t*)gmBackAddr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);

    uint32_t alignSize = padOffset % ONE_DUMP_BACKUP_SIZE;
    uint64_t tmpAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr()));
    uint64_t gmAddr = ptr->dumpAddr;
    for (int i = 0; i < padOffset / ONE_DUMP_BACKUP_SIZE; i++) {
        // LOOP COPY
        DataCopyGM2L1Impl((__cbuf__ T*)tmp.GetPhyAddr(),
                         (__gm__ T*)(tmpAddr + ONE_DUMP_BACKUP_SIZE * i), backupParams);
        PipeBarrier<PIPE_ALL>();
        DataCopyL12GMImpl((__gm__ T*)gmAddr, (__cbuf__ T*)tmp.GetPhyAddr(), backupParams);
        gmAddr += ONE_DUMP_BACKUP_SIZE;
        PipeBarrier<PIPE_ALL>();
    }
    if (alignSize != 0) {
        DumpGMTailImpl(tmp, alignSize, tmpAddr, gmAddr, padOffset);
    }
    // RECOVER
    DataCopyGM2L1Impl((__cbuf__ T*)tmp.GetPhyAddr(), (__gm__ T*)gmBackAddr, backupParams);
    PipeBarrier<PIPE_ALL>();
    ptr->dumpOffset -= padOffset;
    ptr->dumpAddr += padOffset;
    dcci((__gm__ uint64_t*)ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

template <typename T>
__aicore__ inline void DumpTensorGM2GMImpl(const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    DumpTensorGM2GMEntityImpl(src, desc, dumpSize);
    set_ctrl(ctrlValue);
}

__aicore__ inline uint32_t GetArgsNum()
{
    return 0;
}

template <typename T, typename... Args>
__aicore__ inline uint32_t GetArgsNum(T scalar, Args... args)
{
    return 1 + GetArgsNum(args...);
}

__aicore__ inline uint32_t GetStringLength(__gm__ const char* s)
{
    uint32_t i = 0;
    while (*(s + i) != '\0') {
        i++;
    }
    return i + 1;
}

__aicore__ inline uint32_t GetArgsSize()
{
    return 0;
}

template <typename... Args>
__aicore__ inline uint32_t GetArgsSize(Args&&... args);

template <typename... Args>
__aicore__ inline uint32_t GetArgsSizeImpl(__gm__ const char* s, Args&&... args)
{
    uint32_t strLen = GetStringLength(s);
    uint32_t strParamSize = ONE_PARAM_SIZE + strLen;
    return strParamSize + GetArgsSize(args...);
}

template <typename T, typename... Args>
__aicore__ inline uint32_t GetArgsSizeImpl(T scalar, Args&&... args)
{
    return ONE_PARAM_SIZE + GetArgsSize(args...);
}

template <typename... Args>
__aicore__ inline uint32_t GetArgsSize(Args&&... args)
{
    return GetArgsSizeImpl(args...);
}

template <typename... Args>
__aicore__ inline uint32_t GetParamSize(__gm__ const char* fmt, Args&&... args)
{
    uint32_t fmtSize = GetStringLength(fmt);
    uint32_t argsSize = GetArgsSize(args...);
    return fmtSize + argsSize + ONE_PARAM_SIZE;
}

__aicore__ __gm__ inline BlockInfo *GetBlockInfo()
{
    uint8_t core = GetDumpBlockIdx();
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
    __gm__ BlockInfo *blockInfo = (__gm__ BlockInfo *)(dumpWorkspaceStart +  DUMP_UINTSIZE * core);
    return blockInfo;
}

__aicore__ inline void WriteString(__gm__ uint8_t* paramAddr, uint32_t paramIdx, __gm__ const char* s, uint32_t& offset)
{
    __gm__ uint64_t *stringAddr = reinterpret_cast<__gm__ uint64_t *>(paramAddr) + paramIdx;
    __gm__ uint64_t *dstStrAddr = reinterpret_cast<__gm__ uint64_t *>(paramAddr + offset);

    // write string value offset
    *((__gm__ uint64_t *)stringAddr) = static_cast<uint64_t>(offset - ONE_PARAM_SIZE * paramIdx);
    dcci((__gm__ uint64_t*)stringAddr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);

    // write string content
    __gm__ char *d = (__gm__ char *)(dstStrAddr);
    uint32_t strLen = GetStringLength(s);

    for (uint32_t i = 0; i < strLen; i++) {
        *(d + i) = *(s + i);
        dcci((__gm__ uint64_t*)d, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    }
    offset += strLen;
}

template <typename T>
__aicore__ inline void WriteScalar(__gm__ uint8_t* paramAddr, uint32_t paramIdx, T scalar)
{
    __gm__ uint64_t *scalarAddr = (__gm__ uint64_t *)paramAddr + paramIdx;
    *scalarAddr = 0;
    static_assert(!SupportType<T, double>(), "printf unsupport double type");

    if constexpr (SupportType<T, half, float>()) {
        *((__gm__ float *)scalarAddr) = static_cast<float>(scalar);
    } else if constexpr(SupportType<T, bfloat16_t>()) {
        *((__gm__ float *)scalarAddr) = ToFloat(scalar);
    } else if constexpr (std::is_signed<T>::value) {
        *((__gm__ int64_t *)scalarAddr) = static_cast<int64_t>(scalar);
    } else if constexpr(std::is_unsigned<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = static_cast<uint64_t>(scalar);
    } else if constexpr(std::is_pointer<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = (uintptr_t)scalar;
    } else if constexpr(std::is_enum<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = static_cast<uint64_t>(scalar);
    }

    dcci((__gm__ uint64_t*)scalarAddr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

__aicore__ inline void SetParam(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset)
{
    return;
}

template <typename... Args>
__aicore__ inline void SetParam(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, Args&&... args);

template <typename... Args>
__aicore__ inline void SetParamImpl(__gm__ uint8_t *paramAddr, uint32_t paramIdx, uint32_t &offset,
                                    __gm__ const char *s, Args&&... args)
{
    WriteString(paramAddr, paramIdx, s, offset);
    SetParam(paramAddr, paramIdx + 1, offset, args...);
}

template <typename T, typename... Args>
__aicore__ inline void SetParamImpl(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, T scalar,
                                    Args&&... args)
{
    WriteScalar(paramAddr, paramIdx, scalar);
    SetParam(paramAddr, paramIdx + 1, offset, args...);
}

template <typename... Args>
__aicore__ inline void SetParam(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, Args&&... args)
{
    SetParamImpl(paramAddr, paramIdx, offset, args...);
}

__aicore__ inline void WriteTLHead(DumpType printType, __gm__ uint8_t *tlv, uint32_t valueSize)
{
    *((__gm__ uint32_t *)tlv) = static_cast<uint32_t>(printType);
    *((__gm__ uint32_t *)tlv + 1) = valueSize;
    dcci((__gm__ uint64_t*)tlv, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}
__aicore__ inline void UpdateBlockInfo(uint32_t tlvSize)
{
    __gm__ BlockInfo *blockInfo = GetBlockInfo();
    uint32_t remainSize = blockInfo->dumpOffset;
    uint64_t lastDumpAddr = blockInfo->dumpAddr;

    __gm__ uint8_t *blockInfoStart = (__gm__ uint8_t *)blockInfo;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_DUMPOFFSET_POS) = remainSize - tlvSize;
    *((__gm__ uint64_t *)((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_DUMP_ADDR)) = lastDumpAddr + tlvSize;
    dcci((__gm__ uint64_t*)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

template <class... Args>
__aicore__ inline void PrintfEntityImpl(DumpType printType, __gm__ const char* fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
    uint8_t blockIdx = GetDumpBlockIdx();
    if (blockIdx >= DUMP_CORE_COUNT) {
        return;
    }
    __gm__ BlockInfo *blockInfo = GetBlockInfo();
    uint32_t remainSize = blockInfo->dumpOffset;
    uint64_t dumpAddr = blockInfo->dumpAddr;

    uint32_t paramSize = GetParamSize(fmt, args...);
    uint32_t paramNum = GetArgsNum(args...) + 1;
    paramSize = (paramSize + ONE_PARAM_SIZE - 1) & (~(ONE_PARAM_SIZE - 1));

    uint32_t tlvSize = paramSize + ONE_PARAM_SIZE;
    if (tlvSize > remainSize) {
        __gm__ uint8_t *blockInfoStart = (__gm__ uint8_t *)blockInfo;
        *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_RSV_POS) = DUMP_EXC_FLAG;
        dcci((__gm__ uint64_t*)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
        return;
    }

    __gm__ uint8_t *tlvAddr = (__gm__ uint8_t *)dumpAddr;
    WriteTLHead(printType, tlvAddr, paramSize);
    __gm__ uint8_t *paramAddr = tlvAddr + ONE_PARAM_SIZE;
    uint32_t offset = paramNum * ONE_PARAM_SIZE;
    WriteString(paramAddr, 0, fmt, offset);
    uint32_t paramIdx = 1;
    SetParam(paramAddr, paramIdx, offset, args...);

    // update next print addr
    UpdateBlockInfo(tlvSize);
#endif
}

template <class... Args>
__aicore__ inline void PrintfImpl(DumpType printType, __gm__ const char* fmt, Args&&... args)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    PrintfEntityImpl(printType, fmt, args...);
    set_ctrl(ctrlValue);
}

__aicore__ inline void DumpTimeStampImpl(uint32_t descId)
{
#ifdef ASCENDC_TIME_STAMP_ON
    __gm__ BlockInfo *blockInfo = GetBlockInfo();
    uint64_t dumpAddr = blockInfo->dumpAddr;
    // WriteTLHead
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
    InitDumpImpl(mixFlag, gmLen);
#else
    return;
#endif
}
__aicore__ inline void InitDump(bool mixFlag, GM_ADDR dumpStartAddr, uint32_t gmLen)
{
#if defined(ASCENDC_DUMP) || defined(ASCENDC_ACC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
    g_dumpWorkspaceReserved = dumpStartAddr + DUMP_WORKSPACE_SIZE;
    InitDumpImpl(mixFlag, gmLen);
#else
    return;
#endif
}
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
