# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Policies.

cmake_minimum_required(VERSION 3.19)

################################################################################
# Options.

option(WITH_TOOLS "Enable building command line tools" ON)
option(WITH_DEBUG_TOOLS
       "Enable building command line tools used for development and debug" OFF)

option(WITH_TESTS "Build the unit tests" ON)
option(WITH_TESTS_TNC_CD "Enable tests using TNC Test CD Data" OFF)

option(WITH_BENCHMARKS "Enable building benchmark applications" OFF)
option(WITH_BENCHMARKS_VOLK "Enable benchmarking against Volk library" OFF)

# Development options.
# Recommended for use by all developers.
option(WITH_DEVELOPER_STRICT
       "Use extra strict compilation flags such as -Werror" OFF)
option(WITH_DEVELOPER_SANITIZER
       "Use address sanitizer to validate code behavior" OFF)
option(WITH_DEVELOPER_CLANG_TIDY "Enable Clang-Tidy" OFF)

################################################################################
# CMake module path configuration and global includes.

list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/source/cmake
  ${CMAKE_SOURCE_DIR}/source/cmake/module
)

################################################################################
# Project initialization and configuration.

include(build_initialize)

project(radio_core C CXX)

include(build_configure)

################################################################################
# Sources.

add_subdirectory(source)
