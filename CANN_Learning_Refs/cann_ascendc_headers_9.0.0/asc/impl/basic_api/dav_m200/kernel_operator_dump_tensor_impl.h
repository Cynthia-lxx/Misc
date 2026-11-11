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
#pragma message("impl/basic_api/dav_m200/kernel_operator_dump_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H

#include "kernel_utils.h"
#include "kernel_tensor.h"
#include "kernel_operator_common_impl.h"
#include "kernel_tpipe_impl.h"
#include "dav_m200/kernel_operator_data_copy_impl.h"
#include "kernel_pop_stack_buffer.h"
#include "impl/utils/debug/asc_aicore_printf_impl.h"

namespace AscendC {
/* **************************************************************************************************
 * DumpTensorImpl                                             *
 * ************************************************************************************************* */
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_dumpWorkspaceReserved;

template <typename T>
__aicore__ constexpr inline Internal::DumpTensorDataType GetTensorDataType();

template <typename T> __aicore__ inline uint32_t GetDataType(T data)
{
    return static_cast<uint32_t>(GetTensorDataType<T>());
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

__aicore__ __gm__ inline BlockInfo *GetBlockInfo()
{
#ifdef __ENABLE_VECTOR_CORE__
#if defined(__DAV_M200_VEC__)
    uint32_t core = GetBlockIdxImpl() - get_data_main_base();
#else
    uint32_t core = GetBlockIdxImpl() - get_data_main_base() + AIV_CORE_NUM;
#endif
#else
    uint8_t core = GetBlockIdxImpl() + AIV_CORE_NUM;
#endif
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
    __gm__ BlockInfo *blockInfo = (__gm__ BlockInfo *)(dumpWorkspaceStart +  DUMP_UINTSIZE * core);
    return blockInfo;
}

__aicore__ inline void UpdateBlockInfo(uint32_t tlvSize, uint32_t excSize)
{
    __gm__ BlockInfo *blockInfo = GetBlockInfo();
    __gm__ uint8_t *blockInfoStart = (__gm__ uint8_t *)blockInfo;
    dcci((__gm__ uint64_t *)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE);
    uint32_t remainSize = blockInfo->dumpOffset;
    uint64_t lastDumpAddr = blockInfo->dumpAddr;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_LEN_POS) = blockInfo->len;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_CORE_POS) = blockInfo->core;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_BLOCKNUM_POS) = blockInfo->blockNum;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_DUMPOFFSET_POS) = remainSize - tlvSize;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_MAGIC_POS) = 0x5aa5bccd;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_RSV_POS) = excSize;
    *((__gm__ uint64_t *)((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_DUMP_ADDR)) = lastDumpAddr + tlvSize;

    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t *)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE);
}

template <typename T>
__aicore__ inline void InitTmpTensor(LocalTensor<T> &tmp, uint8_t quePos)
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

__aicore__ inline uint32_t GetdataCopyCount(uint32_t dataLen)
{
    if (dataLen % ONE_DUMP_BACKUP_SIZE == 0) {
        return dataLen / ONE_DUMP_BACKUP_SIZE;
    }
    return dataLen / ONE_DUMP_BACKUP_SIZE + 1;
}

__aicore__ inline int64_t GetBlockNum();
__aicore__ inline void InitDumpImpl(bool mixFlag, uint32_t gmLen)
{
    if (g_dumpWorkspaceReserved == nullptr) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "init dump get nullptr system workspace ptr"); });
        return;
    }
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
    uint32_t totalBlockNum = GetBlockNum();
    uint32_t blockDumpSize = DUMP_UINTSIZE; // DUMP_UINTSIZE is 1M
#ifdef __ENABLE_VECTOR_CORE__
#if defined(__DAV_M200_VEC__)
    uint32_t numBlocks = GetBlockIdxImpl() - get_data_main_base();
#else
    uint32_t numBlocks = GetBlockIdxImpl() - get_data_main_base() + AIV_CORE_NUM;
#endif
#else
    uint32_t numBlocks = GetBlockIdxImpl() + AIV_CORE_NUM;
#endif
    if (numBlocks >= DUMP_CORE_COUNT) {
        return;
    }
    uint32_t blkInfoLen = sizeof(BlockInfo);
    uint64_t blockInfoStart = dumpWorkspaceStart + numBlocks * DUMP_UINTSIZE;
    PipeBarrier<PIPE_ALL>();
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_LEN_POS) = blockDumpSize;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_CORE_POS) = numBlocks;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_BLOCKNUM_POS) = totalBlockNum;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_DUMPOFFSET_POS) = blockDumpSize - blkInfoLen - sizeof(DumpMeta);
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_MAGIC_POS) = 0x5aa5bccd;
    *((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_RSV_POS) = 0;
    *((__gm__ uint64_t *)((__gm__ uint32_t *)blockInfoStart + BLOCK_INFO_DUMP_ADDR)) = blockInfoStart + blkInfoLen + sizeof(DumpMeta);
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t *)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE);
    // write DUM_META message
    uint32_t coreType = 1;
#if defined(__DAV_M200_VEC__)
    coreType = 2;
#endif
    blockInfoStart = blockInfoStart + sizeof(BlockInfo);
    *(__gm__ uint32_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_TYPE_POS) =
        static_cast<uint32_t>(DumpType::DUMP_META);
    *(__gm__ uint32_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_LEN_POS) = 8;
    *(__gm__ uint16_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_BLOCK_DIM_POS) =
        static_cast<uint16_t>(GetBlockNum());
    *(__gm__ uint8_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_CORE_TYPE_POS) =
        static_cast<uint8_t>(coreType);
    *(__gm__ uint8_t*)((__gm__ uint8_t*)blockInfoStart + DUMP_META_TASK_RATION) =
        static_cast<uint8_t>(GetTaskRationImpl());
    *((__gm__ uint32_t*)blockInfoStart + DUMP_META_RSV_POS) = 0;
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t *)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE);
}

template <typename T>
__aicore__ inline uint32_t CheckValidPosition(const LocalTensor<T> &cachedData)
{
    uint32_t position = 0;
    if ((Hardware)GetPhyType((TPosition)cachedData.GetPosition()) == Hardware::UB) {
        position = static_cast<uint32_t>(AscendC::Hardware::UB);
        return position;
    } else if ((Hardware)GetPhyType((TPosition)cachedData.GetPosition()) == Hardware::L1) {
        position = static_cast<uint32_t>(AscendC::Hardware::L1);
        return position;
    } else if ((Hardware)GetPhyType((TPosition)cachedData.GetPosition()) == Hardware::L0C) {
        position = static_cast<uint32_t>(AscendC::Hardware::L0C);
        return position;
    }
    return position;
}

__aicore__ inline __gm__ BlockInfo *GetCurCoreHeadPtr()
{
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
#ifdef __ENABLE_VECTOR_CORE__
#if defined(__DAV_M200_VEC__)
    uint32_t core = GetBlockIdxImpl() - get_data_main_base();
#else
    uint32_t core = GetBlockIdxImpl() - get_data_main_base() + AIV_CORE_NUM;
#endif
#else
    uint8_t core = GetBlockIdxImpl() + AIV_CORE_NUM;
#endif
    __gm__ BlockInfo *coreHeadPtr = (__gm__ BlockInfo *)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    return coreHeadPtr;
}

__aicore__ inline void WriteHeadMsg(DumpMessageHead &dumpMsg)
{
    __gm__ BlockInfo *coreHeadPtr = GetCurCoreHeadPtr();
    *((__gm__ uint32_t *)coreHeadPtr->dumpAddr + DUMP_MESSAGE_HEAD_TYPE_POS) = dumpMsg.type; // static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    *((__gm__ uint32_t *)coreHeadPtr->dumpAddr + DUMP_MESSAGE_HEAD_LEN_POS) = dumpMsg.length; // DUMP_MSG_HEAD_SIZE + dumpSize * sizeof(T);
    *((__gm__ uint32_t *)coreHeadPtr->dumpAddr + DUMP_MESSAGE_HEAD_ADDR_POS) = dumpMsg.addr; // static_cast<uint32_t>(tensorAddr);
    *((__gm__ uint32_t *)coreHeadPtr->dumpAddr + DUMP_MESSAGE_HEAD_DATA_TYPE_POS) = dumpMsg.dataType; // GetDataType(data);
    *((__gm__ uint32_t *)coreHeadPtr->dumpAddr + DUMP_MESSAGE_HEAD_DESC_POS) = dumpMsg.desc;
    *((__gm__ uint32_t *)coreHeadPtr->dumpAddr + DUMP_MESSAGE_HEAD_BUFFERID_POS) = dumpMsg.bufferId;
    *((__gm__ uint32_t *)coreHeadPtr->dumpAddr + DUMP_MESSAGE_HEAD_POSITION_POS) = dumpMsg.position;
    *((__gm__ uint32_t *)coreHeadPtr->dumpAddr + DUMP_MESSAGE_HEAD_DUMP_SIZE_POS) = dumpMsg.dumpSize;
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t *)coreHeadPtr->dumpAddr, cache_line_t::ENTIRE_DATA_CACHE);
    UpdateBlockInfo(sizeof(DumpMessageHead), 0);
    return;
}

template <typename T>
__aicore__ inline void TensorDataLoopCopy(LocalTensor<uint8_t> &tmp,
                                          const LocalTensor<T> &src,
                                          uint32_t copyLen)
{
    uint32_t loopCount = GetdataCopyCount(copyLen);
    DataCopyParams backupParams = GetDataCopyParamImpl(ONE_DUMP_BACKUP_SIZE);
    __gm__ BlockInfo *coreHeadPtr = GetCurCoreHeadPtr();
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    for (uint32_t i = 0; i < loopCount; i++) {
        if (srcHWPos == Hardware::L1) {
            PipeBarrier<PIPE_ALL>();
            DataCopyL12UBImpl((__ubuf__ uint8_t *)tmp.GetPhyAddr(),
                              (__cbuf__ uint8_t *)src.GetPhyAddr() + i * ONE_DUMP_BACKUP_SIZE,
                              backupParams); // L1 to UB
            PipeBarrier<PIPE_ALL>();
            DataCopyUB2GMImpl((__gm__ uint8_t *)(coreHeadPtr->dumpAddr + i * ONE_DUMP_BACKUP_SIZE),
                              (__ubuf__ uint8_t *)tmp.GetPhyAddr(),
                              backupParams); // UB to GM
            PipeBarrier<PIPE_ALL>();
        } else if (srcHWPos == Hardware::L0C) {
            DataCopyEnhancedParams enhancedParams;
            enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX;
            DataCopyParams backupParamsL0C;
            backupParamsL0C.blockCount = 1;
            backupParamsL0C.blockLen = 1;
            PipeBarrier<PIPE_ALL>();
            DataCopyL0C2UBImpl((__ubuf__ uint8_t *)tmp.GetPhyAddr(),
                               (__cc__ int32_t *)((__cc__ uint8_t *)src.GetPhyAddr() + i * ONE_DUMP_BACKUP_SIZE),
                               backupParamsL0C,
                               enhancedParams); // L0C to UB
            PipeBarrier<PIPE_ALL>();
            DataCopyUB2GMImpl((__gm__ uint8_t *)(coreHeadPtr->dumpAddr + i * ONE_DUMP_BACKUP_SIZE),
                              (__ubuf__ uint8_t *)tmp.GetPhyAddr(),
                              backupParams); // UB to GM
            PipeBarrier<PIPE_ALL>();
        }
    }
    dcci((__gm__ uint64_t *)coreHeadPtr->dumpAddr, cache_line_t::ENTIRE_DATA_CACHE);
}
template <typename T>
__aicore__ inline void TensorDataCopy(const LocalTensor<T> &src, uint32_t copyLen)
{
    __gm__ BlockInfo *coreHeadPtr = GetCurCoreHeadPtr();
    DataCopyParams repeatParams = GetDataCopyParamImpl(copyLen);
    PipeBarrier<PIPE_ALL>();
    DataCopyUB2GMImpl((__gm__ T *)(coreHeadPtr->dumpAddr), (__ubuf__ T *)src.GetPhyAddr(), repeatParams); // UB to GM
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t *)coreHeadPtr->dumpAddr, cache_line_t::ENTIRE_DATA_CACHE);
}

template <typename T>
__aicore__ inline void TensorDataLoopCopy(LocalTensor<uint8_t> &tmp,
                                          const GlobalTensor<T> &src,
                                          uint32_t copyLen)

{
    dcci((__gm__ uint64_t *)src.GetPhyAddr(), cache_line_t::ENTIRE_DATA_CACHE);
    __gm__ BlockInfo *coreHeadPtr = GetCurCoreHeadPtr();
    DataCopyParams backupParams = GetDataCopyParamImpl(ONE_DUMP_BACKUP_SIZE);
    uint32_t loopCount = GetdataCopyCount(copyLen);
    for (uint32_t i = 0; i < loopCount; i++) {
        PipeBarrier<PIPE_ALL>();
        DataCopyGM2UBImpl((__ubuf__ uint32_t *)tmp.GetPhyAddr(),
                          (__gm__ uint32_t *)((__gm__ uint8_t *)src.GetPhyAddr() + i * ONE_DUMP_BACKUP_SIZE),
                          backupParams); // GM to UB
        PipeBarrier<PIPE_ALL>();
        DataCopyUB2GMImpl((__gm__ uint8_t *)(coreHeadPtr->dumpAddr + i * ONE_DUMP_BACKUP_SIZE),
                          (__ubuf__ uint8_t *)tmp.GetPhyAddr(),
                          backupParams); // UB to GM
        PipeBarrier<PIPE_ALL>();
    }
    dcci((__gm__ uint64_t *)coreHeadPtr->dumpAddr, cache_line_t::ENTIRE_DATA_CACHE);
}

/***********************************每个core内存分配示意图*************************************************

|------------------------------------------------core_0-------- --------------------------------------|
|---已使用addr、 MAGIC---|---bMsg_1---|---data_1---|---bMsg_2---|---data_2---|///未使用///|---backup---|
           |________________________________________________________________^

**********************************************************************************************************/
template <typename T>
__aicore__ inline void DumpTensorLocal2GMEntityImpl(const LocalTensor<T> &src, uint32_t desc, uint32_t dumpSize)
{
    /* offset: 实际data数据大小 copyLen:以ONE_DUMP_BACKUP_SIZE对齐的data大小 */
    uint32_t offset = dumpSize * sizeof(T);
    uint32_t padOffset = AlignUp(offset, ONE_BLK_SIZE);
    uint32_t copyLen = GetdataCopyCount(padOffset) * ONE_DUMP_BACKUP_SIZE;
    uint32_t dumpHeadSize = sizeof(DumpMessageHead);
    uint32_t workOffset = copyLen + dumpHeadSize + ONE_DUMP_BACKUP_SIZE;

    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
#ifdef __ENABLE_VECTOR_CORE__
#if defined(__DAV_M200_VEC__)
    uint32_t core = GetBlockIdxImpl() - get_data_main_base();
#else
    uint32_t core = GetBlockIdxImpl() - get_data_main_base() + AIV_CORE_NUM;
#endif
#else
    uint8_t core = GetBlockIdxImpl() + AIV_CORE_NUM;
#endif
    if (core >= DUMP_CORE_COUNT) {
        return;
    }
    __gm__ BlockInfo *coreHeadPtr = (__gm__ BlockInfo *)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    dcci((__gm__ uint64_t *)coreHeadPtr, cache_line_t::ENTIRE_DATA_CACHE);
    if (coreHeadPtr->dumpOffset < dumpHeadSize + ONE_DUMP_BACKUP_SIZE) {
            KERNEL_LOG(KERNEL_ERROR, "Remaining space[%u] is not enough for check!", coreHeadPtr->dumpOffset);
            return;
    }

    __gm__ uint8_t *gmBackAddr = (__gm__ uint8_t *)(dumpWorkspaceStart + DUMP_UINTSIZE * (core + 1) - ONE_DUMP_BACKUP_SIZE);
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t *)gmBackAddr, cache_line_t::ENTIRE_DATA_CACHE);
    if ((coreHeadPtr->dumpOffset < workOffset)) {
        KERNEL_LOG(KERNEL_ERROR, "Remaining space[%u] is less than workOffset[%u]", coreHeadPtr->dumpOffset, workOffset);
        UpdateBlockInfo(0, DUMP_EXC_FLAG);
        PipeBarrier<PIPE_ALL>();
        return;
    }
    /*---------Write HeadMsg---------*/
    T tmpData;
    DumpMessageHead dumpMsg = DumpMessageHead(static_cast<uint32_t>(DumpType::DUMP_TENSOR),
                                              DUMP_MSG_HEAD_SIZE + padOffset,
                                              static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
                                              GetDataType(tmpData),
                                              desc,
                                              0,
                                              CheckValidPosition(src),
                                              dumpSize);
    WriteHeadMsg(dumpMsg);
    /*---------Copy Data---------*/
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    if (srcHWPos == Hardware::UB) {
        TensorDataCopy(src, copyLen);
    } else {
            /*---------Backup UB start---------*/
        DataCopyParams backupParams = GetDataCopyParamImpl(ONE_DUMP_BACKUP_SIZE);
        LocalTensor<uint8_t> tmp;
        InitTmpTensor(tmp, static_cast<uint8_t>(TPosition::VECIN));
        PipeBarrier<PIPE_ALL>();
        DataCopyUB2GMImpl((__gm__ uint8_t *)gmBackAddr, (__ubuf__ uint8_t *)tmp.GetPhyAddr(), backupParams);
        /*---------data copy---------*/
        TensorDataLoopCopy(tmp, src, copyLen);
        /*---------Recovery UB---------*/
        PipeBarrier<PIPE_ALL>();
        DataCopyGM2UBImpl((__ubuf__ uint32_t *)tmp.GetPhyAddr(), (__gm__ uint32_t *)gmBackAddr, backupParams);
        PipeBarrier<PIPE_ALL>();
    }
    UpdateBlockInfo(padOffset, 0);
}

template <template <typename> class Tensor, typename T>
__aicore__ inline void DumpTensorRingBufImpl(const Tensor<T>& src, uint32_t desc, uint32_t dumpSize);

template <typename T>
__aicore__ inline void DumpTensorLocal2GMImpl(const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE);
    if (g_sysPrintFifoSpace != nullptr) {
        DumpTensorRingBufImpl(src, desc, dumpSize);
    } else {
        DumpTensorLocal2GMEntityImpl(src, desc, dumpSize);
    }
    set_ctrl(ctrlValue);
}

/***********************************每个core内存分配示意图*************************************************

|------------------------------------------------core_0-----------------------------------------------|
|---已使用addr、 MAGIC---|---bMsg_1---|---data_1---|---bMsg_2---|---data_2---|///未使用///|---backup---|
           |________________________________________________________________^

**********************************************************************************************************/
template <typename T>
__aicore__ inline void DumpTensorGM2GMEntityImpl(const GlobalTensor<T> &src, uint32_t desc, uint32_t dumpSize)
{
    /* offset: 实际data数据大小 copyLen:以ONE_DUMP_BACKUP_SIZE对齐的data大小 */
    uint32_t offset = dumpSize * sizeof(T);
    uint32_t padOffset = AlignUp(offset, ONE_BLK_SIZE);
    uint32_t copyLen = GetdataCopyCount(padOffset) * ONE_DUMP_BACKUP_SIZE;
    uint32_t dumpHeadSize = sizeof(DumpMessageHead);
    uint32_t workOffset = copyLen + dumpHeadSize + ONE_DUMP_BACKUP_SIZE;

    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved) - DUMP_WORKSPACE_SIZE;
#ifdef __ENABLE_VECTOR_CORE__
#if defined(__DAV_M200_VEC__)
    uint32_t core = GetBlockIdxImpl() - get_data_main_base();
#else
    uint32_t core = GetBlockIdxImpl() - get_data_main_base() + AIV_CORE_NUM;
#endif
#else
    uint8_t core = GetBlockIdxImpl() + AIV_CORE_NUM;
#endif
    if (core >= DUMP_CORE_COUNT) {
        return;
    }
    __gm__ BlockInfo *coreHeadPtr = (__gm__ BlockInfo *)(dumpWorkspaceStart + DUMP_UINTSIZE * core);
    dcci((__gm__ uint64_t *)coreHeadPtr, cache_line_t::ENTIRE_DATA_CACHE);
    if (coreHeadPtr->dumpOffset < dumpHeadSize + ONE_DUMP_BACKUP_SIZE) {
            KERNEL_LOG(KERNEL_ERROR, "Remaining space[%u] is not enough for check!", coreHeadPtr->dumpOffset);
            return;
    }
    /*---------Backup UB start---------*/
    DataCopyParams backupParams = GetDataCopyParamImpl(ONE_DUMP_BACKUP_SIZE);
    LocalTensor<uint8_t> tmp;
    InitTmpTensor(tmp, static_cast<uint8_t>(TPosition::VECIN));
    __gm__ uint8_t *gmBackAddr = (__gm__ uint8_t *)(dumpWorkspaceStart + DUMP_UINTSIZE * (core + 1) - ONE_DUMP_BACKUP_SIZE);
    PipeBarrier<PIPE_ALL>();
    DataCopyUB2GMImpl((__gm__ uint8_t *)gmBackAddr, (__ubuf__ uint8_t *)tmp.GetPhyAddr(), backupParams);
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t *)gmBackAddr, cache_line_t::ENTIRE_DATA_CACHE);
    if ((coreHeadPtr->dumpOffset < workOffset)) {
        KERNEL_LOG(KERNEL_ERROR, "Remaining space[%u] is less than workOffset[%u]", coreHeadPtr->dumpOffset, workOffset);
        UpdateBlockInfo(0, DUMP_EXC_FLAG);
        PipeBarrier<PIPE_ALL>();
        return;
    }
    /*---------Write HeadMsg---------*/
    T tmpData;
    DumpMessageHead dumpMsg = DumpMessageHead(static_cast<uint32_t>(DumpType::DUMP_TENSOR),
        DUMP_MSG_HEAD_SIZE + padOffset,
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),  GetDataType(tmpData), desc, 0,
        static_cast<uint32_t>(AscendC::Hardware::GM), dumpSize);
    WriteHeadMsg(dumpMsg);
    /*---------Copy Data---------*/
    TensorDataLoopCopy(tmp, src, copyLen);
    UpdateBlockInfo(padOffset, 0);
    /*---------Recovery UB---------*/
    PipeBarrier<PIPE_ALL>();
    DataCopyGM2UBImpl((__ubuf__ uint32_t *)tmp.GetPhyAddr(), (__gm__ uint32_t *)gmBackAddr, backupParams);
    PipeBarrier<PIPE_ALL>();
    return;
}

template <typename T>
__aicore__ inline void DumpTensorGM2GMImpl(const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE);
    if (g_sysPrintFifoSpace != nullptr) {
        DumpTensorRingBufImpl(src, desc, dumpSize);
    } else {
        DumpTensorGM2GMEntityImpl(src, desc, dumpSize);
    }
    set_ctrl(ctrlValue);
}

__aicore__ inline void WriteDumpShapeInfo(const ShapeInfo& shapeInfo)
{
    /* offset: 实际data数据大小 copyLen:以ONE_DUMP_BACKUP_SIZE对齐的data大小 */
    uint32_t offset = sizeof(DumpShapeMessageHead);
    if (offset % ONE_BLK_SIZE != 0) {
        offset = (offset + ONE_BLK_SIZE - 1) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    }
#ifdef __ENABLE_VECTOR_CORE__
#if defined(__DAV_M200_VEC__)
    uint32_t blockIdx = GetBlockIdxImpl() - get_data_main_base();
#else
    uint32_t blockIdx = GetBlockIdxImpl() - get_data_main_base() + AIV_CORE_NUM;
#endif
#else
    uint8_t blockIdx = GetBlockIdxImpl() + AIV_CORE_NUM;
#endif
    if (blockIdx > DUMP_CORE_COUNT) {
        return;
    }
    __gm__ BlockInfo *blockInfo = GetBlockInfo();
    __gm__ uint8_t *blockInfoStart = (__gm__ uint8_t *)blockInfo;
    dcci((__gm__ uint64_t *)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE);
    uint32_t remainSize = blockInfo->dumpOffset - offset;
    uint64_t dumpAddr = blockInfo->dumpAddr;
    uint64_t gmBkUpAddr = dumpAddr + remainSize;
    uint32_t tlvSize = sizeof(DumpShapeMessageHead) + DUMP_SHAPE_MESSAGE_TL_LEN;
    LocalTensor<uint8_t> tmp;
    InitTmpTensor(tmp, static_cast<uint8_t>(TPosition::VECIN));
    DataCopyParams backupParams = GetDataCopyParamImpl(offset);
    PipeBarrier<PIPE_ALL>();
    DataCopyUB2GMImpl((__gm__ uint8_t*)gmBkUpAddr, (__ubuf__ uint8_t *)tmp.GetPhyAddr(), backupParams);
    PipeBarrier<PIPE_ALL>();
    if (tlvSize > remainSize) {
        UpdateBlockInfo(0, DUMP_EXC_FLAG);
        KERNEL_LOG(KERNEL_ERROR, "remain space is not enough for this print");
        PipeBarrier<PIPE_ALL>();
        return;
    }
    __gm__ uint8_t *tlvAddr = (__gm__ uint8_t*)dumpAddr;
    uint64_t ubAddr = (uint64_t)tmp.GetPhyAddr();
    *((__ubuf__ uint32_t *)ubAddr + DUMP_SHAPE_MESSAGE_HEAD_TYPE_POS) = static_cast<uint32_t>(DumpType::DUMP_SHAPE);
    *((__ubuf__ uint32_t *)ubAddr + DUMP_SHAPE_MESSAGE_HEAD_LEN_POS) = sizeof(DumpShapeMessageHead);
    *((__ubuf__ uint32_t *)ubAddr + DUMP_SHAPE_MESSAGE_HEAD_DIM_POS) = shapeInfo.shapeDim;
    for (uint32_t idx = 0; idx < shapeInfo.shapeDim && idx < K_MAX_SHAPE_DIM; idx++) {
        *((__ubuf__ uint32_t*)ubAddr + DUMP_SHAPE_MESSAGE_HEAD_SHAPE_START_POS + idx) = shapeInfo.shape[idx];
    }
    *((__ubuf__ uint32_t*)ubAddr + DUMP_SHAPE_MESSAGE_HEAD_RSV_POS) = 0;
    PipeBarrier<PIPE_ALL>();
    DataCopyParams headParams = GetDataCopyParamImpl(offset);
    DataCopyUB2GMImpl((__gm__ uint64_t*)(tlvAddr), (__ubuf__ uint64_t*)ubAddr, headParams);
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t*)tlvAddr, cache_line_t::ENTIRE_DATA_CACHE);
    UpdateBlockInfo(tlvSize, 0);
    PipeBarrier<PIPE_ALL>();
    DataCopyGM2UBImpl((__ubuf__ uint32_t*)tmp.GetPhyAddr(), (__gm__ uint32_t*)gmBkUpAddr, backupParams);
    PipeBarrier<PIPE_ALL>();
}

__aicore__ inline void WriteRingBufShapeInfo(const ShapeInfo &shapeInfo);

__aicore__ inline void DumpShapeImpl(const ShapeInfo &shapeInfo)
{
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE);
    if (g_sysPrintFifoSpace != nullptr) {
        WriteRingBufShapeInfo(shapeInfo);
    } else {
        WriteDumpShapeInfo(shapeInfo);
    }
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

__aicore__ inline void WriteString(uint64_t tmpAddr, __gm__ uint8_t* paramAddr, uint32_t paramIdx,
                                   __gm__ const char* s, uint32_t& offset)
{
    __gm__ uint64_t *stringAddr = reinterpret_cast<__gm__ uint64_t *>(paramAddr) + paramIdx;
    __gm__ uint64_t *dstStrAddr = reinterpret_cast<__gm__ uint64_t *>(paramAddr + offset);

    // write string value offset
    *((__gm__ uint64_t *)stringAddr) = static_cast<uint64_t>(offset - ONE_PARAM_SIZE * paramIdx);
    dcci((__gm__ uint64_t*)stringAddr, cache_line_t::ENTIRE_DATA_CACHE);
    paramAddr += ONE_PARAM_SIZE;

    // write string content
    __ubuf__ char *d = (__ubuf__ char *)(tmpAddr);
    uint32_t strLen = GetStringLength(s);
    uint32_t alignSize = strLen % ONE_DUMP_BACKUP_SIZE;
    for (uint32_t i = 0; i < strLen / ONE_DUMP_BACKUP_SIZE; i++) {
        for (uint32_t j = 0; j < ONE_DUMP_BACKUP_SIZE; j++) {
            *(d + j) = *(s + j + i * ONE_DUMP_BACKUP_SIZE);
        }

        PipeBarrier<PIPE_ALL>();
        DataCopyParams repeatStrParams = GetDataCopyParamImpl(ONE_DUMP_BACKUP_SIZE);
        copy_ubuf_to_gm((__gm__ void *)(dstStrAddr), (__ubuf__ void *)tmpAddr, 0, repeatStrParams.blockCount,
            repeatStrParams.blockLen, repeatStrParams.srcStride, repeatStrParams.dstStride);
        PipeBarrier<PIPE_ALL>();
        dstStrAddr += (ONE_DUMP_BACKUP_SIZE / B64_BYTE_SIZE);
    }
    if (alignSize != 0) {
        for (uint16_t j = 0; j < alignSize; j++) {
            *(d + j) = *(s + strLen - alignSize + j);
        }
        PipeBarrier<PIPE_ALL>();
        DataCopyParams repeatStrParams = GetDataCopyParamImpl(ONE_DUMP_BACKUP_SIZE);
        copy_ubuf_to_gm((__gm__ void *)(dstStrAddr), (__ubuf__ void *)tmpAddr, 0, repeatStrParams.blockCount,
            repeatStrParams.blockLen, repeatStrParams.srcStride, repeatStrParams.dstStride);
        PipeBarrier<PIPE_ALL>();
    }
    dcci((__gm__ uint64_t*)dstStrAddr, cache_line_t::ENTIRE_DATA_CACHE);
    offset += GetStringLength(s);
}
template <typename T>
__aicore__ inline void WriteScalar(uint64_t tmpAddr, __gm__ uint8_t* paramAddr, uint32_t paramIdx, T scalar)
{
    PipeBarrier<PIPE_ALL>();
    __gm__ uint64_t *scalarAddr = (__gm__ uint64_t *)paramAddr + paramIdx;

    *scalarAddr = 0;
    static_assert(!SupportType<T, double>(), "printf unsupport double type");

    if constexpr (SupportType<T, half, float>()) {
        *((__gm__ float *)scalarAddr) = static_cast<float>(scalar);
    } else if constexpr (std::is_signed<T>::value) {
        *((__gm__ int64_t *)scalarAddr) = static_cast<int64_t>(scalar);
    } else if constexpr(std::is_unsigned<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = static_cast<uint64_t>(scalar);
    } else if constexpr(std::is_pointer<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = (uintptr_t)scalar;
    } else if constexpr(std::is_enum<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = static_cast<uint64_t>(scalar);
    }

    dcci((__gm__ uint64_t*)scalarAddr, cache_line_t::ENTIRE_DATA_CACHE);
    PipeBarrier<PIPE_ALL>();
}

__aicore__ inline void SetParam(uint64_t tmpAddr, __gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset)
{
    return;
}

template <typename... Args>
__aicore__ inline void SetParam(uint64_t tmpAddr, __gm__ uint8_t* paramAddr, uint32_t paramIdx,
                                uint32_t& offset, Args&&... args);

template <typename... Args>
__aicore__ inline void SetParamImpl(uint64_t tmpAddr, __gm__ uint8_t *paramAddr, uint32_t paramIdx, uint32_t &offset,
                                    __gm__ const char *s, Args&&... args)
{
    WriteString(tmpAddr, paramAddr, paramIdx, s, offset);
    SetParam(tmpAddr, paramAddr, paramIdx + 1, offset, args...);
}

template <typename T, typename... Args>
__aicore__ inline void SetParamImpl(uint64_t tmpAddr, __gm__ uint8_t* paramAddr, uint32_t paramIdx,
                                    uint32_t& offset, T scalar, Args&&... args)
{
    WriteScalar(tmpAddr, paramAddr, paramIdx, scalar);
    SetParam(tmpAddr, paramAddr, paramIdx + 1, offset, args...);
}

template <typename... Args>
__aicore__ inline void SetParam(uint64_t tmpAddr, __gm__ uint8_t* paramAddr, uint32_t paramIdx,
                                uint32_t& offset, Args&&... args)
{
    SetParamImpl(tmpAddr, paramAddr, paramIdx, offset, args...);
}

__aicore__ inline void WriteTLHead(DumpType printType, uint64_t tmpAddr, __gm__ uint8_t *tlv, uint32_t valueSize)
{
    *((__ubuf__ uint32_t *)tmpAddr) = static_cast<uint32_t>(printType);
    *((__ubuf__ uint32_t *)tmpAddr + 1) = valueSize;

    PipeBarrier<PIPE_ALL>();
    DataCopyParams headParams = GetDataCopyParamImpl(ONE_BLK_SIZE);
    copy_ubuf_to_gm((__gm__ void *)(tlv), (__ubuf__ void *)tmpAddr, 0, headParams.blockCount,
        headParams.blockLen, headParams.srcStride, headParams.dstStride);
    PipeBarrier<PIPE_ALL>();
    dcci((__gm__ uint64_t*)tlv, cache_line_t::ENTIRE_DATA_CACHE);
}

template <class... Args>
__aicore__ inline void PrintfEntityImpl(DumpType printType, __gm__ const char *fmt, Args&&... args)
{
#ifdef ASCENDC_DUMP
#ifdef __ENABLE_VECTOR_CORE__
#if defined(__DAV_M200_VEC__)
    uint32_t blockIdx = GetBlockIdxImpl() - get_data_main_base();
#else
    uint32_t blockIdx = GetBlockIdxImpl() - get_data_main_base() + AIV_CORE_NUM;
#endif
#else
    uint8_t blockIdx = GetBlockIdxImpl() + AIV_CORE_NUM;
#endif
    if (blockIdx >= DUMP_CORE_COUNT) {
        return;
    }
    __gm__ BlockInfo *blockInfo = GetBlockInfo();
    __gm__ uint8_t *blockInfoStart = (__gm__ uint8_t *)blockInfo;
    dcci((__gm__ uint64_t *)blockInfoStart, cache_line_t::ENTIRE_DATA_CACHE);
    uint32_t remainSize = blockInfo->dumpOffset - ONE_DUMP_BACKUP_SIZE;
    uint64_t dumpAddr = blockInfo->dumpAddr;
    uint64_t gmBackUpAddr = dumpAddr + remainSize;
    uint32_t paramSize = GetParamSize(fmt, args...);
    uint32_t paramNum = GetArgsNum(args...) + 1;
    paramSize = (paramSize + ONE_PARAM_SIZE - 1) & (~(ONE_PARAM_SIZE - 1));
    uint32_t tlvSize = paramSize + ONE_PARAM_SIZE;
    LocalTensor<uint8_t> tmp;
    InitTmpTensor(tmp, static_cast<uint8_t>(TPosition::VECIN));
    DataCopyParams backupParams = GetDataCopyParamImpl(ONE_DUMP_BACKUP_SIZE);
    PipeBarrier<PIPE_ALL>();
    copy_ubuf_to_gm((__gm__ void *)gmBackUpAddr, (__ubuf__ void *)tmp.GetPhyAddr(), 0,
        backupParams.blockCount, backupParams.blockLen, backupParams.srcStride, backupParams.dstStride);
    PipeBarrier<PIPE_ALL>();
    if (tlvSize > remainSize) {
        UpdateBlockInfo(0, DUMP_EXC_FLAG);
        KERNEL_LOG(KERNEL_ERROR, "remain space is not enough for this print");
        PipeBarrier<PIPE_ALL>();
        return;
    }

    __gm__ uint8_t *tlvAddr = (__gm__ uint8_t *)dumpAddr;
    __gm__ uint8_t *paramAddr = tlvAddr + ONE_PARAM_SIZE;
    WriteTLHead(printType, (uint64_t)tmp.GetPhyAddr(), tlvAddr, paramSize); // HEAD 2 UB

    uint32_t offset = paramNum * ONE_PARAM_SIZE;
    uint32_t strLen = GetStringLength(fmt);

    WriteString((uint64_t)tmp.GetPhyAddr(), paramAddr, 0, fmt, offset);

    uint32_t paramIdx = 1;
    SetParam((uint64_t)tmp.GetPhyAddr(), paramAddr, paramIdx, offset, args...);

    UpdateBlockInfo(tlvSize, 0);
    PipeBarrier<PIPE_ALL>();

    copy_gm_to_ubuf((__ubuf__ void *)tmp.GetPhyAddr(), (__gm__ void *)gmBackUpAddr, 0, backupParams.blockCount,
        backupParams.blockLen, backupParams.srcStride, backupParams.dstStride);
    PipeBarrier<PIPE_ALL>();
#endif
}

template <class... Args>
__aicore__ inline void PrintfRingBufImpl(DumpType printType, __gm__ const char* fmt, Args&&... args);

template <class... Args>
__aicore__ inline void PrintfImpl(DumpType printType, __gm__ const char* fmt, Args&&... args)
{
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE);
    if (g_sysPrintFifoSpace != nullptr) {
        PrintfRingBufImpl(printType, fmt, args...);
    } else {
        uint64_t ctrlValue = get_ctrl();
        set_atomic_none();
        PrintfEntityImpl(printType, fmt, args...);
        set_ctrl(ctrlValue);
    }
}

__aicore__ inline void EnablePrintf()
{
#if defined(__ENABLE_ASCENDC_PRINTF__)
#if defined(ASCENDC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
    static const struct BinaryMetaAscFeature __asc_feature_print__ __attribute__ ((used, section (".ascend.meta"))) =
    {4, 4, 1};
#endif // defined(ASCENDC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
#endif // __ENABLE_ASCENDC_PRINTF__
}

__aicore__ __gm__ inline RingBufReadInfo* GetRingBufReadInfo(__gm__ BlockRingBufInfo* blockRingBufInfo)
{
    __gm__ uint8_t* blockHead = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo);

    return reinterpret_cast<__gm__ RingBufReadInfo*>(blockHead + sizeof(BlockRingBufInfo));
}

__aicore__ __gm__ inline RingBufWriteInfo* GetRingBufWriteInfo(__gm__ BlockRingBufInfo* blockRingBufInfo)
{
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo->ringBufAddr);

    return reinterpret_cast<__gm__ RingBufWriteInfo*>(ringBufAddr + blockRingBufInfo->ringBufLen);
}

template <uint64_t timeOutCycle = 1000 * 1000> // 20ms
__aicore__ inline void RingBufferWaitRtsSync()
{
    volatile uint64_t counter = 0;
    while (counter < timeOutCycle) {
        counter++;
        __asm__ __volatile__("");
    }
}

__aicore__ inline bool WaitRingBufBeginRead(volatile __gm__ RingBufReadInfo* readInfo)
{
    constexpr uint32_t maxCounter = 15;
    volatile uint32_t counter = 0;
    while (readInfo->bufOffset == 0) {
        if (counter >= maxCounter) { // max wait 300ms, rts read gm per 200ms
            return false;
        }
        RingBufferWaitRtsSync(); // wait 20 ms
        ++counter;
        dcci((__gm__ uint64_t*)(readInfo), cache_line_t::ENTIRE_DATA_CACHE);
    }
    return true;
}

__aicore__ inline void SkipRingBufDirectly(volatile __gm__ RingBufWriteInfo* writeInfo)
{
    writeInfo->bufOffset = 0;
    dcci((__gm__ uint64_t*)(writeInfo), cache_line_t::ENTIRE_DATA_CACHE);
    return;
}

__aicore__ inline void SkipRingBufWithInfo(
    volatile __gm__ RingBufWriteInfo* writeInfo, __gm__ uint8_t* ringBufAddr, const uint32_t& ringBufLen)
{
    __gm__ SkipTlvInfo* skipInfo = reinterpret_cast<__gm__ SkipTlvInfo*>(ringBufAddr + writeInfo->bufOffset);
    skipInfo->type = static_cast<uint32_t>(DumpType::DUMP_SKIP);
    skipInfo->length = ringBufLen - writeInfo->bufOffset - sizeof(SkipTlvInfo);
    writeInfo->bufOffset = 0;
    writeInfo->packIdx += 1;
    dcci((__gm__ uint64_t*)(skipInfo), cache_line_t::ENTIRE_DATA_CACHE);
    dcci((__gm__ uint64_t*)(writeInfo), cache_line_t::ENTIRE_DATA_CACHE);
    return;
}

__aicore__ inline bool RingBufferWait(volatile __gm__ RingBufReadInfo* readInfo,
    volatile __gm__ RingBufWriteInfo* writeInfo, const uint32_t& tlvLen)
{
    constexpr uint32_t maxCounter = 15;
    volatile uint32_t counter = 0;
    while (writeInfo->bufOffset < readInfo->bufOffset && writeInfo->bufOffset + tlvLen >= readInfo->bufOffset) {
        if (counter >= maxCounter) { // max wait 300ms, rts read gm per 200ms
            return false;
        }
        RingBufferWaitRtsSync(); // wait 20 ms
        ++counter;
        dcci((__gm__ uint64_t*)(readInfo), cache_line_t::ENTIRE_DATA_CACHE);
    }
    return true;
}

__aicore__ inline bool CheckAndWaitRingBufSpace(__gm__ BlockRingBufInfo* blockRingBufInfo, const uint32_t& tlvLen)
{
    constexpr uint32_t minTlvLen = sizeof(SkipTlvInfo);

    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo->ringBufAddr);
    uint32_t ringBufLen = blockRingBufInfo->ringBufLen;

    volatile __gm__ RingBufReadInfo* readInfo = GetRingBufReadInfo(blockRingBufInfo);
    volatile __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    if (minTlvLen >= ringBufLen || tlvLen > ringBufLen) {
        return false;
    } else if (writeInfo->bufOffset + minTlvLen >= ringBufLen) {
        if (!WaitRingBufBeginRead(readInfo)) { // check read is begin
            return false;
        }
        SkipRingBufDirectly(writeInfo);
    } else if (writeInfo->bufOffset + tlvLen > ringBufLen) {
        if (!WaitRingBufBeginRead(readInfo)) { // check read is begin
            return false;
        }
        SkipRingBufWithInfo(writeInfo, ringBufAddr, ringBufLen);
    }
    if (writeInfo->packIdx > 0 &&
        writeInfo->bufOffset < readInfo->bufOffset &&
        writeInfo->bufOffset + tlvLen >= readInfo->bufOffset) {
        return RingBufferWait(readInfo, writeInfo, tlvLen);
    }
    return true;
}

__aicore__ __gm__ inline uint8_t* GetRingBufTlv(__gm__ BlockRingBufInfo* blockRingBufInfo)
{
    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo->ringBufAddr);
    return ringBufAddr + writeInfo->bufOffset;
}

__aicore__ __gm__ inline BlockRingBufInfo* GetBlockRingBufInfo()
{
    uint32_t blockIdx = (GetBlockIdxImpl() & 0x00FF) % DUMP_CORE_COUNT;
    uint32_t blockLength = reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace)->length;
    __gm__ BlockRingBufInfo* ringBufInfo =
        reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace + blockLength * blockIdx);
    return ringBufInfo->magic == 0xAE86 ? ringBufInfo : nullptr;
}

__aicore__ inline void UpdateWriteInfo(__gm__ RingBufWriteInfo* writeInfo, const uint32_t& tlvLen)
{
    writeInfo->bufOffset += tlvLen;
    writeInfo->packIdx += 1;
    dcci((__gm__ uint64_t*)(writeInfo), cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline uint32_t GetPrintArgsLen(uint32_t& argsNum)
{
    return 0;
}

template <typename... Args>
__aicore__ inline uint32_t GetPrintArgsLen(uint32_t& argsNum, Args&&... args);

template <typename... Args>
__aicore__ inline uint32_t GetPrintArgsLenImpl(uint32_t& argsNum, __gm__ const char* s, Args&&... args)
{
    constexpr uint32_t paramSize = sizeof(uint64_t);
    const uint32_t& strLen = GetStringLength(s);
    argsNum += 1;
    return paramSize + strLen + GetPrintArgsLen(argsNum, args...);
}

template <typename T, typename... Args>
__aicore__ inline uint32_t GetPrintArgsLenImpl(uint32_t& argsNum, T scalar, Args&&... args)
{
    constexpr uint32_t paramSize = sizeof(uint64_t);
    argsNum += 1;
    return paramSize + GetPrintArgsLen(argsNum, args...);
}

template <typename... Args>
__aicore__ inline uint32_t GetPrintArgsLen(uint32_t& argsNum, Args&&... args)
{
    return GetPrintArgsLenImpl(argsNum, args...);
}

__aicore__ constexpr uint32_t AlignTlvLen(const uint32_t& dataLen)
{
    constexpr uint32_t num = 7;
    return ((dataLen + num) & ~num) + num + 1;
}

__aicore__ inline void MemCopyGm2Gm(__gm__ uint8_t* dst, __gm__ const uint8_t* src, const uint32_t& len)
{
    if (dst == nullptr || src == nullptr)
    {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        *(dst + i) = *(src + i);
    }
    dcci((__gm__ uint64_t*)(dst), cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline void SetStringArg(__gm__ uint8_t* paramAddr, uint32_t paramIdx, __gm__ const char* s, uint32_t& offset)
{
    __gm__ uint64_t *stringAddr = reinterpret_cast<__gm__ uint64_t *>(paramAddr) + paramIdx;
    __gm__ uint64_t *dstStrAddr = reinterpret_cast<__gm__ uint64_t *>(paramAddr + offset);

    // write string value offset
    *((__gm__ uint64_t *)stringAddr) = static_cast<uint64_t>(offset - ONE_PARAM_SIZE * paramIdx);
    dcci((__gm__ uint64_t*)stringAddr, cache_line_t::ENTIRE_DATA_CACHE);

    // write string content
    __gm__ uint8_t* d = reinterpret_cast<__gm__ uint8_t*>(dstStrAddr);
    uint32_t strLen = GetStringLength(s);
    MemCopyGm2Gm(d, reinterpret_cast<__gm__ const uint8_t*>(s), strLen);
    offset += strLen;
}

template <typename T>
__aicore__ inline void SetValueArg(__gm__ uint8_t* paramAddr, uint32_t paramIdx, T scalar)
{
    static_assert(!SupportType<T, double>(), "printf unsupport double type");

    __gm__ uint64_t *scalarAddr = (__gm__ uint64_t *)paramAddr + paramIdx;
    *scalarAddr = 0;

    if constexpr (SupportType<T, half, float>()) {
        *((__gm__ float *)scalarAddr) = static_cast<float>(scalar);
    } else if constexpr (std::is_signed<T>::value) {
        *((__gm__ int64_t *)scalarAddr) = static_cast<int64_t>(scalar);
    } else if constexpr(std::is_unsigned<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = static_cast<uint64_t>(scalar);
    } else if constexpr(std::is_pointer<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = (uintptr_t)scalar;
    } else if constexpr(std::is_enum<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = static_cast<uint64_t>(scalar);
    }

    dcci((__gm__ uint64_t*)scalarAddr, cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline void WriteRingBufTlvHead(
    DumpType printType, __gm__ PrintTlvInfoHead* printTlv, const uint32_t& tlvLen, const uint32_t& argsNum)
{
    printTlv->type = static_cast<uint32_t>(printType);
    printTlv->length = tlvLen - sizeof(uint32_t[2]);   // exclude type and length
    printTlv->blockIdx = static_cast<uint32_t>(GetBlockIdxImpl());
    printTlv->resv = static_cast<uint32_t>(0U);
    printTlv->fmtOffset = (argsNum + 1) * sizeof(uint64_t);      // include fmt offset
    dcci((__gm__ uint64_t*)(printTlv), cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline void SetArgsData(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset)
{
    return;
}

template <typename... Args>
__aicore__ inline void SetArgsData(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, Args&&... args);

template <typename... Args>
__aicore__ inline void SetArgsDataImpl(__gm__ uint8_t *paramAddr, uint32_t paramIdx, uint32_t &offset,
                                    __gm__ const char *s, Args&&... args)
{
    SetStringArg(paramAddr, paramIdx, s, offset);
    SetArgsData(paramAddr, paramIdx + 1, offset, args...);
}

template <typename T, typename... Args>
__aicore__ inline void SetArgsDataImpl(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, T scalar,
                                    Args&&... args)
{
    SetValueArg(paramAddr, paramIdx, scalar);
    SetArgsData(paramAddr, paramIdx + 1, offset, args...);
}

template <typename... Args>
__aicore__ inline void SetArgsData(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, Args&&... args)
{
    SetArgsDataImpl(paramAddr, paramIdx, offset, args...);
}

template <typename... Args>
__aicore__ inline void WriteRingBufTlvData(__gm__ PrintTlvInfoHead* printTlv, __gm__ const char* fmt, Args&&... args)
{
    const uint32_t& strLen = GetStringLength(fmt);
    __gm__ uint8_t* paramAddr =
        reinterpret_cast<__gm__ uint8_t*>(printTlv + 1);
    __gm__ uint8_t* fmtAddr = paramAddr + printTlv->fmtOffset - sizeof(uint64_t);
    __gm__ uint8_t* strParamAddr = reinterpret_cast<__gm__ uint8_t*>(fmtAddr) + strLen;
    MemCopyGm2Gm(fmtAddr, reinterpret_cast<__gm__ const uint8_t*>(fmt), strLen);
    uint32_t strParamOffset = printTlv->fmtOffset + strLen;
    SetArgsData(paramAddr, 0, strParamOffset, args...);
}

template <class... Args>
__aicore__ inline void PrintfRingBufImpl(DumpType printType, __gm__ const char* fmt, Args&&... args)
{
    __asc_aicore::printf_impl(fmt, args...);
}

template <typename T>
__aicore__ inline Hardware CheckDumpTensorPosition(const LocalTensor<T>& src)
{
    Hardware position = GetPhyType(static_cast<TPosition>(src.GetPosition()));
    if (position != Hardware::UB && position != Hardware::L1 && position != Hardware::L0C) {
        return Hardware::MAX;
    }
    return position;
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
    } else {
        return Internal::DumpTensorDataType::ACL_MAX;
    }
}

template <template<typename> class Tensor, typename T>
__aicore__ inline void WriteRingBufTlvHead(const Tensor<T>& src, __gm__ DumpTensorTlvInfoHead* dumpTensorTlv,
    const uint32_t& alignDumpDataLen, const uint32_t& desc, const uint32_t& dumpSize)
{
    Hardware position;
    if constexpr (IsSameType<Tensor<T>, LocalTensor<T>>::value) {
        position = GetPhyType(static_cast<TPosition>(src.GetPosition()));
    } else if (IsSameType<Tensor<T>, GlobalTensor<T>>::value) {
        position = Hardware::GM;
    }
    dumpTensorTlv->type = static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    dumpTensorTlv->length = sizeof(DumpTensorTlvInfoHead) - sizeof(uint32_t[2]) + alignDumpDataLen;
    dumpTensorTlv->tensorAddr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr()));
    dumpTensorTlv->dataType = static_cast<uint32_t>(GetTensorDataType<T>());
    dumpTensorTlv->desc = desc;
    dumpTensorTlv->bufferId = static_cast<uint32_t>(0U);
    dumpTensorTlv->position = static_cast<uint16_t>(position);
    dumpTensorTlv->blockIdx = static_cast<uint16_t>(GetBlockIdxImpl());
    dumpTensorTlv->dim = static_cast<uint32_t>(0U);
    for (uint32_t i = 0; i < K_MAX_SHAPE_DIM; ++i) {
        dumpTensorTlv->shape[i] = static_cast<uint32_t>(0U);
    }
    dumpTensorTlv->resv1 = static_cast<uint32_t>(0U);
    dumpTensorTlv->dumpSize = dumpSize * sizeof(T);
    dcci((__gm__ uint64_t*)(dumpTensorTlv), cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline void ClearGmData(__gm__ uint8_t* dst, uint32_t len)
{
    if (dst == nullptr)
    {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        *(dst + i) = 0x00;
    }
    dcci((__gm__ uint64_t*)(dst), cache_line_t::ENTIRE_DATA_CACHE);
}

template <typename T>
__aicore__ inline void SetDumpDataL12GM(__gm__ uint8_t* dst, const LocalTensor<T>& src, uint32_t alignDumpDataLen)
{
    DataCopyParams param = {1, static_cast<uint16_t>(alignDumpDataLen / ONE_BLK_SIZE), 0, 0};
    __gm__ uint8_t* gmBackAddr = dst + alignDumpDataLen;
    LocalTensor<uint8_t> tmp;
    InitTmpTensor(tmp, static_cast<uint8_t>(TPosition::VECIN));
    DataCopyUB2GMImpl(reinterpret_cast<__gm__ uint8_t*>(gmBackAddr),
        reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()), param); // backup data in UB
    PipeBarrier<PIPE_ALL>();
    DataCopyL12UBImpl(reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()),
        reinterpret_cast<__cbuf__ uint8_t*>(src.GetPhyAddr()), param); // L1 to UB
    PipeBarrier<PIPE_ALL>();
    DataCopyUB2GMImpl(reinterpret_cast<__gm__ uint8_t*>(dst), reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()),
                        param); // UB to GM
    PipeBarrier<PIPE_ALL>();
    DataCopyGM2UBImpl(reinterpret_cast<__ubuf__ uint32_t*>(tmp.GetPhyAddr()),
        reinterpret_cast<__gm__ uint32_t*>(gmBackAddr), param); // recover data in UB
    PipeBarrier<PIPE_ALL>();
    ClearGmData(gmBackAddr, alignDumpDataLen);
}

template <typename T>
__aicore__ inline void SetDumpDataL0C2GM(__gm__ uint8_t* dst, const LocalTensor<T>& src, uint32_t alignDumpDataLen)
{
    constexpr uint32_t blockSize = 16 * 16 * sizeof(uint32_t);
    uint32_t copyLen = AlignUp(alignDumpDataLen, blockSize);
    DataCopyParams param = {1, static_cast<uint16_t>(copyLen / ONE_BLK_SIZE), 0, 0};
    __gm__ uint8_t* gmBackAddr = dst + copyLen;
    DataCopyEnhancedParams enhancedParams;
    enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX;
    DataCopyParams backupParamsL0C;
    backupParamsL0C.blockCount = 1;
    backupParamsL0C.blockLen = copyLen / blockSize;
    LocalTensor<uint8_t> tmp;
    InitTmpTensor(tmp, static_cast<uint8_t>(TPosition::VECIN));
    DataCopyUB2GMImpl(reinterpret_cast<__gm__ uint8_t*>(gmBackAddr),
        reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()), param); // backup data in UB
    PipeBarrier<PIPE_ALL>();
    DataCopyL0C2UBImpl( reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()),
                        reinterpret_cast<__cc__ int32_t*>(src.GetPhyAddr()),
                        backupParamsL0C,
                        enhancedParams); // L0C to UB
    PipeBarrier<PIPE_ALL>();
    DataCopyUB2GMImpl(  reinterpret_cast<__gm__ uint8_t*>(dst),
                        reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()),
                        param); // UB to GM
    PipeBarrier<PIPE_ALL>();
    DataCopyGM2UBImpl(reinterpret_cast<__ubuf__ uint32_t*>(tmp.GetPhyAddr()),
        reinterpret_cast<__gm__ uint32_t*>(gmBackAddr), param); // recover data in UB
    PipeBarrier<PIPE_ALL>();
    uint32_t tailLen = copyLen * 2 - alignDumpDataLen;
    ClearGmData(gmBackAddr + alignDumpDataLen, tailLen);
}

template <typename T>
__aicore__ inline void WriteRingBufTlvData(const LocalTensor<T>& src, __gm__ DumpTensorTlvInfoHead* dumpTensorTlv,
    const uint32_t& alignDumpDataLen, const uint32_t& dumpSize)
{
    __gm__ T* dumpDataAddr = reinterpret_cast<__gm__ T*>(dumpTensorTlv + 1);

    PipeBarrier<PIPE_ALL>();

    if (dumpTensorTlv->position == static_cast<uint16_t>(Hardware::UB)) { // UB to GM
        DataCopyParams copyParams = {1, static_cast<uint16_t>(alignDumpDataLen / ONE_BLK_SIZE), 0, 0};
        DataCopyUB2GMImpl(dumpDataAddr, reinterpret_cast<__ubuf__ T*>(src.GetPhyAddr()), copyParams);
    } else if (dumpTensorTlv->position == static_cast<uint16_t>(Hardware::L1)) { // L1 to GM
        SetDumpDataL12GM(reinterpret_cast<__gm__ uint8_t*>(dumpDataAddr), src, alignDumpDataLen);
    } else if (dumpTensorTlv->position == static_cast<uint16_t>(Hardware::L0C)) { // L0C to GM
        SetDumpDataL0C2GM(reinterpret_cast<__gm__ uint8_t*>(dumpDataAddr), src, alignDumpDataLen);
    }

    PipeBarrier<PIPE_ALL>();

    dcci((__gm__ uint64_t*)dumpDataAddr, cache_line_t::ENTIRE_DATA_CACHE);
}

template <typename T>
__aicore__ inline void WriteRingBufTlvData(
    const GlobalTensor<T>& src, __gm__ DumpTensorTlvInfoHead* dumpTensorTlv, const uint32_t& dumpSize)
{
    PipeBarrier<PIPE_ALL>();
    __gm__ uint8_t* dst = reinterpret_cast<__gm__ uint8_t*>(dumpTensorTlv + 1);
    MemCopyGm2Gm(dst, reinterpret_cast<__gm__ const uint8_t*>(src.GetPhyAddr()), dumpSize * sizeof(T));
}

template <template <typename> class Tensor, typename T>
__aicore__ inline void DumpTensorRingBufImpl(const Tensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
#ifdef ASCENDC_DUMP
    EnablePrintf();
    if constexpr (GetTensorDataType<T>() == Internal::DumpTensorDataType::ACL_MAX) {
        ASCENDC_ASSERT((false),
                   { KERNEL_LOG(KERNEL_ERROR, "dump tensor not support this data type"); });
        return;
    }
    if (dumpSize == 0) {
        return;
    }
    if constexpr (IsSameType<Tensor<T>, LocalTensor<T>>::value) {
        if (CheckDumpTensorPosition(src) == Hardware::MAX) {
            ASCENDC_ASSERT((false),
                    { KERNEL_LOG(KERNEL_ERROR, "dump tensor only support dump tensor from local to gm"); });
            return;
        }
    }
    __gm__ BlockRingBufInfo* blockRingBufInfo = GetBlockRingBufInfo();
    if (blockRingBufInfo == nullptr) {
        return;
    }
    constexpr uint32_t blockSize = 16 * 16 * sizeof(uint32_t);
    uint32_t alignDumpDataLen = AlignUp(dumpSize * sizeof(T), ONE_BLK_SIZE);
    uint32_t tlvLen = sizeof(DumpTensorTlvInfoHead) + alignDumpDataLen;
    uint32_t maxResvLen = sizeof(DumpTensorTlvInfoHead) + AlignUp(alignDumpDataLen, blockSize) * 2; // resv tmp space
    if (!CheckAndWaitRingBufSpace(blockRingBufInfo, maxResvLen)) { // Reserved Backup Capacity
        return;
    }

    __gm__ DumpTensorTlvInfoHead* dumpTensorTlv =
        reinterpret_cast<__gm__ DumpTensorTlvInfoHead*>(GetRingBufTlv(blockRingBufInfo));

    WriteRingBufTlvHead(src, dumpTensorTlv, alignDumpDataLen, desc, dumpSize);
    if constexpr (IsSameType<Tensor<T>, LocalTensor<T>>::value) {
        WriteRingBufTlvData(src, dumpTensorTlv, alignDumpDataLen, dumpSize);
    } else if (IsSameType<Tensor<T>, GlobalTensor<T>>::value) {
        WriteRingBufTlvData(src, dumpTensorTlv, dumpSize);
    }

    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    UpdateWriteInfo(writeInfo, tlvLen);
#endif // ASCENDC_DUMP
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
    shapeTlv->resv = static_cast<uint32_t>(0U);;
    dcci((__gm__ uint64_t*)shapeTlv, cache_line_t::ENTIRE_DATA_CACHE);

    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    UpdateWriteInfo(writeInfo, tlvLen);
}

__aicore__ inline void DumpTimeStampImpl(uint32_t descId)
{
    return;
}

__aicore__ inline void AscendCTimeStamp(uint32_t descId, uint64_t pcPtr = 0)
{
    return;
}
__aicore__ inline void InitDump(bool mixFlag, uint32_t gmLen)
{
#if defined(ASCENDC_DUMP) || defined(ASCENDC_ACC_DUMP)
    g_dumpWorkspaceReserved = GetSysWorkSpacePtr();
    InitDumpImpl(mixFlag, gmLen);
#else
    return;
#endif
}
__aicore__ inline void InitDump(bool mixFlag, GM_ADDR dumpStartAddr, uint32_t gmLen)
{
#if defined(ASCENDC_DUMP) || defined(ASCENDC_ACC_DUMP)
    g_dumpWorkspaceReserved = dumpStartAddr + DUMP_WORKSPACE_SIZE;
    InitDumpImpl(mixFlag, gmLen);
#else
    return;
#endif
}
}
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
