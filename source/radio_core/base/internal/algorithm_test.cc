// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/algorithm.h"

#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::Eq;
using testing::Pointwise;

TEST(algorithm, CircularShift) {
  //////////////////////////////////////////////////////////////////////////////
  // Special cases.

  // No shift.
  {
    auto arr = std::to_array<int>({0, 1, 2, 3, 4, 5});
    CircularShift(arr, 0);
    EXPECT_THAT(arr, Pointwise(Eq(), std::to_array({0, 1, 2, 3, 4, 5})));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Shift to the right.

  // Single element shift to the right.
  //
  // >>> import numpy as np
  // >>> np.roll([0,1,2,3,4,5], 1)
  // array([5, 0, 1, 2, 3, 4])
  {
    auto arr = std::to_array<int>({0, 1, 2, 3, 4, 5});
    CircularShift(arr, 1);
    EXPECT_THAT(arr, Pointwise(Eq(), std::to_array({5, 0, 1, 2, 3, 4})));
  }

  // Multiple elements shift to the right.
  //
  // >>> import numpy as np
  // >>> np.roll([0,1,2,3,4,5], 4)
  // array([2, 3, 4, 5, 0, 1])
  {
    auto arr = std::to_array<int>({0, 1, 2, 3, 4, 5});
    CircularShift(arr, 4);
    EXPECT_THAT(arr, Pointwise(Eq(), std::to_array({2, 3, 4, 5, 0, 1})));
  }

  // Multiple elements shift to the right with wrapping.
  //
  // >>> import numpy as np
  // >>> np.roll([0,1,2,3,4,5], 13)
  // array([5, 0, 1, 2, 3, 4])
  {
    auto arr = std::to_array<int>({0, 1, 2, 3, 4, 5});
    CircularShift(arr, 13);
    EXPECT_THAT(arr, Pointwise(Eq(), std::to_array({5, 0, 1, 2, 3, 4})));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Shift to the left.

  // Single element shift to the left.
  //
  // >>> import numpy as np
  // >>> np.roll([0,1,2,3,4,5], -1)
  // array([1, 2, 3, 4, 5, 0])
  {
    auto arr = std::to_array<int>({0, 1, 2, 3, 4, 5});
    CircularShift(arr, -1);
    EXPECT_THAT(arr, Pointwise(Eq(), std::to_array({1, 2, 3, 4, 5, 0})));
  }

  // Multiple elements shift to the right.
  //
  // >>> import numpy as np
  // >>> np.roll([0,1,2,3,4,5], -4)
  // array([4, 5, 0, 1, 2, 3])
  {
    auto arr = std::to_array<int>({0, 1, 2, 3, 4, 5});
    CircularShift(arr, -4);
    EXPECT_THAT(arr, Pointwise(Eq(), std::to_array({4, 5, 0, 1, 2, 3})));
  }

  // Multiple elements shift to the right with wrapping.
  //
  // >>> import numpy as np
  // >>> np.roll([0,1,2,3,4,5], -13)
  // array([1, 2, 3, 4, 5, 0])
  {
    auto arr = std::to_array<int>({0, 1, 2, 3, 4, 5});
    CircularShift(arr, -13);
    EXPECT_THAT(arr, Pointwise(Eq(), std::to_array({1, 2, 3, 4, 5, 0})));
  }
}

}  // namespace radio_core
