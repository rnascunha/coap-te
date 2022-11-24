/**
 * @file functions.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief Functions used wih options
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP_

#include <system_error>   // NOLINT

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/constants.hpp"

namespace coap_te {
namespace message {
namespace options {

[[nodiscard]] constexpr bool
is_critical(number op) noexcept {
  return (static_cast<number_type>(op) & 1) != 0;
}

[[nodiscard]] constexpr bool
is_elective(number op) noexcept {
  return !(is_critical(op));
}

[[nodiscard]] constexpr bool
is_unsafe_to_forward(number op) noexcept {
  return (static_cast<number_type>(op) & 0b10) != 0;
}

[[nodiscard]] constexpr bool
is_safe_to_forward(number op) noexcept {
  return !(is_unsafe_to_forward(op));
}

[[nodiscard]] constexpr bool
is_no_cache_key(number op) noexcept {
  return (static_cast<number_type>(op) & no_cache_key) == no_cache_key;
}

// template<typename Buffer,
//          typename CheckOption>
// size_format serialize(
//             number before,
//             number current,
//             Buffer&,
//             std::error_code&) noexcept;

// template<typename Buffer,
//          typename CheckOption>
// size_format serialize(
//             number before,
//             number current,
//             unsigned,
//             Buffer&,
//             std::error_code&) noexcept;

// template<typename Buffer,
//          typename ConstBuffer,
//          typename CheckOption>
// size_format serialize(
//             number before,
//             number current,
//             const ConstBuffer&,
//             Buffer&,
//             std::error_code&) noexcept;

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP_
