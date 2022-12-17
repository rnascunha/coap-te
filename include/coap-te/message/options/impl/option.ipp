/**
 * @file option.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_IMPL_OPTION_IPP_
#define COAP_TE_MESSAGE_OPTIONS_IMPL_OPTION_IPP_

#include "coap-te/core/byte_order.hpp"
#include "coap-te/message/options/option.hpp"

namespace coap_te {
namespace message {
namespace options {

namespace detail {

template<typename CheckOptions,
          bool ToThrow>
constexpr bool
check_constructor([[maybe_unused]] number op,
                  [[maybe_unused]] format type,
                  [[maybe_unused]] std::size_t size) noexcept(!ToThrow) {
  using n_check = check_type<false,
                             CheckOptions::format,
                             CheckOptions::length>;
  if constexpr (n_check::check_any()) {
    auto ec = check<n_check>(
                      coap_te::core::to_underlying(number::invalid),
                      coap_te::core::to_underlying(op),
                      type, size);
    if (ec) {
      if constexpr (ToThrow) {
        throw std::system_error{ec};
      } else {
        return false;
      }
    }
  }
  return true;
}

}  // namespace detail

template<typename CheckOptions /* = check_all */,
         bool ToThrow /* = false */>
[[nodiscard]] constexpr option
create(number op) noexcept(!ToThrow) {
  if (!detail::check_constructor<CheckOptions, ToThrow>(op, format::empty, 0)) {
    return option{};
  }
  return option{op};
}

template<typename CheckOptions /* = check_all */,
         bool ToThrow /* = false */>
[[nodiscard]] constexpr option
create(number op, option::unsigned_type value) noexcept(!ToThrow) {
  if (!detail::check_constructor<CheckOptions, ToThrow>(
              op, format::uint,
              coap_te::core::small_big_endian_size(value))) {
    return option{};
  }
  return option{op, value};
}

template<typename CheckOptions /* = check_all */,
         bool ToThrow /* = false */>
[[nodiscard]] constexpr option
create(number op, std::string_view value) noexcept(!ToThrow) {
  if (!detail::check_constructor<CheckOptions, ToThrow>(op, format::string, value.size())) {
    return option{};
  }
  return option{op, value};
}

template<typename CheckOptions /* = check_all */,
         bool ToThrow /* = false */>
[[nodiscard]] constexpr option
create(number op, const const_buffer& value) noexcept(!ToThrow) {
  if (!detail::check_constructor<CheckOptions, ToThrow>(op, format::opaque, value.size())) {
    return option{};
  }
  return option{op, value};
}

}  // namespace options 
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_IMPL_OPTION_IPP_
