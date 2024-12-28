// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <random>
#include <vector>

#include "radio_core/benchmark/base_app.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/dft.h"

namespace radio_core::benchmark {

using std::cerr;
using std::cout;
using std::endl;

class DFTGoertzelBenchmark : public Benchmark {
 public:
  using Benchmark::Benchmark;

 protected:
  auto GetBenchmarkName() -> std::string override {
    return "CalculateMultipleDFTBinsGoertzel()";
  }

  void ConfigureParser(argparse::ArgumentParser& parser) override {
    parser.add_argument("--num-samples")
        .default_value(8192)
        .help("The number of samples in the signal")
        .scan<'i', int>();

    parser.add_argument("--num-bins")
        .default_value(32)
        .help("The number of bins for which DFT will be calculated")
        .scan<'i', int>();
  }

  auto HandleArguments(argparse::ArgumentParser& parser) -> bool override {
    num_samples_ = parser.get<int>("--num-samples");
    num_bins_ = parser.get<int>("--num-bins");
    return true;
  }

  void Initialize() override {
    cout << endl;
    cout << "Configuration" << endl;
    cout << "=============" << endl;

    cout << "Number of samples    : " << num_samples_ << endl;
    cout << "Number of bins       : " << num_bins_ << endl;
    cout << "Number of iterations : " << GetNumIterations() << endl;

    samples_.resize(num_samples_);
    bins_.resize(num_bins_);
    partial_dft_.resize(num_bins_);

    // Generate test signal.
    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> distribution(0, 1);
    for (Complex& sample : samples_) {
      sample =
          Complex(distribution(random_engine), distribution(random_engine));
    }

    // Generate bin indices.
    for (int i = 0; i < num_bins_; ++i) {
      bins_[i] = i % (num_samples_ / 2);
    }
  }

  void Iteration() override {
    CalculateMultipleDFTBinsGoertzel<float>(samples_, bins_, partial_dft_);
  }

  void Finalize() override {
    samples_[0].imag += 0.001f;

    // Sanity check and endurance that the evaluation is not optimized out.
    bool has_non_finite = false;
    for (const Complex& dft : partial_dft_) {
      if (!IsFinite(dft)) {
        has_non_finite = true;
      }
    }
    if (has_non_finite) {
      cerr << "Result has non-finite values" << endl;
      ::exit(1);
    }
  }

 private:
  // Input parameters: the number of samples and bins to be used for processing.
  int num_samples_{65536};
  int num_bins_{32};

  // Generated signal samples and bin indices for which the partial DFT is
  // calculated.
  std::vector<Complex> samples_;
  std::vector<float> bins_;

  // The result of the partial DFT.
  std::vector<Complex> partial_dft_;
};

}  // namespace radio_core::benchmark

auto main(int argc, char** argv) -> int {
  radio_core::benchmark::DFTGoertzelBenchmark app;
  return app.Run(argc, argv);
}
