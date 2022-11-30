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
#include <system_error>   //NOLINT

namespace coap_te {
namespace message {
namespace options {

template<typename ConstBuffer,
         typename MutableBuffer>
[[nodiscard]] constexpr
std::enable_if_t<
  coap_te::core::is_mutable_buffer_type_v<MutableBuffer>, std::size_t>
serialize(number_type before,
          number_type op,
          const ConstBuffer& input,
          MutableBuffer& output,              // NOLINT
          std::error_code& ec) noexcept;      // NOLINT

template<typename MutableBuffer, typename UnsignedType>
[[nodiscard]] constexpr
std::enable_if_t<std::is_unsigned_v<UnsignedType>, std::size_t>
serialize(number_type before,
          number_type op,
          UnsignedType input,
          MutableBuffer& output,              // NOLINT
          std::error_code& ec) noexcept;      // NOLINT

template<typename MutableBuffer>
[[nodiscard]] constexpr std::size_t
serialize(number_type before,
          number_type op,
          MutableBuffer& output,              // NOLINT
          std::error_code& ec) noexcept;      // NOLINT

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/serialize.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_SERIALIZE_HPP_
