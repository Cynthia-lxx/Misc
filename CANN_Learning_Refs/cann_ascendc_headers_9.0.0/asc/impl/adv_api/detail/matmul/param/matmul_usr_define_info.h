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
* \file matmul_usr_define_info.h
* \brief
*/
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/matmul/param/matmul_usr_define_info.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_USR_DEFINE_INFO_H__
#endif

#ifndef IMPL_MATMUL_PARAM_MATMUL_USER_DEFINE_INFO_H
#define IMPL_MATMUL_PARAM_MATMUL_USER_DEFINE_INFO_H

namespace AscendC {
namespace Impl {
namespace Detail {
template <typename IMPL, const auto &MM_CFG, class USER_DEF_DATA_TYPE, typename = void>
class MatmulUserDefineInfo {
public:
    __aicore__ inline void SetSelfDefineData(USER_DEF_DATA_TYPE dataPtr)
    {}

    __aicore__ inline void SetUserDefineInfo(uint64_t tilingPtr)
    {}

    __aicore__ inline USER_DEF_DATA_TYPE GetSelfDefineData() const
    {
        USER_DEF_DATA_TYPE data;
        return data;
    }

    __aicore__ inline uint64_t GetUserDefineInfo() const
    {
        return 0;
    }

private:
    USER_DEF_DATA_TYPE dataPtr_;
    uint64_t tilingPtr_;
};

template <typename IMPL, const auto &MM_CFG, class USER_DEF_DATA_TYPE>
class MatmulUserDefineInfo<IMPL, MM_CFG, USER_DEF_DATA_TYPE, enable_if_t<
    MatmulFeatureTrait<MM_CFG>::IsSupportUserDefine()>> {
public:
    __aicore__ inline void SetSelfDefineData(USER_DEF_DATA_TYPE dataPtr)
    {
        dataPtr_ = dataPtr;
    }

    __aicore__ inline void SetUserDefineInfo(uint64_t tilingPtr)
    {
        tilingPtr_ = tilingPtr;
    }

    __aicore__ inline USER_DEF_DATA_TYPE GetSelfDefineData() const
    {
        return dataPtr_;
    }

    __aicore__ inline uint64_t GetUserDefineInfo() const
    {
        return tilingPtr_;
    }

private:
    USER_DEF_DATA_TYPE dataPtr_;
    uint64_t tilingPtr_;
};
}  // namespace Detail
}  // namespace Impl
}  // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_USR_DEFINE_INFO_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_USR_DEFINE_INFO_H__
#endif // _MATMUL_USER_DEFINE_INFO_H_