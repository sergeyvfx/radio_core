// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Helper class to measure elapsed time in seconds.
//
// The counting starts when the ScopedTimer is created, and is finished when it
// is destroyed.

#pragma once

#include <chrono>

namespace radio_core {

class ScopedTimer {
 public:
  using Clock = std::chrono::steady_clock;
  using TimePoint = Clock::time_point;
  using Duration = std::chrono::duration<float>;

  ScopedTimer() : start_time_point_(Clock::now()) {}

  // Get time measured in seconds since the creation of the ScopedTimer object.
  inline auto GetElapsedTimeInSeconds() const -> float {
    const Duration duration = Clock::now() - start_time_point_;
    return duration.count();
  }

 private:
  const TimePoint start_time_point_;
};

}  // namespace radio_core
