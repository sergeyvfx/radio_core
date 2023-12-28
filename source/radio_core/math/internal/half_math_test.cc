// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/internal/half_math.h"

#include "radio_core/base/constants.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

#if RADIO_CORE_HAVE_HALF

TEST(Half, Ceil) {
  EXPECT_NEAR(float(Ceil(Half(+2.4f))), +3.0f, 1e-4f);
  EXPECT_NEAR(float(Ceil(Half(-2.4f))), -2.0f, 1e-4f);
  EXPECT_NEAR(float(Ceil(Half(0.0f))), 0.0f, 1e-4f);
}

TEST(Half, Floor) {
  EXPECT_NEAR(float(Floor(Half(+2.7f))), +2.0f, 1e-4f);
  EXPECT_NEAR(float(Floor(Half(-2.7f))), -3.0f, 1e-4f);
  EXPECT_NEAR(float(Floor(Half(0.0f))), 0.0f, 1e-4f);
}

TEST(Half, Round) {
  EXPECT_NEAR(float(Round(Half(+2.3))), 2.0f, 1e-4f);
  EXPECT_NEAR(float(Round(Half(+2.5))), 3.0f, 1e-4f);
  EXPECT_NEAR(float(Round(Half(+2.7))), 3.0f, 1e-4f);
  EXPECT_NEAR(float(Round(Half(-2.3))), -2.0f, 1e-4f);
  EXPECT_NEAR(float(Round(Half(-2.5))), -3.0f, 1e-4f);
  EXPECT_NEAR(float(Round(Half(-2.7))), -3.0f, 1e-4f);

  EXPECT_NEAR(float(Round(Half(0.0f))), 0.0f, 1e-4f);
}

TEST(Half, Trunc) {
  EXPECT_NEAR(float(Trunc(Half(+2.7f))), +2.0f, 1e-4f);
  EXPECT_NEAR(float(Trunc(Half(-2.9f))), -2.0f, 1e-4f);
  EXPECT_NEAR(float(Trunc(Half(+0.7f))), +0.0f, 1e-4f);
  EXPECT_NEAR(float(Trunc(Half(-0.9f))), 0.0f, 1e-4f);
  EXPECT_NEAR(float(Trunc(Half(0.0f))), 0.0f, 1e-4f);
}

TEST(Half, RoundToInt) {
  EXPECT_EQ(RoundToInt(Half(+2.3)), 2);
  EXPECT_EQ(RoundToInt(Half(+2.5)), 3);
  EXPECT_EQ(RoundToInt(Half(+2.7)), 3);
  EXPECT_EQ(RoundToInt(Half(-2.3)), -2);
  EXPECT_EQ(RoundToInt(Half(-2.5)), -3);
  EXPECT_EQ(RoundToInt(Half(-2.7)), -3);

  EXPECT_EQ(RoundToInt(Half(0.0f)), 0);
}

TEST(Half, ArcTan) {
  const float eps = 2e-3f;

  // >>> for i in range(11):
  // ...     x = (i - 5) / 4
  // ...     y = math.atan(x)
  // ...     print(
  // ...         f'EXPECT_NEAR(float(ArcTan(Half({x}f))), {y}f, eps);')

  EXPECT_NEAR(float(ArcTan(Half(-1.25f))), -0.8960553845713439f, eps);
  EXPECT_NEAR(float(ArcTan(Half(-1.0f))), -0.7853981633974483f, eps);
  EXPECT_NEAR(float(ArcTan(Half(-0.75f))), -0.6435011087932844f, eps);
  EXPECT_NEAR(float(ArcTan(Half(-0.5f))), -0.46364760900080615f, eps);
  EXPECT_NEAR(float(ArcTan(Half(-0.25f))), -0.24497866312686414f, eps);
  EXPECT_NEAR(float(ArcTan(Half(0.0f))), 0.0f, eps);
  EXPECT_NEAR(float(ArcTan(Half(0.25f))), 0.24497866312686414f, eps);
  EXPECT_NEAR(float(ArcTan(Half(0.5f))), 0.46364760900080615f, eps);
  EXPECT_NEAR(float(ArcTan(Half(0.75f))), 0.6435011087932844f, eps);
  EXPECT_NEAR(float(ArcTan(Half(1.0f))), 0.7853981633974483f, eps);
  EXPECT_NEAR(float(ArcTan(Half(1.25f))), 0.8960553845713439f, eps);
}

TEST(Half, ArcTan2) {
  EXPECT_NEAR(float(ArcTan2(Half(0), Half(0))), 0.0f, 1e-4f);
  EXPECT_NEAR(float(ArcTan2(Half(7), Half(0))), 1.5707963267948966f, 5e-4f);
}

TEST(Half, Sin) {
  const float pi = constants::pi;
  const float eps = 2e-3f;

  // >>> for i in range(13):
  // ...     x = math.pi / 6 * i
  // ...     y = math.sin(x)
  // ...     print(f'EXPECT_NEAR(float(Sin(Half(pi / 6 * {i}))), {y}f, eps);')

  EXPECT_NEAR(float(Sin(Half(pi / 6 * 0))), 0.0f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 1))), 0.49999999999999994f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 2))), 0.8660254037844386f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 3))), 1.0f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 4))), 0.8660254037844387f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 5))), 0.5000000000000003f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 6))), 1.2246467991473532e-16f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 7))), -0.4999999999999998f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 8))), -0.8660254037844384f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 9))), -1.0f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 10))), -0.866025403784439f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 11))), -0.5000000000000004f, eps);
  EXPECT_NEAR(float(Sin(Half(pi / 6 * 12))), -2.4492935982947064e-16f, eps);
}

TEST(Half, Cos) {
  const float pi = constants::pi;
  const float eps = 2e-3f;

  // >>> for i in range(13):
  // ...     x = math.pi / 6 * i
  // ...     y = math.cos(x)
  // ...     print(f'EXPECT_NEAR(float(Cos(Half(pi / 6 * {i}))), {y}f, eps);')

  EXPECT_NEAR(float(Cos(Half(pi / 6 * 0))), 1.0f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 1))), 0.8660254037844387f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 2))), 0.5000000000000001f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 3))), 6.123233995736766e-17f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 4))), -0.49999999999999983f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 5))), -0.8660254037844385f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 6))), -1.0f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 7))), -0.8660254037844388f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 8))), -0.5000000000000004f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 9))), -1.8369701987210297e-16f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 10))), 0.49999999999999933f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 11))), 0.8660254037844384f, eps);
  EXPECT_NEAR(float(Cos(Half(pi / 6 * 12))), 1.0f, eps);
}

TEST(Half, Exp) {
  EXPECT_NEAR(float(Exp(Half(1.0f))), 2.718281828459045f, 5e-4f);
  EXPECT_NEAR(float(Exp(Half(0.03f))), 1.03045f, 5e-4f);
  EXPECT_NEAR(float(Exp(Half(0.0f))), 1.0f, 5e-4f);
}

TEST(Half, Pow) {
  EXPECT_NEAR(float(Pow(Half(2), Half(10))), 1024.0f, 2e-4f);
  EXPECT_NEAR(float(Pow(Half(2), Half(0.5f))), 1.4142135623730951f, 2e-4f);
  EXPECT_NEAR(float(Pow(Half(-2), Half(-3))), -0.125f, 2e-4f);
}

TEST(Half, Sqrt) {
  EXPECT_NEAR(float(Sqrt(Half(1.0f))), 1.0f, 1e-6f);
  EXPECT_NEAR(float(Sqrt(Half(4.0f))), 2.0f, 1e-6f);

  EXPECT_NEAR(float(Sqrt(Half(15.0f))), 3.873046875f, 1e-6f);
}

TEST(Half, Log) {
  EXPECT_NEAR(float(Log(Half(1.0f))), 0.0f, 1e-6f);
  EXPECT_NEAR(float(Log(Half(constants::e))), 1.0f, 1e-6f);
}

TEST(Half, Log2) {
  EXPECT_NEAR(float(Log2(Half(1.0f))), 0.0f, 1e-6f);
  EXPECT_NEAR(float(Log2(Half(2.0f))), 1.0f, 1e-6f);
  EXPECT_NEAR(float(Log2(Half(4.0f))), 2.0f, 1e-6f);

  EXPECT_NEAR(float(Log2(Half(3.0f))), 1.5849609375f, 1e-6f);
}

TEST(Half, Log10) {
  EXPECT_NEAR(float(Log10(Half(1.0f))), 0.0f, 1e-6f);
  EXPECT_NEAR(float(Log10(Half(10.0f))), 1.0f, 1e-6f);
  EXPECT_NEAR(float(Log10(Half(100.0f))), 2.0f, 1e-6f);
}

TEST(Half, Abs) {
  EXPECT_EQ(Abs(Half(0.0f)), Half(0.0f));
  EXPECT_EQ(Abs(Half(12.3f)), Half(12.3f));
  EXPECT_EQ(Abs(Half(-12.3f)), Half(12.3f));
}

TEST(Half, Hypot) {
  EXPECT_NEAR(float(Hypot(Half(1.0f), Half(1.0f))), 1.4142135623730951f, 2e-4f);
}

TEST(Half, Modulo) {
  EXPECT_NEAR(float(Modulo(Half(+5.1), Half(+3.0))), 2.1f, 2e-3f);
  EXPECT_NEAR(float(Modulo(Half(-5.1), Half(+3.0))), -2.1f, 2e-3f);
  EXPECT_NEAR(float(Modulo(Half(+5.1), Half(-3.0))), 2.1f, 2e-3f);
  EXPECT_NEAR(float(Modulo(Half(-5.1), Half(-3.0))), -2.1f, 2e-3f);
  EXPECT_NEAR(float(Modulo(Half(0.0), Half(1.0))), 0.0f, 2e-3f);
}

TEST(Half, CopySign) {
  EXPECT_EQ(CopySign(Half(12.3f), Half(0.0f)), Half(12.3f));
  EXPECT_EQ(CopySign(Half(12.3f), Half(+0.0f)), Half(12.3f));
  EXPECT_EQ(CopySign(Half(12.3f), Half(-0.0f)), Half(-12.3f));

  EXPECT_EQ(CopySign(Half(12.3f), Half(1.0f)), Half(12.3f));
  EXPECT_EQ(CopySign(Half(12.3f), Half(-1.0f)), Half(-12.3f));
}

TEST(Half, IsFinite) {
  EXPECT_TRUE(IsFinite(Half(0.0f)));
  EXPECT_TRUE(IsFinite(Half(1.0f)));
  EXPECT_TRUE(IsFinite(Half(-1.0f)));

  EXPECT_TRUE(IsFinite(Half(FLT_MIN)));
  EXPECT_TRUE(IsFinite(Half(-FLT_MIN)));

  // EXPECT_TRUE(IsFinite(Half(FLT16_MAX)));
  // EXPECT_TRUE(IsFinite(Half(-FLT16_MAX)));

  EXPECT_FALSE(IsFinite(Half(FLT_MAX)));
  EXPECT_FALSE(IsFinite(Half(-FLT_MAX)));

  EXPECT_FALSE(IsFinite(Half(NAN)));
  EXPECT_FALSE(IsFinite(Half(INFINITY)));
}

TEST(Half, IsNaN) {
  EXPECT_TRUE(IsNaN(Half(NAN)));
  EXPECT_FALSE(IsNaN(Half(INFINITY)));
  EXPECT_FALSE(IsNaN(Half(0.0f)));
  EXPECT_TRUE(IsNaN(Half(0.0f / 0.0f)));
  EXPECT_TRUE(IsNaN(Half(INFINITY - INFINITY)));
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
