// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <random>
#include <vector>

#include "radio_core/base/half.h"
#include "radio_core/benchmark/base_app.h"
#include "radio_core/math/kernel/fast_int_pow.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core::benchmark {

using std::cerr;
using std::cout;
using std::endl;

class FastIntPowBenchmark : public Benchmark {
 public:
  using Benchmark::Benchmark;

 protected:
  auto GetBenchmarkName() -> std::string override { return "FastIntPow<T>()"; }

  void ConfigureParser(argparse::ArgumentParser& parser) override {
    parser.add_argument("input_type")
        .help("Type of arguments: " +
              std::string(kSupportedInputTypesListString));
  }

  auto HandleArguments(argparse::ArgumentParser& parser) -> bool override {
    const auto input_type = parser.get<std::string>("input_type");
    if (input_type == "complex") {
      input_type_ = InputType::kComplex;
    }
#if RADIO_CORE_HAVE_HALF
    else if (input_type == "half_complex") {
      input_type_ = InputType::kHalfComplex;
    }
#endif
    else {
      std::cerr << "Unknown input type " << input_type << std::endl;
      std::cerr << "Supported: " << kSupportedInputTypesListString << std::endl;
      return false;
    }

    return true;
  }

  void Initialize() override {
    cout << endl;
    cout << "Configuration" << endl;
    cout << "=============" << endl;

    switch (input_type_) {
      case InputType::kComplex:
        cout << "Input type           : Complex" << endl;
        InitializeData(complex_data_);
        break;

#if RADIO_CORE_HAVE_HALF
      case InputType::kHalfComplex:
        cout << "Input type           : HalfComplex" << endl;
        InitializeData(half_complex_data_);
        break;
#endif
    }

    cout << "Input size           : " << GetInputSize() << endl;
    cout << "Number of iterations : " << GetNumIterations() << endl;
  }

  void Iteration() override {
    switch (input_type_) {
      case InputType::kComplex:
        kernel::FastIntPow<BaseComplex<float>>(
            complex_data_.base, 4, complex_data_.pow);
        break;

#if RADIO_CORE_HAVE_HALF
      case InputType::kHalfComplex:
        kernel::FastIntPow<BaseComplex<Half>>(
            half_complex_data_.base, 4, half_complex_data_.pow);
        break;
#endif
    }
  }

  void Finalize() override {
    // Sanity check and endurance that the evaluation is not optimized out.

    bool has_non_finite = false;

    for (const Complex& pow : complex_data_.pow) {
      if (!IsFinite(pow)) {
        has_non_finite = true;
      }
    }

#if RADIO_CORE_HAVE_HALF
    for (const HalfComplex& pow : half_complex_data_.pow) {
      if (!IsFinite(pow)) {
        has_non_finite = true;
      }
    }
#endif

    if (has_non_finite) {
      cerr << "Result has non-finite values" << endl;
      ::exit(1);
    }
  }

 private:
  enum class InputType {
    kComplex,
#if RADIO_CORE_HAVE_HALF
    kHalfComplex,
#endif
  };
  static constexpr std::string_view kSupportedInputTypesListString =
      "complex"
#if RADIO_CORE_HAVE_HALF
      ", half_complex"
#endif
      ;

  InputType input_type_;

  template <class T>
  struct Data {
    std::vector<BaseComplex<T>> base;
    std::vector<BaseComplex<T>> pow;
  };

  auto GetInputSize() const -> int { return 65536; }

  template <class T>
  void InitializeData(Data<T>& data) {
    const int size = GetInputSize();

    data.base.resize(size);
    data.pow.resize(size);

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> distribution(0, 1);

    for (BaseComplex<T>& base : data.base) {
      base = BaseComplex<T>(T(distribution(random_engine)),
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
  radio_core::benchmark::FastIntPowBenchmark app;
  return app.Run(argc, argv);
}
