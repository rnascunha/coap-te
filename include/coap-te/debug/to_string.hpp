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
#include <limits>

#include "coap-te/core/traits.hpp"
#include "coap-te/buffer/buffer.hpp"
#include "coap-te/buffer/buffers_iterator.hpp"

namespace coap_te {
namespace debug {

template<typename ConstBuffer>
std::string_view to_string(const ConstBuffer& buffer) noexcept {
  static_assert(coap_te::is_const_buffer_v<ConstBuffer>,
                "Must be const buffer type");

  return std::string_view{static_cast<const char*>(buffer.data()),
                          buffer.size()};
}

template<typename ConstBufferSequence>
std::string to_hex(const ConstBufferSequence& buffer,
                   const std::string_view& separator = " ",
                   std::size_t max_size
                    = std::numeric_limits<std::size_t>::max()) noexcept {
  static_assert(coap_te::is_const_buffer_sequence_v<ConstBufferSequence> ||
                coap_te::is_buffer_range_v<ConstBufferSequence>,
                "Must be const buffer sequence type");
  auto size = (std::min)(coap_te::buffer_size(buffer), max_size);
  if (size == 0)
    return "";

  static constexpr const char* hex = "0123456789ABCDEF";
  std::string s;
  s.reserve(size * 2 + size - 1);

  auto b = coap_te::buffers_begin(buffer);
  for (auto i = size - 1; i != 0; --i, ++b) {
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
