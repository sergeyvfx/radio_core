# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

add_subdirectory(base)
add_subdirectory(crypto)
add_subdirectory(info)
add_subdirectory(math)
add_subdirectory(picture)
add_subdirectory(modulation)
add_subdirectory(protocol)
add_subdirectory(signal)
add_subdirectory(signal_path)

if(WITH_BENCHMARKS)
  add_subdirectory(benchmark)
endif()

if(WITH_TESTS)
  add_subdirectory(unittest)
endif()

add_subdirectory(library)
