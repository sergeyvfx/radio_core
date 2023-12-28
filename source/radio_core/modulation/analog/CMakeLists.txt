# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  iq_demodulator.h

  info.h
  type.h
)

add_library(radio_core_modulation_analog INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_modulation_analog
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_modulation_analog INTERFACE
  radio_core_base
  radio_core_math
  radio_core_signal
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/modulation/analog
)

################################################################################
# Unit tests.

function(radio_core_modulation_test PRIMITIVE_NAME)
  radio_core_test(
      modulation_analog_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_modulation_analog)
endfunction()

radio_core_modulation_test(info)
radio_core_modulation_test(bandwidth)

################################################################################
# Modulation type implementations.

add_subdirectory(am)
add_subdirectory(cw)
add_subdirectory(fm)
add_subdirectory(lsb)
add_subdirectory(nfm)
add_subdirectory(ssb)
add_subdirectory(usb)
add_subdirectory(wfm)
