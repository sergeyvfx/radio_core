// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/num_digits.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(NumDigits, int8_t) {
  EXPECT_EQ(NumDigits<int8_t>(0), 1);
  EXPECT_EQ(NumDigits<int8_t>(1), 1);
  EXPECT_EQ(NumDigits<int8_t>(99), 2);
  EXPECT_EQ(NumDigits<int8_t>(127), 3);

  EXPECT_EQ(NumDigits<int8_t>(0), 1);
  EXPECT_EQ(NumDigits<int8_t>(-1), 1);
  EXPECT_EQ(NumDigits<int8_t>(-99), 2);
  EXPECT_EQ(NumDigits<int8_t>(-128), 3);
}

TEST(NumDigits, int64_t) {
  EXPECT_EQ(NumDigits<int64_t>(0), 1);
  EXPECT_EQ(NumDigits<int64_t>(1), 1);
  EXPECT_EQ(NumDigits<int64_t>(1024), 4);
  EXPECT_EQ(NumDigits<int64_t>(std::numeric_limits<int8_t>::max()), 3);
  EXPECT_EQ(NumDigits<int64_t>(std::numeric_limits<int16_t>::max()), 5);
  EXPECT_EQ(NumDigits<int64_t>(std::numeric_limits<int32_t>::max()), 10);
  EXPECT_EQ(NumDigits<int64_t>(std::numeric_limits<int64_t>::max()), 19);

  EXPECT_EQ(NumDigits<int64_t>(-1), 1);
  EXPECT_EQ(NumDigits<int64_t>(std::numeric_limits<int8_t>::min()), 3);
  EXPECT_EQ(NumDigits<int64_t>(std::numeric_limits<int16_t>::min()), 5);
  EXPECT_EQ(NumDigits<int64_t>(std::numeric_limits<int32_t>::min()), 10);
  EXPECT_EQ(NumDigits<int64_t>(std::numeric_limits<int64_t>::min()), 19);
}

TEST(NumDigits, int32_t) {
  EXPECT_EQ(NumDigits<int32_t>(0), 1);
  EXPECT_EQ(NumDigits<int32_t>(1), 1);
  EXPECT_EQ(NumDigits<int32_t>(1024), 4);
  EXPECT_EQ(NumDigits<int32_t>(std::numeric_limits<int8_t>::max()), 3);
  EXPECT_EQ(NumDigits<int32_t>(std::numeric_limits<int16_t>::max()), 5);
  EXPECT_EQ(NumDigits<int32_t>(std::numeric_limits<int32_t>::max()), 10);

  EXPECT_EQ(NumDigits<int32_t>(-1), 1);
  EXPECT_EQ(NumDigits<int32_t>(std::numeric_limits<int8_t>::min()), 3);
  EXPECT_EQ(NumDigits<int32_t>(std::numeric_limits<int16_t>::min()), 5);
  EXPECT_EQ(NumDigits<int32_t>(std::numeric_limits<int32_t>::min()), 10);
}

}  // namespace radio_core
