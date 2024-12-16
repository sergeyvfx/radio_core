# Copyright (c) 2023 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Encoder.

add_executable(apt_encoder encoder.cc)

target_link_libraries(apt_encoder
  radio_core_picture_apt
  external_stb
  external_tiny_lib
  Argparse::argparse
)

target_set_output_directory(apt_encoder ${TOOL_EXECUTABLE_OUTPUT_DIR})

################################################################################
# Decoder.

add_executable(apt_decoder decoder.cc)

target_link_libraries(apt_decoder
  radio_core_picture_apt
  radio_core_tool
  external_stb
  external_tiny_lib
  Argparse::argparse
)

target_set_output_directory(apt_decoder ${TOOL_EXECUTABLE_OUTPUT_DIR})
