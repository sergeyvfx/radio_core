// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/resample.h"

#include <array>
#include <vector>

#include "radio_core/math/kernel/horizontal_max.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::FloatNear;
using testing::Pointwise;

TEST(math, Resample) {
  // Empty input.
  {
    std::vector<float> resampled;
    ForeachResampledValue<float>(
        {}, 4, kernel::HorizontalMax<float>, [&](const float sample) {
          resampled.push_back(sample);
        });
    EXPECT_TRUE(resampled.empty());
  }

  // Resample with 1:1 ratio.
  {
    std::vector<float> resampled;
    ForeachResampledValue<float>(
        {{0, 1, 2, 3}},
        4,
        kernel::HorizontalMax<float>,
        [&](const float sample) { resampled.push_back(sample); });
    EXPECT_THAT(
        resampled,
        Pointwise(FloatNear(1e-6f), std::to_array<float>({0, 1, 2, 3})));
  }

  // Downsample with ratio of 2.
  {
    std::vector<float> resampled;
    ForeachResampledValue<float>(
        {{0, 1, 2, 3, 4, 5, 6, 7}},
        4,
        kernel::HorizontalMax<float>,
        [&](const float sample) { resampled.push_back(sample); });
    EXPECT_THAT(
        resampled,
        Pointwise(FloatNear(1e-6f), std::to_array<float>({1, 3, 5, 7})));
  }

  // Downsample with ratio of 1.6.
  {
    std::vector<float> resampled;
    ForeachResampledValue<float>(
        {{0, 1, 2, 3, 4, 5, 6, 7}},
        6,
        kernel::HorizontalMax<float>,
        [&](const float sample) { resampled.push_back(sample); });
    EXPECT_THAT(
        resampled,
        Pointwise(FloatNear(1e-6f), std::to_array<float>({0, 1, 3, 4, 5, 7})));
  }

  // Downsample with ratio slightly larger than 2.
  {
    std::vector<float> resampled;
    ForeachResampledValue<float>(
        {{0, 1, 2, 3, 4, 5, 6, 7, 8}},
        4,
        kernel::HorizontalMax<float>,
        [&](const float sample) { resampled.push_back(sample); });
    EXPECT_THAT(
        resampled,
        Pointwise(FloatNear(1e-6f), std::to_array<float>({1, 3, 5, 8})));
  }

  // Upsample with ratio of 2.
  {
    std::vector<float> resampled;
    ForeachResampledValue<float>(
        {{0, 1, 2, 3}},
        8,
        kernel::HorizontalMax<float>,
        [&](const float sample) { resampled.push_back(sample); });
    EXPECT_THAT(resampled,
                Pointwise(FloatNear(1e-6f),
                          std::to_array<float>({0, 0, 1, 1, 2, 2, 3, 3})));
  }

  // Upsample with non-integer ratio of 1.5.
  {
    std::vector<float> resampled;
    ForeachResampledValue<float>(
        {{0, 1, 2}}, 5, kernel::HorizontalMax<float>, [&](const float sample) {
          resampled.push_back(sample);
        });
    EXPECT_THAT(
        resampled,
        Pointwise(FloatNear(1e-6f), std::to_array<float>({0, 0, 1, 1, 2})));
  }

  // Upsample with ratio slightly larger than 2.
  {
    std::vector<float> resampled;
    ForeachResampledValue<float>(
        {{0, 1, 2, 3}},
        9,
        kernel::HorizontalMax<float>,
        [&](const float sample) { resampled.push_back(sample); });
    EXPECT_THAT(resampled,
                Pointwise(FloatNear(1e-6f),
                          std::to_array<float>({0, 0, 0, 1, 1, 2, 2, 3, 3})));
  }
}

}  // namespace radio_core
