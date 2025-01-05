// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

// An implementation of FFT API which uses PFFFT to perform FFT calculation.
// It expects pffft.h available in the include directories passed to the
// translation using which includes this header file.

#pragma once

#include <cassert>
#include <cstddef>
#include <span>
#include <vector>

#include <pffft.h>

#include "radio_core/math/complex.h"
#include "radio_core/math/fft.h"
#include "radio_core/math/fft_api.h"

namespace radio_core::fft {

// Forwards declaration of the PFFFT API.
//
// The PFFFT library only supports single precision floating point API and
// integration with it is implemented as specialization of this class. For the
// unsupported input types this approach will lead to a compilation error.
template <class RealType, template <class> class Allocator = FFTAllocator>
class PFFFT;

namespace pffft_internal {

class Setup;

// Provider of PFFFT setups which can cache setups to speedup cases when
// multiple FFT objects perform transform of the same configuration.
class SetupCache {
 private:
  friend class Setup;

  // Acquire PFFFT_Setup for the given number of points and the given type.
  static auto Acquire(const int num_points, const pffft_transform_t transform)
      -> PFFFT_Setup* {
    // TODO(sergey): Implement actual caching.
    return pffft_new_setup(num_points, transform);
  }

  // Release previously acquired setup.
  static auto Release(PFFFT_Setup* setup) { pffft_destroy_setup(setup); }
};

// A RAII wrapper around PFFFT_Setup.
class Setup {
 public:
  static auto Create(const int num_points, const pffft_transform_t transform)
      -> Setup {
    Setup setup;
    setup.setup_ = SetupCache::Acquire(num_points, transform);
    return setup;
  }

  Setup() = default;
  ~Setup() { Release(); }

  Setup(Setup&& other) noexcept : setup_(other.setup_) {
    other.setup_ = nullptr;
  }

  auto operator=(Setup&& other) noexcept -> Setup& {
    if (this == &other) {
      return *this;
    }

    Release();

    setup_ = other.setup_;

    other.setup_ = nullptr;

    return *this;
  }

  void Release() {
    if (setup_) {
      SetupCache::Release(setup_);
      setup_ = nullptr;
    }
  }

  operator PFFFT_Setup*() const { return setup_; }

  // Disable copy semantic.
  Setup(const Setup& other) = delete;
  auto operator=(const Setup& other) = delete;

 private:
  PFFFT_Setup* setup_{nullptr};
};

// A RAII wrapper around resources required for the PFFFT operation.
template <template <class> class Allocator>
class Work {
 public:
  Work() = default;
  ~Work() = default;

  Work(Work&& other) = default;
  auto operator=(Work&& other) -> Work& = default;

  // Allocate work memory for the given number of floating point values.
  void Allocate(const std::size_t num_floats) { work_.resize(num_floats); }

  void Clear() { work_.clear(); }

  auto GetData() -> float* { return work_.data(); }

  // Disable copy semantic.
  Work(const Work& work) = delete;
  auto operator=(const Work& other) = delete;

 private:
  // The work memory required by the PFFFT library.
  std::vector<float, Allocator<float>> work_;
};

}  // namespace pffft_internal

// Specialization of the FFT API which uses PFFFT to perform real-type FFT.
template <template <class> class Allocator>
class PFFFT<float, Allocator> final : public FFT<float, Allocator> {
 public:
  using SetupOptions = typename FFT<float, Allocator>::SetupOptions;
  using TransformOptions = typename FFT<float, Allocator>::TransformOptions;

  PFFFT() = default;
  explicit PFFFT(const SetupOptions& options) { PFFFT::Configure(options); }

  void Configure(const SetupOptions& options) override {
    setup_ = pffft_internal::Setup::Create(options.num_points, PFFFT_REAL);
    work_.Allocate(options.num_points);
  }

  auto Forward(const std::span<const float> input,
               const std::span<Complex> output,
               const TransformOptions& options = TransformOptions())
      -> std::span<Complex> override {
    const std::size_t output_size = input.size() / 2 + 1;

    pffft_transform_ordered(setup_,
                            input.data(),
                            reinterpret_cast<float*>(output.data()),
                            work_.GetData(),
                            PFFFT_FORWARD);

    const std::span<Complex> result = output.subspan(0, output_size);

    // For real transforms, PFFFT places both 0-frequency and half frequency
    // components, which are real, are assembled in the first entry as `F(0) +
    // i*F(n / 2 + 1)`
    result[output_size - 1] = Complex(result[0].imag);
    result[0].imag = 0.0f;

    if (options.normalize) {
      fft_internal::Normalize(result, input.size());
    }

    return result;
  }

 private:
  pffft_internal::Setup setup_;
  pffft_internal::Work<Allocator> work_;
};

// Specialization of the FFT API which uses PFFFT to perform complex-type FFT.
template <template <class> class Allocator>
class PFFFT<Complex, Allocator> final : public FFT<Complex, Allocator> {
 public:
  using SetupOptions = typename FFT<Complex, Allocator>::SetupOptions;
  using TransformOptions = typename FFT<Complex, Allocator>::TransformOptions;

  PFFFT() = default;
  explicit PFFFT(const SetupOptions& options) { PFFFT::Configure(options); }

  void Configure(const SetupOptions& options) override {
    setup_ = pffft_internal::Setup::Create(options.num_points, PFFFT_COMPLEX);
    work_.Allocate(options.num_points * 2);
  }

  auto Forward(const std::span<const Complex> input,
               const std::span<Complex> output,
               const TransformOptions& options = TransformOptions())
      -> std::span<Complex> override {
    assert(output.size() >= input.size());

    pffft_transform_ordered(setup_,
                            reinterpret_cast<const float*>(input.data()),
                            reinterpret_cast<float*>(output.data()),
                            work_.GetData(),
                            PFFFT_FORWARD);

    const std::span<Complex> result = output.subspan(0, input.size());

    if (options.normalize && options.shift) {
      FFTNormalizeAndShift(result);
    } else {
      if (options.normalize) {
        fft_internal::Normalize(result, input.size());
      }
      if (options.shift) {
        FFTShift(result);
      }
    }

    return result;
  }

 private:
  pffft_internal::Setup setup_;
  pffft_internal::Work<Allocator> work_;
};

}  // namespace radio_core::fft
