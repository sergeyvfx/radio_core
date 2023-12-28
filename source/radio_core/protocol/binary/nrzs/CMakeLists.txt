# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  decoder.h
  encoder.h
)

add_library(radio_core_protocol_binary_nrzs INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_protocol_binary_nrzs
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

             radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/protocol/binary/nrzs
)

function(radio_core_binary_test PRIMITIVE_NAME)
  radio_core_test(
      protocol_binary_nrss_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_protocol_binary_nrzs)
endfunction()

radio_core_binary_test(decoder)
radio_core_binary_test(encoder)
