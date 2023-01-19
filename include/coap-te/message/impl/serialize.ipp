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

#include <cstdint>
#include <type_traits>

#include "coap-te/core/error.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/traits.hpp"
#include "coap-te/message/detail/serialize.hpp"

namespace coap_te {
namespace message {

template<typename MutableBufferSequence,
         typename ConstBufferSequence>
constexpr std::size_t
serialize_header(type tp, code co, message_id mid,
                const ConstBufferSequence& token,
                const MutableBufferSequence& output,
                coap_te::error_code& ec) noexcept {    // NOLINT
  return detail::serialize_header(tp, co, mid, token,
                                  detail::get_mutable_buffer(output),
                                  ec);
}

template<typename MutableBufferSequence,
         typename ConstBufferSequence>
constexpr std::size_t
serialize_payload(const ConstBufferSequence& payload,
                  const MutableBufferSequence& output,
                  coap_te::error_code& ec) noexcept {    // NOLINT
  return detail::serialize_payload(payload,
                                   detail::get_mutable_buffer(output),
                                   ec);
}

template<typename CheckOptions /* = coap_te::message::options::check_none */,
         typename ConstBufferSequenceToken,
         typename ConstBufferSequencePayload,
         typename MutableBufferSequence,
         typename OptionList>
constexpr std::size_t
serialize(type tp, code co, message_id mid,
          const ConstBufferSequenceToken& token,
          const OptionList& opt_list,
          const ConstBufferSequencePayload& payload,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept {   // NOLINT
  return detail::serialize<CheckOptions>(tp, co, mid, token,
                           opt_list, payload,
                           detail::get_mutable_buffer(output),
                           ec);
}

template<typename CheckOptions,
         typename Message,
         typename MutableBufferSequence>
constexpr std::size_t
serialize(const Message& msg,
          message_id mid,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept {   // NOLINT
  static_assert(is_message_v<Message>,
                "CoAPMessage requirements not met");
  return detail::serialize<CheckOptions>(msg.get_type(), msg.get_code(), mid,
                           msg.token(), msg.option_list(),
                           msg.payload(),
                           detail::get_mutable_buffer(output),
                           ec);
}

#if COAP_TE_ENABLE_EXCEPTIONS == 1

template<typename ConstBufferSequence,
         typename MutableBufferSequence>
constexpr std::size_t
serialize_header(type tp, code co, message_id mid,
                const ConstBufferSequence& token,
                const MutableBufferSequence& output) {
  coap_te::error_code ec;
  auto size = serialize_header(tp, co, mid, token,
                               detail::get_mutable_buffer(output),
                               ec);
  coap_te::throw_if_error(ec);
  return size;
}

template<typename MutableBufferSequence,
         typename ConstBufferSequence>
constexpr std::size_t
serialize_payload(const ConstBufferSequence& payload,
                  const MutableBufferSequence& output) {
  coap_te::error_code ec;
  auto size = serialize_payload(payload,
                                detail::get_mutable_buffer(output),
                                ec);
  coap_te::throw_if_error(ec);
  return size;
}

template<typename CheckOptions /* = coap_te::message::options::check_none */,
         typename ConstBufferSequenceToken,
         typename ConstBufferSequencePayload,
         typename MutableBufferSequence,
         typename OptionList>
constexpr std::size_t
serialize(type tp, code co, message_id mid,
          const ConstBufferSequenceToken& token,
          const OptionList& opt_list,
          const ConstBufferSequencePayload& payload,
          const MutableBufferSequence& output) {
  coap_te::error_code ec;
  auto size = serialize<CheckOptions>(tp, co, mid,
                                      token, opt_list,
                                      payload,
                                      detail::get_mutable_buffer(output),
                                      ec);
  coap_te::throw_if_error(ec);
  return size;
}

template<typename CheckOptions,
         typename Message,
         typename MutableBufferSequence>
constexpr std::size_t
serialize(const Message& msg,
          message_id mid,
          const MutableBufferSequence& output) {
  return coap_te::message::serialize<CheckOptions>(
                  msg.get_type(), msg.get_code(),
                  mid, msg.token(),
                  msg.option_list(), msg.payload(),
                  detail::get_mutable_buffer(output));
}

#endif  // COAP_TE_ENABLE_EXCEPTIONS == 1

}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_IMPL_SERIALIZE_IPP_
