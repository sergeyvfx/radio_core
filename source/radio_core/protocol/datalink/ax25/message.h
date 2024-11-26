// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// AX.25 message.
//
// Follows description of the AX.25 Link Access Protocol:
//
//  [ax25] AX.25 Link Access Protocol for Amateur Packet Radio
//         Version 2.2 Revision: July 1998
//         http://www.ax25.net/AX25.2.2-Jul%2098-2.pdf

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <ostream>
#include <string_view>
#include <utility>

#include "radio_core/protocol/datalink/ax25/control.h"

namespace radio_core::protocol::datalink::ax25 {

namespace ax25_internal {
// Simple string implementation which uses fixed amount of space allocated
// within the object itself to store data. It provided very limited operations
// but allows to have sting fields in the AX.25 message without heap
// allocations. If the message itself is allocated on stack or global memory
// then the memory usage is known at compile time (which is useful for embedded
// applications).
template <size_t MaxLength, char EmptySymbol>
class FixedString {
  // Storage for the string data.
  using Data = std::array<char, MaxLength + 1>;

 public:
  using value_type = char;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = typename Data::iterator;
  using const_iterator = typename Data::const_iterator;

  // In-class alias for the maximum capacity (excluding null-terminator).
  static constexpr size_type static_capacity = MaxLength;

  FixedString() { Clear(); }

  explicit FixedString(const std::string_view str) {
    Clear();

    // Clamp string to the maximum allowed number of characters.
    const std::string_view str_clamped = str.substr(0, MaxLength);
    std::copy(str_clamped.begin(), str_clamped.end(), data_.begin());
  }

  FixedString(const FixedString& other) = default;
  FixedString(FixedString&& other) noexcept = default;

  ~FixedString() = default;

  auto operator=(const FixedString& other) -> FixedString& = default;
  auto operator=(FixedString&& other) -> FixedString& = default;

  inline auto operator=(const std::string_view str) -> FixedString& {
    Assign(str);
    return *this;
  }

  inline void Assign(const std::string_view str) {
    Clear();

    // Clamp string to the maximum allowed number of characters.
    const std::string_view str_clamped = str.substr(0, MaxLength);
    std::copy(str_clamped.begin(), str_clamped.end(), data_.begin());
  }

  // Clear the string to the state which is considered empty by the
  // transmitter (all characters are spaces).
  inline void Clear() {
    data_.fill(EmptySymbol);
    data_[MaxLength] = '\0';
  }

  // Returns the number of characters that the string has currently allocated
  // space for.
  inline auto GetCapacity() const -> size_type { return MaxLength; }

  // String length including the empty symbols.
  inline auto size() const -> size_type { return MaxLength; }
  inline auto GetLength() const -> size_type { return size(); }

  // Access null-terminated string.
  //
  // NOTE: Includes trailing empty symbols.
  inline auto c_str() const -> const char* {
    // Due to the way how the data array is initialized and accessed the
    // string null-terminated.
    // If an out-of-bounds write happens outside of this class this might be
    // violated, but there is not much can be done about it.
    return data_.data();
  }

  // Get view which points to the content of the string without trailing empty
  // characters and without null-terminators.
  inline auto GetCleanView() const -> std::string_view {
    std::string_view result(data_.data(), GetLength());

    while (result.ends_with(EmptySymbol)) {
      result.remove_suffix(1);
    }

    return result;
  }

  // Access string element by char index.
  //
  // The index is asserted to be within a valid range. No further range checking
  // is performed and passing index outside of the allowed range will leads to
  // an undefined behavior.
  constexpr auto operator[](const size_type index) -> reference {
    assert(index < data_.size());
    return data_[index];
  }
  constexpr auto operator[](const size_type index) const -> const_reference {
    assert(index < data_.size());
    return data_[index];
  }

  // Comparators.

  inline auto operator==(const FixedString& other) const -> bool {
    return data_ == other.data_;
  }
  inline auto operator!=(const FixedString& other) const -> bool {
    return !(*this == other);
  }

  inline auto operator==(const std::string_view rhs) const -> bool {
    return rhs == c_str();
  }
  inline auto operator!=(const std::string_view rhs) const -> bool {
    return !(*this == rhs);
  }

  // Iterators.
  //
  // NOTE: Includes EmptySymbol characters.

  inline auto begin() -> iterator { return data_.begin(); }
  inline auto begin() const -> const_iterator { return data_.begin(); }

  // NOTE: `end() - 1` so that the explicit null-pointer terminator is not
  // included into the iteration.
  inline auto end() -> iterator { return std::prev(data_.end()); }
  inline auto end() const -> const_iterator { return std::prev(data_.end()); }

 private:
  // Actual storage of the string.
  // The extra element is allocated for a null-terminator.
  Data data_;
};

template <size_t MaxLength, char EmptySymbol>
inline auto operator<<(std::ostream& os,
                       const FixedString<MaxLength, EmptySymbol>& str)
    -> std::ostream& {
  os << str.GetCleanView();
  return os;
}

}  // namespace ax25_internal

// Callsign of source or destination of AX.25 frame.
//
// By the specification the callsign has maximum length of 6 characters and all
// 6 symbols are transmitted. If an actual callsign is shorter than 6 characters
// then "unused" characters are replaces with spaces.
//
// By default the callsign object is constructed with all characters initialized
// to their empty state.
class Callsign : public ax25_internal::FixedString<6, ' '> {
 public:
  using FixedString::FixedString;

  using FixedString::operator=;
};

// Source or destination address of a message.
class Address {
 public:
  Address() = default;
  Address(const Address& other) = default;
  Address(Address&& other) noexcept = default;

  Address(Callsign new_callsign,
          int new_ssid = 0,
          bool new_has_been_repeated = false)
      : callsign(std::move(new_callsign)),
        ssid(new_ssid),
        has_been_repeated(new_has_been_repeated) {}
  Address(std::string_view new_callsign,
          int new_ssid = 0,
          bool new_has_been_repeated = false)
      : callsign(new_callsign),
        ssid(new_ssid),
        has_been_repeated(new_has_been_repeated) {}

  ~Address() = default;

  auto operator=(const Address& other) -> Address& = default;
  auto operator=(Address&& other) -> Address& = default;

  inline auto operator==(const Address& other) const -> bool {
    return callsign == other.callsign && ssid == other.ssid &&
           has_been_repeated == other.has_been_repeated;
  }
  inline auto operator!=(const Address& other) const -> bool {
    return !(*this == other);
  }

  inline void Clear() {
    callsign.Clear();
    ssid = 0;
    has_been_repeated = false;
  }

  Callsign callsign;

  // Secondary Station Identifier.
  // It consists of the SSSS bits of the field encoded in the frame, directly
  // identifying SSID. No need to do any bit operation to access the SSID.
  int ssid = 0;

  // Value of the command/repsonse bit from the SSID.
  // Corresponds to the C bit ([C]ontrol) of the AX.25 shifted to the left so
  // its value is 0 or 1.
  uint8_t command_response_bit = 0;

  // Corresponds to the H bit ([H]as-been-repeated) of the AX.25
  // specification. The H bits indicate that the Layer 2 repeater station
  // has repeated the frame.
  bool has_been_repeated = false;
};

inline auto operator<<(std::ostream& os, const Address& address)
    -> std::ostream& {
  os << address.callsign;

  if (address.ssid) {
    os << "-" << address.ssid;
  }

  if (address.has_been_repeated) {
    os << "*";
  }

  return os;
}

// Class which encapsulates repeater addresses used to deliver AX.25 message.
class Repeaters {
 public:
  using value_type = Address;
  using size_type = size_t;

  // Based on the address field size of 112 octets. Address requires 7 octets,
  // source and destination are always present, leaving enough octets for only
  // 14 repeaters.
  static inline constexpr size_type kMaxNumRepeaters = 14;

  Repeaters() = default;
  Repeaters(const Repeaters& other) = default;
  Repeaters(Repeaters&& other) noexcept = default;

  ~Repeaters() = default;

  auto operator=(const Repeaters& other) -> Repeaters& = default;
  auto operator=(Repeaters&& other) -> Repeaters& = default;

  inline void Clear() {
    repeaters_.fill(Address());
    num_repeaters_ = 0;
  }

  inline auto IsEmpty() const -> bool { return size() == 0; }

  // Similar to the STL containers: number of used repeaters.
  inline auto size() const -> size_type { return num_repeaters_; }

  // Try to append a repeater to the list.
  //
  // If the address is added to the list true is returned.
  // If the number of repeaters exceeds kMaxNumRepeaters then false is returned.
  inline auto TryAppend(const Address& repeater_address) -> bool {
    if (num_repeaters_ == kMaxNumRepeaters) {
      return false;
    }
    repeaters_[num_repeaters_++] = repeater_address;
    return true;
  }

  // Get repeater with the given index.
  //
  // The calls will perform an assert to check whether the index is valid or
  // not.
  inline auto operator[](const size_type index) -> Address& {
    const Repeaters* const_this = this;
    return const_cast<Address&>(((*const_this)[index]));
  }
  inline auto operator[](const size_type index) const -> const Address& {
    assert(index < size());
    return repeaters_[index];
  }

  // Get repeater with the given index.
  // Generates std::out_of_range if !(index < size()).
  inline auto At(const size_type index) -> Address& {
    const Repeaters* const_this = this;
    return const_cast<Address&>((const_this->At(index)));
  }
  inline auto At(const size_type index) const -> const Address& {
    if (!(index < size())) {
      throw std::out_of_range("Repeaters[] index out of bounds");
    }
    return repeaters_[index];
  }

  // Iterators.

  inline auto begin() { return repeaters_.begin(); }
  inline auto begin() const { return repeaters_.begin(); }

  inline auto end() { return begin() + num_repeaters_; }
  inline auto end() const { return begin() + num_repeaters_; }

 private:
  // Repeater addresses.
  std::array<Address, kMaxNumRepeaters> repeaters_;

  // Actual number of used repeater addresses.
  size_type num_repeaters_ = 0;
};

// Standard values for the AX.25 protocol identifiers.
class PID {
 public:
  // No Layer 3 Protocol.
  inline static constexpr int kNoLayer3 = 0xf0;
};

// Information field of the AX.25 message.
//
// The length corresponds to the default length of the I field as per the
// protocol specification.
class Information : public ax25_internal::FixedString<256, '\0'> {
 public:
  using FixedString::FixedString;

  using FixedString::operator=;
};

// Full specification of AX.25 message.
//
// NOTE: Does not include the frame flags as those are handled by the HDLC
// framing.
class Message {
 public:
  Message() { Clear(); }

  Message(const Message& other) = default;
  Message(Message&& other) noexcept = default;

  ~Message() = default;

  auto operator=(const Message& other) -> Message& = default;
  auto operator=(Message&& other) -> Message& = default;

  // Clear the message,bring it back to the initial state.
  inline void Clear() {
    address.source.Clear();
    address.destination.Clear();
    address.repeaters.Clear();

    control = ControlBits::Unnumbered::kUI;
    pid = 0;

    information.Clear();
  }

  // Get version used to encode fields in the frame:
  //
  //   - Version 1 is what the [ax25] refers to as "Previous Version".
  //     In this version there is no information about command/responce fields.
  //   - Version 2 is what the [ax25] refers to V2.
  //     In this version the C bits of the SSID field indicate command/response.
  [[nodiscard]] inline auto GetVersion() const -> int {
    if (address.source.command_response_bit ==
        address.destination.command_response_bit) {
      return 1;
    }
    return 2;
  }

  // Returns true if this message is a command.
  // If the message uses old protocol version false is returned.
  [[nodiscard]] inline auto IsCommand() const -> bool {
    if (GetVersion() != 2) {
      return false;
    }
    return address.destination.command_response_bit == 1 &&
           address.source.command_response_bit == 0;
  }

  // Returns true if this message is a response.
  // If the message uses old protocol version false is returned.
  [[nodiscard]] inline auto IsResponse() const -> bool {
    if (GetVersion() != 2) {
      return false;
    }
    return address.destination.command_response_bit == 0 &&
           address.source.command_response_bit == 1;
  }

  struct Addr {
    Address source;
    Address destination;
    Repeaters repeaters;
  } address;

  // Value of the control field.
  // As-is from the frame encoding.
  int control = ControlBits::Unnumbered::kUI;

  // The Protocol Identifier (PID).
  //
  // NOTE: Not strongly typed because transmission does not guarantee validness
  // of the value: in other words, transmission could use value which will not
  // fall under any of the strong types.
  int pid = 0;

  Information information;
};

}  // namespace radio_core::protocol::datalink::ax25
