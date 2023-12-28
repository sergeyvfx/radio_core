// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <random>
#include <vector>

#include "radio_core/benchmark/base_app.h"
#include "radio_core/math/math.h"
#include "radio_core/signal/decimator.h"

namespace radio_core::benchmark {

using std::cerr;
using std::cout;
using std::endl;

class ReverseStorageRingBufferBenchmark : public Benchmark {
 public:
  using Benchmark::Benchmark;

 protected:
  auto GetBenchmarkName() -> std::string override { return "Decimator"; }

  void Initialize() override {
    decimator_.SetRatio(25);

    input_samples_.resize(65536);
    output_samples_.resize(65536);

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> distribution(0, 1);
    for (float& input_sample : input_samples_) {
      input_sample = distribution(random_engine);
    }

    cout << endl;
    cout << "Configuration" << endl;
    cout << "=============" << endl;

    cout << "Number of input samples : " << input_samples_.size() << endl;
    cout << "Decimation ratio        : " << decimator_.GetRatio() << endl;
    cout << "Number of iterations    : " << GetNumIterations() << endl;
  }

  void Iteration() override { decimator_(input_samples_, output_samples_); }

  void Finalize() override {
    // Sanity check and endurance that the evaluation is not optimized out.
    if (!IsFinite(output_samples_[0])) {
      std::cerr << "Result has non-finite values" << std::endl;
      ::exit(1);
    }
  }

 private:
  signal::Decimator<float> decimator_;
  std::vector<float> input_samples_;
  std::vector<float> output_samples_;
};

}  // namespace radio_core::benchmark

auto main(int argc, char** argv) -> int {
  radio_core::benchmark::ReverseStorageRingBufferBenchmark app;
  return app.Run(argc, argv);
}
