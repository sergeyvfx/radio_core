// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Demodulator of IF to AF.
// Supports all known IQ modulation types.

#pragma once

#include <memory>
#include <span>

#include "radio_core/base/unreachable.h"
#include "radio_core/modulation/analog/am/demodulator.h"
#include "radio_core/modulation/analog/cw/demodulator.h"
#include "radio_core/modulation/analog/iq_demodulator.h"
#include "radio_core/modulation/analog/lsb/demodulator.h"
#include "radio_core/modulation/analog/nfm/demodulator.h"
#include "radio_core/modulation/analog/type.h"
#include "radio_core/modulation/analog/usb/demodulator.h"
#include "radio_core/modulation/analog/wfm/demodulator.h"

namespace radio_core::signal_path::internal {

template <class T, template <class> class Allocator = std::allocator>
class Demodulator {
 public:
  struct Options {
    // A default value to mainly avoid uninitialized fields.
    modulation::analog::Type modulation_type = modulation::analog::Type::kAM;

    typename modulation::analog::am::Demodulator<T>::Options am;

    typename modulation::analog::nfm::Demodulator<T>::Options nfm;
    typename modulation::analog::wfm::Demodulator<T>::Options wfm;

    typename modulation::analog::lsb::Demodulator<T, Allocator>::Options lsb;
    typename modulation::analog::usb::Demodulator<T, Allocator>::Options usb;

    typename modulation::analog::cw::Demodulator<T>::Options cw;
  };

  void Configure(const Options& options) {
    modulation_type_ = options.modulation_type;

    switch (modulation_type_) {
      case modulation::analog::Type::kAM:
        am_demodulator_.Configure(options.am);
        demodulator_ = &am_demodulator_;
        return;

      case modulation::analog::Type::kNFM:
        nfm_demodulator_.Configure(options.nfm);
        demodulator_ = &nfm_demodulator_;
        return;

      case modulation::analog::Type::kWFM:
        wfm_demodulator_.Configure(options.wfm);
        demodulator_ = &wfm_demodulator_;
        return;

      case modulation::analog::Type::kLSB:
        lsb_demodulator_.Configure(options.lsb);
        demodulator_ = &lsb_demodulator_;
        return;
      case modulation::analog::Type::kUSB:
        usb_demodulator_.Configure(options.usb);
        demodulator_ = &usb_demodulator_;
        return;

      case modulation::analog::Type::kCW:
        cw_demodulator_.Configure(options.cw);
        demodulator_ = &cw_demodulator_;
        return;
    }

    Unreachable();
  }

  // Get currently configured modulation type.
  inline auto GetModulationType() const -> modulation::analog::Type {
    return modulation_type_;
  }

  // Demodulate the given input samples.
  //
  // The output samples must have at least the same number of elements as the
  // input samples. It is possible to pass a buffer of a bigger size.
  //
  // Returns the span of actually written samples in the output.
  auto operator()(const std::span<const BaseComplex<T>> input_samples,
                  const std::span<T> output_samples) -> std::span<T> {
    return (*demodulator_)(input_samples, output_samples);
  }

 private:
  // Available demodulator.
  modulation::analog::am::Demodulator<T> am_demodulator_;
  modulation::analog::nfm::Demodulator<T> nfm_demodulator_;
  modulation::analog::wfm::Demodulator<T> wfm_demodulator_;
  modulation::analog::lsb::Demodulator<T, Allocator> lsb_demodulator_;
  modulation::analog::usb::Demodulator<T, Allocator> usb_demodulator_;
  modulation::analog::cw::Demodulator<T> cw_demodulator_;

  // Currently active modulation type and demodulator.
  modulation::analog::Type modulation_type_ = modulation::analog::Type::kAM;
  modulation::analog::IQDemodulator<T>* demodulator_ = &am_demodulator_;
};

}  // namespace radio_core::signal_path::internal
