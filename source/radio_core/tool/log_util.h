// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

// Helper for writing tools.
//
// This module provides functionality which provides higher level logging

#pragma once

#include <ostream>

namespace radio_core::tool {

// Helper class to log the given processing time with its comparison to the
// realtime. It prints the absolute execution time in seconds and a speedup
// in times compared to realtime. For example:
//   0.421567 seconds (142.326x realtime)
//
// An example in code:
//   cout << "Processing took "
//          << LogTimeWithRealtimeComparison(timer.GetElapsedTimeInSeconds(),
//                                           file_duration_in_seconds)
template <class T>
class LogTimeWithRealtimeComparison {
 public:
  LogTimeWithRealtimeComparison(const T processing_time_in_seconds,
                                const T real_time_in_seconds)
      : processing_time_in_seconds_(processing_time_in_seconds),
        real_time_in_seconds_(real_time_in_seconds) {}

  friend auto operator<<(std::ostream& os,
                         const LogTimeWithRealtimeComparison& info)
      -> std::ostream& {
    os << info.processing_time_in_seconds_ << " seconds";
    if (info.processing_time_in_seconds_ != 0) {
      os << " ("
         << (info.real_time_in_seconds_ / info.processing_time_in_seconds_)
         << "x realtime)";
    }

    return os;
  }

 private:
  const T processing_time_in_seconds_;
  const T real_time_in_seconds_;
};

}  // namespace radio_core::tool
