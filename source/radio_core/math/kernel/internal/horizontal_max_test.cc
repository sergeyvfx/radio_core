// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/horizontal_max.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(HorizontalMax, Basic) {
  {
    const std::array<int, 6> samples = {0, -1, 2, -3, 4, -5};
    EXPECT_EQ(HorizontalMax<int>(samples), 4);
  }

  {
    const std::array<int, 6> samples = {-1, -2, -3, -4, -5, -6};
    EXPECT_EQ(HorizontalMax<int>(samples), -1);
  }
}

TEST(HorizontalMax, Float) {
  {
    const auto samples =
        std::to_array<float>({0, -1, 2, -3, 4, -5, 9, 7, -8, -6});
    EXPECT_EQ(HorizontalMax<float>(samples), 9);
  }

  {
    const auto samples =
        std::to_array<float>({-9, -2, -3, -4, -5, -1, -7, -8, -6, -7});
    EXPECT_EQ(HorizontalMax<float>(samples), -1);
  }
}

#if RADIO_CORE_HAVE_HALF

TEST(HorizontalMax, Half) {
  {
    const auto samples =
        std::to_array<Half>({0, -1, 2, -3, 4, -5, 9, 7, -8, -6});
    EXPECT_EQ(HorizontalMax<Half>(samples), Half(9));
  }

  {
    const auto samples =
        std::to_array<Half>({-9, -1, -2, -3, -4, -5, -1, -7, -8, -6});
    EXPECT_EQ(HorizontalMax<Half>(samples), Half(-1));
  }
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
