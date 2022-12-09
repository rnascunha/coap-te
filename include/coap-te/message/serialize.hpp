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

#include <system_error>     // NOLINT

#include "coap-te/core/const_buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/options/checks.hpp"

namespace coap_te {
namespace message {

template<typename ConstBuffer,
         typename MutableBuffer>
std::size_t
serialize_header(type, code, message_id,
                const ConstBuffer& token,
                MutableBuffer&,         //NOLINT
                std::error_code&) noexcept;

template<typename ConstBuffer,
         typename MutableBuffer>
std::size_t
serialize_header(type, code, message_id,
                const ConstBuffer& token,
                MutableBuffer&);         //NOLINT

template<typename CheckOptions = coap_te::message::options::check_none,
         typename ConstBufferToken,
         typename ConstBufferPayload,
         typename MutableBuffer,
         typename OptionList>
std::size_t
serialize(type, code, message_id,
          const ConstBufferToken& token,
          const OptionList& opt_list,
          const ConstBufferPayload& payload,
          MutableBuffer&,         //NOLINT
          std::error_code&) noexcept;

template<typename CheckOptions = coap_te::message::options::check_none,
         typename ConstBufferToken,
         typename ConstBufferPayload,
         typename MutableBuffer,
         typename OptionList>
std::size_t
serialize(type, code, message_id,
          const ConstBufferToken& token,
          const OptionList& opt_list,
          const ConstBufferPayload& payload,
          MutableBuffer&);         //NOLINT


}  // namespace message
}  // namespace coap_te

#include "impl/serialize.ipp"

#endif  // COAP_TE_MESSAGE_SERIALIZE_HPP_
