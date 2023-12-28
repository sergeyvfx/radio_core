// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/colorspace.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(YCbCrToRGB, Basic) {
  {
    const Color3f black_ycc = YCbCrToRGB(Color3f(0.0f, 0.0f, 0.0f));
    EXPECT_NEAR(black_ycc.rgb.r, -0.701f, 1e-4f);
    EXPECT_NEAR(black_ycc.rgb.g, 0.5291f, 1e-4f);
    EXPECT_NEAR(black_ycc.rgb.b, -0.886f, 1e-4f);
  }

  {
    const Color3f white_ycc = YCbCrToRGB(Color3f(1.0f, 1.0f, 1.0f));
    EXPECT_NEAR(white_ycc.rgb.r, 1.701f, 1e-4f);
    EXPECT_NEAR(white_ycc.rgb.g, 0.4709f, 1e-4f);
    EXPECT_NEAR(white_ycc.rgb.b, 1.886f, 1e-4f);
  }

  {
    const Color3f red_mid_ycc = YCbCrToRGB(Color3f(0.5f, 0.26f, 0.26f));
    EXPECT_NEAR(red_mid_ycc.rgb.r, 0.16352f, 1e-4f);
    EXPECT_NEAR(red_mid_ycc.rgb.g, 0.75399f, 1e-4f);
    EXPECT_NEAR(red_mid_ycc.rgb.b, 0.07472f, 1e-4f);
  }
}

}  // namespace radio_core
