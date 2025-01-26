// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/ring_double_buffer.h"

#include <array>
#include <span>
#include <vector>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ElementsAre;

TEST(RingDoubleBuffer, InitZero) {
  RingDoubleBuffer<int> buffer(10);

  EXPECT_THAT(buffer.GetElements(), ElementsAre(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(RingDoubleBuffer, push) {
  RingDoubleBuffer<int> buffer(10);

  for (int i = 0; i < 10; ++i) {
    buffer.push(i);
  }

  EXPECT_THAT(buffer.GetElements(), ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));

  for (int i = 10; i < 15; ++i) {
    buffer.push(i);
  }

  EXPECT_THAT(buffer.GetElements(),
              ElementsAre(5, 6, 7, 8, 9, 10, 11, 12, 13, 14));
}

TEST(RingDoubleBuffer, push_multiple) {
  {
    RingDoubleBuffer<int> buffer(10);

    buffer.push_multiple(std::to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

    EXPECT_THAT(buffer.GetElements(),
                ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));

    buffer.push_multiple(std::to_array({10, 11, 12, 13, 14}));

    EXPECT_THAT(buffer.GetElements(),
                ElementsAre(5, 6, 7, 8, 9, 10, 11, 12, 13, 14));
  }

  {
    RingDoubleBuffer<int> buffer(10);

    buffer.push_multiple(
        std::to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));

    EXPECT_THAT(buffer.GetElements(),
                ElementsAre(5, 6, 7, 8, 9, 10, 11, 12, 13, 14));
  }

  {
    RingDoubleBuffer<int> buffer(10);

    buffer.push_multiple(std::to_array({99, 98, 97, 96}));
    buffer.push_multiple(
        std::to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));

    EXPECT_THAT(buffer.GetElements(),
                ElementsAre(5, 6, 7, 8, 9, 10, 11, 12, 13, 14));
  }
}

TEST(RingDoubleBuffer, ElementAccess) {
  RingDoubleBuffer<int> buffer(10);

  // Push samples 0, 1, 2, ... , 9.
  for (int sample = 0; sample < 10; ++sample) {
    buffer.push(sample);
  }

  // Expect samples 0, 1, 2, ... , 9.
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(buffer[i], i);
    EXPECT_EQ(buffer.at(i), i);
  }

  // Push samples 10, 11, ... , 14.
  for (int sample = 10; sample < 15; ++sample) {
    buffer.push(sample);
  }

  // Expect samples 5, ..., 12, 13, 14.
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(buffer[i], i + 5);
    EXPECT_EQ(buffer.at(i), i + 5);
  }

  EXPECT_THROW_OR_ABORT(void(buffer.at(10)), std::out_of_range);
}

TEST(RingDoubleBuffer, fill) {
  {
    RingDoubleBuffer<int> buffer(10);
    buffer.push_multiple(std::to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    buffer.fill(-2);
    EXPECT_THAT(buffer.GetElements(),
                ElementsAre(-2, -2, -2, -2, -2, -2, -2, -2, -2, -2));
  }
}

TEST(RingDoubleBuffer, front) {
  RingDoubleBuffer<int> buffer(10);
  buffer.push_multiple(std::to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
  EXPECT_EQ(buffer.front(), 1);
}

TEST(RingDoubleBuffer, back) {
  RingDoubleBuffer<int> buffer(10);
  buffer.push_multiple(std::to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
  EXPECT_EQ(buffer.back(), 10);
}

}  // namespace radio_core
