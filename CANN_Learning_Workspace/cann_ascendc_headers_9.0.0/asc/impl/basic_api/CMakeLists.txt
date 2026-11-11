# ----------------------------------------------------------------------------
# This program is free software, you can redistribute it and/or modify it.
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This file is a part of the CANN Open Software.
# Licensed under CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------

add_custom_target(hostapi_headers
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/include/ascendc/host_api/tiling
    COMMAND cp -rf ${ASCENDC_DIR}/include/utils/tiling/template_argument.h ${CMAKE_BINARY_DIR}/include/ascendc/host_api/tiling
 )

add_library(tikcfw_headers INTERFACE)
add_dependencies(tikcfw_headers hostapi_headers)
target_include_directories(tikcfw_headers INTERFACE
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/basic_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/basic_api/reg_compute/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/simt_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/impl/c_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/basic_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/simt_api/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/simt_api/cpp/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/basic_api/reg_compute/>
  $<BUILD_INTERFACE:${ASCENDC_DIR}/include/c_api/>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  $<INSTALL_INTERFACE:include/ascendc>
  $<INSTALL_INTERFACE:include/ascendc/include>
  $<INSTALL_INTERFACE:include/ascendc/include/basic_api>
  $<INSTALL_INTERFACE:include/asc/include>
  $<INSTALL_INTERFACE:include/asc/include/basic_api>
  $<INSTALL_INTERFACE:include/asc/include/simt_api>
  $<INSTALL_INTERFACE:include/asc/include/simt_api/cpp>
  $<INSTALL_INTERFACE:include/asc/include/basic_api/reg_compute>
  $<INSTALL_INTERFACE:include/asc/include/c_api>
  $<INSTALL_INTERFACE:include/asc/impl>
  $<INSTALL_INTERFACE:include/asc/impl/basic_api>
  $<INSTALL_INTERFACE:include/asc/impl/simt_api>
  $<INSTALL_INTERFACE:include/asc/impl/basic_api/reg_compute>
  $<INSTALL_INTERFACE:include/asc/impl/c_api>
)

install(FILES ${ASCENDC_INCLUDE_DIR}/kernel_operator.h
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include OPTIONAL
)

if(BUILD_OPEN_PROJECT)
  install(DIRECTORY ${ASCENDC_INCLUDE_DIR}/basic_api/op_frame
    DESTINATION ${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api OPTIONAL
  )
endif()

install(DIRECTORY ${ASCENDC_INCLUDE_DIR}/basic_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include OPTIONAL
)
install(DIRECTORY ${ASCENDC_IMPL_DIR}/basic_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl OPTIONAL
)

install(DIRECTORY ${ASCENDC_INCLUDE_DIR}/simt_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/include OPTIONAL
)

install(DIRECTORY ${ASCENDC_IMPL_DIR}/simt_api
  DESTINATION ${INSTALL_LIBRARY_DIR}/asc/impl OPTIONAL
)

file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api")
file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl")
file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw")
file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface")
file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils")
file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/base")

# 1 asc/include/interface
install(CODE "file(CREATE_LINK ../basic_api/reg_compute ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/reg_compute SYMBOLIC)")
install(CODE "file(CREATE_LINK ../simt_api ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/simt_api SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../utils/debug ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/debug SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../utils/common_types.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/common_types.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../utils/base/sys_constants.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/base/sys_constants.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../utils/base/sys_macros.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/utils/base/sys_macros.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/core_mng ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/core_mng SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_common.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_common.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_atomic_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_atomic_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_block_sync_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_block_sync_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_cache_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_cache_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_common_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_common_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_conv2d_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_conv2d_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_data_copy_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_data_copy_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_determine_compute_sync_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_determine_compute_sync_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_dump_tensor_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_dump_tensor_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_fixpipe_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_fixpipe_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_gemm_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_gemm_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_limits_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_limits_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_list_tensor_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_list_tensor_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_mm_bitmode_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_mm_bitmode_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_mm_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_mm_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_proposal_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_proposal_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_scalar_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_scalar_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_set_atomic_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_set_atomic_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_swap_mem_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_swap_mem_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_sys_var_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_sys_var_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_utils_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_utils_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_bilinearinterpolation_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_bilinearinterpolation_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_binary_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_binary_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_binary_scalar_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_binary_scalar_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_brcb_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_brcb_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_cmpsel_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_cmpsel_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_createvecindex_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_createvecindex_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_duplicate_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_duplicate_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_gather_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_gather_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_gather_mask_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_gather_mask_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_mulcast_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_mulcast_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_reduce_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_reduce_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_scatter_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_scatter_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_ternary_scalar_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_ternary_scalar_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_transpose_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_transpose_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_unary_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_unary_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_vconv_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_vconv_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_operator_vec_vpadding_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_operator_vec_vpadding_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_prof_trace_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_prof_trace_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_aipp.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_aipp.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_binary.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_binary.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_brcb.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_brcb.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_conv2d.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_conv2d.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_data_copy.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_data_copy.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_fixpipe.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_fixpipe.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_gather.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_gather.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_mm.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_mm.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_proposal.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_proposal.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_transpose.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_transpose.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_unary.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_unary.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_struct_vdeq.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_struct_vdeq.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_tensor.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_tensor.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_tpipe.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_tpipe.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_type.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_type.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_basic_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_basic_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_cube_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_cube_intf.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../basic_api/kernel_vec_intf.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/asc/include/interface/kernel_vec_intf.h SYMBOLIC)")

# 2 ascendc/include/basic_api
if(NOT BUILD_OPEN_PROJECT)
install(CODE "file(CREATE_LINK ../../../asc/include/basic_api/op_frame ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/op_frame SYMBOLIC)")
endif()
install(CODE "file(CREATE_LINK ../../../asc/include/interface ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/interface SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../asc/include/kernel_operator.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/kernel_operator.h SYMBOLIC)")

install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/reg_compute ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/reg_compute SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/simt_api ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/simt_api SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/common_types.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/common_types.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_c100 ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_c100 SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_c220 ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_c220 SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_c310 ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_c310 SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_m200 ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_m200 SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_m300 ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_m300 SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/dav_m310 ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/dav_m310 SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_check.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_check.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_check_util.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_check_util.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_event.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_event.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_log.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_log.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_macros.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_macros.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_atomic_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_atomic_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_common_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_common_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_conv2d_base_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_conv2d_base_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_conv2d_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_conv2d_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_coord.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_coord.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_data_copy_base_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_data_copy_base_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_data_copy_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_data_copy_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_determine_compute_sync_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_determine_compute_sync_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_dump_tensor_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_dump_tensor_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_fixpipe_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_fixpipe_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_gemm_base_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_gemm_base_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_gemm_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_gemm_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_layout.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_layout.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_list_tensor_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_list_tensor_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_base_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_base_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_check.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_check.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_bitmode_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_bitmode_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_bitmode_struct.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_bitmode_struct.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_mm_load2d_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_mm_load2d_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_proposal_base_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_proposal_base_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_proposal_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_proposal_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_scalar_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_scalar_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_set_atomic_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_set_atomic_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_symbol_override_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_symbol_override_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_sys_var_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_sys_var_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_tensor_trait.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_tensor_trait.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_bilinearinterpolation_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_bilinearinterpolation_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_binary_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_binary_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_binary_scalar_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_binary_scalar_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_brcb_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_brcb_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_cmpsel_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_cmpsel_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_createvecindex_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_createvecindex_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_duplicate_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_duplicate_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_gather_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_gather_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_gather_mask_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_gather_mask_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_mulcast_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_mulcast_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_reduce_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_reduce_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_scatter_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_scatter_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_ternary_scalar_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_ternary_scalar_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_transpose_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_transpose_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_unary_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_unary_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_vconv_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_vconv_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_operator_vec_vpadding_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_operator_vec_vpadding_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_pop_stack_buffer.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_pop_stack_buffer.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_process_lock.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_process_lock.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_prof_trace.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_prof_trace.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_prof_trace_intf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_prof_trace_intf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_reg.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_reg.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_scalar_convert.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_scalar_convert.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_scalar.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_scalar.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tbuf_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tbuf_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tbufpool_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tbufpool_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tensor_base.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tensor_base.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tensor_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tensor_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tpipe_base.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tpipe_base.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tpipe_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tpipe_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tquebind_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tquebind_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_tquesync_impl.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_tquesync_impl.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_type_conversion_utils.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_type_conversion_utils.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_utils.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_utils.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/kernel_utils_base.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/kernel_utils_base.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/basic_api/utils ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/utils SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/utils/sys_macros.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/utils/sys_macros.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/utils/sys_constants.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/utils/sys_constants.h SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../../../asc/impl/utils/common_types.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/ascendc/include/basic_api/impl/utils/common_types.h SYMBOLIC)")

# 3 tikcpp/tikcfw
install(CODE "file(CREATE_LINK ../../ascendc/include/basic_api/impl ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw/impl SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../ascendc/include/basic_api/interface ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw/interface SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../ascendc/include/basic_api/op_frame ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw/op_frame SYMBOLIC)")
install(CODE "file(CREATE_LINK ../../ascendc/include/basic_api/kernel_operator.h ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/tikcpp/tikcfw/kernel_operator.h SYMBOLIC)")

if(NOT BUILD_OPEN_PROJECT)
    install_package(
        PACKAGE tikcfw
        TARGETS tikcfw_headers
        DIRECTORY ${ASCENDC_DIR}/impl/basic_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/impl
        DIRECTORY ${ASCENDC_DIR}/impl/simt_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/impl
        DIRECTORY ${ASCENDC_DIR}/impl/c_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/impl

        DIRECTORY ${ASCENDC_DIR}/include/basic_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include
        DIRECTORY ${ASCENDC_DIR}/include/simt_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include
        FILES ${ASCENDC_DIR}/include/kernel_operator.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include
        DIRECTORY ${ASCENDC_DIR}/include/c_api
        DESTINATION ${INSTALL_INCLUDE_DIR}/asc/include

        FILES ${ASCENDC_DIR}/include/utils/tiling/platform/platform_ascendc.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/highlevel_api/tiling/platform
        FILES ${ASCENDC_DIR}/include/utils/context/context_builder.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/highlevel_api/tiling/context
        FILES ${ASCENDC_DIR}/include/utils/tiling/template_argument.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/highlevel_api/tiling
        FILES ${ASCENDC_DIR}/include/utils/log/asc_cpu_log.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/highlevel_api/utils/log
        FILES ${ASCENDC_DIR}/include/aicpu_api/aicpu_api.h
        DESTINATION ${INSTALL_INCLUDE_DIR}/ascendc/include/aicpu_api
    )
endif()
