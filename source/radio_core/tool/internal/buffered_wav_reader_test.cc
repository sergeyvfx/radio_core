// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/tool/buffered_wav_reader.h"

#include <cstdint>
#include <vector>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"
#include "tl_audio_wav/tl_audio_wav_reader.h"
#include "tl_io/tl_io_file.h"

namespace radio_core::tool {

using testing::Eq;
using testing::Pointwise;

using tiny_lib::io_file::File;
using WAVReader = tiny_lib::audio_wav_reader::Reader<File>;
using tiny_lib::audio_wav_reader::FormatSpec;
using Path = std::filesystem::path;

TEST(ReadWAVBuffered, Basic) {
  const Path filename = testing::TestFileAbsolutePath(
      Path("aprs") / "ax25_bell202_1200bd_dw_44100.wav");

  // Read samples from the file without sing buffered API.
  // This will form the ground-truth for hte read samples (assuming the WAV
  // reader itself is tested).
  std::vector<int16_t> expected_samples;
  {
    File file;
    EXPECT_TRUE(file.Open(filename, File::kRead));

    WAVReader wav_reader;
    EXPECT_TRUE(wav_reader.Open(file));

    const bool read_result = wav_reader.ReadAllSamples<int16_t, 16>(
        [&](const std::span<int16_t> sample) {
          EXPECT_EQ(sample.size(), 1);
          expected_samples.push_back(sample.front());
        });
    EXPECT_TRUE(read_result);
  }

  // Read samples using the buffered API.
  std::vector<int16_t> actual_samples;
  {
    File file;
    EXPECT_TRUE(file.Open(filename, File::kRead));

    WAVReader wav_reader;
    EXPECT_TRUE(wav_reader.Open(file));

    ReadWAVBuffered<int16_t, 65536>(
        wav_reader,
        [](const std::span<const float> frame_samples) -> int16_t {
          assert(frame_samples.size() >= 1);
          return frame_samples[0] * 32767;
        },
        [&](const std::span<const int16_t> samples) {
          actual_samples.insert(
              actual_samples.end(), samples.begin(), samples.end());
        });
  }

  // Compare actual and expected samples.
  EXPECT_THAT(actual_samples, Pointwise(Eq(), expected_samples));
}

}  // namespace radio_core::tool
