# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  algorithm.h
  aligned_register.h
  bit_cast.h
  build_config.h
  byte_util.h
  compiler_specific.h
  constants.h
  container.h
  convert.h
  ctype.h
  exception.h
  format.h
  frequency.h
  frequency_duration.h
  half.h
  interval.h
  result.h
  reverse_storage_ring_buffer.h
  reverse_storage_ring_double_buffer.h
  ring_buffer.h
  ring_buffer_stream.h
  scoped_timer.h
  stream_util.h
  string_util.h
  source_location.h
  unreachable.h
  unroll.h
  value_printer.h
  variant.h
  verify.h

  internal/tiny_lib/tl_result.h
  internal/tiny_lib/tl_static_vector.h
  internal/tiny_lib/tl_string_portable.h
)

add_library(radio_core_base INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_base PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

if(Threads_FOUND)
  target_link_libraries(radio_core_base INTERFACE
    Threads::Threads
  )
endif()

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/base
)

################################################################################
# Regression tests.

function(radio_core_base_test PRIMITIVE_NAME)
  radio_core_test(
      base_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_base)
endfunction()

radio_core_base_test(aligned_allocator)
radio_core_base_test(aligned_malloc)
radio_core_base_test(aligned_register)
radio_core_base_test(byte_util)
radio_core_base_test(bit_cast)
radio_core_base_test(build_config)
radio_core_base_test(convert)
radio_core_base_test(container)
radio_core_base_test(ctype)
radio_core_base_test(format)
radio_core_base_test(frequency)
radio_core_base_test(frequency_duration)
radio_core_base_test(half)
radio_core_base_test(interval)
radio_core_base_test(result)
radio_core_base_test(reverse_storage_ring_buffer)
radio_core_base_test(reverse_storage_ring_double_buffer)
radio_core_base_test(ring_buffer)
radio_core_base_test(ring_buffer_stream)
radio_core_base_test(stream_util)
radio_core_base_test(string_util)
radio_core_base_test(source_location)
radio_core_base_test(unreachable)
radio_core_base_test(unroll)
radio_core_base_test(value_printer)
radio_core_base_test(variant)
radio_core_base_test(verify)

################################################################################
# Benchmarks.

function(radio_core_base_benchmark PRIMITIVE_NAME)
  radio_core_benchmark(
      base_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_benchmark.cc
      LIBRARIES radio_core_base
  )
endfunction()

radio_core_base_benchmark(reverse_storage_ring_buffer)
radio_core_base_benchmark(reverse_storage_ring_double_buffer)
