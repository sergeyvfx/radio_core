// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

// Helper for writing tools.
//
// This module provides functionality which reads samples from a WAV file into
// a temporary buffer and passes it to a consumer.

#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <span>

#include "tl_audio_wav/tl_audio_wav_reader.h"

namespace radio_core::tool {

// Read samples from the given wav_reader, buffer them in a buffer and invoke
// the given functor with the buffer.
//
// Template arguments:
//
// - SampleType is the type of single element of a buffer that is passed to the
//   consumer F.
//
// - BufferSize is the number of samples that are passed to the consumer F.
//
// - FileReader is the I/O implementation for
//   tiny_lib::audio_wav_reader::Reader.
//   Typically automatically deducted from the wav_reader.
//
//  The rest of the template arguments are better understood from their
//  corresponding function arguments.
//
// Arguments:
//
// - frame_to_sample is a function which converts WAV file frame sample (a span
//   of per-audio channel values) to SampleType.
//   Its type FrameToSample is typically deducted automatically.
//   The frame samples are always float.
//
//   Example of the frame_to_sample:
//
//     [](const std::span<const float> frame_samples) -> Complex {
//       return {frame_samples[0], frame_samples[1]};
//     }
//
// - callback is a function which is invoked with the samples buffer.
//   The samples buffer is passed after the given arguments args.
//   Its type FrameToSample is typically deducted automatically.
//
//   Example of the callback:
//
//     [&](const std::span<const Complex> samples) {
//       pipeline.PushSamples(samples);
//     }
//
// Example:
//   File iq_file;
//   if (!iq_file.Open(input_iq_filepath, File::kRead)) {
//     /* Error handling. */
//   }
//
//   audio_wav_reader::Reader<File> iq_wav_file_reader;
//   if (!iq_wav_file_reader.Open(iq_file)) {
//     /* Error handling. */
//   }
//
//   SimpleSignalPath<float> signal_path;
//   /* Configure the signal path. */
//
//   ReadWAVBuffered<DSPComplex, 65536>(
//       iq_wav_file_reader,
//       [](const std::span<const float> frame_samples) -> Complex {
//         return {frame_samples[0], frame_samples[1]};
//       },
//       [&](const std::span<const Complex> samples) {
//         signal_path.PushSamples(samples);
//       });
template <class SampleType,
          size_t BufferSize,
          class FileReader,
          class FrameToSample,
          class F,
          class... Args>
inline void ReadWAVBuffered(
    tiny_lib::audio_wav_reader::Reader<FileReader>& wav_reader,
    FrameToSample&& frame_to_sample,
    F&& callback,
    Args&&... args) {
  std::array<SampleType, BufferSize> buffer;

  // The number of samples in the buffer.
  size_t num_samples = 0;

  wav_reader.template ReadAllSamples<float, 2>(
      [&](const std::span<const float> frame_samples) {
        // Convert the WAV file frame samples to the buffer sample type.
        buffer[num_samples++] = frame_to_sample(frame_samples);

        // Invoke the callback with the full buffer if it is full.
        if (num_samples == BufferSize) {
          std::invoke(
              std::forward<F>(callback), std::forward<Args>(args)..., buffer);
          num_samples = 0;
        }
      });

  // Invoke the callback with the trailing samples of a partially full buffer
  // at the end of the file.
  if (num_samples != 0) {
    std::invoke(std::forward<F>(callback),
                std::forward<Args>(args)...,
                std::span(buffer.data(), num_samples));
  }
}

}  // namespace radio_core::tool
