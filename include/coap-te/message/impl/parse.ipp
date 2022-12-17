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
#include <system_error>

#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/options/parse.hpp"
// #include "coap-te/message/options/vector_options.hpp"
#include "coap-te/message/options/parse_options.hpp"

namespace coap_te {
namespace message {

template<typename ConstBuffer,
         typename Message>
std::size_t parse_header(ConstBuffer& input,
                  Message& message,
                  std::error_code& ec) noexcept {
  if (input.size() < minimum_header_size) {
    ec = std::make_error_code(std::errc::no_buffer_space);
    return 0;
  }
  
  // byte 0
  std::uint8_t byte0 = input[0];
  if (byte0 >> 6 != version) {
    ec = std::make_error_code(std::errc::protocol_not_supported);
    return 0;
  }

  std::size_t token_len = byte0 & 0xF;
  if (token_len > max_token_size) {
    ec = std::make_error_code(std::errc::protocol_error);
    return 0;
  }

  if (input.size() < minimum_header_size + token_len) {
    ec = std::make_error_code(std::errc::no_buffer_space);
    return 0;
  }

  message.set(static_cast<type>((byte0 >> 4) & 0b11));
  std::size_t size = 1;
  input += 1;

  // byte 1;
  message.set(static_cast<code>(input[0]));
  size += 1;
  input += 1;

  // byte 2-3
  std::uint16_t mid = static_cast<std::uint16_t>(input[0] << 8) | input[1];
  message.mid(mid);
  size += 2;
  input += 2;

  // byte 4-4+token_len
  message.token({input.data(), token_len});
  size += token_len;
  input += token_len;

  return size;
}

template<typename ConstBuffer,
         typename Message>
std::size_t parse(ConstBuffer& input,
                  Message& message,
                  std::error_code& ec) noexcept {
  auto size = parse_header(input, message, ec);
  if (ec) {
    return size;
  }

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
  size += input.size();
  input += input.size();

  return size;
}

}  // namespace message 
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_IMPL_PARSE_IPP_