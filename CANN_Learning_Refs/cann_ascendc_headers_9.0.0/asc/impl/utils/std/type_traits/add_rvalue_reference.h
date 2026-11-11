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
 * \file add_rvalue_reference.h
 * \brief
 */
#ifndef IMPL_STD_ASCENDC_STD_ADD_RVALUE_REFERENCE_IMPL__H
#define IMPL_STD_ASCENDC_STD_ADD_RVALUE_REFERENCE_IMPL__H

#include "is_referenceable.h"

namespace AscendC {
namespace Std {

template <typename Tp, bool = is_referenceable<Tp>::value>
struct AddRvalueReferenceImpl {
    using type = Tp;
};

template <typename Tp>
struct AddRvalueReferenceImpl<Tp, true> {
    using type = Tp&&;
};

template <typename Tp>
using add_rvalue_reference_t  = typename AddRvalueReferenceImpl<Tp>::type;

template <typename Tp>
struct add_rvalue_reference {
    using type = add_rvalue_reference_t<Tp>;
};

}
}

#endif // IMPL_STD_ASCENDC_STD_ADD_RVALUE_REFERENCE_IMPL__H
