# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  internal/matrix.h
  internal/vector.h

  numeric.h
)

add_library(radio_core_numeric INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_numeric PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_numeric INTERFACE
  radio_core_math
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/numeric
)

################################################################################
# Regression tests.

function(radio_core_numeric_test PRIMITIVE_NAME)
  radio_core_test(
      math_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_numeric)
endfunction()

radio_core_numeric_test(matrix)
radio_core_numeric_test(numeric)
radio_core_numeric_test(vector)
