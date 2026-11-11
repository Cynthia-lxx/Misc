/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/adv_api/detail/api_check/kernel_check/sort/mrgsort/mrgsort_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/sort.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MRGSORT_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_SORT_MRGSORT_MRGSORT_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_SORT_MRGSORT_MRGSORT_CHECK_COMMON_H_

#include <cstdint>
#include "../../basic_check/datatype_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
constexpr uint8_t ONE_PROPOSAL_SIZE = 8;
constexpr uint8_t MRG_SORT_ELEMENT_LEN = 4;
constexpr uint8_t VALID_BIT_0 = 3;
constexpr uint8_t VALID_BIT_1 = 7;
constexpr uint8_t VALID_BIT_2 = 15;
constexpr uint16_t MAX_ELEMENT_COUNT = 4095;

class CheckMrgSortParamsClass {
public:
    __aicore__ inline CheckMrgSortParamsClass()
    {}
    __aicore__ inline CheckMrgSortParamsClass(__gm__ const char* apiName)
    {
        this->apiName = apiName;
    }

public:
    template <typename T, bool isExhaustedSuspension = false>
    __aicore__ inline void CheckMrgSortParams(const LocalTensor<T>& dst, const MrgSortSrcList<T>& sortList,
                                              const uint16_t elementCountList[4], uint32_t sortedNum[4],
                                              uint16_t validBit, const int32_t repeatTime)
    {
        VerifyingParameters<T, isExhaustedSuspension>(dst, sortList, elementCountList, sortedNum, validBit, repeatTime);

        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, isExhaustedSuspension>(dst, sortList, elementCountList, sortedNum, validBit, repeatTime);
        }
    }

private:
    template <typename T, bool isExhaustedSuspension = false>
    __aicore__ inline bool CheckRepeatTimeValid(const MrgSortSrcList<T>& sortList, const uint16_t elementCountList[4],
                                                uint16_t validBit, uint8_t elementSize)
    {
        // validBit = 15 && isExhaustedSuspension = false
        if (validBit != VALID_BIT_2 || isExhaustedSuspension) {
            return false;
        }
        // 4 region proposals has same lengths
        bool cond1 = (elementCountList[0] == elementCountList[1]) && (elementCountList[1] == elementCountList[2])
                     && (elementCountList[2] == elementCountList[3]);

        // continuous stored
        uint64_t src0Addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(sortList.src1.GetPhyAddr()));
        uint64_t src1Addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(sortList.src2.GetPhyAddr()));
        uint64_t src2Addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(sortList.src3.GetPhyAddr()));
        uint64_t src3Addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(sortList.src4.GetPhyAddr()));

        bool cond2 = ((src0Addr + elementCountList[0] * elementSize) == src1Addr)
                     && ((src1Addr + elementCountList[1] * elementSize) == src2Addr)
                     && ((src2Addr + elementCountList[2] * elementSize) == src3Addr);

        return cond1 && cond2;
    }

    template <typename T, bool isExhaustedSuspension = false>
    __aicore__ inline void VerifyingParametersCommonVersion(const LocalTensor<T>& dst, const MrgSortSrcList<T>& sortList,
                                                            const uint16_t elementCountList[4], uint32_t sortedNum[4],
                                                            uint16_t validBit, const int32_t repeatTime)
    {
        ASCENDC_ASSERT(((validBit == VALID_BIT_0 || validBit == VALID_BIT_1 || validBit == VALID_BIT_2)
                        || HighLevelAPIParametersPrint),
                       {
                           KERNEL_LOG(KERNEL_ERROR,
                                      "[%s] Failed to check validBit value, its valid value is %u / %u / %u, current "
                                      "value is %u.",
                                      this->apiName, VALID_BIT_0, VALID_BIT_1, VALID_BIT_2, validBit);
                       });

        for (uint8_t i = 0; i < MRG_SORT_ELEMENT_LEN; i++) {
            ASCENDC_ASSERT(((elementCountList[i] >= 0 && elementCountList[i] <= MAX_ELEMENT_COUNT)
                            || HighLevelAPIParametersPrint),
                           {
                               KERNEL_LOG(KERNEL_ERROR, "[%s] The elementCountList[%u] is %u, should in range [0, %u].",
                                          this->apiName, i, elementCountList[i], MAX_ELEMENT_COUNT);
                           });
        }
    }

    template <typename T, bool isExhaustedSuspension = false>
    __aicore__ inline void VerifyingParametersVersion220(const LocalTensor<T>& dst, const MrgSortSrcList<T>& sortList,
                                                         const uint16_t elementCountList[4], uint32_t sortedNum[4],
                                                         uint16_t validBit, const int32_t repeatTime)
    {
        if (CheckRepeatTimeValid<T, isExhaustedSuspension>(sortList, elementCountList, validBit, ONE_PROPOSAL_SIZE)) {
            ASCENDC_ASSERT(((repeatTime >= 1 && repeatTime <= MAX_REPEAT_TIMES) || HighLevelAPIParametersPrint), {
                KERNEL_LOG(KERNEL_ERROR, "[%s] The repeatTime is %d, should in range [1, %u].", this->apiName,
                           repeatTime, MAX_REPEAT_TIMES);
            });
        }
    }

    template <typename T, bool isExhaustedSuspension = false>
    __aicore__ inline void VerifyingParametersVersion200(const LocalTensor<T>& dst, const MrgSortSrcList<T>& sortList,
                                                         const uint16_t elementCountList[4], uint32_t sortedNum[4],
                                                         uint16_t validBit, const int32_t repeatTime)
    {
        if (CheckRepeatTimeValid<T, isExhaustedSuspension>(sortList, elementCountList, validBit, ONE_PROPOSAL_SIZE * sizeof(T))) {
            ASCENDC_ASSERT(((repeatTime >= 1 && repeatTime <= MAX_REPEAT_TIMES) || HighLevelAPIParametersPrint), {
                KERNEL_LOG(KERNEL_ERROR, "[%s] The repeatTime is %d, should in range [1, %u].", this->apiName,
                           repeatTime, MAX_REPEAT_TIMES);
            });
        }
    }

    template <typename T, bool isExhaustedSuspension = false, typename... Args>
    __aicore__ inline void VerifyingParameters(Args... args)
    {
        VerifyingParametersCommonVersion<T, isExhaustedSuspension>(args...);
        if constexpr (__NPU_ARCH__ == 2201) {
            VerifyingParametersVersion220<T, isExhaustedSuspension>(args...);
        } else if constexpr (__NPU_ARCH__ == 2002) {
            VerifyingParametersVersion200<T, isExhaustedSuspension>(args...);
        }
    }

    template <typename T, bool isExhaustedSuspension = false>
    __aicore__ inline void PrintParameters(const LocalTensor<T>& dst, const MrgSortSrcList<T>& sortList,
                                           const uint16_t elementCountList[4], uint32_t sortedNum[4], uint16_t validBit,
                                           const int32_t repeatTime)
    {
        KERNEL_LOG(KERNEL_INFO, "[%s] The repeatTime is %d, validBit is %u.", this->apiName, repeatTime, validBit);
    }

private:
    __gm__ const char* apiName = nullptr;
};

template <typename T, bool isExhaustedSuspension = false>
class CheckFuncClassMrgSort : public DataTypeCheckFuncBasicClass,
                              public ReuseSourceCheckFuncBasicClass,
                              public SingleTensorCheckFuncBasicClass,
                              public MultipleTensorCheckFuncBasicClass,
                              public CheckMrgSortParamsClass {
public:
    __aicore__ inline CheckFuncClassMrgSort()
    {}
    __aicore__ inline CheckFuncClassMrgSort(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName), ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName), MultipleTensorCheckFuncBasicClass(apiName),
          CheckMrgSortParamsClass(apiName)
    {}

public:
    __aicore__ inline void VerifyingParameters(const LocalTensor<T>& dst, const MrgSortSrcList<T>& sortList,
                                               const uint16_t elementCountList[4], uint32_t sortedNum[4],
                                               uint16_t validBit, const int32_t repeatTime)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>("template parameter (T) is not half "
                                                                                 "or float");

        CheckMrgSortParamsClass::CheckMrgSortParams<T, isExhaustedSuspension>(dst, sortList, elementCountList,
                                                                              sortedNum, validBit, repeatTime);
    }
};

}  // namespace HighLevelApiCheck
}  // namespace AscendC

#endif  // IMPL_API_CHECK_KERNEL_CHECK_SORT_MRGSORT_MRGSORT_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MRGSORT_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MRGSORT_CHECK_COMMON_H__
#endif