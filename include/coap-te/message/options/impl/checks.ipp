/**
 * @file checks.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_IMPL_CHECKS_IPP_
#define COAP_TE_MESSAGE_OPTIONS_IMPL_CHECKS_IPP_

#include <type_traits>
#include <system_error> //NOLINT
#include <algorithm>

#include "coap-te/core/traits.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/definitions.hpp"

namespace coap_te {
namespace message {
namespace options {

namespace detail {

template <class ...Ts>
struct is_check_type : coap_te::core::is_instance<Ts..., check_type>{};

std::error_code
check_sequence(number_type before,
               number_type op,
               const definition& def) noexcept {
  if (before > op ||
      (before == op &&
       !def.repeatable)) {
    return std::make_error_code(std::errc::protocol_error);
  }
  return std::error_code{};
}

std::error_code
check_format(format type,
             const definition& def) noexcept {
  if (def.type != type) {
    return std::make_error_code(std::errc::wrong_protocol_type);
  }
  return std::error_code{};
}

std::error_code
check_format(const std::initializer_list<format>& types,
             const definition& def) noexcept {
  if (std::find(std::begin(types), std::end(types), def.type)
            == std::end(types)) {
    return std::make_error_code(std::errc::wrong_protocol_type);
  }
  return std::error_code{};
}

std::error_code
check_length(std::size_t length,
             const definition& def) noexcept {
  if (length < def.length_min ||
      length > def.length_max) {
    return std::make_error_code(std::errc::argument_out_of_domain);
  }
  return std::error_code{};
}

}  // namespace detail

template<typename CheckOptions>
std::error_code
check([[maybe_unused]] number_type before,
      [[maybe_unused]] number_type op,
      [[maybe_unused]] format type,
      [[maybe_unused]] std::size_t opt_length) noexcept {
  static_assert(detail::is_check_type<CheckOptions>::value,
    "'CheckOptions' must be instance of check_type");

  if constexpr (CheckOptions::check_any()) {
    const auto& def = get_definition(static_cast<number>(op));
    if (!def)
      return std::make_error_code(std::errc::no_protocol_option);
    if constexpr (CheckOptions::sequence) {
      if (auto ec = detail::check_sequence(before, op, def))
        return ec;
    }
    if constexpr (CheckOptions::format) {
      if (auto ec = detail::check_format(type, def)) {
        return ec;
      }
    }
    if constexpr (CheckOptions::length) {
      if (auto ec = detail::check_length(opt_length, def)) {
        return ec;
      }
    }
    return {};
  } else {
    return {};
  }
}

template<typename CheckOptions>
std::error_code
check([[maybe_unused]] number_type before,
      [[maybe_unused]] number_type op,
      [[maybe_unused]] const std::initializer_list<format>& types,
      [[maybe_unused]] std::size_t opt_length) noexcept {
  static_assert(detail::is_check_type<CheckOptions>::value,
    "'CheckOptions' must be instance of check_type");

  if constexpr (CheckOptions::check_any()) {
    const auto& def = get_definition(static_cast<number>(op));
    if (!def)
      return std::make_error_code(std::errc::no_protocol_option);
    if constexpr (CheckOptions::sequence) {
      if (auto ec = detail::check_sequence(before, op, def))
        return ec;
    }
    if constexpr (CheckOptions::format) {
      if (auto ec = detail::check_format(types, def)) {
        return ec;
      }
    }
    if constexpr (CheckOptions::length) {
      if (auto ec = detail::check_length(opt_length, def)) {
        return ec;
      }
    }
    return {};
  } else {
    return {};
  }
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_IMPL_CHECKS_IPP_
