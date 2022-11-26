/**
 * @file serialize.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_SERIALIZE_HPP_
#define COAP_TE_MESSAGE_OPTIONS_SERIALIZE_HPP_

#include <system_error>   //NOLINT
#include <string_view>
#include <utility>
#include <cstring>        // std::memcpy
#include <cstdint>

#include "coap-te/message/options/config.hpp"
#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/utility.hpp"

namespace coap_te {
namespace message {
namespace options {

struct header {
  std::uint16_t data_extend = 0;
  std::uint8_t  byte_op = static_cast<std::uint8_t>(extend::no_extend);
  std::uint16_t size = 0;
};

constexpr void
serialize_option_header(std::size_t size,
                       header& hdr) noexcept {  //NOLINT
  hdr.byte_op = static_cast<std::uint8_t>(extend::no_extend);
  hdr.data_extend = 0;
  hdr.size = 0;

  if (size < 13) {
    hdr.byte_op = static_cast<std::uint8_t>(size);
  } else if (size < 269) {
    hdr.byte_op = static_cast<std::uint8_t>(extend::one_byte);
    hdr.data_extend = static_cast<std::uint16_t>(size - 13);
    size = 1;
  } else {
    hdr.byte_op = static_cast<std::uint8_t>(extend::two_bytes);
    hdr.data_extend = static_cast<std::uint16_t>(size - 269);
    size = 2;
  }
}

template<typename ConstBuffer,
         typename MutableBuffer>
constexpr
std::size_t serialize(number before,
                      number op,
                      const ConstBuffer& input,
                      MutableBuffer& output,              // NOLINT
                      std::error_code& ec) noexcept {     // NOLINT
  header delta{};
  serialize_option_header(static_cast<std::uint16_t>(op) -
                          static_cast<std::uint16_t>(before),
                          delta);

  header length{};
  serialize_option_header(input.size(), length);

  std::size_t size = 1 + delta.size + length.size + input.size();
  if (size > output.size) {
    ec = std::make_error_code(std::errc::no_buffer_space);
    return 0;
  }

  output[0] = (delta.byte_op << 4) | length.byte_op;
  output += 1;

  for (const header& h : {delta, length}) {
    switch (static_cast<extend>(h.byte_op)) {
      case extend::one_byte:
        output[0] = static_cast<std::uint8_t>(h.data_extend);
        break;
      case extend::two_bytes:
        coap_te::core::to_big_endian(output.data(), h.data_extend);
        break;
      default:
        break;
    }
    output += h.size;
  }

  std::memcpy(output.data(), input.data(), input.size());
  output += input.size();

  return size;
}

template<typename MutableBuffer>
constexpr
std::size_t serialize(number before,
                      number op,
                      unsigned input,
                      MutableBuffer& output,              // NOLINT
                      std::error_code& ec) noexcept {     // NOLINT
  auto size = ::coap_te::core::make_short_unsigned(input);
  return serialize(before, op,
                  ::coap_te::const_buffer{&input, size},
                  output,
                  ec);
}

template<typename MutableBuffer>
constexpr
std::size_t serialize(number before,
                      number op,
                      MutableBuffer& output,              // NOLINT
                      std::error_code& ec) noexcept {     // NOLINT
  return serialize(before, op, ::coap_te::const_buffer{}, output, ec);
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_SERIALIZE_HPP_
