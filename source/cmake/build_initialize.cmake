# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

# Initialize build system prior the project is defined.
# This script sets up the environment required for the `project()` to perform
# an initial configuration.

set(CMAKE_CXX_STANDARD 20)

# NOTE: Keep prior to project(), as per CMake documentation.
if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
  # Specify the minimum target, if higher version SDK is used weak linking
  # happens.
  set(CMAKE_OSX_DEPLOYMENT_TARGET "11.00" CACHE STRING "" FORCE)
endif()
