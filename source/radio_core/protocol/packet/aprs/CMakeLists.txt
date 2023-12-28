# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  decoder.h
  encoder.h
)

add_library(radio_core_protocol_packet_aprs INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_protocol_packet_aprs
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_protocol_packet_aprs INTERFACE
  radio_core_base
  radio_core_modulation_digital_fsk
  radio_core_protocol_binary_nrzs
  radio_core_protocol_datalink_ax25
  radio_core_protocol_datalink_hdlc
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/protocol/packet/aprs
)

################################################################################
# Regression tests.

function(radio_core_packet_test PRIMITIVE_NAME)
  set(test_definitions)

  if(WITH_TESTS_TNC_CD)
    list(APPEND test_definitions WITH_TESTS_TNC_CD)
  endif()

  radio_core_test(
      protocol_packet_aprs_${PRIMITIVE_NAME}
      internal/${PRIMITIVE_NAME}_test.cc
      DEFINITIONS ${test_definitions}
      LIBRARIES radio_core_protocol_datalink external_tiny_lib
      ${ARGN})
endfunction()

radio_core_packet_test(decoder)
radio_core_packet_test(encoder)

################################################################################
# Tools.

if(WITH_TOOLS)
  add_subdirectory(tool)
endif()
