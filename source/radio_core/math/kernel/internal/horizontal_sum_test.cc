// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/horizontal_sum.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(HorizontalSum, Basic) {
  {
    const std::array<int, 6> samples = {0, -1, 2, -3, 4, -5};
    EXPECT_EQ(kernel::HorizontalSum<int>(samples), -3);
  }

  {
    const std::array<int, 6> samples = {-1, -2, -3, -4, -5, -6};
    EXPECT_EQ(kernel::HorizontalSum<int>(samples), -21);
  }
}

TEST(HorizontalSum, Float) {
  {
    const auto samples =
        std::to_array<float>({0, -1, 2, -3, 4, -5, 9, 7, -8, -6});
    EXPECT_EQ(kernel::HorizontalSum<float>(samples), -1);
  }

  {
    const auto samples =
        std::to_array<float>({-9, -2, -3, -4, -5, -1, -7, -8, -6, -7});
    EXPECT_EQ(kernel::HorizontalSum<float>(samples), -52);
  }
}

#if RADIO_CORE_HAVE_HALF

TEST(HorizontalSum, Half) {
  {
    const auto samples =
        std::to_array<Half>({0, -1, 2, -3, 4, -5, 9, 7, -8, -6});
    EXPECT_EQ(kernel::HorizontalSum<Half>(samples), Half(-1));
  }

  {
    const auto samples =
        std::to_array<Half>({-9, -1, -2, -3, -4, -5, -1, -7, -8, -6});
    EXPECT_EQ(kernel::HorizontalSum<Half>(samples), Half(-46));
  }
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
