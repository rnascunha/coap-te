/**
 * @file option_func.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_OPTION_FUNC_HPP_
#define COAP_TE_MESSAGE_OPTIONS_OPTION_FUNC_HPP_

#include <type_traits>
#include <cstdint>

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/functions.hpp"
#include "coap-te/message/options/traits.hpp"

namespace coap_te {
namespace message {
namespace options {

template<typename Option,
         typename = std::enable_if_t<is_option_v<Option>>>
[[nodiscard]] constexpr bool
is_valid(Option const& op) noexcept {
  return op.option_number() != number::invalid;
}

template<typename Unsigned = unsigned,
         typename Option,
         typename = std::enable_if_t<is_option_v<Option>>>
constexpr Unsigned
unsigned_option(const Option& op) noexcept {
  return unsigned_option<Unsigned>(
              static_cast<const std::uint8_t*>(op.data()),
              op.data_size());
}


template<typename Option,
        typename = std::enable_if_t<is_option_v<Option>>>
[[nodiscard]] constexpr std::size_t
header_size(const Option& op, number previous) noexcept {
  std::size_t size = 1;
  std::size_t diff = coap_te::core::to_underlying(op.option_number()) -
                     coap_te::core::to_underlying(previous);

  for (std::size_t s : {diff, op.data_size()}) {
    if (s >= 269)
      size +=  2;
    else if (s >= 13)
      size += 1;
  }
  return size;
}

template<typename Option,
         typename = std::enable_if_t<is_option_v<Option>>>
[[nodiscard]] constexpr std::size_t
size(const Option& op, number previous) noexcept {
  return header_size(op, previous) + op.data_size();
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_OPTION_FUNC_HPP_
