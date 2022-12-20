/**
 * @file to_string.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_DEBUG_TO_STRING_HPP_
#define COAP_TE_DEBUG_TO_STRING_HPP_

#include <string>
#include <string_view>

#include "coap-te/core/traits.hpp"

namespace coap_te {
namespace debug {

template<typename ConstBuffer>
std::string_view to_string(const ConstBuffer& buffer) noexcept {
  static_assert(coap_te::core::is_const_buffer_v<ConstBuffer>,
                "Must be const buffer type");

  return std::string_view{static_cast<const char*>(buffer.data()),
                          buffer.size()};
}

template<typename ConstBuffer>
std::string to_hex(const ConstBuffer& buffer,
                   const std::string_view& separator = " ") noexcept {
  static_assert(coap_te::core::is_const_buffer_v<ConstBuffer>,
                "Must be const buffer type");

  if (buffer.size() == 0)
    return "";

  static constexpr const char* hex = "0123456789ABCDEF";
  std::string s;
  s.reserve(buffer.size() * 2 + buffer.size() - 1);

  auto* b = reinterpret_cast<const std::uint8_t*>(buffer.data());
  for (auto i = buffer.size() - 1; i != 0; --i, ++b) {
    s.push_back(hex[(*b) >> 4]);
    s.push_back(hex[(*b) & 0x0F]);
    s.append(separator);
  }
  s.push_back(hex[(*b) >> 4]);
  s.push_back(hex[(*b) & 0x0F]);

  return s;
}

}  // namespace debug
}  // namespace coap_te

#endif  // COAP_TE_DEBUG_TO_STRING_HPP_
