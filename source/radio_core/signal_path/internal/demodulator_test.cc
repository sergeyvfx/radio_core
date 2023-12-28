// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal_path/internal/demodulator.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal_path::internal {

// Compilation check to ensure no warnings or compilation are generated.
TEST(Demodulator, Compilation) {
  Demodulator<float> demodulator;

  Demodulator<float>::Options options;
  demodulator.Configure(options);
}

}  // namespace radio_core::signal_path::internal
