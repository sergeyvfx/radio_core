// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Static compile-time information about specific mode.
//
// This is a declaration of the specification, which is specialized in the
// specific mode headers.

#pragma once

#include "radio_core/picture/sstv/mode.h"

namespace radio_core::picture::sstv {

template <class RealType, Mode kMode>
struct ModeSpecInfo;

}  // namespace radio_core::picture::sstv
