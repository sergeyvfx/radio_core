# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

add_library(radio_core_test_main
  internal/memory_file_writer.cc

  internal/test.cc
  internal/test_main.cc

  internal/test-internal.h

  memory_file_writer.h
  mock.h
  mock_unittest.h
  test.h
)

target_link_libraries(radio_core_test_main
  GTest::gtest
  GTest::gmock
  gflags::gflags
)

################################################################################
# Regression tests.

function(radio_core_unittest_test PRIMITIVE_NAME)
  radio_core_test(
      signal_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc)
endfunction()

radio_core_test(unittest internal/unittest_test.cc)

radio_core_unittest_test(memory_file_writer)
