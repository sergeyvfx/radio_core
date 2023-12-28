// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/ushort3.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(UShort3, Load) {
  {
    const uint16_t values[3] = {0xfff2u, 0xfff3u, 0xfff4u};
    const UShort3 value = UShort3(values);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfff4u);
  }

  {
    const UShort3 value = UShort3(0xfff2u, 0xfff3u, 0xfff4u);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfff4u);
  }

  {
    const UShort3 value = UShort3(0xfff2u);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff2u);
    EXPECT_EQ(value.Extract<2>(), 0xfff2u);
  }
}

TEST(UShort3, Store) {
  {
    const UShort3 value = UShort3(2, 3, 4);
    uint16_t data[3] = {0, 0, 0};
    value.Store(data);
    EXPECT_EQ(data[0], 2);
    EXPECT_EQ(data[1], 3);
    EXPECT_EQ(data[2], 4);
  }

  {
    const UShort3 value = UShort3(2, 3, 4);
    uint16_t data;

    value.Store<0>(&data);
    EXPECT_EQ(data, 2);

    value.Store<1>(&data);
    EXPECT_EQ(data, 3);

    value.Store<2>(&data);
    EXPECT_EQ(data, 4);
  }
}

TEST(UShort3, Extract) {
  {
    const UShort3 value = UShort3(2, 3, 4);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 4);
  }
}

TEST(UShort3, ExtractXYZ) {
  {
    const UShort3 value = UShort3(2, 3, 4);

    EXPECT_EQ(value.x(), 2);
    EXPECT_EQ(value.y(), 3);
    EXPECT_EQ(value.z(), 4);
  }
}

TEST(UShort3, Min) {
  {
    const UShort3 result = Min(UShort3(12, 13, 14), UShort3(21, 22, 23));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
  }

  {
    const UShort3 result = Min(UShort3(12, 13, 14), UShort3(1, 22, 23));
    EXPECT_EQ(result.Extract<0>(), 1);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
  }

  {
    const UShort3 result = Min(UShort3(12, 13, 14), UShort3(21, 2, 23));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 2);
    EXPECT_EQ(result.Extract<2>(), 14);
  }

  {
    const UShort3 result = Min(UShort3(12, 13, 14), UShort3(21, 22, 3));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 3);
  }
}

TEST(UShort3, Max) {
  {
    const UShort3 result = Max(UShort3(102, 101, 104), UShort3(1, 2, 3));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
  }

  {
    const UShort3 result = Max(UShort3(102, 101, 104), UShort3(105, 2, 3));
    EXPECT_EQ(result.Extract<0>(), 105);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
  }

  {
    const UShort3 result = Max(UShort3(102, 101, 104), UShort3(1, 106, 3));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 106);
    EXPECT_EQ(result.Extract<2>(), 104);
  }

  {
    const UShort3 result = Max(UShort3(102, 101, 104), UShort3(1, 2, 107));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 107);
  }
}

TEST(UShort3, HorizontalMax) {
  EXPECT_EQ(HorizontalMax(UShort3(12, 1, 4)), 12);
  EXPECT_EQ(HorizontalMax(UShort3(2, 11, 4)), 11);
  EXPECT_EQ(HorizontalMax(UShort3(2, 1, 14)), 14);
}

TEST(UShort3, Select) {
  const UShort3 mask(0xffffu, 0, 0xffffu);
  const UShort3 source1(2, 3, 4);
  const UShort3 source2(6, 7, 8);

  const UShort3 result = Select(mask, source1, source2);
  EXPECT_EQ(result.Extract<0>(), 2);
  EXPECT_EQ(result.Extract<1>(), 7);
  EXPECT_EQ(result.Extract<2>(), 4);
}

}  // namespace radio_core
