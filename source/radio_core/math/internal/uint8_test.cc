// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/uint8.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(UInt8, Load) {
  {
    const uint32_t values[8] = {0xfffffff2u,
                                0xfffffff3u,
                                0xfffffff4u,
                                0xfffffff5u,
                                0xfffffff6u,
                                0xfffffff7u,
                                0xfffffff8u,
                                0xfffffff9u};
    const UInt8 value = UInt8(values);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfffffff5u);
    EXPECT_EQ(value.Extract<4>(), 0xfffffff6u);
    EXPECT_EQ(value.Extract<5>(), 0xfffffff7u);
    EXPECT_EQ(value.Extract<6>(), 0xfffffff8u);
    EXPECT_EQ(value.Extract<7>(), 0xfffffff9u);
  }

  {
    const UInt8 value = UInt8(0xfffffff2u,
                              0xfffffff3u,
                              0xfffffff4u,
                              0xfffffff5u,
                              0xfffffff6u,
                              0xfffffff7u,
                              0xfffffff8u,
                              0xfffffff9u);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfffffff5u);
    EXPECT_EQ(value.Extract<4>(), 0xfffffff6u);
    EXPECT_EQ(value.Extract<5>(), 0xfffffff7u);
    EXPECT_EQ(value.Extract<6>(), 0xfffffff8u);
    EXPECT_EQ(value.Extract<7>(), 0xfffffff9u);
  }

  {
    const UInt8 value = UInt8(0xfffffff2u);

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<3>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<4>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<5>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<6>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<7>(), 0xfffffff2u);
  }

  {
    const UInt8 value =
        UInt8(UInt4(0xfffffff2u, 0xfffffff3u, 0xfffffff4u, 0xfffffff5u),
              UInt4(0xfffffff6u, 0xfffffff7u, 0xfffffff8u, 0xfffffff9u));

    EXPECT_EQ(value.Extract<0>(), 0xfffffff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfffffff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfffffff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfffffff5u);
    EXPECT_EQ(value.Extract<4>(), 0xfffffff6u);
    EXPECT_EQ(value.Extract<5>(), 0xfffffff7u);
    EXPECT_EQ(value.Extract<6>(), 0xfffffff8u);
    EXPECT_EQ(value.Extract<7>(), 0xfffffff9u);
  }
}

TEST(UInt8, Store) {
  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    uint32_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    value.Store(data);
    EXPECT_EQ(data[0], 2);
    EXPECT_EQ(data[1], 3);
    EXPECT_EQ(data[2], 4);
    EXPECT_EQ(data[3], 5);
    EXPECT_EQ(data[4], 6);
    EXPECT_EQ(data[5], 7);
    EXPECT_EQ(data[6], 8);
    EXPECT_EQ(data[7], 9);
  }

  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    uint32_t data;

    value.Store<0>(&data);
    EXPECT_EQ(data, 2);

    value.Store<1>(&data);
    EXPECT_EQ(data, 3);

    value.Store<2>(&data);
    EXPECT_EQ(data, 4);

    value.Store<3>(&data);
    EXPECT_EQ(data, 5);

    value.Store<4>(&data);
    EXPECT_EQ(data, 6);

    value.Store<5>(&data);
    EXPECT_EQ(data, 7);

    value.Store<6>(&data);
    EXPECT_EQ(data, 8);

    value.Store<7>(&data);
    EXPECT_EQ(data, 9);
  }
}

TEST(UInt8, Extract) {
  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);

    EXPECT_EQ(value.Extract<0>(), 2);
    EXPECT_EQ(value.Extract<1>(), 3);
    EXPECT_EQ(value.Extract<2>(), 4);
    EXPECT_EQ(value.Extract<3>(), 5);
    EXPECT_EQ(value.Extract<4>(), 6);
    EXPECT_EQ(value.Extract<5>(), 7);
    EXPECT_EQ(value.Extract<6>(), 8);
    EXPECT_EQ(value.Extract<7>(), 9);
  }
}

TEST(UInt8, ExtractLow) {
  const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);

  const UInt4 low = value.ExtractLow();

  EXPECT_EQ(low.Extract<0>(), 2);
  EXPECT_EQ(low.Extract<1>(), 3);
  EXPECT_EQ(low.Extract<2>(), 4);
  EXPECT_EQ(low.Extract<3>(), 5);
}

TEST(UInt8, ExtractHigh) {
  const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);

  const UInt4 low = value.ExtractHigh();

  EXPECT_EQ(low.Extract<0>(), 6);
  EXPECT_EQ(low.Extract<1>(), 7);
  EXPECT_EQ(low.Extract<2>(), 8);
  EXPECT_EQ(low.Extract<3>(), 9);
}

TEST(UInt8, SetLane) {
  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    const UInt8 new_value = value.SetLane<0>(99);

    EXPECT_EQ(new_value.Extract<0>(), 99);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 5);
    EXPECT_EQ(new_value.Extract<4>(), 6);
    EXPECT_EQ(new_value.Extract<5>(), 7);
    EXPECT_EQ(new_value.Extract<6>(), 8);
    EXPECT_EQ(new_value.Extract<7>(), 9);
  }

  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    const UInt8 new_value = value.SetLane<1>(99);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 99);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 5);
    EXPECT_EQ(new_value.Extract<4>(), 6);
    EXPECT_EQ(new_value.Extract<5>(), 7);
    EXPECT_EQ(new_value.Extract<6>(), 8);
    EXPECT_EQ(new_value.Extract<7>(), 9);
  }

  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    const UInt8 new_value = value.SetLane<2>(99);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 99);
    EXPECT_EQ(new_value.Extract<3>(), 5);
    EXPECT_EQ(new_value.Extract<4>(), 6);
    EXPECT_EQ(new_value.Extract<5>(), 7);
    EXPECT_EQ(new_value.Extract<6>(), 8);
    EXPECT_EQ(new_value.Extract<7>(), 9);
  }

  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    const UInt8 new_value = value.SetLane<3>(99);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 99);
    EXPECT_EQ(new_value.Extract<4>(), 6);
    EXPECT_EQ(new_value.Extract<5>(), 7);
    EXPECT_EQ(new_value.Extract<6>(), 8);
    EXPECT_EQ(new_value.Extract<7>(), 9);
  }

  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    const UInt8 new_value = value.SetLane<4>(99);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 5);
    EXPECT_EQ(new_value.Extract<4>(), 99);
    EXPECT_EQ(new_value.Extract<5>(), 7);
    EXPECT_EQ(new_value.Extract<6>(), 8);
    EXPECT_EQ(new_value.Extract<7>(), 9);
  }

  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    const UInt8 new_value = value.SetLane<5>(99);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 5);
    EXPECT_EQ(new_value.Extract<4>(), 6);
    EXPECT_EQ(new_value.Extract<5>(), 99);
    EXPECT_EQ(new_value.Extract<6>(), 8);
    EXPECT_EQ(new_value.Extract<7>(), 9);
  }

  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    const UInt8 new_value = value.SetLane<6>(99);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 5);
    EXPECT_EQ(new_value.Extract<4>(), 6);
    EXPECT_EQ(new_value.Extract<5>(), 7);
    EXPECT_EQ(new_value.Extract<6>(), 99);
    EXPECT_EQ(new_value.Extract<7>(), 9);
  }

  {
    const UInt8 value = UInt8(2, 3, 4, 5, 6, 7, 8, 9);
    const UInt8 new_value = value.SetLane<7>(99);

    EXPECT_EQ(new_value.Extract<0>(), 2);
    EXPECT_EQ(new_value.Extract<1>(), 3);
    EXPECT_EQ(new_value.Extract<2>(), 4);
    EXPECT_EQ(new_value.Extract<3>(), 5);
    EXPECT_EQ(new_value.Extract<4>(), 6);
    EXPECT_EQ(new_value.Extract<5>(), 7);
    EXPECT_EQ(new_value.Extract<6>(), 8);
    EXPECT_EQ(new_value.Extract<7>(), 99);
  }
}

TEST(UInt8, Min) {
  {
    const UInt8 result = Min(UInt8(12, 13, 14, 15, 16, 17, 18, 19),
                             UInt8(21, 22, 23, 24, 25, 26, 27, 28));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
    EXPECT_EQ(result.Extract<4>(), 16);
    EXPECT_EQ(result.Extract<5>(), 17);
    EXPECT_EQ(result.Extract<6>(), 18);
    EXPECT_EQ(result.Extract<7>(), 19);
  }

  {
    const UInt8 result = Min(UInt8(12, 13, 14, 15, 16, 17, 18, 19),
                             UInt8(1, 22, 23, 24, 25, 26, 27, 28));
    EXPECT_EQ(result.Extract<0>(), 1);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
    EXPECT_EQ(result.Extract<4>(), 16);
    EXPECT_EQ(result.Extract<5>(), 17);
    EXPECT_EQ(result.Extract<6>(), 18);
    EXPECT_EQ(result.Extract<7>(), 19);
  }

  {
    const UInt8 result = Min(UInt8(12, 13, 14, 15, 16, 17, 18, 19),
                             UInt8(21, 2, 23, 24, 25, 26, 27, 28));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 2);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
    EXPECT_EQ(result.Extract<4>(), 16);
    EXPECT_EQ(result.Extract<5>(), 17);
    EXPECT_EQ(result.Extract<6>(), 18);
    EXPECT_EQ(result.Extract<7>(), 19);
  }

  {
    const UInt8 result = Min(UInt8(12, 13, 14, 15, 16, 17, 18, 19),
                             UInt8(21, 22, 3, 24, 25, 26, 27, 28));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 3);
    EXPECT_EQ(result.Extract<3>(), 15);
    EXPECT_EQ(result.Extract<4>(), 16);
    EXPECT_EQ(result.Extract<5>(), 17);
    EXPECT_EQ(result.Extract<6>(), 18);
    EXPECT_EQ(result.Extract<7>(), 19);
  }

  {
    const UInt8 result = Min(UInt8(12, 13, 14, 15, 16, 17, 18, 19),
                             UInt8(21, 22, 23, 4, 25, 26, 27, 28));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 4);
    EXPECT_EQ(result.Extract<4>(), 16);
    EXPECT_EQ(result.Extract<5>(), 17);
    EXPECT_EQ(result.Extract<6>(), 18);
    EXPECT_EQ(result.Extract<7>(), 19);
  }

  {
    const UInt8 result = Min(UInt8(12, 13, 14, 15, 16, 17, 18, 19),
                             UInt8(21, 22, 23, 24, 5, 26, 27, 28));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
    EXPECT_EQ(result.Extract<4>(), 5);
    EXPECT_EQ(result.Extract<5>(), 17);
    EXPECT_EQ(result.Extract<6>(), 18);
    EXPECT_EQ(result.Extract<7>(), 19);
  }

  {
    const UInt8 result = Min(UInt8(12, 13, 14, 15, 16, 17, 18, 19),
                             UInt8(21, 22, 23, 24, 25, 6, 27, 28));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
    EXPECT_EQ(result.Extract<4>(), 16);
    EXPECT_EQ(result.Extract<5>(), 6);
    EXPECT_EQ(result.Extract<6>(), 18);
    EXPECT_EQ(result.Extract<7>(), 19);
  }

  {
    const UInt8 result = Min(UInt8(12, 13, 14, 15, 16, 17, 18, 19),
                             UInt8(21, 22, 23, 24, 25, 26, 7, 28));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
    EXPECT_EQ(result.Extract<4>(), 16);
    EXPECT_EQ(result.Extract<5>(), 17);
    EXPECT_EQ(result.Extract<6>(), 7);
    EXPECT_EQ(result.Extract<7>(), 19);
  }

  {
    const UInt8 result = Min(UInt8(12, 13, 14, 15, 16, 17, 18, 19),
                             UInt8(21, 22, 23, 24, 25, 26, 27, 8));
    EXPECT_EQ(result.Extract<0>(), 12);
    EXPECT_EQ(result.Extract<1>(), 13);
    EXPECT_EQ(result.Extract<2>(), 14);
    EXPECT_EQ(result.Extract<3>(), 15);
    EXPECT_EQ(result.Extract<4>(), 16);
    EXPECT_EQ(result.Extract<5>(), 17);
    EXPECT_EQ(result.Extract<6>(), 18);
    EXPECT_EQ(result.Extract<7>(), 8);
  }
}

TEST(UInt8, Max) {
  {
    const UInt8 result = Max(UInt8(102, 101, 104, 103, 106, 105, 108, 107),
                             UInt8(1, 2, 3, 4, 5, 6, 7, 8));
    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
    EXPECT_EQ(result.Extract<4>(), 106);
    EXPECT_EQ(result.Extract<5>(), 105);
    EXPECT_EQ(result.Extract<6>(), 108);
    EXPECT_EQ(result.Extract<7>(), 107);
  }

  {
    const UInt8 result = Max(UInt8(102, 101, 104, 103, 106, 105, 108, 107),
                             UInt8(105, 2, 3, 4, 5, 6, 7, 8));

    EXPECT_EQ(result.Extract<0>(), 105);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
    EXPECT_EQ(result.Extract<4>(), 106);
    EXPECT_EQ(result.Extract<5>(), 105);
    EXPECT_EQ(result.Extract<6>(), 108);
    EXPECT_EQ(result.Extract<7>(), 107);
  }

  {
    const UInt8 result = Max(UInt8(102, 101, 104, 103, 106, 105, 108, 107),
                             UInt8(1, 106, 3, 4, 5, 6, 7, 8));

    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 106);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
    EXPECT_EQ(result.Extract<4>(), 106);
    EXPECT_EQ(result.Extract<5>(), 105);
    EXPECT_EQ(result.Extract<6>(), 108);
    EXPECT_EQ(result.Extract<7>(), 107);
  }

  {
    const UInt8 result = Max(UInt8(102, 101, 104, 103, 106, 105, 108, 107),
                             UInt8(1, 2, 107, 4, 5, 6, 7, 8));

    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 107);
    EXPECT_EQ(result.Extract<3>(), 103);
    EXPECT_EQ(result.Extract<4>(), 106);
    EXPECT_EQ(result.Extract<5>(), 105);
    EXPECT_EQ(result.Extract<6>(), 108);
    EXPECT_EQ(result.Extract<7>(), 107);
  }

  {
    const UInt8 result = Max(UInt8(102, 101, 104, 103, 106, 105, 108, 107),
                             UInt8(1, 2, 3, 108, 5, 6, 7, 8));

    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 108);
    EXPECT_EQ(result.Extract<4>(), 106);
    EXPECT_EQ(result.Extract<5>(), 105);
    EXPECT_EQ(result.Extract<6>(), 108);
    EXPECT_EQ(result.Extract<7>(), 107);
  }

  {
    const UInt8 result = Max(UInt8(102, 101, 104, 103, 106, 105, 108, 107),
                             UInt8(1, 2, 3, 4, 110, 6, 7, 8));

    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
    EXPECT_EQ(result.Extract<4>(), 110);
    EXPECT_EQ(result.Extract<5>(), 105);
    EXPECT_EQ(result.Extract<6>(), 108);
    EXPECT_EQ(result.Extract<7>(), 107);
  }

  {
    const UInt8 result = Max(UInt8(102, 101, 104, 103, 106, 105, 108, 107),
                             UInt8(1, 2, 3, 4, 5, 110, 7, 8));

    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
    EXPECT_EQ(result.Extract<4>(), 106);
    EXPECT_EQ(result.Extract<5>(), 110);
    EXPECT_EQ(result.Extract<6>(), 108);
    EXPECT_EQ(result.Extract<7>(), 107);
  }

  {
    const UInt8 result = Max(UInt8(102, 101, 104, 103, 106, 105, 108, 107),
                             UInt8(1, 2, 3, 4, 5, 6, 110, 8));

    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
    EXPECT_EQ(result.Extract<4>(), 106);
    EXPECT_EQ(result.Extract<5>(), 105);
    EXPECT_EQ(result.Extract<6>(), 110);
    EXPECT_EQ(result.Extract<7>(), 107);
  }

  {
    const UInt8 result = Max(UInt8(102, 101, 104, 103, 106, 105, 108, 107),
                             UInt8(1, 2, 3, 4, 5, 6, 7, 110));

    EXPECT_EQ(result.Extract<0>(), 102);
    EXPECT_EQ(result.Extract<1>(), 101);
    EXPECT_EQ(result.Extract<2>(), 104);
    EXPECT_EQ(result.Extract<3>(), 103);
    EXPECT_EQ(result.Extract<4>(), 106);
    EXPECT_EQ(result.Extract<5>(), 105);
    EXPECT_EQ(result.Extract<6>(), 108);
    EXPECT_EQ(result.Extract<7>(), 110);
  }
}

TEST(UInt8, HorizontalMax) {
  EXPECT_EQ(HorizontalMax(UInt8(12, 3, 4, 5, 6, 7, 8, 9)), 12);
  EXPECT_EQ(HorizontalMax(UInt8(2, 13, 4, 5, 6, 7, 8, 9)), 13);
  EXPECT_EQ(HorizontalMax(UInt8(2, 3, 14, 5, 6, 7, 8, 9)), 14);
  EXPECT_EQ(HorizontalMax(UInt8(2, 3, 4, 15, 6, 7, 8, 9)), 15);
  EXPECT_EQ(HorizontalMax(UInt8(2, 3, 4, 5, 16, 7, 8, 9)), 16);
  EXPECT_EQ(HorizontalMax(UInt8(2, 3, 4, 5, 6, 17, 8, 9)), 17);
  EXPECT_EQ(HorizontalMax(UInt8(2, 3, 4, 5, 6, 7, 18, 9)), 18);
  EXPECT_EQ(HorizontalMax(UInt8(2, 3, 4, 5, 6, 7, 8, 19)), 19);
}

TEST(UInt8, Select) {
  const UInt8 mask(
      0xffffffffu, 0, 0xffffffffu, 0, 0, 0xffffffffu, 0, 0xffffffffu);
  const UInt8 source1(2, 3, 4, 5, 10, 11, 12, 13);
  const UInt8 source2(6, 7, 8, 9, 14, 15, 16, 17);

  const UInt8 result = Select(mask, source1, source2);
  EXPECT_EQ(result.Extract<0>(), 2);
  EXPECT_EQ(result.Extract<1>(), 7);
  EXPECT_EQ(result.Extract<2>(), 4);
  EXPECT_EQ(result.Extract<3>(), 9);
  EXPECT_EQ(result.Extract<4>(), 14);
  EXPECT_EQ(result.Extract<5>(), 11);
  EXPECT_EQ(result.Extract<6>(), 16);
  EXPECT_EQ(result.Extract<7>(), 13);
}

TEST(UInt8, Reverse) {
  {
    const UInt8 result = Reverse(UInt8(1, 2, 3, 4, 5, 6, 7, 8));
    EXPECT_EQ(result.Extract<0>(), 8);
    EXPECT_EQ(result.Extract<1>(), 7);
    EXPECT_EQ(result.Extract<2>(), 6);
    EXPECT_EQ(result.Extract<3>(), 5);
    EXPECT_EQ(result.Extract<4>(), 4);
    EXPECT_EQ(result.Extract<5>(), 3);
    EXPECT_EQ(result.Extract<6>(), 2);
    EXPECT_EQ(result.Extract<7>(), 1);
  }
}

}  // namespace radio_core
