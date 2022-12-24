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

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/traits.hpp"

namespace coap_te {
namespace message {
namespace options {

template<typename Option, 
         typename std::enable_if_t<is_option_v<Option>, int> = 0>
[[nodiscard]] constexpr bool
is_valid(Option const& op) noexcept {
  return op.option_number() != number::invalid;
}

// template<typename Option, 
//          typename std::enable_if_t<is_option_v<Option>, int> = 0>
// [[nodiscard]] constexpr bool
// operator==(const Option& lhs, const Option& rhs) noexcept {
//   return lhs.option_number() == rhs.option_number();
// }

// template<typename Option, 
//          typename std::enable_if_t<is_option_v<Option>, int> = 0>
// [[nodiscard]] constexpr bool
// operator<(const Option& lhs, const Option& rhs) noexcept {
//   return lhs.option_number() < rhs.option_number();
// }

// template<typename Option, 
//          typename std::enable_if_t<is_option_v<Option>, int> = 0>
// [[nodiscard]] constexpr bool
// operator==(const Option& op, number num) noexcept {
//   return op.option_number() == num;
// }

// template<typename Option, 
//          typename std::enable_if_t<is_option_v<Option>, int> = 0>
// [[nodiscard]] constexpr bool
// operator<(const Option& op, number num) noexcept {
//   return op.option_number() < num;
// }

}  // namespace options 
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_OPTION_FUNC_HPP_
