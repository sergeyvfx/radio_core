# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Decoder.

add_executable(aprs_decoder decoder.cc)

target_link_libraries(aprs_decoder
  radio_core_protocol_packet_aprs
  radio_core_tool
  external_tiny_lib
  Argparse::argparse
)

target_set_output_directory(aprs_decoder ${TOOL_EXECUTABLE_OUTPUT_DIR})

################################################################################
# Encoder.

add_executable(aprs_encoder encoder.cc)

target_link_libraries(aprs_encoder
  radio_core_protocol_packet_aprs
  external_tiny_lib
  Argparse::argparse
)

target_set_output_directory(aprs_encoder ${TOOL_EXECUTABLE_OUTPUT_DIR})
