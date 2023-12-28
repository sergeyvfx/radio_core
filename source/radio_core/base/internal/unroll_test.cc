// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/unroll.h"

#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::Eq;
using testing::Pointwise;

TEST(base, Unroll) {
  std::array<int, 5> array;

  Unroll<5>([&](const auto i) { array[i] = i + 1; });

  EXPECT_THAT(array, Pointwise(Eq(), std::to_array<int>({1, 2, 3, 4, 5})));
}

TEST(base, Unroll2D) {
  std::array<std::array<int, 3>, 2> array;

  Unroll<2, 3>([&](const auto i, const auto j) {
    array[i][j] = (i + 1) * 10 + (j + 1);
  });

  EXPECT_THAT(array[0], Pointwise(Eq(), std::to_array<int>({11, 12, 13})));
  EXPECT_THAT(array[1], Pointwise(Eq(), std::to_array<int>({21, 22, 23})));
}

}  // namespace radio_core
