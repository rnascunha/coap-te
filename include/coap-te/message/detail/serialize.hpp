/**
 * @file serialize.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_MESSAGE_DETAIL_SERIALIZE_HPP_
#define COAP_TE_MESSAGE_DETAIL_SERIALIZE_HPP_

#include <type_traits>

#include "coap-te/core/traits.hpp"
#include "coap-te/core/error.hpp"
#include "coap-te/buffer/buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/detail/buffer.hpp"
#include "coap-te/message/options/checks.hpp"
#include "coap-te/message/options/serialize.hpp"

namespace coap_te {
namespace message {
namespace detail {

[[nodiscard]] constexpr std::uint8_t
header_byte_0(type tp, std::uint8_t token_len) noexcept {
  return (version << 6) | (static_cast<std::uint8_t>(tp) << 4) | token_len;
}

[[nodiscard]] constexpr std::uint8_t
header_byte_1(code co) noexcept {
  return static_cast<std::uint8_t>(co);
}

[[nodiscard]] constexpr std::uint8_t
header_byte_3(message_id mid) noexcept {
  return (mid >> 8);
}

[[nodiscard]] constexpr std::uint8_t
header_byte_4(message_id mid) noexcept {
  return mid & 0xFF;
}

template<typename MutableBufferOrRange,
         typename ConstBufferSequence>
constexpr std::size_t
serialize_header(type tp, code co, message_id mid,
                const ConstBufferSequence& token,
                const MutableBufferOrRange& output,
                coap_te::error_code& ec) noexcept { // NOLINT
  static_assert(is_const_buffer_sequence_v<ConstBufferSequence>,
                "ConstBufferSequence requirements not met");
  static_assert(is_mutable_buffer_or_range_v<MutableBufferOrRange>,
                "MutableBufferOrRange requirements not met");

  std::uint8_t token_len =
                static_cast<std::uint8_t>(
                  clamp_token_size(buffer_size(token)));
  std::size_t size = minimum_header_size + token_len;
  if (output.size() < size) {
    ec = coap_te::errc::no_buffer_space;
    return 0;
  }

  const std::uint8_t header[minimum_header_size]{
    header_byte_0(tp, token_len),
    header_byte_1(co),
    header_byte_3(mid),
    header_byte_4(mid)
  };

  buffer_copy(output, coap_te::buffer(header));
  if (token_len > 0) {
    buffer_copy(output + 4, token, token_len);
  }

  return size;
}

template<typename MutableBufferOrRange,
         typename ConstBufferSequence>
constexpr std::size_t
serialize_payload(const ConstBufferSequence& payload,
                  const MutableBufferOrRange& output ,
                  coap_te::error_code& ec) noexcept {    // NOLINT
  static_assert(is_const_buffer_sequence_v<ConstBufferSequence>,
                "ConstBufferSequence requirements not met");
  static_assert(is_mutable_buffer_or_range_v<MutableBufferOrRange>,
                "MutableBufferOrRange requirements not met");

  auto size = buffer_size(payload);
  if (size == 0) {
    return 0;
  }

  if (output.size() < 1 + size) {
    ec = coap_te::errc::no_buffer_space;
    return 0;
  }
  buffer_copy(output, buffer(&payload_marker, 1));
  buffer_copy(output + 1, payload);

  return size + 1;
}

template<typename CheckOptions,
         typename ConstBufferSequenceToken,
         typename ConstBufferSequencePayload,
         typename MutableBufferOrRange,
         typename OptionList>
constexpr std::size_t
serialize(type tp, code co, message_id mid,
          const ConstBufferSequenceToken& token,
          const OptionList& opt_list,
          const ConstBufferSequencePayload& payload,
          const MutableBufferOrRange& output,
          coap_te::error_code& ec) noexcept { // NOLINT
  static_assert(coap_te::is_const_buffer_sequence_v<ConstBufferSequenceToken>,
                "ConstBufferSequence requirements not met");
  static_assert(coap_te::is_const_buffer_sequence_v<ConstBufferSequencePayload>,
                "ConstBufferSequence requirements not met");
  static_assert(options::is_option_list_v<OptionList>,
                "CoAPOptionList requirements not met");
  static_assert(is_mutable_buffer_or_range_v<MutableBufferOrRange>,
                "MutableBufferOrRange requirements not met");

  auto size = coap_te::message::detail::serialize_header(
                  tp, co, mid, token, output, ec);
  if (ec) {
    return size;
  }

  size += options::serialize<CheckOptions>(opt_list, output + size, ec);
  if (ec) {
    return size;
  }

  return size + serialize_payload(payload, output + size, ec);
}

}  // namespace detail
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_DETAIL_SERIALIZE_HPP_
