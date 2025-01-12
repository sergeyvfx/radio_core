// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/filter_energy.h"

#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(filter_energy, CalculateFilterEnergy) {
  // >>> import numpy as np
  // >>> np.sum(np.array([0.1, 0.2, 0.3])**2
  // np.float64(0.14)
  EXPECT_NEAR(CalculateFilterEnergy<float>({{0.1f, 0.2f, 0.3f}}), 0.14f, 1e-6f);
}

TEST(filter_energy, ScaleFilterToUnityEnergy) {
  std::array<float, 3> filter({{0.1f, 0.2f, 0.3f}});
  ScaleFilterToUnityEnergy<float>(filter);

  // >>> import numpy as np
  // >>> np.array([0.1, 0.2, 0.3]) / np.sqrt(0.14)
  // array([0.26726124, 0.53452248, 0.80178373])
  EXPECT_THAT(
      filter,
      Pointwise(FloatNear(1e-6f),
                std::to_array({0.26726124f, 0.53452248f, 0.80178373f})));
}

}  // namespace radio_core::signal
