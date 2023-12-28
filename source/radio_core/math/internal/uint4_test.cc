// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/uint4.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(UInt4, Load) {
  {
    const uint32_t values[4] = {
        0xfffffff2u, 0xfffffff3u, 0xfffffff4u, 0xfffffff5u};
    const UInt4 value = UInt4(values);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfffffff5u);
  }

  {
    const UInt4 value =
        UInt4(0xfffffff2u, 0xfffffff3u, 0xfffffff4u, 0xfffffff5u);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfffffff5u);
  }

  {
    const UInt4 value = UInt4(0xfffffff2u);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<3>(), 0xfffffff2u);
  }
}

TEST(UInt4, Store) {
  {
    const UInt4 value = UInt4(2, 3, 4, 5);
    uint32_t data[4] = {0, 0, 0, 0};
    value.Store(data);
    EXPECT_EQ(data[0], 2);
    EXPECT_EQ(data[1], 3);
    EXPECT_EQ(data[2], 4);
    EXPECT_EQ(data[3], 5);
  }

  {
    const UInt4 value = UInt4(2, 3, 4, 5);
    uint32_t data;

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

TEST(UInt4, Extract) {
  {
    const UInt4 value = UInt4(2, 3, 4, 5);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 4);
    EXPECT_EQ(value.Extract<3>(), 5);
  }
}

TEST(UInt4, ExtractXYZW) {
  {
    const UInt4 value = UInt4(2, 3, 4, 5);

    EXPECT_EQ(value.x(), 2);
    EXPECT_EQ(value.y(), 3);
    EXPECT_EQ(value.z(), 4);
    EXPECT_EQ(value.w(), 5);
  }
}

TEST(UInt4, SetLane) {
  {
    const UInt4 value = UInt4(2, 3, 4, 5);
    const UInt4 new_value = value.SetLane<0>(9);

    EXPECT_EQ(new_value.Extract<0>(), 9);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 5);
  }

  {
    const UInt4 value = UInt4(2, 3, 4, 5);
    const UInt4 new_value = value.SetLane<1>(9);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 9);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 5);
  }

  {
    const UInt4 value = UInt4(2, 3, 4, 5);
    const UInt4 new_value = value.SetLane<2>(9);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 9);
    EXPECT_EQ(new_value.Extract<3>(), 5);
  }

  {
    const UInt4 value = UInt4(2, 3, 4, 5);
    const UInt4 new_value = value.SetLane<3>(9);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 9);
  }
}

TEST(UInt4, SetLaneXYZW) {
  {
    UInt4 value = UInt4(2, 3, 4, 5);
    value.x(9);

    EXPECT_EQ(value.Extract<0>(), 9);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 4);
    EXPECT_EQ(value.Extract<3>(), 5);
  }

  {
    UInt4 value = UInt4(2, 3, 4, 5);
    value.y(9);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 9);
    EXPECT_EQ(value.Extract<2>(), 4);
    EXPECT_EQ(value.Extract<3>(), 5);
  }

  {
    UInt4 value = UInt4(2, 3, 4, 5);
    value.z(9);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 9);
    EXPECT_EQ(value.Extract<3>(), 5);
  }

  {
    UInt4 value = UInt4(2, 3, 4, 5);
    value.w(9);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 4);
    EXPECT_EQ(value.Extract<3>(), 9);
  }
}

TEST(UInt4, Min) {
  {
    const UInt4 result = Min(UInt4(12, 13, 14, 15), UInt4(21, 22, 23, 24));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
  }

  {
    const UInt4 result = Min(UInt4(12, 13, 14, 15), UInt4(1, 22, 23, 24));
    EXPECT_EQ(result.Extract<0>(), 1);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
  }

  {
    const UInt4 result = Min(UInt4(12, 13, 14, 15), UInt4(21, 2, 23, 24));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 2);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
  }

  {
    const UInt4 result = Min(UInt4(12, 13, 14, 15), UInt4(21, 22, 3, 24));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 3);
    EXPECT_EQ(result.Extract<3>(), 15);
  }

  {
    const UInt4 result = Min(UInt4(12, 13, 14, 15), UInt4(21, 22, 23, 4));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 4);
  }
}

TEST(UInt4, Max) {
  {
    const UInt4 result = Max(UInt4(102, 101, 104, 103), UInt4(1, 2, 3, 4));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
  }

  {
    const UInt4 result = Max(UInt4(102, 101, 104, 103), UInt4(105, 2, 3, 4));
    EXPECT_EQ(result.Extract<0>(), 105);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
  }

  {
    const UInt4 result = Max(UInt4(102, 101, 104, 103), UInt4(1, 106, 3, 4));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 106);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
  }

  {
    const UInt4 result = Max(UInt4(102, 101, 104, 103), UInt4(1, 2, 107, 4));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 107);
    EXPECT_EQ(result.Extract<3>(), 103);
  }

  {
    const UInt4 result = Max(UInt4(102, 101, 104, 103), UInt4(1, 2, 3, 108));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 108);
  }
}

TEST(UInt4, HorizontalMax) {
  EXPECT_EQ(HorizontalMax(UInt4(12, 1, 4, 3)), 12);
  EXPECT_EQ(HorizontalMax(UInt4(2, 11, 4, 3)), 11);
  EXPECT_EQ(HorizontalMax(UInt4(2, 1, 14, 3)), 14);
  EXPECT_EQ(HorizontalMax(UInt4(2, 1, 4, 13)), 13);
}

TEST(UInt4, Select) {
  const UInt4 mask(0xffffffffu, 0, 0xffffffffu, 0);
  const UInt4 source1(2, 3, 4, 5);
  const UInt4 source2(6, 7, 8, 9);

  const UInt4 result = Select(mask, source1, source2);
  EXPECT_EQ(result.Extract<0>(), 2);
  EXPECT_EQ(result.Extract<1>(), 7);
  EXPECT_EQ(result.Extract<2>(), 4);
  EXPECT_EQ(result.Extract<3>(), 9);
}

TEST(UInt4, Reverse) {
  {
    const UInt4 result = Reverse(UInt4(1, 2, 3, 4));
    EXPECT_EQ(result.Extract<0>(), 4);
    EXPECT_EQ(result.Extract<1>(), 3);
    EXPECT_EQ(result.Extract<2>(), 2);
    EXPECT_EQ(result.Extract<3>(), 1);
  }
}

}  // namespace radio_core
