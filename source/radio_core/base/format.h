// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cassert>
#include <cstdint>
#include <span>
#include <type_traits>
#include <utility>

namespace radio_core {

namespace format_internal {

// Reverse characters in the given buffer.
inline void Reverse(const std::span<char> buffer) {
  if (buffer.empty()) {
    return;
  }

  char* left = buffer.data();
  char* right = buffer.data() + buffer.size() - 1;
  while (left < right) {
    std::swap(*left, *right);
    ++left;
    --right;
  }
}

template <class Int, std::enable_if_t<std::is_signed_v<Int>, bool> = true>
static auto GetAbsoluteValue(const Int value) -> Int {
  return value >= 0 ? value : -value;
}

template <class Int, std::enable_if_t<std::is_unsigned_v<Int>, bool> = true>
static auto GetAbsoluteValue(const Int value) -> Int {
  return value;
}

}  // namespace format_internal

// Formater of an integer value to a string.
struct IntFormat {
  using size_type = std::size_t;

  struct Options {
    // Use "+" prefix for values non-negative values.
    bool use_positive_prefix{false};

    // When use_positive_prefix is true and value is 0 prepend the plus sign.
    // When set to false the value of 0 uses no prefix.
    bool use_positive_zero{true};

    // When is not equal to '\0' this is the character which is used to separate
    // thousand (every 3rd digit from the right).
    //
    // This options is effectively ignored when use_highest_decimal_unit_prefix
    // is true since the integer values are never above 1000.
    char thousand_separator{'\0'};

    // Align the converted value to the right of the buffer.
    //
    // The prefix of the string is left unmodified.
    bool align_to_right{false};

    // Convert the value to a scientific-like notation with integer part lower
    // than 1000, followed with optional decimal digits of 3 places maximum and
    // finally followed by a decimal unit prefix in metric system (k, M, G and
    // so on).
    bool use_highest_decimal_unit_prefix{false};

    // Add space prior to the space unit letter.
    bool use_space_before_unit{false};

    // When true an extra character denoting null-terminator will ba appended to
    // the result.
    bool append_null_terminator{true};
  };

  // Formats integer to a string buffer, following the given formatting options.
  //
  // The result is written to the given string buffer, starting from its
  // beginning. No additional memory is allocated, and the same buffer is used
  // to hold an intermediate result. This means that If the buffer is not big
  // enough to hold the converted result then the content of the buffer after
  // this function is undefined.
  //
  // Returns the number of characters (including the null-terminator) written to
  // the buffer. If error occurs 0 is returned.
  template <class Int>
  static auto FormatToBuffer(const Options& options,
                             const Int value,
                             const std::span<char> buffer) -> size_type {
    if (options.align_to_right) {
      return FormatAlignToRight(options, value, buffer);
    }

    return FormatAlignToLeft(options, value, buffer);
  }

  // Formats integer to a string buffer, following the given formatting options.
  //
  // The result is written to the string, using at maximum str.size() bytes.
  // After the conversion string is resized to the written content size by
  // str.resize().
  //
  // Returns true if the conversion is successful.
  template <class Int, class StringType>
  static auto FormatToString(const Options& options,
                             const Int value,
                             StringType& str) -> bool {
    Options str_options = options;
    str_options.append_null_terminator = false;

    const size_type num_chars_written = FormatToBuffer(str_options, value, str);
    if (num_chars_written == 0) {
      return false;
    }

    // When aligning to the right rely on the null-terminator provided by the
    // string implementation.
    if (!options.align_to_right) {
      str.resize(num_chars_written);
    }

    return true;
  }

 private:
  // Implementation of Format() with the result being aligned to the left.
  template <class Int>
  static auto FormatAlignToLeft(const Options& options,
                                const Int value,
                                const std::span<char> buffer) -> size_type {
    class Append {
     public:
      explicit Append(std::span<char> buffer)
          : buffer_(buffer), buffer_size_(buffer.size()) {}
      auto operator()(const char ch) -> bool {
        if (buffer_index_ >= buffer_size_) {
          return false;
        }
        buffer_[buffer_index_++] = ch;
        return true;
      }

      auto GetAssignedSpan() -> std::span<char> {
        return buffer_.subspan(0, buffer_index_);
      }

      auto GetNumCharsWritten() const -> size_type { return buffer_index_; }

     private:
      const std::span<char> buffer_;
      size_t buffer_size_{0};
      size_t buffer_index_{0};
    };

    Append append(buffer);
    if (!AppendResultInReverseOrder(options, append, value)) {
      return 0;
    }

    // Reverse the string (as above the least significant digits are appended
    // starting from the beginning of the string).
    format_internal::Reverse(append.GetAssignedSpan());

    // Null-terminate.
    if (options.append_null_terminator) {
      if (!append('\0')) {
        return 0;
      }
    }

    return append.GetNumCharsWritten();
  }

  // Implementation of Format() with the result being aligned to the right.
  template <class Int>
  static auto FormatAlignToRight(const Options& options,
                                 const Int value,
                                 const std::span<char> buffer) -> size_type {
    class Append {
     public:
      explicit Append(std::span<char> buffer)
          : buffer_(buffer),
            buffer_size_(buffer.size()),
            buffer_index_(buffer_size_) {}
      auto operator()(const char ch) -> bool {
        if (buffer_index_ == 0) {
          return false;
        }
        buffer_[--buffer_index_] = ch;
        return true;
      }

      auto GetNumCharsWritten() const -> size_type {
        return buffer_size_ - buffer_index_;
      }

     private:
      const std::span<char> buffer_;
      size_t buffer_size_{0};
      size_t buffer_index_{0};
    };

    Append append(buffer);

    // Null-terminate.
    if (options.append_null_terminator) {
      if (!append('\0')) {
        return 0;
      }
    }

    if (!AppendResultInReverseOrder(options, append, value)) {
      return 0;
    }

    return append.GetNumCharsWritten();
  }

  // A main logic of conversion which invokes given functor append with all
  // digits and symbols of the resulting value in the reverse order.
  //
  // For example, value of 123 the append is called with characters '3', '2',
  // and '1'.
  //
  // The thousand separator and the sign symbol is provided to the functor.
  // The null-terminator is to be handled outside of this function.
  template <class Int, class Append>
  static auto AppendResultInReverseOrder(const Options& options,
                                         Append&& append,
                                         const Int value) -> bool {
    const Int positive_value = format_internal::GetAbsoluteValue<Int>(value);

    // Convert positive value to string.
    if (options.use_highest_decimal_unit_prefix) {
      if (!AppendHighestDecimalPrefixDigitsAndSeparatorsReverseOrder(
              options, std::forward<Append>(append), positive_value)) {
        return false;
      }
    } else {
      if (!AppendDigitsAndSeparatorsReverseOrder(
              options, std::forward<Append>(append), positive_value)) {
        return false;
      }
    }

    // Append sign.
    if (value < 0) {
      if (!append('-')) {
        return false;
      }
    } else if (options.use_positive_prefix) {
      if (!options.use_positive_zero && value == 0) {
        // Pass.
      } else if (!append('+')) {
        return false;
      }
    }

    return true;
  }

  // Invokes the append functor for all digits and thousand separators needed to
  // represent the given value as a string.
  //
  // The value is expected to be positive which implies that the sign is to be
  // handled outside of this function.
  //
  // The null-terminator is to be handled outside of the function/
  template <class Int, class Append>
  static auto AppendDigitsAndSeparatorsReverseOrder(const Options& options,
                                                    Append&& append,
                                                    const Int value) -> bool {
    assert(value >= 0);

    // Convert positive value to string.
    Int value_convert = value;
    size_t digit_index = 0;
    do {
      // Convert digit from integer value to character.
      const int digit = value_convert % 10;
      value_convert /= 10;
      if (!append(char('0' + digit))) {
        return false;
      }
      ++digit_index;

      // Append thousand separator when needed.
      if (options.thousand_separator != '\0') {
        if (digit_index % 3 == 0 && value_convert != 0) {
          if (!append(options.thousand_separator)) {
            return false;
          }
        }
      }
    } while (value_convert != 0);

    return true;
  }

  // Append digits, decimal separator, and decimal unit prefix of the given
  // value. The value first is converted to a notation where the integer part
  // is less than 1000.
  //
  // The value is expected to be positive which implies that the sign is to be
  // handled outside of this function.
  //
  // The null-terminator is to be handled outside of the function.
  template <class Int, class Append>
  static auto AppendHighestDecimalPrefixDigitsAndSeparatorsReverseOrder(
      const Options& options, Append&& append, const Int value) -> bool {
    assert(value >= 0);

    // Simplify the value as much as possible.
    Int integer_part = value;
    Int decimal_part = 0;
    size_t prefix_index = 0;
    while (integer_part >= 1000) {
      decimal_part = integer_part % 1000;
      integer_part /= 1000;
      ++prefix_index;
    }

    // Append decimal unit prefix.
    if (prefix_index) {
      constexpr char kPrefixes[] = {
          '\0', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};
      if (prefix_index >= sizeof(kPrefixes)) {
        return false;
      }

      // Append unit letter.
      if (!append(kPrefixes[prefix_index])) {
        return false;
      }

      // Append optional space.
      if (options.use_space_before_unit) {
        if (!append(' ')) {
          return false;
        }
      }
    }

    // Convert decimal part.
    if (decimal_part != 0) {
      Int decimal_part_without_trailing_zeros = decimal_part;
      while (decimal_part_without_trailing_zeros % 10 == 0) {
        decimal_part_without_trailing_zeros /= 10;
      }

      if (!AppendDigitsAndSeparatorsReverseOrder(
              options,
              std::forward<Append>(append),
              decimal_part_without_trailing_zeros)) {
        return false;
      }

      if (decimal_part < 100) {
        if (!append('0')) {
          return false;
        }
      }
      if (decimal_part < 10) {
        if (!append('0')) {
          return false;
        }
      }

      if (!append('.')) {
        return false;
      }
    }

    // Convert integer part.
    if (!AppendDigitsAndSeparatorsReverseOrder(
            options, std::forward<Append>(append), integer_part)) {
      return false;
    }

    return true;
  }
};

}  // namespace radio_core
