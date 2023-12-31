// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Printer of various objects to a steram in the most human readable manner
// possible.
//
// The printing rules are happening in the following order;
//
//  - If an object implements operator<< to a stream this operator is used.
//
//  - Enumeration values are printed as integer values corresponding to the
//    internal value of the enumerator value.
//
//  - When nothing from above fits then the object is printed as a memory
//    dump. When object os too large then only head and tail of its memory is
//    printed.
//
// Example:
//
//   class MyClass {
//     int foo;
//     float bar;
//   };
//
//   MyClass object;
//   ValuePrinter::PrintTo(object, std::cout);

#pragma once

#include <ostream>

namespace radio_core {

namespace value_printer_internal {

static_assert(sizeof(unsigned char) == 1);

template <class T>
struct HasPutToStreamOperatorImpl {
  template <class U>
  static auto Test(U*)
      -> decltype(std::declval<std::ostream>() << std::declval<U>());

  template <class U>
  static auto Test(...) -> std::false_type;

  using type = typename std::negation<
      std::is_same<std::false_type, decltype(Test<T>(nullptr))>>::type;
};

// If T can be put to a `std::ostream stream`, provides the member constant
// value equal to `true`.
// Otherwise value is `false`.
template <class T>
struct HasPutToStreamOperator : HasPutToStreamOperatorImpl<T>::type {};

// Print a sub-span of the memory as human readable hexadecimal values.
//
// Use old-style indices instead of std::span to minimize dependencies of this
// file.
inline void PrintObjectBytesSegmentTo(const unsigned char* data,
                                      const size_t start_byte,
                                      const size_t num_bytes,
                                      std::ostream& os) {
  // Digits in the hexidecimal base.
  //
  // clang-format off
  constexpr char kHexDigits[] = {
      '0', '1', '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',
      'A', 'B', 'C', 'D', 'E', 'F',
  };
  // clang-format on

  // Buffer for hexidecimal conversion.
  //
  // NOTE: Not using modern std::array to minimize amount of headers pulled in.
  char hex[3] = "";

  for (int i = 0; i < num_bytes; ++i) {
    const size_t byte_index = start_byte + i;

    // Convert byte to hex.
    const unsigned char byte = data[byte_index];
    hex[0] = kHexDigits[byte / 16];
    hex[1] = kHexDigits[byte % 16];

    if (i != 0) {
      if ((byte_index % 2) == 0) {
        os << ' ';
      } else {
        os << '-';
      }
    }

    os << hex;
  }
}

// Print object as a human-readable memory dump.
inline void PrintObjectBytesTo(const unsigned char* data,
                               const size_t num_bytes,
                               std::ostream& os) {
  os << num_bytes << "-byte object <";

  // Maximum size of an object in bytes which printed as a whole continuous
  // memory dump. When this size is exceeded only head and tail of the object
  // memory is printed.
  constexpr size_t kMaxFullObjectSizeInBytes = 132;

  // Size of head and tail in bytes when the object memory is dumped partially.
  constexpr size_t kChunkSizeInBytes = 64;

  if (num_bytes < kMaxFullObjectSizeInBytes) {
    PrintObjectBytesSegmentTo(data, 0, num_bytes, os);
  } else {
    PrintObjectBytesSegmentTo(data, 0, kChunkSizeInBytes, os);

    os << " ... ";

    // Round tail start to the 2-byte boundary.
    const size_t tail_start = (num_bytes - kChunkSizeInBytes + 1) / 2 * 2;

    PrintObjectBytesSegmentTo(data, tail_start, num_bytes - tail_start, os);
  }

  os << ">";
}

}  // namespace value_printer_internal

class ValuePrinter {
 public:
  template <class T>
  static void PrintTo(const T& value, std::ostream& os) {
    // NOTE: Keep `else` to silence unreachable code warning generated by MSVC.

    if constexpr (value_printer_internal::HasPutToStreamOperator<T>::value) {
      os << value;
    } else if constexpr (std::is_enum_v<T>) {
      os << static_cast<typename std::underlying_type<T>::type>(value);
    } else {
      value_printer_internal::PrintObjectBytesTo(
          reinterpret_cast<const unsigned char*>(&value), sizeof(T), os);
    }
  }
};

}  // namespace radio_core
