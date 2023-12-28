// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/variant.h"

#include <sstream>

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Variant, match) {
  const std::variant<int, float, double> my_variant = 18;

  std::stringstream stream;

  my_variant | match{
                   [&](int value) { stream << "INT " << value; },
                   [&](float value) { stream << "FLOAT " << value; },
                   [&](double value) { stream << "DOUBLE " << value; },
               };

  EXPECT_EQ(stream.str(), "INT 18");
}

}  // namespace radio_core
