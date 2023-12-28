// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/color_gradient.h"

#include "radio_core/unittest/test.h"

#include <array>

namespace radio_core {

TEST(math_color, RasterizeGradient) {
  std::array<GradientPoint<Color3f>, 2> points = {{
      {0.0f, Color3f(0.1f, 0.2f, 0.3f)},
      {1.0f, Color3f(0.6f, 0.5f, 0.4f)},
  }};

  std::array<Color3f, 3> pixels;
  RasterizeGradient<Color3f>(points, pixels);

  EXPECT_NEAR(pixels[0].rgb.r, 0.1f, 1e-6f);
  EXPECT_NEAR(pixels[0].rgb.g, 0.2f, 1e-6f);
  EXPECT_NEAR(pixels[0].rgb.b, 0.3f, 1e-6f);

  EXPECT_NEAR(pixels[1].rgb.r, 0.35f, 1e-6f);
  EXPECT_NEAR(pixels[1].rgb.g, 0.35f, 1e-6f);
  EXPECT_NEAR(pixels[1].rgb.b, 0.35f, 1e-6f);

  EXPECT_NEAR(pixels[2].rgb.r, 0.6f, 1e-6f);
  EXPECT_NEAR(pixels[2].rgb.g, 0.5f, 1e-6f);
  EXPECT_NEAR(pixels[2].rgb.b, 0.4f, 1e-6f);
}

TEST(math_color, GradientSampleNearest) {
  const std::array<Color3f, 3> pixels = {{
      Color3f(0.1f, 0.2f, 0.3f),
      Color3f(0.35f, 0.35f, 0.35f),
      Color3f(0.6f, 0.5f, 0.4f),
  }};

  {
    const Color3f c = GradientSampleNearest<Color3f>(pixels, 0.1f);
    EXPECT_NEAR(c.rgb.r, 0.1f, 1e-6f);
    EXPECT_NEAR(c.rgb.g, 0.2f, 1e-6f);
    EXPECT_NEAR(c.rgb.b, 0.3f, 1e-6f);
  }

  {
    const Color3f c = GradientSampleNearest<Color3f>(pixels, 0.35f);
    EXPECT_NEAR(c.rgb.r, 0.35f, 1e-6f);
    EXPECT_NEAR(c.rgb.g, 0.35f, 1e-6f);
    EXPECT_NEAR(c.rgb.b, 0.35f, 1e-6f);
  }

  {
    const Color3f c = GradientSampleNearest<Color3f>(pixels, 0.9f);
    EXPECT_NEAR(c.rgb.r, 0.6f, 1e-6f);
    EXPECT_NEAR(c.rgb.g, 0.5f, 1e-6f);
    EXPECT_NEAR(c.rgb.b, 0.4f, 1e-6f);
  }
}

}  // namespace radio_core
