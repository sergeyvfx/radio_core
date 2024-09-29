# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  internal/decimation_ratio.h
  internal/demodulator.h
  internal/receive_filter.h

  async_sink.h
  base_signal_path.h
  simple_signal_path.h
  sink.h
  sink_collection.h
  sink_method_wrapper.h
)

add_library(radio_core_signal_path INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_signal_path
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_signal_path INTERFACE
  radio_core_base
  radio_core_modulation_analog
  radio_core_signal
)

if(Threads_FOUND)
  target_link_libraries(radio_core_signal_path INTERFACE
    Threads::Threads
  )
endif()

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/signal_path
)

function(radio_core_signal_path_test PRIMITIVE_NAME)
  radio_core_test(
      signal_path_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_signal_path)
endfunction()

radio_core_signal_path_test(async_sink)
radio_core_signal_path_test(decimation_ratio)
radio_core_signal_path_test(demodulator)
radio_core_signal_path_test(simple_signal_path)
radio_core_signal_path_test(sink_collection)
radio_core_signal_path_test(sink_method_wrapper)
radio_core_signal_path_test(receive_filter)

################################################################################
# Tools.

if(WITH_TOOLS)
  add_subdirectory(tool)
endif()
