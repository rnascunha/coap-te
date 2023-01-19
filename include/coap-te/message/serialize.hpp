/**
 * @file serialize.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_SERIALIZE_HPP_
#define COAP_TE_MESSAGE_SERIALIZE_HPP_

#include "coap-te/core/error.hpp"
#include "coap-te/buffer/buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/options/checks.hpp"

namespace coap_te {
namespace message {

template<typename MutableBufferSequence,
         typename ConstBufferSequence>
constexpr std::size_t
serialize_header(type tp, code co, message_id mid,
                const ConstBufferSequence& token,
                const MutableBufferSequence& output,
                coap_te::error_code& ec) noexcept;    // NOLINT

template<typename MutableBufferSequence,
         typename ConstBufferSequence>
constexpr std::size_t
serialize_payload(const ConstBufferSequence& payload,
                  const MutableBufferSequence& output,
                  coap_te::error_code& ec) noexcept;    // NOLINT

template<typename CheckOptions = coap_te::message::options::check_none,
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
          coap_te::error_code& ec) noexcept;    // NOLINT

template<typename CheckOptions = coap_te::message::options::check_none,
         typename Message,
         typename MutableBufferSequence>
constexpr std::size_t
serialize(const Message& msg,
          message_id mid,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept;    // NOLINT

#if COAP_TE_ENABLE_EXCEPTIONS == 1

template<typename ConstBufferSequence,
         typename MutableBufferSequence>
constexpr std::size_t
serialize_header(type tp, code co, message_id mid,
                const ConstBufferSequence& token,
                const MutableBufferSequence& output);

template<typename MutableBufferSequence,
         typename ConstBufferSequence>
constexpr std::size_t
serialize_payload(const ConstBufferSequence& payload,
                  const MutableBufferSequence& output);

template<typename CheckOptions = coap_te::message::options::check_none,
         typename ConstBufferSequenceToken,
         typename ConstBufferSequencePayload,
         typename MutableBufferSequence,
         typename OptionList>
constexpr std::size_t
serialize(type tp, code co, message_id mid,
          const ConstBufferSequenceToken& token,
          const OptionList& opt_list,
          const ConstBufferSequencePayload& payload,
          const MutableBufferSequence& output);

template<typename CheckOptions,
         typename Message,
         typename MutableBufferSequence>
constexpr std::size_t
serialize(const Message& msg,
          message_id mid,
          const MutableBufferSequence& output);

#endif  // COAP_TE_ENABLE_EXCEPTIONS == 1

}  // namespace message
}  // namespace coap_te

#include "impl/serialize.ipp"

#endif  // COAP_TE_MESSAGE_SERIALIZE_HPP_
