// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Tones specifications for Bell 202 modem.
//
// It specifies audio frequency-shift keying (AFSK) to encode and transfer data
// at a rate of 1200 bits per second, half-duplex.
//
// Bell 202 AFSK uses a 1200 Hz tone for mark (typically a binary 1) and 2200 Hz
// for space (typically a binary 0).

#pragma once

#include "radio_core/modulation/digital/fsk/tones.h"

namespace radio_core::modulation::digital::fsk {

// Tones of Bell 202 modem running at 1200 bps.
inline static constexpr Tones kBell202Tones(1200, 2200);

}  // namespace radio_core::modulation::digital::fsk
