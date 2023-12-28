// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/window.h"

#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(Window, Boxcar) {
  // scipy.signal.windows.boxcar(31)
  const std::array<float, 31> kExpectedWindow = {{
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
  }};

  std::array<float, 31> actual_window;
  GenerateWindow<float>(actual_window,
                        WindowEquation<float, Window::kBoxcar>());

  EXPECT_THAT(actual_window, Pointwise(FloatNear(1e-6f), kExpectedWindow));
}

TEST(Window, Triangular) {
  // scipy.signal.windows.bartlett(31)
  const std::array<float, 31> kExpectedWindow = {{
      0.0f, 0.06666667f, 0.13333333f, 0.2f, 0.26666667f,  0.333333330f,
      0.4f, 0.46666667f, 0.53333333f, 0.6f, 0.666666670f, 0.73333333f,
      0.8f, 0.86666667f, 0.93333333f, 1.0f, 0.93333333f,  0.86666667f,
      0.8f, 0.73333333f, 0.66666667f, 0.6f, 0.53333333f,  0.46666667f,
      0.4f, 0.33333333f, 0.26666667f, 0.2f, 0.13333333f,  0.06666667f,
      0.0f,
  }};

  std::array<float, 31> actual_window;
  GenerateWindow<float>(actual_window,
                        WindowEquation<float, Window::kTriangular>());

  EXPECT_THAT(actual_window, Pointwise(FloatNear(1e-6f), kExpectedWindow));
}

TEST(Window, Hann) {
  // scipy.signal.windows.hann(31)
  const std::array<float, 31> kExpectedWindow = {{
      0.0f,  0.0109262f, 0.04322727f, 0.0954915f,  0.1654347f,
      0.25f, 0.3454915f, 0.44773577f, 0.55226423f, 0.6545085f,
      0.75f, 0.8345653f, 0.9045085f,  0.95677273f, 0.9890738f,
      1.0f,  0.9890738f, 0.95677273f, 0.9045085f,  0.8345653f,
      0.75f, 0.6545085f, 0.55226423f, 0.44773577f, 0.3454915f,
      0.25f, 0.1654347f, 0.0954915f,  0.04322727f, 0.0109262f,
      0.0f,
  }};

  std::array<float, 31> actual_window;
  GenerateWindow<float>(actual_window, WindowEquation<float, Window::kHann>());

  EXPECT_THAT(actual_window, Pointwise(FloatNear(1e-6f), kExpectedWindow));
}

TEST(Window, Hamming) {
  // scipy.signal.windows.hamming(31)
  const std::array<float, 31> kExpectedWindow = {{
      0.08f, 0.0900521f,  0.11976909f, 0.16785218f, 0.23219992f,
      0.31f, 0.39785218f, 0.49191691f, 0.58808309f, 0.68214782f,
      0.77f, 0.84780008f, 0.91214782f, 0.96023091f, 0.9899479f,
      1.0f,  0.9899479f,  0.96023091f, 0.91214782f, 0.84780008f,
      0.77f, 0.68214782f, 0.58808309f, 0.49191691f, 0.39785218f,
      0.31f, 0.23219992f, 0.16785218f, 0.11976909f, 0.0900521f,
      0.08f,
  }};

  std::array<float, 31> actual_window;
  GenerateWindow<float>(actual_window,
                        WindowEquation<float, Window::kHamming>());

  EXPECT_THAT(actual_window, Pointwise(FloatNear(1e-6f), kExpectedWindow));
}

TEST(Window, OptimalHamming) {
  // scipy.signal.windows.general_hamming(31, 0.53836)
  const std::array<float, 31> kExpectedWindow = {{
      0.07672f, 0.08680794f, 0.11663087f, 0.16488539f, 0.22946255f,
      0.30754f, 0.39570539f, 0.49010548f, 0.58661452f, 0.68101461f,
      0.76918f, 0.84725745f, 0.91183461f, 0.96008913f, 0.98991206f,
      1.0f,     0.98991206f, 0.96008913f, 0.91183461f, 0.84725745f,
      0.76918f, 0.68101461f, 0.58661452f, 0.49010548f, 0.39570539f,
      0.30754f, 0.22946255f, 0.16488539f, 0.11663087f, 0.08680794f,
      0.07672f,
  }};

  std::array<float, 31> actual_window;
  GenerateWindow<float>(actual_window,
                        WindowEquation<float, Window::kOptimalHamming>());

  EXPECT_THAT(actual_window, Pointwise(FloatNear(1e-6f), kExpectedWindow));
}

TEST(Window, Blackman) {
  // scipy.signal.windows.blackman(31)
  const std::array<float, 31> kExpectedWindow = {{
      -1.38777878e-17f,
      0.00400983f,
      0.01675771f,
      0.04021286f,
      0.07707241f,
      0.13f,
      0.20077014f,
      0.28948396f,
      0.39401242f,
      0.50978713f,
      0.63f,
      0.74620302f,
      0.84922985f,
      0.93030317f,
      0.98215743f,
      1.0f,
      0.98215743f,
      0.93030317f,
      0.84922985f,
      0.74620302f,
      0.63f,
      0.50978713f,
      0.39401242f,
      0.28948396f,
      0.20077014f,
      0.13f,
      0.07707241f,
      0.04021286f,
      0.01675771f,
      0.00400983f,
      -1.38777878e-17f,
  }};

  std::array<float, 31> actual_window;
  GenerateWindow<float>(actual_window,
                        WindowEquation<float, Window::kBlackman>());

  EXPECT_THAT(actual_window, Pointwise(FloatNear(1e-6f), kExpectedWindow));
}

TEST(Window, Cosine) {
  // scipy.signal.windows.cosine(31)
  const std::array<float, 31> kExpectedWindow = {{
      0.05064917f, 0.15142778f, 0.25065253f, 0.34730525f, 0.44039415f,
      0.52896401f, 0.61210598f, 0.68896692f, 0.75875812f, 0.82076344f,
      0.87434662f, 0.91895781f, 0.95413926f, 0.97952994f, 0.99486932f,
      1.0f,        0.99486932f, 0.97952994f, 0.95413926f, 0.91895781f,
      0.87434662f, 0.82076344f, 0.75875812f, 0.68896692f, 0.61210598f,
      0.52896401f, 0.44039415f, 0.34730525f, 0.25065253f, 0.15142778f,
      0.05064917f,
  }};

  std::array<float, 31> actual_window;
  GenerateWindow<float>(actual_window,
                        WindowEquation<float, Window::kCosine>());

  EXPECT_THAT(actual_window, Pointwise(FloatNear(1e-6f), kExpectedWindow));
}

TEST(Window, Kaiser) {
  // scipy.signal.windows.kaiser(31, 3)
  const std::array<float, 31> kExpectedWindow = {{
      0.20488476f, 0.26875039f, 0.33671844f, 0.40763038f, 0.48020156f,
      0.5530529f,  0.62474621f, 0.6938218f,  0.75883754f, 0.81840786f,
      0.87124187f, 0.9161791f,  0.95222186f, 0.97856333f, 0.99461031f,
      1.0f,        0.99461031f, 0.97856333f, 0.95222186f, 0.9161791f,
      0.87124187f, 0.81840786f, 0.75883754f, 0.6938218f,  0.62474621f,
      0.5530529f,  0.48020156f, 0.40763038f, 0.33671844f, 0.26875039f,
      0.20488476f,
  }};

  std::array<float, 31> actual_window;
  GenerateWindow<float>(actual_window,
                        WindowEquation<float, Window::kKaiser>(3.0f));

  EXPECT_THAT(actual_window, Pointwise(FloatNear(1e-6f), kExpectedWindow));
}

}  // namespace radio_core::signal
