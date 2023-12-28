// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Simple analog signal hysteresis implementation:
//
//   - If an input sample is outside of the pre-defined window the sample is
//     returned as-is.
//   - Otherwise the last known sample outside of the window is returned.
//
// The initial value equals to the trigger level (center of the window).

#pragma once

namespace radio_core::signal {

template <class T>
class Hysteresis {
 public:
  Hysteresis() = default;

  Hysteresis(const T level, const T window) { SetThreshold(level, window); }

  constexpr void SetThreshold(const T level, const T window) {
    const T half_window = window / 2;
    min_ = level - half_window;
    max_ = level + half_window;

    previous_result_ = level;
  }

  constexpr auto operator()(const T sample) -> T {
    if (sample > max_) {
      previous_result_ = sample;
    }
    if (sample < min_) {
      previous_result_ = sample;
    }

    return previous_result_;
  }

 protected:
  T min_{0};
  T max_{0};

  T previous_result_{0};
};

}  // namespace radio_core::signal
