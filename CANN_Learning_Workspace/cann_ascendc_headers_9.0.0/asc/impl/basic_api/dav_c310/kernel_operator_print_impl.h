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
 * \file kernel_operator_print_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/basic_api/dav_c310/kernel_operator_print_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_PRINT_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_PRINT_IMPL_H
#define ASCENDC_MODULE_OPERATOR_PRINT_IMPL_H

#include "kernel_tpipe_impl.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_data_copy_impl.h"
#include "kernel_pop_stack_buffer.h"
#include "impl/utils/debug/asc_aicore_printf_impl.h"

namespace AscendC {
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_dumpWorkspaceReserved;

__aicore__ inline void EnablePrintf()
{
#if defined(__ENABLE_ASCENDC_PRINTF__)
#if defined(ASCENDC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
    static const struct BinaryMetaAscFeature __asc_feature_print__ __attribute__ ((used, section (".ascend.meta"))) =
    {4, 4, 1};
#endif // defined(ASCENDC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
#endif // __ENABLE_ASCENDC_PRINTF__
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
__aicore__ inline void WriteTLHead(DumpType printType, __gm__ uint8_t *tlv, uint32_t valueSize)
{
    *((__gm__ uint32_t *)tlv) = static_cast<uint32_t>(printType);
    *((__gm__ uint32_t *)tlv + 1) = valueSize;
    dcci((__gm__ uint64_t*)tlv, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
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

    if constexpr (SupportType<T, half, float>()) {
        *((__gm__ float *)scalarAddr) = static_cast<float>(scalar);
    } else if constexpr (SupportType<T, double>()) {
        *((__gm__ double *)scalarAddr) = static_cast<double>(scalar);
    } else if constexpr (std::is_signed<T>::value) {
        *((__gm__ int64_t *)scalarAddr) = static_cast<int64_t>(scalar);
    } else if constexpr(std::is_unsigned<T>::value) {
        *((__gm__ uint64_t *)scalarAddr) = static_cast<uint64_t>(scalar);
    } else if constexpr(SupportType<T, bfloat16_t, fp8_e5m2_t, fp8_e8m0_t, fp8_e4m3fn_t, hifloat8_t>()) {
        *((__gm__ float *)scalarAddr) = ToFloat(scalar);
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

__aicore__ inline uint8_t GetDumpBlockIdx()
{
    if ASCEND_IS_AIV {
        return GetBlockIdxImpl();
    } else {
        return GetBlockIdxImpl() + AIV_CORE_NUM;
    }
}

__aicore__ __gm__ inline BlockInfo *GetBlockInfo()
{
    uint8_t core = GetDumpBlockIdx();
    uint64_t dumpWorkspaceStart = reinterpret_cast<uint64_t>(g_dumpWorkspaceReserved);
    __gm__ BlockInfo *blockInfo = (__gm__ BlockInfo *)(dumpWorkspaceStart +  DUMP_UINTSIZE * core);
    dcci((__gm__ uint64_t*)blockInfo, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    return blockInfo;
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
    uint8_t blockIdx = GetDumpBlockIdx();
    if (blockIdx >= DUMP_CORE_COUNT) {
        return;
    }
    __gm__ BlockInfo *blockInfo = GetBlockInfo();
    dcci((__gm__ uint64_t*)blockInfo, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
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
}

template <class... Args>
__aicore__ inline void PrintfRingBufImpl(DumpType printType, __gm__ const char* fmt, Args&&... args);

template <class... Args>
__aicore__ inline void PrintfImpl(DumpType printType, __gm__ const char* fmt, Args&&... args)
{
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE,
        dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        PrintfRingBufImpl(printType, fmt, args...);
    } else {
        uint64_t ctrlValue = get_ctrl();
        set_atomic_none();
        PrintfEntityImpl(printType, fmt, args...);
        set_ctrl(ctrlValue);
    }
}

template <uint64_t timeoutCycle = 15 * 1000 * 1000> // 20ms * 15
__aicore__ inline void RingBufferWaitRtsSync()
{
    const uint64_t firstTimeStamp = static_cast<uint64_t>(GetSystemCycle());
    while (static_cast<uint64_t>(GetSystemCycle()) - firstTimeStamp < timeoutCycle) {
        // Wait for RTS sync
    }
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
    dcci((__gm__ uint64_t*)dst, cache_line_t::ENTIRE_DATA_CACHE,
        dcci_dst_t::CACHELINE_OUT);
}

__aicore__ __gm__ inline BlockRingBufInfo* GetBlockRingBufInfo()
{
    uint32_t blockIdx = (get_coreid() & 0x00FF) % DUMP_CORE_COUNT;
    uint32_t blockLength = reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace)->length;
    __gm__ BlockRingBufInfo* ringBufInfo =
        reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace + blockLength * blockIdx);
    if (ringBufInfo->magic != 0xAE86) {
        return nullptr;
    }
    if ASCEND_IS_AIV {
        ringBufInfo->flag = 1;
    } else {
        ringBufInfo->flag = 0;
    }
    dcci((__gm__ uint64_t*)ringBufInfo, cache_line_t::ENTIRE_DATA_CACHE,
        dcci_dst_t::CACHELINE_OUT);
    return ringBufInfo;
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

__aicore__ __gm__ inline uint8_t* GetRingBufTlv(__gm__ BlockRingBufInfo* blockRingBufInfo)
{
    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo->ringBufAddr);
    return ringBufAddr + writeInfo->bufOffset;
}

__aicore__ inline void UpdateWriteInfo(__gm__ RingBufWriteInfo* writeInfo, const uint32_t& tlvLen)
{
    writeInfo->bufOffset += tlvLen;
    writeInfo->packIdx += 1;
    dcci((__gm__ uint64_t*)writeInfo, cache_line_t::ENTIRE_DATA_CACHE,
        dcci_dst_t::CACHELINE_OUT);
}

__aicore__ inline bool WaitRingBufBeginRead(__gm__ RingBufReadInfo* readInfo)
{
    constexpr uint32_t maxCounter = 15;
    uint32_t counter = 0;
    while (readInfo->bufOffset == 0) {
        if (counter >= maxCounter) { // max wait 15 * 300ms, rts read gm per 200ms
            return false;
        }
        RingBufferWaitRtsSync(); // wait 20 * 15 ms
        ++counter;
        dcci((__gm__ uint64_t*)readInfo, cache_line_t::ENTIRE_DATA_CACHE,
            dcci_dst_t::CACHELINE_OUT);
    }
    return true;
}

__aicore__ inline void SkipRingBufDirectly(__gm__ RingBufWriteInfo* writeInfo)
{
    writeInfo->bufOffset = 0;
    dcci((__gm__ uint64_t*)writeInfo, cache_line_t::ENTIRE_DATA_CACHE,
        dcci_dst_t::CACHELINE_OUT);
    return;
}

__aicore__ inline void SkipRingBufWithInfo(
    __gm__ RingBufWriteInfo* writeInfo, __gm__ uint8_t* ringBufAddr, const uint32_t& ringBufLen)
{
    __gm__ SkipTlvInfo* skipInfo = reinterpret_cast<__gm__ SkipTlvInfo*>(ringBufAddr + writeInfo->bufOffset);
    skipInfo->type = static_cast<uint32_t>(DumpType::DUMP_SKIP);
    skipInfo->length = ringBufLen - writeInfo->bufOffset - sizeof(SkipTlvInfo);
    writeInfo->bufOffset = 0;
    writeInfo->packIdx += 1;
    dcci((__gm__ uint64_t*)skipInfo, cache_line_t::ENTIRE_DATA_CACHE,
        dcci_dst_t::CACHELINE_OUT);
    dcci((__gm__ uint64_t*)writeInfo, cache_line_t::ENTIRE_DATA_CACHE,
        dcci_dst_t::CACHELINE_OUT);
    return;
}

__aicore__ inline bool RingBufferWait(__gm__ RingBufReadInfo* readInfo, __gm__ RingBufWriteInfo* writeInfo,
                                      const uint32_t& tlvLen)
{
    constexpr uint32_t maxCounter = 15;
    uint32_t counter = 0;
    while (writeInfo->bufOffset < readInfo->bufOffset && writeInfo->bufOffset + tlvLen >= readInfo->bufOffset) {
        if (counter >= maxCounter) { // max wait 15 * 300ms, rts read gm per 200ms
            return false;
        }
        RingBufferWaitRtsSync(); // wait 20 * 15 ms
        ++counter;
        dcci((__gm__ uint64_t*)readInfo, cache_line_t::ENTIRE_DATA_CACHE,
            dcci_dst_t::CACHELINE_OUT);
    }
    return true;
}

__aicore__ inline bool CheckAndWaitRingBufSpace(__gm__ BlockRingBufInfo* blockRingBufInfo, const uint32_t& tlvLen)
{
    constexpr uint32_t minTlvLen = sizeof(SkipTlvInfo);

    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo->ringBufAddr);
    uint32_t ringBufLen = blockRingBufInfo->ringBufLen;

    __gm__ RingBufReadInfo* readInfo = GetRingBufReadInfo(blockRingBufInfo);
    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

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

__aicore__ inline void WriteRingBufTlvHead(
    DumpType printType, __gm__ PrintTlvInfoHead* printTlv, const uint32_t& tlvLen, const uint32_t& argsNum)
{
    printTlv->type = static_cast<uint32_t>(printType);
    printTlv->length = tlvLen - sizeof(uint32_t[2]);   // exclude type and length
    printTlv->blockIdx = static_cast<uint32_t>(GetBlockIdxImpl());
    printTlv->resv = static_cast<uint32_t>(0U);
    printTlv->fmtOffset = (argsNum + 1) * sizeof(uint64_t);      // include fmt offset
    dcci((__gm__ uint64_t*)printTlv, cache_line_t::ENTIRE_DATA_CACHE,
        dcci_dst_t::CACHELINE_OUT);
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
    SetParam(paramAddr, 0, strParamOffset, args...);
}

template <class... Args>
__aicore__ inline void PrintfRingBufImpl(DumpType printType, __gm__ const char* fmt, Args&&... args)
{
    __asc_aicore::printf_impl(fmt, args...);
}


}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_PRINT_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_PRINT_IMPL_H__
#endif
