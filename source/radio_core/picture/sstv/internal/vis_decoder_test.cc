// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/picture/sstv/vis_decoder.h"

#include <span>
#include <vector>

#include "radio_core/picture/sstv/prefilter.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"
#include "tl_audio_wav/tl_audio_wav_reader.h"
#include "tl_io/tl_io_file.h"

#include <type_traits>

namespace radio_core::picture::sstv {

using testing::Eq;
using testing::Pointwise;

using File = tiny_lib::io_file::File;

namespace audio_wav_reader = tiny_lib::audio_wav_reader;

// Test VIS decoder using samples from WAV file.
class VISDecoderWAVTest : public testing::Test {
 protected:
  auto GetFilepath(const std::filesystem::path& filename)
      -> std::filesystem::path {
    return testing::TestFileAbsolutePath(std::filesystem::path("sstv") / "vis" /
                                         filename);
  }

  void ConfigureAndRunTest(const std::filesystem::path& filename,
                           const std::span<const int>& expected_vis_codes) {
    // Open the file for read.
    File file;
    ASSERT_TRUE(file.Open(GetFilepath(filename), File::kRead));

    // Open the audio WAV reader from the file.
    audio_wav_reader::Reader<File> wav_file_reader;
    ASSERT_TRUE(wav_file_reader.Open(file));

    const audio_wav_reader::FormatSpec format_spec =
        wav_file_reader.GetFormatSpec();

    // Configure prefilter.
    const Prefilter<float>::Options prefilter_options = {
        .sample_rate = float(format_spec.sample_rate),
    };
    Prefilter<float> prefilter;
    prefilter.Configure(prefilter_options);

    // Configure the decoder.
    const VISDecoder<float>::Options vis_options = {
        .sample_rate = float(format_spec.sample_rate),
    };
    VISDecoder<float> vis_decoder;
    vis_decoder.Configure(vis_options);

    // Audio sample processor.
    std::vector<int> decoded_vis_codes;
    auto processor = [&](const float sample) {
      const float frequency_sample = prefilter(sample);
      auto vis_result = vis_decoder(frequency_sample);
      if (vis_result.Ok()) {
        decoded_vis_codes.push_back(vis_result.GetValue());
      }
    };

    // Read all samples from the WAV file and process them.
    wav_file_reader.ReadAllSamples<float, 16>(
        [&](const std::span<const float> sample) { processor(sample[0]); });

    // Push extra samples so that the samples which are currently in the filters
    // are also processed.
    for (int i = 0; i < 1000; ++i) {
      processor(0.0f);
    }

    EXPECT_THAT(decoded_vis_codes, Pointwise(Eq(), expected_vis_codes));
  }
};

TEST_F(VISDecoderWAVTest, vis_ideal_11025) {
  ConfigureAndRunTest("vis_ideal_11025.wav", {{0x5f, 0x5f, 0x5f, 0x5f}});
}

TEST_F(VISDecoderWAVTest, vis_ideal_44100) {
  ConfigureAndRunTest("vis_ideal_44100.wav", {{0x5f, 0x5f, 0x5f, 0x5f}});
}

// NOTE: When optimizing the number of decoded VIS keep in mind computational
// complexity and quality of received image. If a change to VIS decoder allows
// to decode vis with lower SNR but without being able to see a picture the
// added computational complexity is likely not worth it.

TEST_F(VISDecoderWAVTest, vis_snr_30_0_1_11025) {
  // Expected to decode the ideal VIS with code 0x7f, and all VIS with SNR in
  // the range [30, 15].
  ConfigureAndRunTest(
      "vis_snr_30_0_1_11025.wav",
      {{0x7f, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15}});
}

TEST_F(VISDecoderWAVTest, vis_snr_30_0_1_44100) {
  // Expected to decode the ideal VIS with code 0x7f, and all VIS with SNR in
  // the range [30, 10].
  ConfigureAndRunTest("vis_snr_30_0_1_44100.wav",
                      {{0x7f, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
                        20,   19, 18, 17, 16, 15, 14, 13, 12, 11, 10}});
}

}  // namespace radio_core::picture::sstv
