/**
 * @file serialize.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_IMPL_SERIALIZE_IPP_
#define COAP_TE_MESSAGE_IMPL_SERIALIZE_IPP_

#include <system_error>     // NOLINT

#include "coap-te/core/traits.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/token.hpp"

namespace coap_te {
namespace message {

template<typename ConstBuffer,
         typename MutableBuffer>
std::size_t
serialize_header(type tp, code co, message_id mid,
                const ConstBuffer& token,
                MutableBuffer& output,         //NOLINT
                std::error_code& ec) noexcept {
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBuffer>, "Must be const buffer type");
  static_assert(coap_te::core::is_mutable_buffer_type_v<MutableBuffer>, "Must be mutable buffer type");

  std::uint8_t token_len = static_cast<std::uint8_t>(clamp_token_size(token.size()));
  std::size_t size = minimum_header_size + token_len;
  if (output.size() < size) {
    ec = std::make_error_code(std::errc::no_buffer_space);
    return 0;
  }

  output[0] = (version << 6) | (static_cast<std::uint8_t>(tp) << 4) | token_len;
  output += 1;
  output[0] = static_cast<std::uint8_t>(co);
  output += 1;
  mid = (mid >> 8) | ((mid << 8) & 0xFF00);     // Inverting, network byte order
  std::memcpy(output.data(), &mid, sizeof(message_id));
  output += sizeof(message_id);
  std::memcpy(output.data(), token.data(), token_len);
  output += token_len;

  return size;
}

template<typename ConstBuffer,
         typename MutableBuffer>
std::size_t
serialize_header(type tp, code co, message_id mid,
                const ConstBuffer& token,
                MutableBuffer& output) {         //NOLINT
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBuffer>, "Must be const buffer type");
  static_assert(coap_te::core::is_mutable_buffer_type_v<MutableBuffer>, "Must be mutable buffer type");

  std::error_code ec;
  auto size = serialize_header(tp, co, mid, token, output, ec);
  if (ec) {
    throw std::system_error{ec};
  }
  return size;
}

template<typename CheckOptions /* = coap_te::message::options::check_none */,
         typename ConstBufferToken,
         typename ConstBufferPayload,
         typename MutableBuffer,
         typename OptionList>
std::size_t
serialize(type tp, code co, message_id mid,
          const ConstBufferToken& token,
          const OptionList& opt_list,
          const ConstBufferPayload& payload,
          MutableBuffer& output,         //NOLINT
          std::error_code& ec) noexcept {
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBufferToken>, "Must be const buffer type");
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBufferPayload>, "Must be const buffer type");
  static_assert(coap_te::core::is_mutable_buffer_type_v<MutableBuffer>, "Must be mutable buffer type");
  // static_assert(coap_te::core::is_option_list_type_v<OptionList>, "Must be option list type");

  auto size = serialize_header(tp, co, mid, token, output, ec);
  if (ec) {
    return size;
  }

  options::number prev = options::number::invalid;
  for (const options::option& opt : opt_list) {
    size += opt.serialize<CheckOptions>(prev, output, ec);
    if (ec) {
      return size;
    }
    prev = opt.option_number();
  }

  if (payload.size() == 0)
    return size;

  if (output.size() < 1 + payload.size()) {
    ec = std::make_error_code(std::errc::no_buffer_space);
    return size;
  }
  size += 1 + payload.size();
  output[0] = payload_marker;
  output += 1;
  std::memcpy(output.data(), payload.data(), payload.size());

  return size;
}

template<typename CheckOptions /* = coap_te::message::options::check_none */,
         typename ConstBufferToken,
         typename ConstBufferPayload,
         typename MutableBuffer,
         typename OptionList>
std::size_t
serialize(type tp, code co, message_id mid,
          const ConstBufferToken& token,
          const OptionList& opt_list,
          const ConstBufferPayload& payload,
          MutableBuffer& output) {
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBufferToken>, "Must be const buffer type");
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBufferPayload>, "Must be const buffer type");
  static_assert(coap_te::core::is_mutable_buffer_type_v<MutableBuffer>, "Must be mutable buffer type");
  // static_assert(coap_te::core::is_option_list_type_v<OptionList>, "Must be option list type");

  std::error_code ec;
  auto size = serialize<CheckOptions>(tp, co, mid, token, opt_list, payload, output, ec);
  if (ec) {
    throw std::system_error{ec};
  }
  return size;
}

}  // message 
}  // coap_te

#endif  // COAP_TE_MESSAGE_IMPL_SERIALIZE_IPP_