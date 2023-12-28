# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

add_library(radio_core_test_main
  internal/test.cc
  internal/test_main.cc

  internal/test-internal.h

  mock.h
  mock_unittest.h
  test.h
)

target_link_libraries(radio_core_test_main
  GTest::gtest
  GTest::gmock
  gflags::gflags
)

radio_core_test(unittest internal/unittest_test.cc)
