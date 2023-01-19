/**
 * @file parse.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_IMPL_PARSE_IPP_
#define COAP_TE_MESSAGE_IMPL_PARSE_IPP_

#include <cstdint>

#include "coap-te/core/error.hpp"
#include "coap-te/core/traits.hpp"
#include "coap-te/buffer/buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/traits.hpp"
#include "coap-te/message/options/parse.hpp"
#include "coap-te/message/options/parse_options.hpp"

namespace coap_te {
namespace message {

template<typename Message>
constexpr std::size_t
parse_header(const const_buffer& data,
             Message& message,                               // NOLINT
             coap_te::error_code& ec) noexcept {             // NOLINT
  static_assert(is_message_v<Message>,
                "CoAPMessage requirements not met");
  const_buffer input(data);

  if (input.size() < minimum_header_size) {
    ec = coap_te::errc::no_buffer_space;
    return 0;
  }

  std::uint8_t header[minimum_header_size];   // NOLINT
  buffer_copy(coap_te::buffer(header), input);

  // byte 0
  std::uint8_t byte0 = header[0];
  if (byte0 >> 6 != version) {
    ec = coap_te::errc::invalid_version;
    return 0;
  }

  std::size_t token_len = byte0 & 0xF;
  if (token_len > max_token_size) {
    ec = coap_te::errc::token_length;
    return 0;
  }

  if (input.size() < minimum_header_size + token_len) {
    ec = coap_te::errc::no_buffer_space;
    return 0;
  }

  message.set(static_cast<type>((byte0 >> 4) & 0b11));
  std::size_t size = 1;

  // byte 1;
  message.set(static_cast<code>(header[1]));
  size += 1;

  // byte 2-3
  std::uint16_t mid = static_cast<std::uint16_t>(header[2] << 8) | header[3];
  message.mid(mid);
  size += 2;
  input += 4;

  // byte 4-4+token_len
  message.token({input.data(), token_len});
  size += token_len;

  return size;
}

template<typename Message>
constexpr std::size_t
parse(const const_buffer& data,
      Message& message,                     // NOLINT
      coap_te::error_code& ec) noexcept {   // NOLINT
  static_assert(is_message_v<Message>,
                "Must be of type message");
  const_buffer input(data);

  auto size = parse_header(input, message, ec);
  if (ec) {
    return size;
  }
  input += size;

  auto size_opt = parse_options(input, message.option_list(), ec);
  if (ec) {
    return size;
  }
  input += size_opt;
  size += size_opt;

  if (input.size() > 0) {
    // option_list_size garanties that this is a payload marker
    input += 1;
    size += 1;
  }
  message.payload(input);

  return size + input.size();
}

}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_IMPL_PARSE_IPP_
