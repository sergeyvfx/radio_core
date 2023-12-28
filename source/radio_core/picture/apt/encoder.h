// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Encoder which encapsulates an entire pipeline of encoding an Automatic
// Picture Transmission (APT) from given images.
//
// The input is an APT message.
// The output is a sequence of audio samples at the configured sample rate which
// denote the amplitude-modulated signal at the APT's sub-carrier frequency.

#pragma once

#include <span>

#include "radio_core/picture/apt/info.h"
#include "radio_core/picture/apt/message.h"
#include "radio_core/signal/generator.h"

namespace radio_core::picture::apt {

template <class RealType, template <class> class Allocator = std::allocator>
class Encoder {
 public:
  struct Options {
    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate;
  };

  void Configure(const Options& options) {
    generator_.Configure(options.sample_rate);

    full_scale_bit_.frequency = Info::kSubCarrierFrequency;
    full_scale_bit_.duration_ms = RealType(1000) / Info::kBaudRate;
  }

  // Encode the message into APT into amplitude samples.
  //
  // The given list of args... is passed to the callback after the modulated
  // amplitude sample. This makes the required callback signature to be:
  //
  //   callback(const RealType& sample, <optional arguments>)
  template <class F, class... Args>
  void operator()(const Message& message, F&& callback, Args&&... args) {
    using PixelAccessor = Message::PixelAccessor;
    using Spec = PixelAccessor::Spec;

    // Validate the pixel accessors are provided.
    if (!message.pixel_accessor_a || !message.pixel_accessor_b) {
      return;
    }

    // Validate the image width matches the width of images in the APT.
    const Spec spec_a = message.pixel_accessor_a->GetSpec();
    const Spec spec_b = message.pixel_accessor_b->GetSpec();
    if (spec_a.width != Info::kImageWidth ||
        spec_b.width != Info::kImageWidth) {
      return;
    }

    // Validate height of the images is the same.
    if (spec_a.height != spec_b.height) {
      return;
    }

    for (int i = 0; i < spec_a.height; ++i) {
      // Sync A.
      EncodeSync(
          Info::kSyncA, std::forward<F>(callback), std::forward<Args>(args)...);

      // Space A.
      EncodeSpace(std::forward<F>(callback), std::forward<Args>(args)...);

      // Image A.
      EncodeImageRow(*message.pixel_accessor_a,
                     i,
                     std::forward<F>(callback),
                     std::forward<Args>(args)...);

      // Telemetry A.
      EncodeTelemetry(std::forward<F>(callback), std::forward<Args>(args)...);

      // Sync B.
      EncodeSync(
          Info::kSyncB, std::forward<F>(callback), std::forward<Args>(args)...);

      // Space B.
      EncodeSpace(std::forward<F>(callback), std::forward<Args>(args)...);

      // Image B.
      EncodeImageRow(*message.pixel_accessor_b,
                     i,
                     std::forward<F>(callback),
                     std::forward<Args>(args)...);

      // Telemetry B.
      EncodeTelemetry(std::forward<F>(callback), std::forward<Args>(args)...);
    }

    generator_.FadeToZero([&](const RealType sample) {
      std::invoke(
          std::forward<F>(callback), sample, std::forward<Args>(args)...);
    });
  }

 private:
  // Encode synchronization marker.
  template <class F, class... Args>
  void EncodeSync(const std::span<const uint8_t> sync,
                  F&& callback,
                  Args&&... args) {
    for (const uint8_t bit : sync) {
      EncodeValue(bit ? 255 : 0,
                  std::forward<F>(callback),
                  std::forward<Args>(args)...);
    }
  }

  // Encode Space field of the APT.
  template <class F, class... Args>
  void EncodeSpace(F&& callback, Args&&... args) {
    for (int i = 0; i < Info::kSpaceWidth; ++i) {
      // TODO(sergey): There seems to be a horizontal black line in the space
      // every now and then in the real APT.
      EncodeValue(255, std::forward<F>(callback), std::forward<Args>(args)...);
    }
  }

  // Encode Telemetry field of the APT.
  template <class F, class... Args>
  void EncodeTelemetry(F&& callback, Args&&... args) {
    for (int i = 0; i < Info::kTelemetryWidth; ++i) {
      // TODO(sergey): Encode real wedges.
      EncodeValue(255, std::forward<F>(callback), std::forward<Args>(args)...);
    }
  }

  // Encode image row of the APT.
  template <class F, class... Args>
  void EncodeImageRow(const Message::PixelAccessor& pixel_accessor,
                      const int row,
                      F&& callback,
                      Args&&... args) {
    for (int i = 0; i < Info::kImageWidth; ++i) {
      const Color1ub color = pixel_accessor.GetPixel(i, row);
      EncodeValue(
          color.value, std::forward<F>(callback), std::forward<Args>(args)...);
    }
  }

  // Encode value.
  //
  // Will amplitude modulate the local sub-carrier generator with the value
  // and invoke callback with the amplitude sample.
  template <class F, class... Args>
  void EncodeValue(const uint8_t value, F&& callback, Args&&... args) {
    const RealType amplitude = RealType(value) / 255;
    generator_(full_scale_bit_, [&](const RealType sample) {
      std::invoke(std::forward<F>(callback),
                  sample * amplitude,
                  std::forward<Args>(args)...);
    });
  }

  // Generator used to generate a full-scale [-1 .. 1] tone at the sub-carries
  // frequency.
  signal::Generator<RealType> generator_;

  // A full-scale [-1 .. 1] tone of a single bit or a pixel.
  FrequencyDuration<RealType> full_scale_bit_;
};

}  // namespace radio_core::picture::apt
