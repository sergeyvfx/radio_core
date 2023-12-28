// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/convert.h"

#include <array>

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(base_convert, StringToInt) {
  {
    EXPECT_EQ(StringToInt<int>("0"), 0);
    EXPECT_EQ(StringToInt<int>("1"), 1);
    EXPECT_EQ(StringToInt<int>("13"), 13);

    EXPECT_EQ(StringToInt<int>("+13"), 13);
    EXPECT_EQ(StringToInt<int>("-13"), -13);

    EXPECT_EQ(StringToInt<int32_t>("-2147483648"), -2147483648);
    EXPECT_EQ(StringToInt<int32_t>("2147483647"), 2147483647);

    // Use -9223372036854775807-1 as the -9223372036854775808 are two literals
    // ("-" and "9223372036854775808") and the former one does not fit into a
    // signed integral value.
    EXPECT_EQ(StringToInt<int64_t>("-9223372036854775808"),
              -9223372036854775807 - 1);
    EXPECT_EQ(StringToInt<int64_t>("9223372036854775807"),
              9223372036854775807LL);
  }

  {
    std::string_view remainder;

    EXPECT_EQ(StringToInt<int>("0", remainder), 0);
    EXPECT_EQ(remainder, "");

    EXPECT_EQ(StringToInt<int>("18.45", remainder), 18);
    EXPECT_EQ(remainder, ".45");
  }
}

TEST(base_convert, StringToFloat) {
  constexpr float kTolerance = 1e-6f;

  // Integer values.
  {
    EXPECT_NEAR(StringToFloat<float>("0"), 0, kTolerance);
    EXPECT_NEAR(StringToFloat<float>("1"), 1, kTolerance);
    EXPECT_NEAR(StringToFloat<float>("13"), 13, kTolerance);

    EXPECT_NEAR(StringToFloat<float>("+13"), 13, kTolerance);
    EXPECT_NEAR(StringToFloat<float>("-13"), -13, kTolerance);
  }

  // Fractional values.
  {
    EXPECT_NEAR(StringToFloat<float>("0.123"), 0.123f, kTolerance);
    EXPECT_NEAR(StringToFloat<float>("1.2345"), 1.2345f, kTolerance);
    EXPECT_NEAR(StringToFloat<float>("13.2345"), 13.2345f, kTolerance);

    EXPECT_NEAR(StringToFloat<float>("+13.2345"), 13.2345f, kTolerance);
    EXPECT_NEAR(StringToFloat<float>("-13.2345"), -13.2345f, kTolerance);
  }
}

}  // namespace radio_core
