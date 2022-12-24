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

#include "coap-te/core/utility.hpp"
#include "coap-te/core/byte_order.hpp"
#include "coap-te/message/options/config.hpp"

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

[[nodiscard]] constexpr std::size_t
header_size(number previous,
            number current,
            std::size_t data_size) noexcept {
  std::size_t size = 1;
  std::size_t diff = coap_te::core::to_underlying(current) -
              coap_te::core::to_underlying(previous);

  for (std::size_t s : {diff, data_size}) {
    if (s >= 269)
      size +=  2;
    else if (s >= 13)
      size += 1;
  }
  return size;
}

template<typename Unsigned = unsigned>
constexpr Unsigned
unsigned_option(const std::uint8_t* data,
                std::size_t size) noexcept {
  return coap_te::core::from_small_big_endian<Unsigned>(data, size);
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP_
