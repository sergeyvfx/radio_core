# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

# Extrernal external libraries which sources are not boundled with this project.

# TODO(sergey): Support building with system-wide librarie for Goggle Test.

# Configure Threads.
find_package(Threads REQUIRED)

# Python for regression tests.
if(WITH_TESTS)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
endif()

# Volk.
if(WITH_BENCHMARKS_VOLK)
  find_package(Volk REQUIRED)
endif()
