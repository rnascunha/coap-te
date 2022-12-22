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

namespace coap_te {
namespace message {
namespace options {

template<typename CheckOptions = check_all,
         typename ConstBuffer,
         typename MutableBuffer>
[[nodiscard]] constexpr
std::enable_if_t<
  coap_te::core::is_const_buffer_v<ConstBuffer>, std::size_t>
serialize(number_type before,
          number_type op,
          const ConstBuffer& input,
          MutableBuffer& output,              // NOLINT
          coap_te::error_code& ec) noexcept;      // NOLINT

template<typename CheckOptions = check_all,
         typename MutableBuffer,
         typename UnsignedType>
[[nodiscard]] constexpr
std::enable_if_t<std::is_unsigned_v<UnsignedType>, std::size_t>
serialize(number_type before,
          number_type op,
          UnsignedType input,
          MutableBuffer& output,              // NOLINT
          coap_te::error_code& ec) noexcept;      // NOLINT

template<typename CheckOptions = check_all,
         typename MutableBuffer>
[[nodiscard]] constexpr std::size_t
serialize(number_type before,
          number_type op,
          MutableBuffer& output,              // NOLINT
          coap_te::error_code& ec) noexcept;      // NOLINT

/**
 * Serialize list
 */
template<typename CheckOptions = check_all,
         typename ForwardIt,
         typename MutableBuffer>
[[nodiscard]] constexpr std::size_t
serialize(ForwardIt begin,
          ForwardIt end,
          MutableBuffer& output,              // NOLINT
          coap_te::error_code& ec) noexcept;      // NOLINT

template<typename CheckOptions = check_all,
         typename Container,
         typename MutableBuffer>
[[nodiscard]] constexpr std::size_t
serialize(const Container& options_list,
          MutableBuffer& output,              // NOLINT
          coap_te::error_code& ec) noexcept;      // NOLINT

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/serialize.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_SERIALIZE_HPP_
