# Copyright (c) 2023 radio core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  info.h
  demodulator.h
)

add_library(radio_core_modulation_analog_ssb INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_modulation_analog_ssb
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_modulation_analog_ssb INTERFACE
  radio_core_base
  radio_core_math
  radio_core_modulation_analog
  radio_core_signal
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/modulation/analog/ssb
)

function(radio_core_modulation_test PRIMITIVE_NAME)
  radio_core_test(
      modulation_analog_ssb_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_modulation_analog_ssb)
endfunction()

radio_core_modulation_test(demodulator)
