# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Base utility library.

set(PUBLIC_HEADERS
  pixel_accessor.h
  memory_pixel_accessor.h
)

add_library(radio_core_picture INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_picture
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_picture INTERFACE
  radio_core_math
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/picture
)

################################################################################
# Unit tests.

function(radio_core_picture_test PRIMITIVE_NAME)
  radio_core_test(
      picture_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_picture)
endfunction()

radio_core_picture_test(memory_pixel_accessor)

################################################################################
# Protocol and modulation implementations.

add_subdirectory(apt)
add_subdirectory(sstv)
