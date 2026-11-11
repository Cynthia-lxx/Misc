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
 * \file move.h
 * \brief
 */
#ifndef IMPL_STD_ASCENDC_STD_MOVE_IMPL__H
#define IMPL_STD_ASCENDC_STD_MOVE_IMPL__H

#include "../type_traits/remove_reference.h"

namespace AscendC {
namespace Std {

template <typename Tp>
ASCENDC_HOST_AICORE inline constexpr remove_reference_t<Tp>&& move(Tp&& t) noexcept
{
    using Up = remove_reference_t<Tp> ;
    return static_cast<Up&&>(t);
}

}
}

#endif // IMPL_STD_ASCENDC_STD_MOVE_IMPL__H
