// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/uint3.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(UInt3, Load) {
  {
    const uint32_t values[3] = {0xfffffff2u, 0xfffffff3u, 0xfffffff4u};
    const UInt3 value = UInt3(values);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff4u);
  }

  {
    const UInt3 value = UInt3(0xfffffff2u, 0xfffffff3u, 0xfffffff4u);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff4u);
  }

  {
    const UInt3 value = UInt3(0xfffffff2u);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff2u);
  }
}

TEST(UInt3, Store) {
  {
    const UInt3 value = UInt3(2, 3, 4);
    uint32_t data[3] = {0, 0, 0};
    value.Store(data);
    EXPECT_EQ(data[0], 2);
    EXPECT_EQ(data[1], 3);
    EXPECT_EQ(data[2], 4);
  }

  {
    const UInt3 value = UInt3(2, 3, 4);
    uint32_t data;

    value.Store<0>(&data);
    EXPECT_EQ(data, 2);

    value.Store<1>(&data);
    EXPECT_EQ(data, 3);

    value.Store<2>(&data);
    EXPECT_EQ(data, 4);
  }
}

TEST(UInt3, Extract) {
  {
    const UInt3 value = UInt3(2, 3, 4);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 4);
  }
}

TEST(UInt3, ExtractXYZ) {
  {
    const UInt3 value = UInt3(2, 3, 4);

    EXPECT_EQ(value.x(), 2);
    EXPECT_EQ(value.y(), 3);
    EXPECT_EQ(value.z(), 4);
  }
}

TEST(UInt3, SetLane) {
  {
    const UInt3 value = UInt3(2, 3, 4);
    const UInt3 new_value = value.SetLane<0>(9);

    EXPECT_EQ(new_value.Extract<0>(), 9);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 4);
  }

  {
    const UInt3 value = UInt3(2, 3, 4);
    const UInt3 new_value = value.SetLane<1>(9);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 9);
    EXPECT_EQ(new_value.Extract<2>(), 4);
  }

  {
    const UInt3 value = UInt3(2, 3, 4);
    const UInt3 new_value = value.SetLane<2>(9);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 9);
  }
}

TEST(UInt3, SetLaneXYZ) {
  {
    UInt3 value = UInt3(2, 3, 4);
    value.x(9);

    EXPECT_EQ(value.Extract<0>(), 9);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 4);
  }

  {
    UInt3 value = UInt3(2, 3, 4);
    value.y(9);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 9);
    EXPECT_EQ(value.Extract<2>(), 4);
  }

  {
    UInt3 value = UInt3(2, 3, 4);
    value.z(9);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 9);
  }
}

TEST(UInt3, Min) {
  {
    const UInt3 result = Min(UInt3(12, 13, 14), UInt3(21, 22, 23));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
  }

  {
    const UInt3 result = Min(UInt3(12, 13, 14), UInt3(1, 22, 23));
    EXPECT_EQ(result.Extract<0>(), 1);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
  }

  {
    const UInt3 result = Min(UInt3(12, 13, 14), UInt3(21, 2, 23));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 2);
    EXPECT_EQ(result.Extract<2>(), 14);
  }

  {
    const UInt3 result = Min(UInt3(12, 13, 14), UInt3(21, 22, 3));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 3);
  }
}

TEST(UInt3, Max) {
  {
    const UInt3 result = Max(UInt3(102, 101, 104), UInt3(1, 2, 3));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
  }

  {
    const UInt3 result = Max(UInt3(102, 101, 104), UInt3(105, 2, 3));
    EXPECT_EQ(result.Extract<0>(), 105);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
  }

  {
    const UInt3 result = Max(UInt3(102, 101, 104), UInt3(1, 106, 3));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 106);
    EXPECT_EQ(result.Extract<2>(), 104);
  }

  {
    const UInt3 result = Max(UInt3(102, 101, 104), UInt3(1, 2, 107));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 107);
  }
}

TEST(UInt3, HorizontalMax) {
  EXPECT_EQ(HorizontalMax(UInt3(12, 1, 4)), 12);
  EXPECT_EQ(HorizontalMax(UInt3(2, 11, 4)), 11);
  EXPECT_EQ(HorizontalMax(UInt3(2, 1, 14)), 14);
}

TEST(UInt3, Select) {
  const UInt3 mask(0xffffffffu, 0, 0xffffffffu);
  const UInt3 source1(2, 3, 4);
  const UInt3 source2(6, 7, 8);

  const UInt3 result = Select(mask, source1, source2);
  EXPECT_EQ(result.Extract<0>(), 2);
  EXPECT_EQ(result.Extract<1>(), 7);
  EXPECT_EQ(result.Extract<2>(), 4);
}

TEST(UInt3, Reverse) {
  {
    const UInt3 result = Reverse(UInt3(1, 2, 3));
    EXPECT_EQ(result.Extract<0>(), 3);
    EXPECT_EQ(result.Extract<1>(), 2);
    EXPECT_EQ(result.Extract<2>(), 1);
  }
}

}  // namespace radio_core
