// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Utility class to write signal to a debug file for the signal visualization.
// The visualization is done by the `plot_debug.py` tool.

#pragma once

#include <cstdio>
#include <filesystem>
#include <span>
#include <string>

#include "radio_core/base/build_config.h"

namespace radio_core::signal {

class DebugWriter {
 public:
  DebugWriter() = default;

  ~DebugWriter() { Close(); }

  DebugWriter(DebugWriter&& other) noexcept : stream_(other.stream_) {
    other.stream_ = nullptr;
  }
  DebugWriter& operator=(DebugWriter&& other) {
    if (this == &other) {
      return *this;
    }

    stream_ = other.stream_;

    other.stream_ = nullptr;

    return *this;
  }

  // Disallow copy.
  DebugWriter(const DebugWriter& other) = delete;
  DebugWriter& operator=(const DebugWriter& other) = delete;

  // Open writer to the specific file.
  inline auto Open(const std::filesystem::path& path) -> bool {
    need_close_ = true;

#if COMPILER_MSVC
    // Use deprecated std::filesystem::u8path as it seems to be the only way to
    // support non-ASCII file names.
    //
    // Using both u8string and string to create path and pass path.c_str() to
    // any function which expects wide string (_wsopen_s, CreateFileW, ...) does
    // not handle Unicode names correctly.
#  pragma warning(push)
#  pragma warning(disable : 4996)

    const errno_t error = ::_wfopen_s(
        &stream_, std::filesystem::u8path(path.string()).c_str(), L"w");

#  pragma warning(pop)

    if (error != 0) {
      return false;
    }
#else
    stream_ = ::fopen(path.c_str(), "w");
#endif

    if (!stream_) {
      return false;
    }

    return true;
  }

  // Open writer which will print signal into the given stream.
  // It is up to the caller to close the stream if and when needed.
  inline auto Open(FILE* stream) -> bool {
    need_close_ = false;

    stream_ = stream;
    if (!stream_) {
      return false;
    }

    return true;
  }

  // Write sample of the signal denoted by its label.
  inline void WriteSample(const char* label, const float x, const float y) {
    fprintf(stream_, "l:%s x:%f y:%f\n", label, double(x), double(y));
  }

  // Write signal in the format which is understood by the plotting tool.
  void WriteSignal(const char* label,
                   const std::span<float> signal,
                   const int sample_rate) {
    const size_t num_samples = signal.size();
    const float inv_sample_rate = 1.0f / float(sample_rate);
    for (size_t i = 0; i < num_samples; ++i) {
      const float time = float(i) * inv_sample_rate;
      WriteSample(label, time, signal[i]);
    }
  }

 private:
  inline void Close() {
    if (need_close_ && stream_) {
      ::fclose(stream_);
      stream_ = nullptr;
    }
  }

  FILE* stream_{nullptr};
  bool need_close_{false};
};

}  // namespace radio_core::signal
