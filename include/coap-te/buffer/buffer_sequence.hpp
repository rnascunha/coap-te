/**
 * @file buffer_sequence.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_BUFFER_BUFFER_SEQUENCE_HPP_
#define COAP_TE_BUFFER_BUFFER_SEQUENCE_HPP_

#include <type_traits>

namespace coap_te {

class mutable_buffer;
class const_buffer;

/// Checking if argument is a buffer (not a iterator)
template <typename MutableBuffer>
[[nodiscard]] constexpr const mutable_buffer*
buffer_sequence_begin(const MutableBuffer& b,
    std::enable_if_t<
      std::is_convertible_v<const MutableBuffer*, const mutable_buffer*>,
      int> = 0) noexcept {
  return static_cast<const mutable_buffer*>(std::addressof(b));
}

template <typename ConstBuffer>
[[nodiscard]] constexpr const const_buffer*
buffer_sequence_begin(const ConstBuffer& b,
    std::enable_if_t<
      std::is_convertible_v<const ConstBuffer*, const const_buffer*>,
    int> = 0) noexcept {
  return static_cast<const const_buffer*>(std::addressof(b));
}

/// Checking if argument is a iterator (not a buffer)
template <typename C>
[[nodiscard]] constexpr auto
buffer_sequence_begin(C& c,           // NOLINT
    std::enable_if_t<
      !std::is_convertible_v<const C*, const mutable_buffer*>
        && !std::is_convertible_v<const C*, const const_buffer*>,
    int> = 0) noexcept -> decltype(std::begin(c)) {
  return std::begin(c);
}

template <typename C>
[[nodiscard]] constexpr auto
buffer_sequence_begin(const C& c,
    std::enable_if_t<
      !std::is_convertible_v<const C*, const mutable_buffer*>
        && !std::is_convertible_v<const C*, const const_buffer*>,
    int> = 0) noexcept -> decltype(std::begin(c)) {
  return std::begin(c);
}

/**
 * End
 */
/// Checking if argument is a buffer (not a iterator)
template <typename MutableBuffer>
[[nodiscard]] constexpr const mutable_buffer*
buffer_sequence_end(const MutableBuffer& b,
    std::enable_if_t<
      std::is_convertible_v<const MutableBuffer*, const mutable_buffer*>,
    int> = 0) noexcept {
  return static_cast<const mutable_buffer*>(std::addressof(b)) + 1;
}

template <typename ConstBuffer>
[[nodiscard]] constexpr const const_buffer*
buffer_sequence_end(const ConstBuffer& b,
    std::enable_if_t<
      std::is_convertible_v<const ConstBuffer*, const const_buffer*>,
    int> = 0) noexcept {
  return static_cast<const const_buffer*>(std::addressof(b)) + 1;
}

/// Checking if argument is a iterator (not a buffer)
template <typename C>
[[nodiscard]] constexpr auto
buffer_sequence_end(C& c,     // NOLINT
    std::enable_if_t<
      !std::is_convertible_v<const C*, const mutable_buffer*>
        && !std::is_convertible_v<const C*, const const_buffer*>,
    int> = 0) noexcept -> decltype(std::end(c)) {
  return std::end(c);
}

template <typename C>
[[nodiscard]] constexpr auto
buffer_sequence_end(const C& c,
    std::enable_if_t<
      !std::is_convertible_v<const C*, const mutable_buffer*>
        && !std::is_convertible_v<const C*, const const_buffer*>,
    int> = 0) noexcept -> decltype(std::end(c)) {
  return std::end(c);
}

}  // namespace coap_te

#include "coap-te/buffer/buffer_size.hpp"
#include "coap-te/buffer/buffer_copy.hpp"
#include "coap-te/buffer/buffer_compare.hpp"

#endif  // COAP_TE_BUFFER_BUFFER_SEQUENCE_HPP_
