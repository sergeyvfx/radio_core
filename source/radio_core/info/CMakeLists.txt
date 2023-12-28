# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  internal/bands_itu1.h

  band_plan.h
)

add_library(radio_core_info INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_info PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_info
 INTERFACE
  radio_core_base
  radio_core_modulation_analog
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/info
)

################################################################################
# Regression tests.

function(radio_core_info_test PRIMITIVE_NAME)
  radio_core_test(
      info_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_info)
endfunction()

radio_core_info_test(band_plan)
