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
 * \file conv3d_common_sub_api.h
 * \brief
 */

#ifndef API_CONV3D_COMMON_SUB_API_H
#define API_CONV3D_COMMON_SUB_API_H

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
    #include "dav_m220/conv3d_sub_api.h"
    #include "dav_m220/conv3d_pointwise_sub_api.h"
    #include "dav_m220/conv3d_groupopt_sub_api.h"
#endif
#endif // __API_CONV3D_COMMON_SUB_API_H__
