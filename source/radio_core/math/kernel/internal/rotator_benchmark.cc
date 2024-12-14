// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <random>
#include <vector>

#if defined(WITH_BENCHMARKS_VOLK)
#  include <volk/volk.h>
#endif

#include "radio_core/base/half.h"
#include "radio_core/benchmark/base_app.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/kernel/rotator.h"
#include "radio_core/math/math.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core::benchmark {

using std::cerr;
using std::cout;
using std::endl;

class FastAbsBenchmark : public Benchmark {
 public:
  using Benchmark::Benchmark;

 protected:
  auto GetBenchmarkName() -> std::string override { return "Rotator<T>()"; }

  void ConfigureParser(argparse::ArgumentParser& parser) override {
    parser.add_argument("input_sample_type")
        .help("Type of arguments: " +
              std::string(kSupportedInputSampleTypesListString));

#if defined(WITH_BENCHMARKS_VOLK)
    parser.add_argument("--use-volk")
        .help("Benchmark using implementation from the Volk library: ")
        .default_value(false)
        .implicit_value(true);
#endif
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
      cerr << "Unknown input type " << input_sample_type << endl;
      cerr << "Supported: " << kSupportedInputSampleTypesListString << endl;
      return false;
    }

#if defined(WITH_BENCHMARKS_VOLK)
    use_volk_ = parser.get<bool>("--use-volk");

#  if RADIO_CORE_HAVE_HALF
    if (use_volk_) {
      if (input_sample_type_ == InputSampleType::kHalfComplex) {
        cerr << "Volk implementation is not available for the requested "
                "sample type.";
        return false;
      }
    }
#  endif
#endif
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
      case InputSampleType::kComplex: {
#if defined(WITH_BENCHMARKS_VOLK)
        if (use_volk_) {
          const lv_32fc_t phase_inc = lv_cmake(Cos(-0.1f), Sin(-0.1f));
          lv_32fc_t phase = lv_cmake(1.f, 0.0f);
#  if VOLK_VERSION >= 030100
          volk_32fc_s32fc_x2_rotator2_32fc(
              (lv_32fc_t*)complex_data_.samples.data(),
              (const lv_32fc_t*)complex_data_.samples.data(),
              (lv_32fc_t*)&phase_inc,
              &phase,
              complex_data_.samples.size());
#  else
          volk_32fc_s32fc_x2_rotator_32fc(
              (lv_32fc_t*)complex_data_.samples.data(),
              (const lv_32fc_t*)complex_data_.samples.data(),
              phase_inc,
              &phase,
              complex_data_.samples.size());
#  endif
        } else
#endif
        {
          Complex phase(1.0f, 0.0f);
          kernel::Rotator<float>(complex_data_.samples,
                                 phase,
                                 Complex(Cos(-0.1f), Sin(-0.1f)),
                                 complex_data_.samples);
        }
        break;
      }

#if RADIO_CORE_HAVE_HALF
      case InputSampleType::kHalfComplex:
        HalfComplex phase(1.0f, 0.0f);
        kernel::Rotator<Half>(half_complex_data_.samples,
                              phase,
                              HalfComplex(Cos(-0.1f), Sin(-0.1f)),
                              half_complex_data_.samples);
        break;
#endif
    }
  }

  void Finalize() override {
    // Sanity check and endurance that the evaluation is not optimized out.

    bool has_non_finite = false;

    switch (input_sample_type_) {
      case InputSampleType::kComplex:
        for (const Complex& sample : complex_data_.samples) {
          if (!IsFinite(sample)) {
            has_non_finite = true;
          }
        }
        break;

#if RADIO_CORE_HAVE_HALF
      case InputSampleType::kHalfComplex:
        for (const HalfComplex& sample : half_complex_data_.samples) {
          if (!IsFinite(sample)) {
            has_non_finite = true;
          }
        }
        break;
#endif
    }

    if (has_non_finite) {
      std::cerr << "Result has non-finite values" << std::endl;
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
  };

  auto GetNumSamples() const -> int { return 65536; }

  template <class T>
  void InitializeData(Data<T>& data) {
    const int num_samples = GetNumSamples();

    data.samples.resize(num_samples);

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

#if defined(WITH_BENCHMARKS_VOLK)
  bool use_volk_{false};
#endif
};

}  // namespace radio_core::benchmark

auto main(int argc, char** argv) -> int {
  radio_core::benchmark::FastAbsBenchmark app;
  return app.Run(argc, argv);
}
