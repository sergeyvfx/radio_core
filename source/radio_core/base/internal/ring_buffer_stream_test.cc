// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/ring_buffer_stream.h"

#include <array>
#include <thread>
#include <vector>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::Eq;
using testing::Pointwise;

TEST(RingBufferStream, Basic) {
  RingBufferStream<int> stream;

  // Initial configuration.
  {
    EXPECT_EQ(stream.Capacity(), 0);
    EXPECT_EQ(stream.Size(), 0);
    EXPECT_TRUE(stream.IsEmpty());

    stream.Reserve(7);
    EXPECT_EQ(stream.Capacity(), 7);
    EXPECT_EQ(stream.Size(), 0);
    EXPECT_TRUE(stream.IsEmpty());
  }

  // Write from the beginning of the buffer.
  {
    stream.Write({{1, 2, 3, 4, 5}});
    EXPECT_EQ(stream.Size(), 5);
    EXPECT_FALSE(stream.IsEmpty());
  }

  // Read from the beginning of the buffer.
  {
    std::array<int, 3> elements;
    stream.Read(elements);
    EXPECT_THAT(elements, Pointwise(Eq(), std::to_array<int>({1, 2, 3})));
    EXPECT_EQ(stream.Size(), 2);
    EXPECT_FALSE(stream.IsEmpty());
  }

  // Write from the middle of the buffer, and fill in the entire buffer.
  {
    stream.Write({{6, 7}});
    stream.Write({{8, 9, 10}});
    EXPECT_EQ(stream.Size(), 7);
    EXPECT_FALSE(stream.IsEmpty());
  }

  // Read from the middle of the buffer.
  {
    std::array<int, 7> elements{};
    stream.Read(elements);
    EXPECT_THAT(elements,
                Pointwise(Eq(), std::to_array<int>({4, 5, 6, 7, 8, 9, 10})));
    EXPECT_EQ(stream.Size(), 0);
    EXPECT_TRUE(stream.IsEmpty());
  }
}

TEST(RingBufferStream, ProducerConsumerThreads) {
  RingBufferStream<int> stream;

  stream.Reserve(5);

  std::thread producer_thread([&]() {
    stream.Write({{1, 2}});
    stream.Write({{3, 4, 5, 6, 7, 8, 9, 10}});
  });

  std::vector<int> consumed_elements;
  std::thread consumer_thread([&]() {
    std::array<int, 10> elements{};
    stream.Read(elements);
    consumed_elements.insert(
        consumed_elements.end(), elements.begin(), elements.end());
  });

  producer_thread.join();
  consumer_thread.join();

  EXPECT_THAT(
      consumed_elements,
      Pointwise(Eq(), std::to_array<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10})));
}

// Make suer producer thread exits from wait when the stream is closed.
TEST(RingBufferStream, ProducerClose) {
  RingBufferStream<int> stream;
  stream.Reserve(5);

  std::thread producer_thread([&]() {
    stream.Write({{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}});
  });

  stream.Close();

  producer_thread.join();
}

// Make suer consumer thread exits from wait when the stream is closed.
TEST(RingBufferStream, ConsumerClose) {
  RingBufferStream<int> stream;
  stream.Reserve(5);

  std::thread consumer_thread([&]() {
    std::array<int, 10> elements{};
    stream.Read(elements);
  });

  stream.Close();

  consumer_thread.join();
}

TEST(RingBufferStream, WriteNonblocking) {
  RingBufferStream<int> stream;

  stream.Reserve(5);

  EXPECT_EQ(stream.WriteNonblocking({{1, 2}}), 2);
  EXPECT_EQ(stream.WriteNonblocking({{3, 4, 5, 6, 7, 8, 9, 10}}), 3);

  {
    std::array<int, 5> elements{};
    EXPECT_TRUE(stream.Read(elements));

    EXPECT_THAT(elements, Pointwise(Eq(), std::to_array<int>({1, 2, 3, 4, 5})));
  }
}

TEST(RingBufferStream, ReadNonblocking) {
  RingBufferStream<int> stream;

  stream.Reserve(10);

  {
    std::array<int, 10> elements{};
    EXPECT_EQ(stream.ReadNonblocking(elements), 0);
  }

  stream.Write({{1, 2, 3, 4}});

  {
    std::array<int, 10> elements{};
    EXPECT_EQ(stream.ReadNonblocking(elements), 4);

    EXPECT_THAT(
        elements,
        Pointwise(Eq(), std::to_array<int>({1, 2, 3, 4, 0, 0, 0, 0, 0, 0})));
  }
}

}  // namespace radio_core
