// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/value_printer.h"

#include <array>
#include <bit>
#include <sstream>

#include "radio_core/unittest/test.h"

namespace radio_core {

namespace {

class NonPrintableClass {
 public:
  uint32_t value;

  explicit NonPrintableClass(const uint32_t new_value) : value(new_value) {}
};

class NonPrintableLargeClass {
 public:
  uint32_t head;
  std::array<uint32_t, 128> padding = {};
  uint32_t tail;

  explicit NonPrintableLargeClass(const uint32_t new_head,
                                  const uint32_t new_tail)
      : head(new_head), tail(new_tail) {}
};

class ClassWithPutToStreamOperator {
 public:
  int value;

  explicit ClassWithPutToStreamOperator(const int new_value)
      : value(new_value) {}

  friend auto operator<<(std::ostream& os,
                         const ClassWithPutToStreamOperator& obj)
      -> std::ostream& {
    return os << "value:" << obj.value;
  }
};

enum class NonPrintableEnumClass {
  kOne,
  kTwo,
  kThree,
};

}  // namespace

TEST(ValuePrinter, Integral) {
  // Boolean.
  {
    std::ostringstream stream;
    ValuePrinter::PrintTo(true, stream);
    ValuePrinter::PrintTo(false, stream);
    EXPECT_EQ(stream.str(), "10");
  }

  // Integer.
  {
    std::ostringstream stream;
    ValuePrinter::PrintTo(3910, stream);
    EXPECT_EQ(stream.str(), "3910");
  }
}

TEST(ValuePrinter, FloatingPoint) {
  // Float.
  {
    std::ostringstream stream;
    ValuePrinter::PrintTo(3910.18f, stream);
    EXPECT_EQ(stream.str(), "3910.18");
  }

  // Double.
  {
    std::ostringstream stream;
    ValuePrinter::PrintTo(3910.18, stream);
    EXPECT_EQ(stream.str(), "3910.18");
  }
}

TEST(ValuePrinter, NonPrintableClass) {
  {
    std::ostringstream stream;
    ValuePrinter::PrintTo(NonPrintableClass(40048086), stream);
    if constexpr (std::endian::native == std::endian::little) {
      EXPECT_EQ(stream.str(), "4-byte object <D6-15 63-02>");
    } else {
      EXPECT_EQ(stream.str(), "4-byte object <63-02 D6-15>");
    }
  }

  {
    std::ostringstream stream;
    ValuePrinter::PrintTo(NonPrintableLargeClass(40048086, 80864004), stream);
    if constexpr (std::endian::native == std::endian::little) {
      EXPECT_EQ(stream.str(),
                "520-byte object "
                "<D6-15 63-02 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 "
                "... "
                "00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 04-E3 D1-04>");
    } else {
      EXPECT_EQ(stream.str(),
                "520-byte object "
                "<63-02 D6-15 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 "
                "... "
                "00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 00-00 "
                "00-00 00-00 00-00 00-00 00-00 D1-04 04-E3>");
    }
  }
}

TEST(ValuePrinter, ClassWithPutToStreamOperator) {
  {
    std::ostringstream stream;
    ValuePrinter::PrintTo(ClassWithPutToStreamOperator(3910), stream);
    EXPECT_EQ(stream.str(), "value:3910");
  }
}

TEST(ValuePrinter, NonPrintableEnumClass) {
  {
    std::ostringstream stream;
    ValuePrinter::PrintTo(NonPrintableEnumClass::kTwo, stream);
    EXPECT_EQ(stream.str(),
              std::to_string(static_cast<int>(NonPrintableEnumClass::kTwo)));
  }
}

}  // namespace radio_core
