// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include <array>
#include <iostream>
#include <random>

#include "radio_core/base/ring_double_buffer.h"
#include "radio_core/benchmark/base_app.h"
#include "radio_core/math/math.h"

namespace radio_core::benchmark {

class RingDoubleBufferBenchmark : public Benchmark {
 public:
  using Benchmark::Benchmark;

 protected:
  auto GetBenchmarkName() -> std::string override {
    return "ReverseStorageRingDoubleBuffer";
  }

  void Initialize() override {
    ring_buffer_.resize(65536);

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> distribution(0, 1);
    for (float& random_sample : random_samples_) {
      random_sample = distribution(random_engine);
    }
  }

  void Iteration() override { ring_buffer_.push_multiple(random_samples_); }

  void Finalize() override {
    // Sanity check and endurance that the evaluation is not optimized out.
    if (!IsFinite(ring_buffer_[0])) {
      std::cerr << "Result has non-finite values" << std::endl;
      ::exit(1);
    }
  }

 private:
  RingDoubleBuffer<float> ring_buffer_;
  std::array<float, 131> random_samples_;
};

}  // namespace radio_core::benchmark

auto main(int argc, char** argv) -> int {
  radio_core::benchmark::RingDoubleBufferBenchmark app;
  return app.Run(argc, argv);
}
