// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal_path/sink_collection.h"

#include <array>
#include <vector>

#include "radio_core/signal_path/sink.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal_path {

using testing::FloatNear;
using testing::Pointwise;

class MyAsyncSink : public Sink<float> {
 public:
  explicit MyAsyncSink(std::vector<float>& out) : out_(out) {}

  void PushSamples(const std::span<const float> samples) override {
    for (const float sample : samples) {
      out_.push_back(sample * 2);
    }
  }

 private:
  std::vector<float>& out_;
};

TEST(SinkCollection, Single) {
  const std::array in = std::to_array<float>({1, 2, 3, 4, 5, 6});

  std::vector<float> out_a;
  MyAsyncSink sink_a(out_a);

  SinkCollection<float> sink_collection;
  sink_collection.AddSink(sink_a);

  sink_collection.PushSamples(in);

  EXPECT_THAT(
      out_a,
      Pointwise(FloatNear(1e-6f), std::to_array<float>({2, 4, 6, 8, 10, 12})));
}

TEST(SinkCollection, Multiple) {
  const std::array in = std::to_array<float>({1, 2, 3, 4, 5, 6});

  std::vector<float> out_a;
  MyAsyncSink sink_a(out_a);

  std::vector<float> out_b;
  MyAsyncSink sink_b(out_b);

  SinkCollection<float> sink_collection;
  sink_collection.AddSink(sink_a);
  sink_collection.AddSink(sink_b);

  sink_collection.PushSamples(in);

  EXPECT_THAT(
      out_a,
      Pointwise(FloatNear(1e-6f), std::to_array<float>({2, 4, 6, 8, 10, 12})));
  EXPECT_THAT(
      out_b,
      Pointwise(FloatNear(1e-6f), std::to_array<float>({2, 4, 6, 8, 10, 12})));

  // Test removing the sink.

  sink_collection.RemoveSink(sink_a);

  sink_collection.PushSamples(in);

  EXPECT_THAT(
      out_a,
      Pointwise(FloatNear(1e-6f), std::to_array<float>({2, 4, 6, 8, 10, 12})));

  EXPECT_THAT(out_b,
              Pointwise(FloatNear(1e-6f),
                        std::to_array<float>(
                            {2, 4, 6, 8, 10, 12, 2, 4, 6, 8, 10, 12})));
}

}  // namespace radio_core::signal_path
