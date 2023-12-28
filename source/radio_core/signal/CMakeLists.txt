# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  analytical_signal.h
  awgn_noise_injector.h
  dc_blocker.h
  debug_writer.h
  decimator.h
  digital_hysteresis.h
  digital_pll.h
  edge_detector.h
  ema_agc.h
  filter.h
  filter_design.h
  filter_gain.h
  filter_window_heuristic.h
  fir_filter.h
  frequency.h
  frequency_shifter.h
  generator.h
  hilbert.h
  hysteresis.h
  instant_frequency.h
  instant_phase.h
  integer_delay.h
  interpolator.h
  local_oscillator.h
  peak_detector.h
  polyphase_filter.h
  root_raised_cosine.h
  simple_fir_filter.h
  window.h
)

add_library(radio_core_signal INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_signal PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_signal INTERFACE
  radio_core_base
  radio_core_math
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/signal
)

################################################################################
# Regression tests.

function(radio_core_signal_test PRIMITIVE_NAME)
  radio_core_test(
      signal_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_signal)
endfunction()

radio_core_signal_test(analytical_signal)
radio_core_signal_test(awgn_noise_injector)
radio_core_signal_test(dc_blocker)
radio_core_signal_test(decimator)
radio_core_signal_test(digital_hysteresis)
radio_core_signal_test(digital_pll)
radio_core_signal_test(edge_detector)
radio_core_signal_test(ema_agc)
radio_core_signal_test(filter)
radio_core_signal_test(filter_design)
radio_core_signal_test(filter_window_heuristic)
radio_core_signal_test(fir_filter)
radio_core_signal_test(frequency)
radio_core_signal_test(frequency_shifter)
radio_core_signal_test(generator)
radio_core_signal_test(hilbert)
radio_core_signal_test(hysteresis)
radio_core_signal_test(instant_frequency)
radio_core_signal_test(instant_phase)
radio_core_signal_test(integer_delay)
radio_core_signal_test(interpolator)
radio_core_signal_test(local_oscillator)
radio_core_signal_test(peak_detector)
radio_core_signal_test(polyphase_filter)
radio_core_signal_test(root_raised_cosine)
radio_core_signal_test(simple_fir_filter)
radio_core_signal_test(window)

################################################################################
# Benchmarks.

function(radio_core_signal_benchmark PRIMITIVE_NAME)
  radio_core_benchmark(
      signal_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_benchmark.cc
      LIBRARIES radio_core_signal
  )
endfunction()

radio_core_signal_benchmark(decimator)

################################################################################
# Tools.

if(WITH_TOOLS OR WITH_DEBUG_TOOLS)
  add_subdirectory(tool)
endif()
