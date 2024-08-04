# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

# Utility functions for defining regression and benchmark targets.

# Define a benchmark target.
#
# The benchmark target is an executable application which times an algorithm and
# prints the benchmark result.
#
# If the benchmarks are disabled as per CMake configuration this function has no
# affect.
# Otherwise a new target named BENCHMARK_NAME is created, and it compiles the
# given file FILENAME. It is possible to provide extra compiler definitions and
# libraries to be linked against by using DEFINITIONS and LIBRARIES "keywords".
#
# Example:
#   radio_core_benchmark(my_benchmark internal/my_benchmark.cc
#                        LIBRARIES my_library)
function(radio_core_benchmark BENCHMARK_NAME FILENAME)
  if(NOT WITH_BENCHMARKS)
    return()
  endif()

  cmake_parse_arguments(
    BENCHMARK
    ""
    ""
    "LIBRARIES;DEFINITIONS"
    ${ARGN}
  )

  set(target_name "radio_core_${BENCHMARK_NAME}_benchmark")

  add_executable(${target_name} ${FILENAME})

  target_compile_definitions(${target_name} PRIVATE ${BENCHMARK_DEFINITIONS})

  target_link_libraries(${target_name}
    radio_core_benchmark
    ${BENCHMARK_LIBRARIES}
  )

  if(WITH_BENCHMARKS_VOLK)
    target_link_libraries(${target_name} Volk::volk)
    target_compile_definitions(${target_name} PRIVATE WITH_BENCHMARKS_VOLK)
  endif()

  # Make sure test is created in it's own dedicated directory.
  target_set_output_directory(${target_name} ${BENCHMARK_EXECUTABLE_OUTPUT_DIR})
endfunction()
