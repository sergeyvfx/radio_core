# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  frame.h
)

add_library(radio_core_protocol_datalink INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_protocol_datalink
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

             radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/protocol/datalink
)

function(radio_core_datalink_test PRIMITIVE_NAME)
  radio_core_test(
      protocol_datalink_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_protocol_datalink)
endfunction()

radio_core_datalink_test(frame)

add_subdirectory(hdlc)
add_subdirectory(ax25)
