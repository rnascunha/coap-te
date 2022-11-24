/**
 * @file checks.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_CHECKS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_CHECKS_HPP_

#include <type_traits>
#include <system_error> //NOLINT

#include "coap-te/message/options/definitions.hpp"

namespace coap_te {
namespace message {
namespace options {

template<bool CheckSequence,
         bool CheckFormat,
         bool CheckLength>
struct check_type {
  static constexpr bool sequence    = CheckSequence;
  static constexpr bool format      = CheckFormat;
  static constexpr bool length      = CheckLength;

  static bool constexpr
  check_any() noexcept {
    return sequence || format || length;
  }
};

using check_all = check_type<true, true , true>;
using check_none = check_type<false, false, false>;

template<typename CheckOptions>
constexpr bool
check_sequence(number before, number current,
      const definition& def) noexcept {
  if constexpr (!CheckOptions::sequence) {
    return true;
  } else {
    if (before > current)
      return false;
    if (before == current && !def.repeatable) {
      return false;
    }
    return true;
  }
}

template<typename CheckOptions, typename UnsignedType>
constexpr bool
check_length(UnsignedType value,
             const definition& def,
             uint_format_tag) noexcept {
  static_assert(std::is_unsigned_v<UnsignedType>, "Not unsigned type");

  if constexpr (!CheckOptions::length) {
    return true;
  } else {
    if ((def.length_min == 0u || value >= (1u << (8u * def.length_min))) &&
        value < (1u << (8u * def.length_max))) {
      return true;
    }
    return false;
  }
}

template<typename CheckOptions, typename BufferType>
constexpr bool
check_length(const BufferType& buffer,
             const definition& def,
             opaque_format_tag) noexcept {
  if constexpr (!CheckOptions::length) {
    return true;
  } else {
    if (buffer.size() >= def.length_min &&
        buffer.size() <= def.length_max) {
      return true;
    }
    return false;
  }
}

template<typename CheckOptions, typename BufferType>
constexpr bool
check_length(const BufferType& buffer,
             const definition& def,
             string_format_tag) noexcept {
  if constexpr (!CheckOptions::length) {
    return true;
  } else {
    if (buffer.size() >= def.length_min &&
        buffer.size() <= def.length_max) {
      return true;
    }
    return false;
  }
}

template<typename CheckOptions>
constexpr std::error_code
check(number op) noexcept {
  if constexpr (CheckOptions::check_any()) {
    const auto& def = get_definition(op);
    if (!def)
      return std::make_error_code(std::errc::no_protocol_option);

    if constexpr (CheckOptions::format) {
      if (def.type != format::empty)
        return std::make_error_code(std::errc::wrong_protocol_type);
    }
    return {};
  } else {
    return {};
  }
}

template<typename CheckOptions, typename UnsignedType>
constexpr std::error_code
check(number op, UnsignedType value) noexcept {
  static_assert(std::is_unsigned_v<UnsignedType>, "No unsigned type");

  if constexpr (CheckOptions::check_any()) {
    const auto& def = get_definition(op);
    if (!def)
      return std::make_error_code(std::errc::no_protocol_option);

    if constexpr (CheckOptions::format) {
      if (def.type != format::uint)
        return std::make_error_code(std::errc::wrong_protocol_type);
    }

    if (!check_length(value, def, uint_format_tag{})) {
      return std::make_error_code(std::errc::argument_out_of_domain);
    }

    return {};
  } else {
    return {};
  }
}

template<typename CheckOptions, typename BufferType>
constexpr std::error_code
check(number op, const BufferType& value) noexcept {
  if constexpr (CheckOptions::check_any()) {
    const auto& def = get_definition(op);
    if (!def)
      return std::make_error_code(std::errc::no_protocol_option);

    if constexpr (CheckOptions::format) {
      if (def.type != format::uint)
        return std::make_error_code(std::errc::wrong_protocol_type);
    }

    if (!check_length(value, def, opaque_format_tag{})) {
      return std::make_error_code(std::errc::argument_out_of_domain);
    }

    return {};
  } else {
    return {};
  }
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_CHECKS_HPP_
