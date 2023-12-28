// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal_path/internal/receive_filter.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal_path::internal {

// Compilation check to ensure no warnings or compilation are generated.
TEST(ReceiveFilter, Compilation) {
  ReceiveFilter<float> receive_filter;

  ReceiveFilter<float>::Options options;
  receive_filter.Configure(options);
}

}  // namespace radio_core::signal_path::internal
