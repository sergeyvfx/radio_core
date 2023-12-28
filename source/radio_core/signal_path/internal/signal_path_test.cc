// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal_path/signal_path.h"

#include "radio_core/math/complex.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal_path {

namespace {

// Dummy sink for the IF stage. Performs no real processing.
class DummyIFSink : public Sink<Complex> {
 public:
  void PushSamples(std::span<const SampleType> /*samples*/) override {}
};

// Dummy sink for the AF stage. Performs no real processing.
class DummyAFSink : public Sink<float> {
 public:
  void PushSamples(std::span<const SampleType> /*samples*/) override {}
};

}  // namespace

TEST(SignalPath, Configure) {
  using SignalPath = SignalPath<float>;

  // Configure fake options so that there is no numerical issues like division
  // by zero.
  SignalPath::Options options;
  options.input.sample_rate = 6000000;
  options.audio.sample_rate = 48000;

  SignalPath signal_path;
  signal_path.Configure(options);
}

TEST(SignalPath, IFSink) {
  using SignalPath = SignalPath<float>;

  // Ensure explicit function attachment compiles.
  {
    SignalPath signal_path;

    DummyIFSink af_sink;
    signal_path.AddIFSink(af_sink);
  }

  // Ensure templates attachment compiles.
  {
    SignalPath signal_path;

    DummyIFSink af_sink;
    signal_path.AddSink<SignalPath::Stage::kIF>(af_sink);
  }
}

TEST(SignalPath, AFSink) {
  using SignalPath = SignalPath<float>;

  // Ensure explicit function attachment compiles.
  {
    SignalPath signal_path;

    DummyAFSink af_sink;
    signal_path.AddAFSink(af_sink);
  }

  // Ensure templates attachment compiles.
  {
    SignalPath signal_path;

    DummyAFSink af_sink;
    signal_path.AddSink<SignalPath::Stage::kAF>(af_sink);
  }
}

}  // namespace radio_core::signal_path
