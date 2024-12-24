// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

// Helper class which reads stereo WAV file interpreting channels as a stream
// of complex values: channel 1 being real part and channel 2 being an imaginary
// part.

#pragma once

#include <filesystem>
#include <functional>

#include "radio_core/math/complex.h"
#include "radio_core/unittest/test.h"
#include "tl_audio_wav/tl_audio_wav_reader.h"
#include "tl_io/tl_io_file.h"

namespace radio_core::testing {

class ComplexWAVFileReader {
  using File = tiny_lib::io_file::File;
  using Path = std::filesystem::path;
  using WAVReader = tiny_lib::audio_wav_reader::Reader<File>;

 public:
  using FormatSpec = tiny_lib::audio_wav_reader::FormatSpec;

  ComplexWAVFileReader() = default;

  ~ComplexWAVFileReader() { Close(); }

  // Open EAV file for reading.
  // Opens both disk file reader and the WAV file decoder.
  // Returns true on success.
  auto Open(const Path& path) -> bool {
    // Open file for reading.
    if (!file_.Open(path, File::kRead)) {
      return false;
    }

    // Open the WAV reader.
    if (!wav_reader_.Open(file_)) {
      return false;
    }

    // The file is expected ot have 2 channels: first is I, second is Q.
    if (wav_reader_.GetFormatSpec().num_channels != 2) {
      return false;
    }

    return true;
  }

  // Open file with a relative path to the test data.
  auto OpenTestDataFile(const Path& path) -> bool {
    return Open(TestFileAbsolutePath(path));
  }

  void Close() { file_.Close(); }

  // Get access to the format specification.
  // The return value is undefined if the file is not currently open.
  auto GetFormatSpec() const -> const FormatSpec& {
    return wav_reader_.GetFormatSpec();
  }

  // Invoke given callback with the given arguments and a complex sample from
  // the file.
  template <class F, class... Args>
  auto ForeachSample(F&& callback, Args&&... args) -> bool {
    bool all_samples_ok = true;
    bool read_result = wav_reader_.ReadAllSamples<float, 2>(
        [&](const std::span<float> sample) {
          if (sample.size() != 2) {
            all_samples_ok = false;
            return;
          }
          const Complex x{sample[0], sample[1]};
          std::invoke(
              std::forward<F>(callback), std::forward<Args>(args)..., x);
        });
    return read_result && all_samples_ok;
  }

 private:
  File file_;
  WAVReader wav_reader_;
};

}  // namespace radio_core::testing
