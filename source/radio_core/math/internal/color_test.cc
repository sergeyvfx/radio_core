// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/color.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

////////////////////////////////////////////////////////////////////////////////
// Color1.

TEST(Color1, ConvertTo) {
  EXPECT_EQ(Color1ub(127).ConvertTo<Color1f>(), Color1f(127.0f / 255));

  EXPECT_EQ(Color1f(127.0f / 255).ConvertTo<Color1ub>(), Color1ub(127));
}

TEST(Color1, UnaryMath) {
  // Add.
  {
    Color1ub color(12);
    color += Color1ub(17);
    EXPECT_EQ(color, Color1ub(29));
  }

  // Subtract.
  {
    Color1ub color(17);
    color -= Color1ub(12);
    EXPECT_EQ(color, Color1ub(5));
  }

  // Multiply.
  {
    Color1ub color(17);
    color *= 3;
    EXPECT_EQ(color, Color1ub(51));
  }

  // Divide.
  {
    Color1ub color(51);
    color /= 3;
    EXPECT_EQ(color, Color1ub(17));
  }
}

TEST(Color1, BinaryMath) {
  // Add.
  EXPECT_EQ(Color1ub(12) + Color1ub(17), Color1ub(29));

  // Subtract.
  EXPECT_EQ(Color1ub(29) - Color1ub(12), Color1ub(17));

  // Multiply.
  EXPECT_EQ(Color1ub(17) * 3, Color1ub(51));

  // Divide.
  EXPECT_EQ(Color1ub(51) / 3, Color1ub(17));
}

////////////////////////////////////////////////////////////////////////////////
// Color3.

TEST(Color3, ConvertTo) {
  EXPECT_EQ(Color3ub(0, 127, 255).ConvertTo<Color3f>(),
            Color3f(0.0, 127.0f / 255, 1.0f));

  EXPECT_EQ(Color3f(0.0, 127.0f / 255, 1.0f).ConvertTo<Color3ub>(),
            Color3ub(0, 127, 255));
}

TEST(Color3, UnaryMath) {
  // Add.
  {
    Color3ub color(12, 23, 34);
    color += Color3ub(17, 28, 39);
    EXPECT_EQ(color, Color3ub(29, 51, 73));
  }

  // Subtract.
  {
    Color3ub color(29, 51, 73);
    color -= Color3ub(17, 28, 39);
    EXPECT_EQ(color, Color3ub(12, 23, 34));
  }

  // Multiply.
  {
    Color3ub color(17, 18, 19);
    color *= 3;
    EXPECT_EQ(color, Color3ub(51, 54, 57));
  }

  // Divide.
  {
    Color3ub color(51, 54, 57);
    color /= 3;
    EXPECT_EQ(color, Color3ub(17, 18, 19));
  }
}

TEST(Color3, BinaryMath) {
  // Add.
  EXPECT_EQ(Color3ub(12, 23, 34) + Color3ub(17, 28, 39), Color3ub(29, 51, 73));

  // Subtract.
  EXPECT_EQ(Color3ub(29, 51, 73) - Color3ub(17, 28, 39), Color3ub(12, 23, 34));

  // Multiply.
  EXPECT_EQ(Color3ub(17, 18, 19) * 3, Color3ub(51, 54, 57));

  // Divide.
  EXPECT_EQ(Color3ub(51, 54, 57) / 3, Color3ub(17, 18, 19));
}

////////////////////////////////////////////////////////////////////////////////
// Color3f.

TEST(Color3f, compare) {
  EXPECT_TRUE(Color3f(0.1f, 0.2f, 0.3f) == Color3f(0.1f, 0.2f, 0.3f));
  EXPECT_FALSE(Color3f(0.1f, 0.2f, 0.3f) == Color3f(0.6f, 0.5f, 0.4f));

  EXPECT_FALSE(Color3f(0.1f, 0.2f, 0.3f) != Color3f(0.1f, 0.2f, 0.3f));
  EXPECT_TRUE(Color3f(0.1f, 0.2f, 0.3f) != Color3f(0.6f, 0.5f, 0.4f));
}

TEST(Color3f, Saturate) {
  EXPECT_EQ(Saturate(Color3f(0.1f, 0.2f, 0.3f)), Color3f(0.1f, 0.2f, 0.3f));

  EXPECT_EQ(Saturate(Color3f(1.1f, 0.2f, 0.3f)), Color3f(1.0f, 0.2f, 0.3f));
  EXPECT_EQ(Saturate(Color3f(0.1f, 1.2f, 0.3f)), Color3f(0.1f, 1.0f, 0.3f));
  EXPECT_EQ(Saturate(Color3f(0.1f, 0.2f, 1.3f)), Color3f(0.1f, 0.2f, 1.0f));

  EXPECT_EQ(Saturate(Color3f(-0.1f, 0.2f, 0.3f)), Color3f(0.0f, 0.2f, 0.3f));
  EXPECT_EQ(Saturate(Color3f(0.1f, -0.2f, 0.3f)), Color3f(0.1f, 0.0f, 0.3f));
  EXPECT_EQ(Saturate(Color3f(0.1f, 0.2f, -0.3f)), Color3f(0.1f, 0.2f, 0.0f));
}

TEST(Color3f, Lerp) {
  {
    const Color3f result =
        Lerp(Color3f(0.1f, 0.2f, 0.3f), Color3f(0.6f, 0.5f, 0.4f), 0.0f);

    EXPECT_NEAR(result.rgb.r, 0.1f, 1e-6f);
    EXPECT_NEAR(result.rgb.g, 0.2f, 1e-6f);
    EXPECT_NEAR(result.rgb.b, 0.3f, 1e-6f);
  }

  {
    const Color3f result =
        Lerp(Color3f(0.1f, 0.2f, 0.3f), Color3f(0.6f, 0.5f, 0.4f), 1.0f);

    EXPECT_NEAR(result.rgb.r, 0.6f, 1e-6f);
    EXPECT_NEAR(result.rgb.g, 0.5f, 1e-6f);
    EXPECT_NEAR(result.rgb.b, 0.4f, 1e-6f);
  }

  {
    const Color3f result =
        Lerp(Color3f(0.1f, 0.2f, 0.3f), Color3f(0.6f, 0.5f, 0.4f), 0.25f);

    EXPECT_NEAR(result.rgb.r, 0.225f, 1e-6f);
    EXPECT_NEAR(result.rgb.g, 0.275f, 1e-6f);
    EXPECT_NEAR(result.rgb.b, 0.325f, 1e-6f);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Color4.

TEST(Color4, ConvertTo) {
  EXPECT_EQ(Color4ub(0, 64, 127, 255).ConvertTo<Color4f>(),
            Color4f(0.0, 64.0f / 255, 127.0f / 255, 1.0f));

  EXPECT_EQ(Color4f(0.0, 64.0f / 255, 127.0f / 255, 1.0f).ConvertTo<Color4ub>(),
            Color4ub(0, 64, 127, 255));
}

TEST(Color4, UnaryMath) {
  // Add.
  {
    Color4ub color(12, 23, 34, 45);
    color += Color4ub(17, 28, 39, 50);
    EXPECT_EQ(color, Color4ub(29, 51, 73, 95));
  }

  // Subtract.
  {
    Color4ub color(29, 51, 73, 95);
    color -= Color4ub(17, 28, 39, 50);
    EXPECT_EQ(color, Color4ub(12, 23, 34, 45));
  }

  // Multiply.
  {
    Color4ub color(17, 18, 19, 20);
    color *= 3;
    EXPECT_EQ(color, Color4ub(51, 54, 57, 60));
  }

  // Divide.
  {
    Color4ub color(51, 54, 57, 60);
    color /= 3;
    EXPECT_EQ(color, Color4ub(17, 18, 19, 20));
  }
}

TEST(Color4, BinaryMath) {
  // Add.
  EXPECT_EQ(Color4ub(12, 23, 34, 45) + Color4ub(17, 28, 39, 50),
            Color4ub(29, 51, 73, 95));

  // Subtract.
  EXPECT_EQ(Color4ub(29, 51, 73, 95) - Color4ub(17, 28, 39, 50),
            Color4ub(12, 23, 34, 45));

  // Multiply.
  EXPECT_EQ(Color4ub(17, 18, 19, 20) * 3, Color4ub(51, 54, 57, 60));

  // Divide.
  EXPECT_EQ(Color4ub(51, 54, 57, 60) / 3, Color4ub(17, 18, 19, 20));
}

////////////////////////////////////////////////////////////////////////////////
// Color4f.

TEST(Color4f, compare) {
  EXPECT_TRUE(Color4f(0.1f, 0.2f, 0.3f, 0.4f) ==
              Color4f(0.1f, 0.2f, 0.3f, 0.4f));
  EXPECT_FALSE(Color4f(0.1f, 0.2f, 0.3f, 0.4f) ==
               Color4f(0.8f, 0.7f, 0.6f, 0.5f));

  EXPECT_FALSE(Color4f(0.1f, 0.2f, 0.3f, 0.4f) !=
               Color4f(0.1f, 0.2f, 0.3f, 0.4f));
  EXPECT_TRUE(Color4f(0.1f, 0.2f, 0.3f, 0.4f) !=
              Color4f(0.8f, 0.7f, 0.6f, 0.5f));
}

TEST(Color4f, Saturate) {
  EXPECT_EQ(Saturate(Color4f(0.1f, 0.2f, 0.3f, 0.4f)),
            Color4f(0.1f, 0.2f, 0.3f, 0.4f));

  EXPECT_EQ(Saturate(Color4f(1.1f, 0.2f, 0.3f, 0.4f)),
            Color4f(1.0f, 0.2f, 0.3f, 0.4f));
  EXPECT_EQ(Saturate(Color4f(0.1f, 1.2f, 0.3f, 0.4f)),
            Color4f(0.1f, 1.0f, 0.3f, 0.4f));
  EXPECT_EQ(Saturate(Color4f(0.1f, 0.2f, 1.3f, 0.4f)),
            Color4f(0.1f, 0.2f, 1.0f, 0.4f));
  EXPECT_EQ(Saturate(Color4f(0.1f, 0.2f, 0.3f, 1.4f)),
            Color4f(0.1f, 0.2f, 0.3f, 1.0f));

  EXPECT_EQ(Saturate(Color4f(-0.1f, 0.2f, 0.3f, 0.4f)),
            Color4f(0.0f, 0.2f, 0.3f, 0.4f));
  EXPECT_EQ(Saturate(Color4f(0.1f, -0.2f, 0.3f, 0.4f)),
            Color4f(0.1f, 0.0f, 0.3f, 0.4f));
  EXPECT_EQ(Saturate(Color4f(0.1f, 0.2f, -0.3f, 0.4f)),
            Color4f(0.1f, 0.2f, 0.0f, 0.4f));
  EXPECT_EQ(Saturate(Color4f(0.1f, 0.2f, 0.3f, -0.4f)),
            Color4f(0.1f, 0.2f, 0.3f, 0.0f));
}

TEST(Color4f, Lerp) {
  {
    const Color4f result = Lerp(
        Color4f(0.1f, 0.2f, 0.3f, 0.4f), Color4f(0.8f, 0.7f, 0.6f, 0.5f), 0.0f);

    EXPECT_NEAR(result.rgba.r, 0.1f, 1e-6f);
    EXPECT_NEAR(result.rgba.g, 0.2f, 1e-6f);
    EXPECT_NEAR(result.rgba.b, 0.3f, 1e-6f);
    EXPECT_NEAR(result.rgba.a, 0.4f, 1e-6f);
  }

  {
    const Color4f result = Lerp(
        Color4f(0.1f, 0.2f, 0.3f, 0.4f), Color4f(0.8f, 0.7f, 0.6f, 0.5f), 1.0f);

    EXPECT_NEAR(result.rgba.r, 0.8f, 1e-6f);
    EXPECT_NEAR(result.rgba.g, 0.7f, 1e-6f);
    EXPECT_NEAR(result.rgba.b, 0.6f, 1e-6f);
    EXPECT_NEAR(result.rgba.a, 0.5f, 1e-6f);
  }

  {
    const Color4f result = Lerp(Color4f(0.1f, 0.2f, 0.3f, 0.4f),
                                Color4f(0.8f, 0.7f, 0.6f, 0.5f),
                                0.25f);

    EXPECT_NEAR(result.rgba.r, 0.275f, 1e-6f);
    EXPECT_NEAR(result.rgba.g, 0.325f, 1e-6f);
    EXPECT_NEAR(result.rgba.b, 0.375f, 1e-6f);
    EXPECT_NEAR(result.rgba.a, 0.425f, 1e-6f);
  }
}

}  // namespace radio_core
