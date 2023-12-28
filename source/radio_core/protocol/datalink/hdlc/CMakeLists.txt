# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  decoder.h
  encoder.h
  spec.h
)

add_library(radio_core_protocol_datalink_hdlc INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_protocol_datalink_hdlc
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_protocol_datalink_hdlc INTERFACE
  radio_core_base
  radio_core_protocol_datalink
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/protocol/datalink/hdlc
)

function(radio_core_datalink_test PRIMITIVE_NAME)
  radio_core_test(
      protocol_datalink_hdlc_${PRIMITIVE_NAME}
      internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_protocol_datalink_hdlc)
endfunction()

radio_core_datalink_test(decoder)
radio_core_datalink_test(encoder)
