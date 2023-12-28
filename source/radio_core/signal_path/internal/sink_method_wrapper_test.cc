// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal_path/sink_method_wrapper.h"

#include <array>
#include <vector>

#include "radio_core/signal_path/sink.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal_path {

using testing::FloatNear;
using testing::Pointwise;

class MyProcessor {
 public:
  explicit MyProcessor(std::vector<float>& out) : out_(out) {}

  void ProcessSamples(const std::span<const float> samples) {
    for (const float sample : samples) {
      out_.push_back(sample * 2);
    }
  }

 private:
  std::vector<float>& out_;
};

TEST(SinkMethodWrapper, Single) {
  const std::array in = std::to_array<float>({1, 2, 3, 4, 5, 6});

  std::vector<float> out_a;
  MyProcessor processor(out_a);

  auto sink = SinkMethodWrapper<float,
                                MyProcessor,
                                decltype(&MyProcessor::ProcessSamples)>(
      &processor, &MyProcessor::ProcessSamples);

  sink.PushSamples(in);

  EXPECT_THAT(
      out_a,
      Pointwise(FloatNear(1e-6f), std::to_array<float>({2, 4, 6, 8, 10, 12})));
}

}  // namespace radio_core::signal_path
