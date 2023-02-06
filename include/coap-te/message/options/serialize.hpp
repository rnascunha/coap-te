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

#include <type_traits>

#include "coap-te/core/error.hpp"
#include "coap-te/message/options/checks.hpp"
#include "coap-te/buffer/buffer.hpp"

namespace coap_te {
namespace message {
namespace options {

template<typename CheckOptions = check_all,
         typename ConstBufferSequence,
         typename MutableBufferOrRange>
[[nodiscard]] constexpr
std::enable_if_t<
  coap_te::is_const_buffer_sequence_v<ConstBufferSequence>, std::size_t>
serialize(number_type before,
          number_type op,
          const ConstBufferSequence& input,
          const MutableBufferOrRange& output,
          coap_te::error_code& ec) noexcept;    // NOLINT

template<typename CheckOptions = check_all,
         typename MutableBufferSequence,
         typename UnsignedType>
[[nodiscard]] constexpr
std::enable_if_t<std::is_unsigned_v<UnsignedType>, std::size_t>
serialize(number_type before,
          number_type op,
          UnsignedType input,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept;    // NOLINT

template<typename CheckOptions = check_all,
         typename MutableBufferSequence>
[[nodiscard]] constexpr std::size_t
serialize(number_type before,
          number_type op,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept;    // NOLINT

template<typename CheckOptions = check_repeat,
         typename Option,
         typename MutableBufferSequence>
[[nodiscard]] constexpr std::size_t
serialize(number before,
          const Option& option,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept;    // NOLINT

/**
 * Serialize list
 */
template<typename CheckOptions = check_all,
         typename ForwardIt,
         typename MutableBufferSequence>
[[nodiscard]] constexpr std::size_t
serialize(ForwardIt begin,
          ForwardIt end,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept;    // NOLINT

template<typename CheckOptions = check_all,
         typename Container,
         typename MutableBufferSequence>
[[nodiscard]] constexpr std::size_t
serialize(const Container& option_list,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept;      // NOLINT

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/serialize.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_SERIALIZE_HPP_
