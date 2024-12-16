# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Encoder.

add_executable(sstv_encoder encoder.cc)

target_link_libraries(sstv_encoder
  radio_core_picture_sstv
  external_stb
  external_tiny_lib
  Argparse::argparse
)

target_set_output_directory(sstv_encoder ${TOOL_EXECUTABLE_OUTPUT_DIR})

################################################################################
# Decoder.

add_executable(sstv_decoder decoder.cc)

target_link_libraries(sstv_decoder
  radio_core_picture_sstv
  radio_core_tool
  external_stb
  external_tiny_lib
  Argparse::argparse
)

target_set_output_directory(sstv_decoder ${TOOL_EXECUTABLE_OUTPUT_DIR})

################################################################################
# VIS geenrator.

add_executable(sstv_vis_generator vis_generator.cc)

target_link_libraries(sstv_vis_generator
  radio_core_picture_sstv
  external_tiny_lib
  Argparse::argparse
)

target_set_output_directory(sstv_vis_generator ${TOOL_EXECUTABLE_OUTPUT_DIR})
