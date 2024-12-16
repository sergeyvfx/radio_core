// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

// Helper for writing tools.
//
// This module provides functionality which accumulates samples in a buffer
// before sending them to the WAV writer. This optimizes the memory and disk
// access pattern.

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <span>
#include <type_traits>

#include "tl_audio_wav/tl_audio_wav_writer.h"

namespace radio_core::tool {

// Class which takes care of buffering samples before sending them to the WAV
// writer.
//
// Note that it is required to perform an explicit flush once writing is done to
// ensure all samples are written while the writen object is still available.
//
// The WAV writer must be available throughout the lifetime of this object.
//
// SampleType - type of channel samples which are provided to the write
// functions. All the write functions have the same type, and this type is also
// what is used for the internal buffer.
//
// BufferSize - is the size of the internal buffer. Once this buffer is filled
// it is passed to the actual WAV writer.
template <class SampleType, size_t BufferSize, class FileWriter>
class BufferedWAVWriter {
 public:
  explicit BufferedWAVWriter(
      tiny_lib::audio_wav_writer::Writer<FileWriter>& wav_writer)
      : wav_writer_(wav_writer) {}

  ~BufferedWAVWriter() { assert(num_stored_samples_ == 0); }

  auto WriteSingleSample(std::span<const SampleType> sample) -> bool {
    const size_t num_samples = sample.size();
    if (num_samples != wav_writer_.GetFormatSpec().num_channels) {
      return false;
    }
    if (num_samples > BufferSize) {
      return false;
    }

    // Write accumulated samples.
    if (num_stored_samples_ + num_samples > BufferSize) {
      if (!Flush()) {
        return false;
      }
    }

    static_assert(std::is_trivial_v<SampleType> == true);
    std::memcpy(
        &buffer_[num_stored_samples_], sample.data(), sample.size_bytes());

    num_stored_samples_ += num_samples;

    return true;
  }

  // Write multiple samples to the file.
  // The samples are provided as a single continuous emory with the channels
  // interleaved. It is required to have the number of samples multiple of the
  // number of channels.
  auto WriteMultipleSamples(std::span<const SampleType> samples) -> bool {
    const size_t num_channels = wav_writer_.GetFormatSpec().num_channels;
    const size_t num_frames = samples.size() / num_channels;
    const size_t num_samples = num_frames * num_channels;

    // Check that the provided samples buffer have expected size.
    if (num_frames * num_channels != samples.size()) {
      return false;
    }

    // If the number of provided samples large enough skip etra memory copy to
    // the buffer and use the provided buffer as-is.
    if (num_samples >= BufferSize) {
      if (!Flush()) {
        return false;
      }
      return wav_writer_.WriteMultipleSamples(samples);
    }

    // Calculate the number of samples that can be put to the buffer.
    const size_t remaining_buffer_size = BufferSize - num_stored_samples_;
    const size_t num_samples_to_buffer = std::min(
        (remaining_buffer_size / num_channels) * num_channels, num_samples);

    // Append new samples to the buffer.
    static_assert(std::is_trivial_v<SampleType> == true);
    std::memcpy(&buffer_[num_stored_samples_],
                samples.data(),
                num_samples_to_buffer * sizeof(SampleType));
    num_stored_samples_ += num_samples_to_buffer;

    // Flush the buffer if there is no more space remained in it.
    if (num_stored_samples_ >= BufferSize - num_channels) {
      if (!Flush()) {
        return false;
      }
    }

    const size_t num_remaining_samples = num_samples - num_samples_to_buffer;
    if (num_remaining_samples) {
      std::memcpy(&buffer_[num_stored_samples_],
                  samples.data() + num_samples_to_buffer,
                  num_remaining_samples * sizeof(SampleType));
      num_stored_samples_ += num_remaining_samples;
    }

    return true;
  }

  auto Flush() -> bool {
    if (num_stored_samples_) {
      wav_writer_.WriteMultipleSamples(
          std::span<const SampleType>(buffer_.data(), num_stored_samples_));
      num_stored_samples_ = 0;
    }
    return true;
  }

 private:
  tiny_lib::audio_wav_writer::Writer<FileWriter>& wav_writer_;

  std::array<SampleType, BufferSize> buffer_;
  size_t num_stored_samples_ = 0;
};

}  // namespace radio_core::tool
