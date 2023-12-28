// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/hilbert.h"

#include <array>

#include "radio_core/signal/window.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(GenerateWindowedHilbertTransformer, Basic) {
  std::array<float, 51> actual_kernel;
  GenerateWindowedHilbertTransformer<float>(
      actual_kernel, WindowEquation<float, Window::kBoxcar>());

  // The values are obtained by carefully verifying formula and running it prior
  // to any optimization.

  EXPECT_THAT(
      actual_kernel,
      Pointwise(FloatNear(1e-6f),
                std::to_array({
                    -0.02546479f, -0.00000000f, -0.02767912f, -0.00000000f,
                    -0.03031523f, -0.00000000f, -0.03350630f, -0.00000000f,
                    -0.03744822f, -0.00000000f, -0.04244132f, -0.00000000f,
                    -0.04897075f, -0.00000000f, -0.05787452f, -0.00000000f,
                    -0.07073553f, -0.00000000f, -0.09094568f, -0.00000000f,
                    -0.12732396f, -0.00000000f, -0.21220659f, -0.00000000f,
                    -0.63661975f, 0.00000000f,  0.63661975f,  0.00000000f,
                    0.21220659f,  0.00000000f,  0.12732396f,  0.00000000f,
                    0.09094568f,  0.00000000f,  0.07073553f,  0.00000000f,
                    0.05787452f,  0.00000000f,  0.04897075f,  0.00000000f,
                    0.04244132f,  0.00000000f,  0.03744822f,  0.00000000f,
                    0.03350630f,  0.00000000f,  0.03031523f,  0.00000000f,
                    0.02767912f,  0.00000000f,  0.02546479f,
                })));
}

}  // namespace radio_core::signal
