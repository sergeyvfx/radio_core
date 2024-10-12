// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <random>
#include <vector>

#include "radio_core/base/half.h"
#include "radio_core/benchmark/base_app.h"
#include "radio_core/math/kernel/norm.h"
#include "radio_core/math/math.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core::benchmark {

using std::cerr;
using std::cout;
using std::endl;

class AbsBenchmark : public Benchmark {
 public:
  using Benchmark::Benchmark;

 protected:
  auto GetBenchmarkName() -> std::string override { return "Norm<T>()"; }

  void ConfigureParser(argparse::ArgumentParser& parser) override {
    parser.add_argument("input_sample_type")
        .help("Type of arguments: " +
              std::string(kSupportedInputSampleTypesListString));
  }

  auto HandleArguments(argparse::ArgumentParser& parser) -> bool override {
    const auto input_sample_type = parser.get<std::string>("input_sample_type");
    if (input_sample_type == "complex") {
      input_sample_type_ = InputSampleType::kComplex;
    }
#if RADIO_CORE_HAVE_HALF
    else if (input_sample_type == "half_complex") {
      input_sample_type_ = InputSampleType::kHalfComplex;
    }
#endif
    else {
      std::cerr << "Unknown input type " << input_sample_type << std::endl;
      std::cerr << "Supported: " << kSupportedInputSampleTypesListString
                << std::endl;
      return false;
    }

    return true;
  }

  void Initialize() override {
    cout << endl;
    cout << "Configuration" << endl;
    cout << "=============" << endl;

    switch (input_sample_type_) {
      case InputSampleType::kComplex:
        cout << "Input sample type    : Complex" << endl;
        InitializeData(complex_data_);
        break;

#if RADIO_CORE_HAVE_HALF
      case InputSampleType::kHalfComplex:
        cout << "Input sample type    : HalfComplex" << endl;
        InitializeData(half_complex_data_);
        break;
#endif
    }

    cout << "Number of samples    : " << GetNumSamples() << endl;
    cout << "Number of iterations : " << GetNumIterations() << endl;
  }

  void Iteration() override {
    switch (input_sample_type_) {
      case InputSampleType::kComplex:
        Norm<float>(complex_data_.samples, complex_data_.magnitude);
        break;

#if RADIO_CORE_HAVE_HALF
      case InputSampleType::kHalfComplex:
        Norm<Half>(half_complex_data_.samples, half_complex_data_.magnitude);
        break;
#endif
    }
  }

  void Finalize() override {
    // Sanity check and endurance that the evaluation is not optimized out.

    bool has_non_finite = false;

    for (const float magnitude : complex_data_.magnitude) {
      if (!IsFinite(magnitude)) {
        has_non_finite = true;
      }
    }

    if (has_non_finite) {
      cerr << "Result has non-finite values" << endl;
      ::exit(1);
    }
  }

 private:
  enum class InputSampleType {
    kComplex,
#if RADIO_CORE_HAVE_HALF
    kHalfComplex,
#endif
  };
  static constexpr std::string_view kSupportedInputSampleTypesListString =
      "complex"
#if RADIO_CORE_HAVE_HALF
      ", half_complex"
#endif
      ;

  InputSampleType input_sample_type_;

  template <class T>
  struct Data {
    std::vector<BaseComplex<T>> samples;
    std::vector<T> magnitude;
  };

  auto GetNumSamples() const -> int { return 65536; }

  template <class T>
  void InitializeData(Data<T>& data) {
    const int num_samples = GetNumSamples();

    data.samples.resize(num_samples);
    data.magnitude.resize(num_samples);

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> distribution(0, 1);

    for (BaseComplex<T>& sample : data.samples) {
      sample = BaseComplex<T>(T(distribution(random_engine)),
                              T(distribution(random_engine)));
    }
  }

  Data<float> complex_data_;

#if RADIO_CORE_HAVE_HALF
  Data<Half> half_complex_data_;
#endif
};

}  // namespace radio_core::benchmark

auto main(int argc, char** argv) -> int {
  radio_core::benchmark::AbsBenchmark app;
  return app.Run(argc, argv);
}
