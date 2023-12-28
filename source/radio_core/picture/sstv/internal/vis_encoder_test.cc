// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/picture/sstv/vis_encoder.h"

#include <array>
#include <vector>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::picture::sstv {

using testing::Eq;
using testing::Pointwise;

TEST(sstv, VISEncoder) {
  using Sample = FrequencyDuration<float>;
  std::vector<Sample> samples;

  // Push PD120 VIS.
  // BInary form is 0b1011111.
  VISEncoder<float> encoder;
  encoder(0x5f, [&samples](const FrequencyDuration<float>& sample) {
    samples.push_back(sample);
  });

  EXPECT_THAT(samples,
              Pointwise(Eq(),
                        std::to_array<Sample>({
                            {1900, 300},  // Leader
                            {1200, 10},   // Break
                            {1900, 300},  // Leader
                            {1200, 30},   // Start
                            {1100, 30},   // 1
                            {1100, 30},   // 1
                            {1100, 30},   // 1
                            {1100, 30},   // 1
                            {1100, 30},   // 1
                            {1300, 30},   // 0
                            {1100, 30},   // 1
                            {1300, 30},   // 0
                            {1200, 30},   // End
                        })));
}

}  // namespace radio_core::picture::sstv
