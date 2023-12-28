# Copyright (c) 2023 radio core authors
#
# SPDX-License-Identifier: MIT-0

# Automatic Picture Transmission (APT) encoders, decoders, utility applications.

################################################################################
# Library.

set(PUBLIC_HEADERS
  encoder.h
  decoder.h
  info.h
  message.h
  result.h
)

add_library(radio_core_picture_apt INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_picture_apt
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_picture_apt INTERFACE
  radio_core_base
  radio_core_math
  radio_core_signal
  radio_core_picture
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/picture/apt
)

################################################################################
# Regression tests.

if(WITH_TESTS)
  add_test(
    NAME radio_core_picture_apt_decode_test
    COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/test/decoder_test.py
        --test_srcdir ${PROJECT_SOURCE_DIR}/data/test/apt
        --report_rootdir=${CMAKE_BINARY_DIR}/test
        --apt_decoder=$<TARGET_FILE:apt_decoder>
  )
endif()

################################################################################
# Tools.

if(WITH_TOOLS)
  add_subdirectory(tool)
endif()
