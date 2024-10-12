// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <memory>
#include <random>
#include <string_view>
#include <vector>

#include "radio_core/base/half.h"
#include "radio_core/benchmark/base_app.h"
#include "radio_core/math/kernel/dot_flip.h"
#include "radio_core/math/math.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core::benchmark {

using std::cerr;
using std::cout;
using std::endl;

class DotBenchmark : public Benchmark {
 public:
  using Benchmark::Benchmark;

 protected:
  auto GetBenchmarkName() -> std::string override { return "Dot<F, G>()"; }

  void ConfigureParser(argparse::ArgumentParser& parser) override {
    parser.add_argument("arguments_type")
        .help("Type of arguments: " +
              std::string(kSupportedArgumentTypesListString));
  }

  auto HandleArguments(argparse::ArgumentParser& parser) -> bool override {
    const auto arguments_type = parser.get<std::string>("arguments_type");
    if (arguments_type == "float_float") {
      arguments_type_ = ArgumentsType::kFloatFloat;
    } else if (arguments_type == "complex_float") {
      arguments_type_ = ArgumentsType::kComplexFloat;
    }
#if RADIO_CORE_HAVE_HALF
    else if (arguments_type == "half_half") {
      arguments_type_ = ArgumentsType::kHalfHalf;
    } else if (arguments_type == "half_complex_half") {
      arguments_type_ = ArgumentsType::kHalfComplexHalf;
    }
#endif
    else {
      cerr << "Unknown arguments type " << arguments_type << endl;
      cerr << "Supported: " << kSupportedArgumentTypesListString << endl;
      return false;
    }

    return true;
  }

  void Initialize() override {
    cout << endl;
    cout << "Configuration" << endl;
    cout << "=============" << endl;

    switch (arguments_type_) {
      case ArgumentsType::kFloatFloat:
        cout << "Arguments            : float x float" << endl;
        InitializeData(float_float_data_);
        break;

      case ArgumentsType::kComplexFloat:
        cout << "Arguments            : Complex x float" << endl;
        InitializeData(complex_float_data_);
        break;

#if RADIO_CORE_HAVE_HALF
      case ArgumentsType::kHalfHalf:
        cout << "Arguments            : Half x Half" << endl;
        InitializeData(half_half_data_);
        break;
      case ArgumentsType::kHalfComplexHalf:
        cout << "Arguments            : HalfComplex x Half" << endl;
        InitializeData(half_complex_half_data_);
        break;
#endif
    }

    cout << "Number of samples    : " << GetNumSamples() << endl;
    cout << "Number of iterations : " << GetNumIterations() << endl;
  }

  void Iteration() override {
    bool is_finite = false;

    switch (arguments_type_) {
      case ArgumentsType::kFloatFloat: {
        const float d = kernel::experimental::DotFlipG<float, float>(
            float_float_data_.f, float_float_data_.g);

        is_finite = IsFinite(d);
        break;
      }

      case ArgumentsType::kComplexFloat: {
        const Complex d = kernel::experimental::DotFlipG<Complex, float>(
            complex_float_data_.f, complex_float_data_.g);

        is_finite = IsFinite(d);
        break;
      }

#if RADIO_CORE_HAVE_HALF
      case ArgumentsType::kHalfHalf: {
        const Half d = kernel::experimental::DotFlipG<Half, Half>(
            half_half_data_.f, half_half_data_.g);
        is_finite = IsFinite(d);
        break;
      }

      case ArgumentsType::kHalfComplexHalf: {
        const HalfComplex d = kernel::experimental::DotFlipG<HalfComplex, Half>(
            half_complex_half_data_.f, half_complex_half_data_.g);
        is_finite = IsFinite(d);
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
  enum class ArgumentsType {
    kFloatFloat,
    kComplexFloat,
#if RADIO_CORE_HAVE_HALF
    kHalfHalf,
    kHalfComplexHalf,
#endif
  };
  static constexpr std::string_view kSupportedArgumentTypesListString =
      "float_float"
      ", complex_float"
#if RADIO_CORE_HAVE_HALF
      ", half_half"
      ", half_complex_half"
#endif
      ;

  ArgumentsType arguments_type_;

  template <class F, class G>
  struct Data {
    std::vector<F> f;
    std::vector<G> g;
  };

  auto GetNumSamples() const -> int { return 65536; }

  template <class T, class G>
  void InitializeData(Data<T, G>& data) {
    const int num_samples = GetNumSamples();

    data.f.resize(num_samples);
    data.g.resize(num_samples);

    InitializeSamples<T>(data.f);
    InitializeSamples<G>(data.g);
  }

  template <class T>
  void InitializeSamples(std::span<T> samples) {
    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> distribution(0, 1);

    for (T& sample : samples) {
      sample = T(distribution(random_engine));
    }
  }

  template <class T>
  void InitializeSamples(std::span<BaseComplex<T>> samples) {
    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> distribution(0, 1);

    for (BaseComplex<T>& sample : samples) {
      sample = BaseComplex<T>(T(distribution(random_engine)),
                              T(distribution(random_engine)));
    }
  }

  Data<float, float> float_float_data_;
  Data<Complex, float> complex_float_data_;

#if RADIO_CORE_HAVE_HALF
  Data<Half, Half> half_half_data_;
  Data<HalfComplex, Half> half_complex_half_data_;
#endif
};

}  // namespace radio_core::benchmark

auto main(int argc, char** argv) -> int {
  radio_core::benchmark::DotBenchmark app;
  return app.Run(argc, argv);
}
