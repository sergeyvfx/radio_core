// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/ushort8.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(UShort8, Load) {
  {
    const uint16_t values[8] = {
        0xfff2u, 0xfff3u, 0xfff4u, 0xfff5u, 0xfff6u, 0xfff7u, 0xfff8u, 0xfff9u};
    const UShort8 value = UShort8(values);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfff5u);
    EXPECT_EQ(value.Extract<4>(), 0xfff6u);
    EXPECT_EQ(value.Extract<5>(), 0xfff7u);
    EXPECT_EQ(value.Extract<6>(), 0xfff8u);
    EXPECT_EQ(value.Extract<7>(), 0xfff9u);
  }

  {
    const UShort8 value = UShort8(
        0xfff2u, 0xfff3u, 0xfff4u, 0xfff5u, 0xfff6u, 0xfff7u, 0xfff8u, 0xfff9u);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfff5u);
    EXPECT_EQ(value.Extract<4>(), 0xfff6u);
    EXPECT_EQ(value.Extract<5>(), 0xfff7u);
    EXPECT_EQ(value.Extract<6>(), 0xfff8u);
    EXPECT_EQ(value.Extract<7>(), 0xfff9u);
  }

  {
    const UShort8 value = UShort8(0xfff2u);

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff2u);
    EXPECT_EQ(value.Extract<2>(), 0xfff2u);
    EXPECT_EQ(value.Extract<3>(), 0xfff2u);
    EXPECT_EQ(value.Extract<4>(), 0xfff2u);
    EXPECT_EQ(value.Extract<5>(), 0xfff2u);
    EXPECT_EQ(value.Extract<6>(), 0xfff2u);
    EXPECT_EQ(value.Extract<7>(), 0xfff2u);
  }

  {
    const UShort8 value = UShort8(UShort4(0xfff2u, 0xfff3u, 0xfff4u, 0xfff5u),
                                  UShort4(0xfff6u, 0xfff7u, 0xfff8u, 0xfff9u));

    EXPECT_EQ(value.Extract<0>(), 0xfff2u);
    EXPECT_EQ(value.Extract<1>(), 0xfff3u);
    EXPECT_EQ(value.Extract<2>(), 0xfff4u);
    EXPECT_EQ(value.Extract<3>(), 0xfff5u);
    EXPECT_EQ(value.Extract<4>(), 0xfff6u);
    EXPECT_EQ(value.Extract<5>(), 0xfff7u);
    EXPECT_EQ(value.Extract<6>(), 0xfff8u);
    EXPECT_EQ(value.Extract<7>(), 0xfff9u);
  }
}

TEST(UShort8, Store) {
  {
    const UShort8 value = UShort8(2, 3, 4, 5, 6, 7, 8, 9);
    uint16_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
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
    const UShort8 value = UShort8(2, 3, 4, 5, 6, 7, 8, 9);
    uint16_t data;

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

TEST(UShort8, Extract) {
  {
    const UShort8 value = UShort8(2, 3, 4, 5, 6, 7, 8, 9);

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

TEST(UShort8, ExtractLow) {
  const UShort8 value = UShort8(2, 3, 4, 5, 6, 7, 8, 9);

  const UShort4 low = value.ExtractLow();

  EXPECT_EQ(low.Extract<0>(), 2);
  EXPECT_EQ(low.Extract<1>(), 3);
  EXPECT_EQ(low.Extract<2>(), 4);
  EXPECT_EQ(low.Extract<3>(), 5);
}

TEST(UShort8, ExtractHigh) {
  const UShort8 value = UShort8(2, 3, 4, 5, 6, 7, 8, 9);

  const UShort4 low = value.ExtractHigh();

  EXPECT_EQ(low.Extract<0>(), 6);
  EXPECT_EQ(low.Extract<1>(), 7);
  EXPECT_EQ(low.Extract<2>(), 8);
  EXPECT_EQ(low.Extract<3>(), 9);
}

TEST(UShort8, Min) {
  {
    const UShort8 result = Min(UShort8(12, 13, 14, 15, 16, 17, 18, 19),
                               UShort8(21, 22, 23, 24, 25, 26, 27, 28));
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
    const UShort8 result = Min(UShort8(12, 13, 14, 15, 16, 17, 18, 19),
                               UShort8(1, 22, 23, 24, 25, 26, 27, 28));
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
    const UShort8 result = Min(UShort8(12, 13, 14, 15, 16, 17, 18, 19),
                               UShort8(21, 2, 23, 24, 25, 26, 27, 28));
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
    const UShort8 result = Min(UShort8(12, 13, 14, 15, 16, 17, 18, 19),
                               UShort8(21, 22, 3, 24, 25, 26, 27, 28));
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
    const UShort8 result = Min(UShort8(12, 13, 14, 15, 16, 17, 18, 19),
                               UShort8(21, 22, 23, 4, 25, 26, 27, 28));
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
    const UShort8 result = Min(UShort8(12, 13, 14, 15, 16, 17, 18, 19),
                               UShort8(21, 22, 23, 24, 5, 26, 27, 28));
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
    const UShort8 result = Min(UShort8(12, 13, 14, 15, 16, 17, 18, 19),
                               UShort8(21, 22, 23, 24, 25, 6, 27, 28));
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
    const UShort8 result = Min(UShort8(12, 13, 14, 15, 16, 17, 18, 19),
                               UShort8(21, 22, 23, 24, 25, 26, 7, 28));
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
    const UShort8 result = Min(UShort8(12, 13, 14, 15, 16, 17, 18, 19),
                               UShort8(21, 22, 23, 24, 25, 26, 27, 8));
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

TEST(UShort8, Max) {
  {
    const UShort8 result = Max(UShort8(102, 101, 104, 103, 106, 105, 108, 107),
                               UShort8(1, 2, 3, 4, 5, 6, 7, 8));
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
    const UShort8 result = Max(UShort8(102, 101, 104, 103, 106, 105, 108, 107),
                               UShort8(105, 2, 3, 4, 5, 6, 7, 8));

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
    const UShort8 result = Max(UShort8(102, 101, 104, 103, 106, 105, 108, 107),
                               UShort8(1, 106, 3, 4, 5, 6, 7, 8));

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
    const UShort8 result = Max(UShort8(102, 101, 104, 103, 106, 105, 108, 107),
                               UShort8(1, 2, 107, 4, 5, 6, 7, 8));

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
    const UShort8 result = Max(UShort8(102, 101, 104, 103, 106, 105, 108, 107),
                               UShort8(1, 2, 3, 108, 5, 6, 7, 8));

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
    const UShort8 result = Max(UShort8(102, 101, 104, 103, 106, 105, 108, 107),
                               UShort8(1, 2, 3, 4, 110, 6, 7, 8));

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
    const UShort8 result = Max(UShort8(102, 101, 104, 103, 106, 105, 108, 107),
                               UShort8(1, 2, 3, 4, 5, 110, 7, 8));

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
    const UShort8 result = Max(UShort8(102, 101, 104, 103, 106, 105, 108, 107),
                               UShort8(1, 2, 3, 4, 5, 6, 110, 8));

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
    const UShort8 result = Max(UShort8(102, 101, 104, 103, 106, 105, 108, 107),
                               UShort8(1, 2, 3, 4, 5, 6, 7, 110));

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

TEST(UShort8, HorizontalMax) {
  EXPECT_EQ(HorizontalMax(UShort8(12, 3, 4, 5, 6, 7, 8, 9)), 12);
  EXPECT_EQ(HorizontalMax(UShort8(2, 13, 4, 5, 6, 7, 8, 9)), 13);
  EXPECT_EQ(HorizontalMax(UShort8(2, 3, 14, 5, 6, 7, 8, 9)), 14);
  EXPECT_EQ(HorizontalMax(UShort8(2, 3, 4, 15, 6, 7, 8, 9)), 15);
  EXPECT_EQ(HorizontalMax(UShort8(2, 3, 4, 5, 16, 7, 8, 9)), 16);
  EXPECT_EQ(HorizontalMax(UShort8(2, 3, 4, 5, 6, 17, 8, 9)), 17);
  EXPECT_EQ(HorizontalMax(UShort8(2, 3, 4, 5, 6, 7, 18, 9)), 18);
  EXPECT_EQ(HorizontalMax(UShort8(2, 3, 4, 5, 6, 7, 8, 19)), 19);
}

TEST(UShort8, Select) {
  const UShort8 mask(0xffffu, 0, 0xffffu, 0, 0, 0xffffu, 0, 0xffffu);
  const UShort8 source1(2, 3, 4, 5, 10, 11, 12, 13);
  const UShort8 source2(6, 7, 8, 9, 14, 15, 16, 17);

  const UShort8 result = Select(mask, source1, source2);
  EXPECT_EQ(result.Extract<0>(), 2);
  EXPECT_EQ(result.Extract<1>(), 7);
  EXPECT_EQ(result.Extract<2>(), 4);
  EXPECT_EQ(result.Extract<3>(), 9);
  EXPECT_EQ(result.Extract<4>(), 14);
  EXPECT_EQ(result.Extract<5>(), 11);
  EXPECT_EQ(result.Extract<6>(), 16);
  EXPECT_EQ(result.Extract<7>(), 13);
}

TEST(UShort8, Reverse) {
  {
    const UShort8 result = Reverse(UShort8(1, 2, 3, 4, 5, 6, 7, 8));
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
