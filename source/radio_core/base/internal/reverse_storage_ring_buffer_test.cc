// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/reverse_storage_ring_buffer.h"

#include <array>
#include <span>
#include <vector>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ElementsAre;

template <class T>
static auto BufferToFlatNewToOldStorage(
    const ReverseStorageRingBuffer<T>& buffer) -> std::vector<T> {
  const size_t size = buffer.size();

  std::vector<T> result;
  result.reserve(size);

  const std::span<const T> new_elements = buffer.GetContinuousNewElements();
  for (const T& element : new_elements) {
    result.push_back(element);
  }

  const std::span<const T> old_elements = buffer.GetContinuousOldElements();
  for (const T& element : old_elements) {
    result.push_back(element);
  }

  return result;
}

TEST(ReverseStorageRingBuffer, InitZero) {
  ReverseStorageRingBuffer<int> buffer(10);

  EXPECT_THAT(BufferToFlatNewToOldStorage(buffer),
              ElementsAre(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(ReverseStorageRingBuffer, push) {
  ReverseStorageRingBuffer<int> buffer(10);

  for (int i = 0; i < 10; ++i) {
    buffer.push(i);
  }

  EXPECT_THAT(BufferToFlatNewToOldStorage(buffer),
              ElementsAre(9, 8, 7, 6, 5, 4, 3, 2, 1, 0));

  for (int i = 10; i < 15; ++i) {
    buffer.push(i);
  }

  EXPECT_THAT(BufferToFlatNewToOldStorage(buffer),
              ElementsAre(14, 13, 12, 11, 10, 9, 8, 7, 6, 5));
}

TEST(ReverseStorageRingBuffer, push_multiple) {
  {
    ReverseStorageRingBuffer<int> buffer(10);

    buffer.push_multiple(std::to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

    EXPECT_THAT(BufferToFlatNewToOldStorage(buffer),
                ElementsAre(9, 8, 7, 6, 5, 4, 3, 2, 1, 0));

    buffer.push_multiple(std::to_array({10, 11, 12, 13, 14}));

    EXPECT_THAT(BufferToFlatNewToOldStorage(buffer),
                ElementsAre(14, 13, 12, 11, 10, 9, 8, 7, 6, 5));
  }

  {
    ReverseStorageRingBuffer<int> buffer(10);

    buffer.push_multiple(
        std::to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));

    EXPECT_THAT(BufferToFlatNewToOldStorage(buffer),
                ElementsAre(14, 13, 12, 11, 10, 9, 8, 7, 6, 5));
  }

  {
    ReverseStorageRingBuffer<int> buffer(10);

    buffer.push_multiple(std::to_array({99, 98, 97, 96}));
    buffer.push_multiple(
        std::to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));

    EXPECT_THAT(BufferToFlatNewToOldStorage(buffer),
                ElementsAre(14, 13, 12, 11, 10, 9, 8, 7, 6, 5));
  }
}

TEST(ReverseStorageRingBuffer, ElementAccess) {
  ReverseStorageRingBuffer<int> buffer(10);

  // Push samples 0, 1, 2, ... , 9.
  for (int sample = 0; sample < 10; ++sample) {
    buffer.push(sample);
  }

  // Expect samples 9, 8, 8 ... , 0.
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(buffer[i], 9 - i);
  }

  // Push samples 10, 11, ... , 14.
  for (int sample = 10; sample < 15; ++sample) {
    buffer.push(sample);
  }

  // Expect samples 14, 13, 12 ... , 5.
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(buffer[i], (9 - i + 5));
  }
}

}  // namespace radio_core
