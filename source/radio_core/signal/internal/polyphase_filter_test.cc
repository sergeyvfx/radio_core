// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/polyphase_filter.h"

#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::ElementsAre;

TEST(polyphase_filter, PolyphaseDecompositionSize) {
  EXPECT_EQ(PolyphaseDecompositionSize(100, 2), 50);

  EXPECT_EQ(PolyphaseDecompositionSize(99, 2), 49);
}

TEST(polyphase_filter, PolyphaseComponentDecomposition) {
  const auto h = std::to_array<float>({1, 2, 3, 4, 5, 6, 7, 8, 9});

  {
    std::array<float, 4> e;
    PolyphaseComponentDecomposition<float>(h, 2, 0, e);

    EXPECT_THAT(e, ElementsAre(1, 3, 5, 7));
  }

  {
    std::array<float, 4> e;
    PolyphaseComponentDecomposition<float>(h, 2, 1, e);

    EXPECT_THAT(e, ElementsAre(2, 4, 6, 8));
  }
}

}  // namespace radio_core::signal
