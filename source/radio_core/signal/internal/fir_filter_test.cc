// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/fir_filter.h"

#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(FIRFilter, ImpulseResponse_Scalar) {
  std::array<float, 5> kernel = {{0.1f, 0.2f, 0.3f, 0.25f, 0.15f}};
  FIRFilter<float, float> filter(kernel);

  EXPECT_NEAR(filter(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(filter(1.0f), 0.1f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.2f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.3f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.25f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.15f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.0f, 1e-6f);
}

TEST(FIRFilter, ImpulseResponse_Block) {
  const std::array<float, 5> kernel = {{0.1f, 0.2f, 0.3f, 0.25f, 0.15f}};

  // clang-format off
  const auto input_samples = std::to_array({
      // Impulse 1.
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      // Impulse 2.
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  });
  // clang-format on

  FIRFilter<float, float> filter(kernel);
  std::array<float, input_samples.size()> output_samples;

  const std::span<float> filtered_samples =
      filter(input_samples, output_samples);

  EXPECT_EQ(filtered_samples.size(), 16);

  // clang-format off
  EXPECT_THAT(filtered_samples,
              Pointwise(FloatNear(1e-6f),
                        std::to_array<float>({
                            // Impulse 1.
                            0.0f, 0.0f, 0.1f, 0.2f, 0.3f, 0.25f, 0.15f, 0.0f,
                            // Impulse 2.
                            0.0f, 0.0f, 0.1f, 0.2f, 0.3f, 0.25f, 0.15f, 0.0f,
                        })));
  // clang-format on

  // DO a round of simple scalar round as well, to ensure the block left the
  // filter in a proper state.
  EXPECT_NEAR(filter(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(filter(1.0f), 0.1f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.2f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.3f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.25f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.15f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.0f, 1e-6f);
}

}  // namespace radio_core::signal
