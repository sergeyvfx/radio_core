// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <random>
#include <vector>

#include "radio_core/base/half.h"
#include "radio_core/benchmark/base_app.h"
#include "radio_core/math/kernel/horizontal_max.h"
#include "radio_core/math/math.h"

namespace radio_core::benchmark {

using std::cerr;
using std::cout;
using std::endl;

class HorizontalMaxBenchmark : public Benchmark {
 public:
  using Benchmark::Benchmark;

 protected:
  auto GetBenchmarkName() -> std::string override {
    return "HorizontalMax<T>()";
  }

  void ConfigureParser(argparse::ArgumentParser& parser) override {
    parser.add_argument("input_sample_type")
        .help("Type of arguments: " +
              std::string(kSupportedInputSampleTypesListString));
  }

  auto HandleArguments(argparse::ArgumentParser& parser) -> bool override {
    const auto input_sample_type = parser.get<std::string>("input_sample_type");
    if (input_sample_type == "float") {
      input_sample_type_ = InputSampleType::kFloat;
    }
#if RADIO_CORE_HAVE_HALF
    else if (input_sample_type == "half") {
      input_sample_type_ = InputSampleType::kHalf;
    }
#endif
    else {
      cerr << "Unknown input type " << input_sample_type << endl;
      cerr << "Supported: " << kSupportedInputSampleTypesListString << endl;
      return false;
    }

    return true;
  }

  void Initialize() override {
    cout << endl;
    cout << "Configuration" << endl;
    cout << "=============" << endl;

    switch (input_sample_type_) {
      case InputSampleType::kFloat:
        cout << "Input sample type    : float" << endl;
        InitializeData(float_data_);
        break;

#if RADIO_CORE_HAVE_HALF
      case InputSampleType::kHalf:
        cout << "Input sample type    : Half" << endl;
        InitializeData(half_data_);
        break;
#endif
    }

    cout << "Number of samples    : " << GetNumSamples() << endl;
    cout << "Number of iterations : " << GetNumIterations() << endl;
  }

  void Iteration() override {
    bool is_finite = true;

    switch (input_sample_type_) {
      case InputSampleType::kFloat: {
        const float max = kernel::HorizontalMax<float>(float_data_.samples);
        is_finite = IsFinite(max);
        break;
      }

#if RADIO_CORE_HAVE_HALF
      case InputSampleType::kHalf: {
        const Half max = kernel::HorizontalMax<Half>(half_data_.samples);
        is_finite = IsFinite(max);
        break;
      }
#endif
    }

    if (!is_finite) {
      cerr << "Result has non-finite values" << endl;
      ::exit(1);
    }
  }

 private:
  enum class InputSampleType {
    kFloat,
#if RADIO_CORE_HAVE_HALF
    kHalf,
#endif
  };
  static constexpr std::string_view kSupportedInputSampleTypesListString =
      "float"
#if RADIO_CORE_HAVE_HALF
      ", half"
#endif
      ;

  InputSampleType input_sample_type_;

  template <class T>
  struct Data {
    std::vector<T> samples;
  };

  auto GetNumSamples() const -> int { return 65536; }

  template <class T>
  void InitializeData(Data<T>& data) {
    const int num_samples = GetNumSamples();

    data.samples.resize(num_samples);

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> distribution(0, 1);

    for (T& sample : data.samples) {
      sample = T(distribution(random_engine));
    }
  }

  Data<float> float_data_;

#if RADIO_CORE_HAVE_HALF
  Data<Half> half_data_;
#endif
};

}  // namespace radio_core::benchmark

auto main(int argc, char** argv) -> int {
  radio_core::benchmark::HorizontalMaxBenchmark app;
  return app.Run(argc, argv);
}
