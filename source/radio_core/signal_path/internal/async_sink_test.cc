// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal_path/async_sink.h"

#include <array>
#include <vector>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal_path {

using testing::FloatNear;
using testing::Pointwise;

class MyAsyncSink : public AsyncSink<float> {
 public:
  explicit MyAsyncSink(std::vector<float>& out) : out_(out) {}

 protected:
  void ProcessSamples(const std::span<const float> samples) override {
    for (const float sample : samples) {
      out_.push_back(sample * 2);
    }
  }

  std::vector<float>& out_;
};

TEST(AsyncSink, Basic) {
  const std::array in = std::to_array<float>({1, 2, 3, 4, 5, 6});
  std::vector<float> out;

  MyAsyncSink sink(out);

  sink.PushSamples(in);
  sink.Wait();

  EXPECT_THAT(
      out,
      Pointwise(FloatNear(1e-6f), std::to_array<float>({2, 4, 6, 8, 10, 12})));
}

}  // namespace radio_core::signal_path
