// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Encoder of image pixels for SSTV transmission.
//
// This encoder only handles scanlines of the picture. VOX, VIS, and FSKID are
// handled outside (in the typical pipeline at the `SSTVEncoder` level).

#pragma once

#include <functional>

#include "radio_core/base/unreachable.h"
#include "radio_core/math/color.h"
#include "radio_core/math/colorspace.h"
#include "radio_core/picture/pixel_accessor.h"
#include "radio_core/picture/sstv/luma.h"
#include "radio_core/picture/sstv/mode_spec.h"

namespace radio_core::picture::sstv {

template <class RealType>
class PictureEncoder {
 public:
  // Encode the pixels of the picture into SSTV transmission.
  //
  // Encoding happens into a sequence of frequencies of specific duration.
  //
  // The frequency and duration is passed to the callback as an object of
  // FrequencyDuration<RealType>. The given list of args... is passed to the
  // callback after the frequency duration sample. This makes the required
  // callback signature to be:
  //
  //   callback(const FrequencyDuration<RealType>& sample, <optional arguments>)
  template <class F, class... Args>
  void operator()(const Message& message, F&& callback, Args&&... args) {
    using PixelAccessor = Message::PixelAccessor;

    const ModeSpec mode_spec = ModeSpec<RealType>::Get(message.mode);

    // Sanity check to avoid access past image boundaries.
    // Simply reject transmitting the image in this case.
    const PixelAccessor::Spec image_spec = message.pixel_accessor->GetSpec();
    if (image_spec.width < mode_spec.image_width ||
        image_spec.height < mode_spec.image_height) {
      return;
    }

    // While this approach is not very modular it allows to configure SSTV
    // encoder without heap memory allocation as well as allows to encode
    // messages using different modes without re-allocations.

    switch (mode_spec.line_encoding) {
      case LineEncoding::kUnknown:
        // This is more of an error situation which is not supposed to happen
        // under normal usage of modem API.
        //
        // TODO(sergey): What is the proper way to handle it?
        return;
      case LineEncoding::kYccAverageCrCb:
        return EncodeYCbCrAverageCrRb(mode_spec,
                                      message,
                                      std::forward<F>(callback),
                                      std::forward<Args>(args)...);
    }

    UNREACHABLE();
  }

 protected:
  template <class F, class... Args>
  void EncodeYCbCrAverageCrRb(const ModeSpec<RealType>& mode_spec,
                              const Message& message,
                              F&& callback,
                              Args&&... args) {
    using Sample = FrequencyDuration<RealType>;
    using ColorType = Color3<RealType>;

    for (int y = 0; y < mode_spec.image_height; y += 2) {
      // Line synchronization and porch.
      std::invoke(std::forward<F>(callback),
                  mode_spec.line_sync,
                  std::forward<Args>(args)...);
      std::invoke(std::forward<F>(callback),
                  mode_spec.line_porch,
                  std::forward<Args>(args)...);

      // While the RGB->YCbCr conversion happens 3 times for every pixel this
      // allows to have minimal memory footprint (which is desirable for
      // embedded systems).
      //
      // Hopefully compiler optimizes out calculations of color channels which
      // are unused.

      // y0
      for (int x = 0; x < mode_spec.image_width; ++x) {
        const ColorType rgb =
            message.pixel_accessor->GetPixel(x, y).ConvertTo<ColorType>();
        const ColorType ycc = RGBToYCbCr(rgb);

        const Frequency frequency = LumaToFrequency(mode_spec, ycc.ycc.y);

        std::invoke(std::forward<F>(callback),
                    Sample{frequency, mode_spec.pixel_duration_ms},
                    std::forward<Args>(args)...);
      }

      // r-y, b-y
      for (int x = 0; x < mode_spec.image_width; ++x) {
        const ColorType rgb0 =
            message.pixel_accessor->GetPixel(x, y).ConvertTo<ColorType>();
        const ColorType ycc0 = RGBToYCbCr(rgb0);

        const ColorType rgb1 =
            message.pixel_accessor->GetPixel(x, y + 1).ConvertTo<ColorType>();
        const ColorType ycc1 = RGBToYCbCr(rgb1);

        const RealType luma = (ycc0.ycc.cr + ycc1.ycc.cr) / 2;
        const Frequency frequency = LumaToFrequency(mode_spec, luma);

        std::invoke(std::forward<F>(callback),
                    Sample{frequency, mode_spec.pixel_duration_ms},
                    std::forward<Args>(args)...);
      }
      for (int x = 0; x < mode_spec.image_width; ++x) {
        const ColorType rgb0 =
            message.pixel_accessor->GetPixel(x, y).ConvertTo<ColorType>();
        const ColorType ycc0 = RGBToYCbCr(rgb0);

        const ColorType rgb1 =
            message.pixel_accessor->GetPixel(x, y + 1).ConvertTo<ColorType>();
        const ColorType ycc1 = RGBToYCbCr(rgb1);

        const RealType luma = (ycc0.ycc.cb + ycc1.ycc.cb) / 2;
        const Frequency frequency = LumaToFrequency(mode_spec, luma);

        std::invoke(std::forward<F>(callback),
                    Sample{frequency, mode_spec.pixel_duration_ms},
                    std::forward<Args>(args)...);
      }

      // y1
      for (int x = 0; x < mode_spec.image_width; ++x) {
        const ColorType rgb =
            message.pixel_accessor->GetPixel(x, y + 1).ConvertTo<ColorType>();
        const ColorType ycc = RGBToYCbCr(rgb);

        const Frequency frequency = LumaToFrequency(mode_spec, ycc.ycc.y);

        std::invoke(std::forward<F>(callback),
                    Sample{frequency, mode_spec.pixel_duration_ms},
                    std::forward<Args>(args)...);
      }
    }
  }
};

}  // namespace radio_core::picture::sstv
