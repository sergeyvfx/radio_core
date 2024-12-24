// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/ring_buffer.h"

#include <algorithm>
#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ElementsAre;

TEST(RingBuffer, size) {
  {
    RingBuffer<float> buffer(5);
    EXPECT_EQ(buffer.size(), 5);
  }

  {
    RingBuffer<float> buffer;
    EXPECT_EQ(buffer.size(), 0);
    buffer.resize(10);
    EXPECT_EQ(buffer.size(), 10);
  }
}

TEST(RingBuffer, empty) {
  RingBuffer<float> buffer;
  EXPECT_TRUE(buffer.empty());
  buffer.resize(10);
  EXPECT_FALSE(buffer.empty());
}

TEST(RingBuffer, front) {
  {
    RingBuffer<int> buffer(5);
    for (int i = 1; i <= 7; ++i) {
      buffer.push_back(i);
    }
    EXPECT_EQ(buffer.front(), 3);
  }
}

TEST(RingBuffer, back) {
  {
    RingBuffer<int> buffer(5);
    for (int i = 1; i <= 7; ++i) {
      buffer.push_back(i);
    }
    EXPECT_EQ(buffer.back(), 7);
  }
}

TEST(RingBuffer, ElementAccess) {
  RingBuffer<int> buffer(5);
  for (int i = 1; i <= 7; ++i) {
    buffer.push_back(i);
  }

  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(buffer[i], i + 3) << "at index " << i;
    EXPECT_EQ(buffer.at(i), i + 3) << "at index " << i;
  }

  EXPECT_THROW_OR_ABORT(void(buffer.at(10)), std::out_of_range);
}

TEST(RingBuffer, ElementSpanAccess) {
  RingBuffer<int> buffer(5);
  for (int i = 1; i <= 7; ++i) {
    buffer.push_back(i);
  }

  EXPECT_THAT(buffer.GetContinuousOldElements(), ElementsAre(3, 4, 5));
  EXPECT_THAT(buffer.GetContinuousNewElements(), ElementsAre(6, 7));

  buffer.push_back(8);
  EXPECT_THAT(buffer.GetContinuousOldElements(), ElementsAre(4, 5));
  EXPECT_THAT(buffer.GetContinuousNewElements(), ElementsAre(6, 7, 8));
}

TEST(RingBuffer, Initial) {
  RingBuffer<float> buffer(5);
  EXPECT_THAT(buffer, ElementsAre(0, 0, 0, 0, 0));
}

TEST(RingBuffer, fill) {
  RingBuffer<float> buffer(5);
  buffer.push_back_multiple({{1, 2, 3, 4, 5, 6, 7}});
  buffer.fill(0);
  EXPECT_THAT(buffer, ElementsAre(0, 0, 0, 0, 0));
}

TEST(RingBuffer, push_back) {
  {
    RingBuffer<float> buffer(5);
    EXPECT_THAT(buffer, ElementsAre(0, 0, 0, 0, 0));

    buffer.push_back(1);
    EXPECT_THAT(buffer, ElementsAre(0, 0, 0, 0, 1));

    buffer.push_back(2);
    EXPECT_THAT(buffer, ElementsAre(0, 0, 0, 1, 2));

    buffer.push_back(3);
    EXPECT_THAT(buffer, ElementsAre(0, 0, 1, 2, 3));

    buffer.push_back(4);
    EXPECT_THAT(buffer, ElementsAre(0, 1, 2, 3, 4));

    buffer.push_back(5);
    EXPECT_THAT(buffer, ElementsAre(1, 2, 3, 4, 5));

    buffer.push_back(6);
    EXPECT_THAT(buffer, ElementsAre(2, 3, 4, 5, 6));

    buffer.push_back(7);
    EXPECT_THAT(buffer, ElementsAre(3, 4, 5, 6, 7));
  }
}

TEST(RingBuffer, push_back_multiple) {
  {
    RingBuffer<float> buffer(5);

    buffer.push_back_multiple({{1, 2, 3, 4, 5, 6, 7}});
    EXPECT_THAT(buffer, ElementsAre(3, 4, 5, 6, 7));

    buffer.push_back_multiple({{8, 9, 10}});
    EXPECT_THAT(buffer, ElementsAre(6, 7, 8, 9, 10));

    buffer.push_back_multiple({{11, 12}});
    EXPECT_THAT(buffer, ElementsAre(8, 9, 10, 11, 12));
  }
}

TEST(RingBuffer, Iterator) {
  {
    RingBuffer<float> buffer(5);

    int num_iterated_elements = 0;
    for (const float element : buffer) {
      (void)element;
      ++num_iterated_elements;
    }
    EXPECT_EQ(num_iterated_elements, 5);
  }

  {
    RingBuffer<int> buffer(5);
    for (int i = 1; i <= 7; ++i) {
      buffer.push_back(i);
    }

    EXPECT_THAT(buffer, ElementsAre(3, 4, 5, 6, 7));

    RingBuffer<int>::iterator it;

    // Prefix increment.
    it = buffer.begin();
    EXPECT_EQ(*(++it), 4);

    // Postfix increment.
    it = buffer.begin();
    EXPECT_EQ(*(it++), 3);
  }
}

TEST(RingBuffer, Algorithm) {
  using Buffer = RingBuffer<int>;

  Buffer buffer;
  buffer.resize(3);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);

  EXPECT_TRUE(std::equal(
      buffer.begin(), buffer.end(), std::to_array({1, 2, 3}).begin()));
}

TEST(RingBuffer, Assign) {
  {
    RingBuffer<float> source_buffer(5);
    source_buffer.push_back_multiple({{1, 2, 3, 4, 5, 6, 7}});

    RingBuffer<float> destination_buffer(5);
    destination_buffer = source_buffer;

    EXPECT_THAT(destination_buffer, ElementsAre(3, 4, 5, 6, 7));
  }
}

}  // namespace radio_core
