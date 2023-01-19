/**
 * @file buffer.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_MESSAGE_DETAIL_BUFFER_HPP_
#define COAP_TE_MESSAGE_DETAIL_BUFFER_HPP_

namespace coap_te {
namespace message {
namespace detail {

template<typename T>
static constexpr bool
is_mutable_buffer_or_range_v =
                is_mutable_buffer_range_v<T> ||
                std::is_same_v<mutable_buffer,
                  coap_te::core::remove_cvref_t<T>>;

template<typename T>
[[nodiscard]] constexpr auto
get_mutable_buffer(const T& buffer) noexcept {
  static_assert(is_mutable_buffer_or_range_v<T> ||
                is_mutable_buffer_sequence_v<T>);
  if constexpr (is_mutable_buffer_or_range_v<T>) {
    return buffer;
  } else {
    return buffer_range{buffer};
  }
}

template<typename T>
static constexpr bool
is_const_buffer_or_range_v =
                is_const_buffer_range_v<T> ||
                std::is_same_v<const_buffer,
                  coap_te::core::remove_cvref_t<T>>;

template<typename T>
static constexpr bool
is_const_buffer_sequence_or_range_v =
                is_const_buffer_range_v<T> ||
                is_const_buffer_sequence_v<T>;

}   // namespace detail
}   // namespace message
}   // namespace coap_te

#endif  // COAP_TE_MESSAGE_DETAIL_BUFFER_HPP_
