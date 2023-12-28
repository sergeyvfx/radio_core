// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/ushort4.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(UShort4, Load) {
  {
    const uint16_t values[4] = {0xfff2u, 0xfff3u, 0xfff4u, 0xfff5u};
    const UShort4 value = UShort4(values);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfff5u);
  }

  {
    const UShort4 value = UShort4(0xfff2u, 0xfff3u, 0xfff4u, 0xfff5u);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfff5u);
  }

  {
    const UShort4 value = UShort4(0xfff2u);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff2u);
    EXPECT_EQ(value.Extract<2>(), 0xfff2u);
    EXPECT_EQ(value.Extract<3>(), 0xfff2u);
  }
}

TEST(UShort4, Store) {
  {
    const UShort4 value = UShort4(2, 3, 4, 5);
    uint16_t data[4] = {0, 0, 0, 0};
    value.Store(data);
    EXPECT_EQ(data[0], 2);
    EXPECT_EQ(data[1], 3);
    EXPECT_EQ(data[2], 4);
    EXPECT_EQ(data[3], 5);
  }

  {
    const UShort4 value = UShort4(2, 3, 4, 5);
    uint16_t data;

    value.Store<0>(&data);
    EXPECT_EQ(data, 2);

    value.Store<1>(&data);
    EXPECT_EQ(data, 3);

    value.Store<2>(&data);
    EXPECT_EQ(data, 4);

    value.Store<3>(&data);
    EXPECT_EQ(data, 5);
  }
}

TEST(UShort4, Extract) {
  {
    const UShort4 value = UShort4(2, 3, 4, 5);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 4);
    EXPECT_EQ(value.Extract<3>(), 5);
  }
}

TEST(UShort4, ExtractXYZW) {
  {
    const UShort4 value = UShort4(2, 3, 4, 5);

    EXPECT_EQ(value.x(), 2);
    EXPECT_EQ(value.y(), 3);
    EXPECT_EQ(value.z(), 4);
    EXPECT_EQ(value.w(), 5);
  }
}

TEST(UShort4, Min) {
  {
    const UShort4 result =
        Min(UShort4(12, 13, 14, 15), UShort4(21, 22, 23, 24));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
  }

  {
    const UShort4 result = Min(UShort4(12, 13, 14, 15), UShort4(1, 22, 23, 24));
    EXPECT_EQ(result.Extract<0>(), 1);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
  }

  {
    const UShort4 result = Min(UShort4(12, 13, 14, 15), UShort4(21, 2, 23, 24));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 2);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
  }

  {
    const UShort4 result = Min(UShort4(12, 13, 14, 15), UShort4(21, 22, 3, 24));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 3);
    EXPECT_EQ(result.Extract<3>(), 15);
  }

  {
    const UShort4 result = Min(UShort4(12, 13, 14, 15), UShort4(21, 22, 23, 4));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 4);
  }
}

TEST(UShort4, Max) {
  {
    const UShort4 result =
        Max(UShort4(102, 101, 104, 103), UShort4(1, 2, 3, 4));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
  }

  {
    const UShort4 result =
        Max(UShort4(102, 101, 104, 103), UShort4(105, 2, 3, 4));
    EXPECT_EQ(result.Extract<0>(), 105);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
  }

  {
    const UShort4 result =
        Max(UShort4(102, 101, 104, 103), UShort4(1, 106, 3, 4));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 106);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
  }

  {
    const UShort4 result =
        Max(UShort4(102, 101, 104, 103), UShort4(1, 2, 107, 4));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 107);
    EXPECT_EQ(result.Extract<3>(), 103);
  }

  {
    const UShort4 result =
        Max(UShort4(102, 101, 104, 103), UShort4(1, 2, 3, 108));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 108);
  }
}

TEST(UShort4, HorizontalMax) {
  EXPECT_EQ(HorizontalMax(UShort4(12, 1, 4, 3)), 12);
  EXPECT_EQ(HorizontalMax(UShort4(2, 11, 4, 3)), 11);
  EXPECT_EQ(HorizontalMax(UShort4(2, 1, 14, 3)), 14);
  EXPECT_EQ(HorizontalMax(UShort4(2, 1, 4, 13)), 13);
}

TEST(UShort4, Select) {
  const UShort4 mask(0xffffu, 0, 0xffffu, 0);
  const UShort4 source1(2, 3, 4, 5);
  const UShort4 source2(6, 7, 8, 9);

  const UShort4 result = Select(mask, source1, source2);
  EXPECT_EQ(result.Extract<0>(), 2);
  EXPECT_EQ(result.Extract<1>(), 7);
  EXPECT_EQ(result.Extract<2>(), 4);
  EXPECT_EQ(result.Extract<3>(), 9);
}

TEST(UShort4, Reverse) {
  {
    const UShort4 result = Reverse(UShort4(1, 2, 3, 4));
    EXPECT_EQ(result.Extract<0>(), 4);
    EXPECT_EQ(result.Extract<1>(), 3);
    EXPECT_EQ(result.Extract<2>(), 2);
    EXPECT_EQ(result.Extract<3>(), 1);
  }
}

}  // namespace radio_core
