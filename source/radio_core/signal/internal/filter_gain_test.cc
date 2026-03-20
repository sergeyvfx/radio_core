// Copyright (c) 2026 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/filter_gain.h"

#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(filter_gain, CalculateFilterGainAtDC) {
  constexpr std::array<float, 5> filter{0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
  EXPECT_NEAR(CalculateFilterGainAtDC<float>(filter), 1.5f, 1e-6f);
}

TEST(filter_gain, CalculateFilterGain) {
  constexpr std::array<float, 5> filter{0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
  EXPECT_NEAR(CalculateFilterGain<float>(filter, 0.0f), 1.5f, 1e-6f);

  // TODO(sergey): Cross-reference with an independent implementation.
  EXPECT_NEAR(CalculateFilterGain<float>(filter, 0.1f), 0.97082036f, 1e-6f);
}

TEST(filter_gain, ScaleFilterToUnityGainAtFrequency) {
  // Unity gain at DC.
  {
    // >>> import numpy as np
    // >>> a = np.array([0.1, 0.2, 0.3, 0.4, 0.5])
    // >>> a / np.sum(a)
    // array([0.06666667, 0.13333333, 0.2       , 0.26666667, 0.33333333])
    std::array<float, 5> filter{0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    ScaleFilterToUnityGainAtFrequency<float>(filter, 0.0f);
    EXPECT_THAT(
        filter,
        Pointwise(
            FloatNear(1e-6f),
            std::to_array(
                {0.06666667f, 0.13333333f, 0.2f, 0.26666667f, 0.33333333f})));
  }

  // Unity gain at frequency.
  {
    // 0,97082036733627319 comes from CalculateFilterGain<float>(filter, 0.1f).
    //
    // >>> import numpy as np
    // >>> a = np.array([0.1, 0.2, 0.3, 0.4, 0.5])
    // >>> a / 0.97082036733627319
    // array([0.10300567, 0.20601134, 0.309017  , 0.41202267, 0.51502834])
    std::array<float, 5> filter{0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    ScaleFilterToUnityGainAtFrequency<float>(filter, 0.1f);
    EXPECT_THAT(filter,
                Pointwise(FloatNear(1e-6f),
                          std::to_array({0.10300567f,
                                         0.20601134f,
                                         0.309017f,
                                         0.41202267f,
                                         0.51502834f})));
  }
}

TEST(filter_gain, ScaleFilterToGainAtDC) {
  std::array<float, 5> filter{0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
  ScaleFilterToGainAtDC<float>(filter, 2.0f);

  // >>> import numpy as np
  // >>> a = np.array([0.1, 0.2, 0.3, 0.4, 0.5])
  // >>> a / np.sum(a) * 2
  // array([0.13333333, 0.26666667, 0.4       , 0.53333333, 0.66666667])
  EXPECT_THAT(
      filter,
      Pointwise(
          FloatNear(1e-6f),
          std::to_array(
              {0.13333333f, 0.26666667f, 0.4f, 0.53333333f, 0.66666667f})));
}

}  // namespace radio_core::signal
