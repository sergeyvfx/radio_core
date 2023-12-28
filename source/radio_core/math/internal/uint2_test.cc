// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/uint2.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(UInt2, Load) {
  {
    const uint32_t values[2] = {0xfffffff2u, 0xfffffff3u};
    const UInt2 value = UInt2(values);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff3u);
  }

  {
    const UInt2 value = UInt2(0xfffffff2u, 0xfffffff3u);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff3u);
  }

  {
    const UInt2 value = UInt2(0xfffffff2u);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff2u);
  }
}

TEST(UInt2, Store) {
  {
    const UInt2 value = UInt2(2, 3);
    uint32_t data[2] = {0, 0};
    value.Store(data);
    EXPECT_EQ(data[0], 2);
    EXPECT_EQ(data[1], 3);
  }

  {
    const UInt2 value = UInt2(2, 3);
    uint32_t data;

    value.Store<0>(&data);
    EXPECT_EQ(data, 2);

    value.Store<1>(&data);
    EXPECT_EQ(data, 3);
  }
}

TEST(UInt2, Extract) {
  {
    const UInt2 value = UInt2(2, 3);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
  }
}

TEST(UInt2, ExtractXY) {
  {
    const UInt2 value = UInt2(2, 3);

    EXPECT_EQ(value.x(), 2);
    EXPECT_EQ(value.y(), 3);
  }
}

TEST(UInt2, SetLane) {
  {
    const UInt2 value = UInt2(2, 3);
    const UInt2 new_value = value.SetLane<0>(9);

    EXPECT_EQ(new_value.Extract<0>(), 9);
    EXPECT_EQ(new_value.Extract<1>(), 3);
  }

  {
    const UInt2 value = UInt2(2, 3);
    const UInt2 new_value = value.SetLane<1>(9);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 9);
  }
}

TEST(UInt2, SetLaneXY) {
  {
    UInt2 value = UInt2(2, 3);
    value.x(9);

    EXPECT_EQ(value.Extract<0>(), 9);
    EXPECT_EQ(value.Extract<1>(), 3);
  }

  {
    UInt2 value = UInt2(2, 3);
    value.y(9);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 9);
  }
}

TEST(UInt2, Min) {
  {
    const UInt2 result = Min(UInt2(12, 13), UInt2(21, 22));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
  }

  {
    const UInt2 result = Min(UInt2(12, 13), UInt2(1, 22));
    EXPECT_EQ(result.Extract<0>(), 1);
    EXPECT_EQ(result.Extract<1>(), 13);
  }

  {
    const UInt2 result = Min(UInt2(12, 13), UInt2(21, 2));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 2);
  }
}

TEST(UInt2, Max) {
  {
    const UInt2 result = Max(UInt2(102, 101), UInt2(1, 2));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
  }

  {
    const UInt2 result = Max(UInt2(102, 101), UInt2(105, 2));
    EXPECT_EQ(result.Extract<0>(), 105);
    EXPECT_EQ(result.Extract<1>(), 101);
  }

  {
    const UInt2 result = Max(UInt2(102, 101), UInt2(1, 106));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 106);
  }
}

TEST(UInt2, HorizontalMax) {
  EXPECT_EQ(HorizontalMax(UInt2(12, 1)), 12);
  EXPECT_EQ(HorizontalMax(UInt2(2, 11)), 11);
}

TEST(UInt2, Select) {
  const UInt2 mask(0xffffffffu, 0);
  const UInt2 source1(2, 3);
  const UInt2 source2(6, 7);

  const UInt2 result = Select(mask, source1, source2);
  EXPECT_EQ(result.Extract<0>(), 2);
  EXPECT_EQ(result.Extract<1>(), 7);
}

TEST(UInt2, Reverse) {
  {
    const UInt2 result = Reverse(UInt2(1, 2));
    EXPECT_EQ(result.Extract<0>(), 2);
    EXPECT_EQ(result.Extract<1>(), 1);
  }
}

}  // namespace radio_core
