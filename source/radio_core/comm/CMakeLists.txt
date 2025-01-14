# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  digital_pll.h
)

add_library(radio_core_comm INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_comm PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_comm INTERFACE
  radio_core_base
  radio_core_math
  radio_core_signal
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/comm
)

################################################################################
# Regression tests.

function(radio_core_comm_test PRIMITIVE_NAME)
  radio_core_test(
      comm_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_comm)
endfunction()

radio_core_comm_test(digital_pll)
