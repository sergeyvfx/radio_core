# Copyright (c) 2024 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

add_library(radio_core_tool INTERFACE
  buffered_wav_reader.h
  buffered_wav_writer.h
  log_util.h
)

target_link_libraries(radio_core_tool
 INTERFACE
  external_tiny_lib
)

################################################################################
# Regression tests.

function(radio_core_tool_test PRIMITIVE_NAME)
  radio_core_test(
      tool_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_tool)
endfunction()

radio_core_tool_test(buffered_wav_reader)
radio_core_tool_test(buffered_wav_writer)
radio_core_tool_test(log_util)
