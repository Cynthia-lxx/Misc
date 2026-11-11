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
* \file global_cache.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/resource/cube_in_buffer/global_cache.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_IN_BUFFER_GLOBAL_CACHE_H__
#endif

#ifndef IMPL_MATMUL_RESOURCE_CUBE_IN_BUFFER_GLOBAL_CACHE_H
#define IMPL_MATMUL_RESOURCE_CUBE_IN_BUFFER_GLOBAL_CACHE_H

namespace AscendC {
namespace Impl {
namespace Detail {
class GlobalCache;
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
__BLOCK_LOCAL__ __inline__ AscendC::Impl::Detail::GlobalCache* gL1Cache;
__aicore__ inline AscendC::Impl::Detail::GlobalCache* GetGlobalCachePtr()
{
    return gL1Cache;
}

namespace AscendC {
namespace Impl {
namespace Detail {

class GlobalCache {
public:
    __aicore__ inline GlobalCache() = default;

    __aicore__ inline void Init()
    {
        cacheSize_ = 0;
        gL1Cache = this;
        alloc_ = false;
    }

    __aicore__ inline void InitBuffer(int32_t baseBlockSize)
    {
        GetTPipePtr()->InitBuffer(cacheQue_, 1, baseBlockSize);
    }

    template <class SrcT>
    __aicore__ inline bool Hit(const GlobalTensor<SrcT>& globalMatrix)
    {
        return (alloc_ && (reinterpret_cast<GM_ADDR>(globalMatrix.address_) == srcAddr_));
    }

    template <class T>
    __aicore__ inline void EnQue(const LocalTensor<T>& tensor)
    {
        ++cacheSize_;
        cacheQue_.template EnQue<T>(tensor);
    }

    template <class T>
    __aicore__ inline LocalTensor<T> DeQue()
    {
        return cacheQue_.template DeQue<T>();
    }

    template <class T>
    __aicore__ inline LocalTensor<T> AllocTensor()
    {
        if (alloc_) {
            LocalTensor<T> cache;
            cache.address_ = cacheHead_;
            return cache;
        }

        auto cache = cacheQue_.template AllocTensor<T>();
        cacheHead_ = cache.address_;
        alloc_ = true;
        return cache;
    }

    template <class T>
    __aicore__ inline void FreeTensor(LocalTensor<T>& tensor)
    {
        cacheQue_.FreeTensor(tensor);
    }

    template <class SrcT>
    __aicore__ inline void ClearCache()
    {
        cacheSize_ = 0;
        if (alloc_) {
            LocalTensor<SrcT> a;
            a.SetAddr(cacheHead_);
            cacheQue_.FreeTensor(a);
            FreeAllEvent();
            alloc_ = false;
        }
    }

    template <class SrcT>
    __aicore__ inline LocalTensor<SrcT> GetCacheHead()
    {
        LocalTensor<SrcT> a;
        a.SetAddr(cacheHead_);
        return a;
    }

    template <class SrcT>
    __aicore__ inline void SetCacheHead(LocalTensor<SrcT>& cacheHead)
    {
        cacheHead_ = cacheHead.address_;
    }

    template <class SrcT>
    __aicore__ inline void SetOrgTensor(const GlobalTensor<SrcT>& globalMatrix)
    {
        srcAddr_ = reinterpret_cast<GM_ADDR>(globalMatrix.address_);
    }

    template <class SrcT>
    __aicore__ inline GlobalTensor<SrcT> GetOrgTensor()
    {
        GlobalTensor<SrcT> globalMatrix;
        globalMatrix.SetGlobalBuffer(srcAddr_);
        return globalMatrix;
    }

    __aicore__ inline void FreeAllEvent()
    {
        cacheSize_ = 0;
        alloc_ = false;
        cacheQue_.FreeAllEvent();
    }

    __aicore__ inline int32_t GetCacheSize()
    {
        return cacheSize_;
    }

    __aicore__ inline void ReduceCacheSize()
    {
        --cacheSize_;
    }

    TQue<TPosition::A1, 1, 1> cacheQue_;
    TBuffAddr cacheHead_;
    GM_ADDR srcAddr_;
    int32_t cacheSize_;
    bool alloc_;
};

}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif // _GLOBAL_CACHE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_IN_BUFFER_GLOBAL_CACHE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_RESOURCE_CUBE_IN_BUFFER_GLOBAL_CACHE_H__
#endif