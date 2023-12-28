// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/ushort2.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(UShort2, Load) {
  {
    const uint16_t values[2] = {0xfff2u, 0xfff3u};
    const UShort2 value = UShort2(values);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff3u);
  }

  {
    const UShort2 value = UShort2(0xfff2u, 0xfff3u);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff3u);
  }

  {
    const UShort2 value = UShort2(0xfff2u);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff2u);
  }
}

TEST(UShort2, Store) {
  {
    const UShort2 value = UShort2(2, 3);
    uint16_t data[2] = {0, 0};
    value.Store(data);
    EXPECT_EQ(data[0], 2);
    EXPECT_EQ(data[1], 3);
  }

  {
    const UShort2 value = UShort2(2, 3);
    uint16_t data;

    value.Store<0>(&data);
    EXPECT_EQ(data, 2);

    value.Store<1>(&data);
    EXPECT_EQ(data, 3);
  }
}

TEST(UShort2, Extract) {
  {
    const UShort2 value = UShort2(2, 3);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
  }
}

TEST(UShort2, ExtractXY) {
  {
    const UShort2 value = UShort2(2, 3);

    EXPECT_EQ(value.x(), 2);
    EXPECT_EQ(value.y(), 3);
  }
}

TEST(UShort2, Min) {
  {
    const UShort2 result = Min(UShort2(12, 13), UShort2(21, 22));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
  }

  {
    const UShort2 result = Min(UShort2(12, 13), UShort2(1, 22));
    EXPECT_EQ(result.Extract<0>(), 1);
    EXPECT_EQ(result.Extract<1>(), 13);
  }

  {
    const UShort2 result = Min(UShort2(12, 13), UShort2(21, 2));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 2);
  }
}

TEST(UShort2, Max) {
  {
    const UShort2 result = Max(UShort2(102, 101), UShort2(1, 2));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
  }

  {
    const UShort2 result = Max(UShort2(102, 101), UShort2(105, 2));
    EXPECT_EQ(result.Extract<0>(), 105);
    EXPECT_EQ(result.Extract<1>(), 101);
  }

  {
    const UShort2 result = Max(UShort2(102, 101), UShort2(1, 106));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 106);
  }
}

TEST(UShort2, HorizontalMax) {
  EXPECT_EQ(HorizontalMax(UShort2(12, 1)), 12);
  EXPECT_EQ(HorizontalMax(UShort2(2, 11)), 11);
}

TEST(UShort2, Select) {
  const UShort2 mask(0xffffu, 0);
  const UShort2 source1(2, 3);
  const UShort2 source2(6, 7);

  const UShort2 result = Select(mask, source1, source2);
  EXPECT_EQ(result.Extract<0>(), 2);
  EXPECT_EQ(result.Extract<1>(), 7);
}

TEST(UShort2, Reverse) {
  {
    const UShort2 result = Reverse(UShort2(1, 2));
    EXPECT_EQ(result.Extract<0>(), 2);
    EXPECT_EQ(result.Extract<1>(), 1);
  }
}

}  // namespace radio_core
