# Copyright (c) 2023 radio core authors
#
# SPDX-License-Identifier: MIT-0

add_executable(signal_path signal_path.cc)

target_link_libraries(signal_path
  radio_core_signal_path
  external_tiny_lib
  Argparse::argparse
)

target_set_output_directory(signal_path ${TOOL_EXECUTABLE_OUTPUT_DIR})
