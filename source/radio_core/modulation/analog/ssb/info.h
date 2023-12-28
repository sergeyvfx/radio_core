// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Single Side Band modulation information.

#pragma once

#include <array>

#include "radio_core/modulation/analog/type.h"

namespace radio_core::modulation::analog::ssb {

class Info {
 public:
  // The commonly used default bandwidth of the receive filter for this
  // modulation type.
  // While the bandwidth might vary from application to application, the goal of
  // this field is to give decent default value to be used in the interface.
  static constexpr int kDefaultBandwidth = 4000;

  // An ascending array of bandwidth which are commonly used by the
  // modulation type.
  static constexpr auto kTypicalBandwidths =
      std::to_array<int>({500, 1200, 1800, 2400, 2700, 3000, 3300, 4000});
};

}  // namespace radio_core::modulation::analog::ssb
