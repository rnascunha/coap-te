/**
 * @file buffer.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_BUFFER_BUFFER_HPP_
#define COAP_TE_BUFFER_BUFFER_HPP_

#include <algorithm>    // std::min
#include <type_traits>
#include <limits>
#include <cstring>

#include "coap-te/core/traits.hpp"
#include "coap-te/buffer/mutable_buffer.hpp"
#include "coap-te/buffer/const_buffer.hpp"

namespace coap_te {

/// Copy
[[nodiscard]] constexpr mutable_buffer
buffer(const mutable_buffer& b) noexcept {
  return mutable_buffer(b);
}

[[nodiscard]] constexpr const_buffer
buffer(const const_buffer& b) noexcept {
  return const_buffer{b};
}

/// Copy and clamp
[[nodiscard]] constexpr mutable_buffer
buffer(const mutable_buffer& b, std::size_t max_size) noexcept {
  return mutable_buffer{b.data(), (std::min)(b.size(), max_size)};
}

[[nodiscard]] constexpr const_buffer
buffer(const const_buffer& b, std::size_t max_size) noexcept {
  return const_buffer{b.data(), (std::min)(b.size(), max_size)};
}

/// Contructor
[[nodiscard]] constexpr mutable_buffer
buffer(void* data, std::size_t size) noexcept {
  return mutable_buffer{data, size};
}

[[nodiscard]] constexpr const_buffer
buffer(const void* data, std::size_t size) noexcept {
  return const_buffer{data, size};
}

/// From array
template<typename T, std::size_t N>
[[nodiscard]] constexpr mutable_buffer
buffer(T (&data)[N]) noexcept {
  return mutable_buffer{data, N * sizeof(T)};
}

template<typename T, std::size_t N>
[[nodiscard]] constexpr const_buffer
buffer(const T (&data)[N]) noexcept {
  return const_buffer{data, N * sizeof(T)};
}

/// From array and clamp
template<typename T, std::size_t N>
[[nodiscard]] constexpr mutable_buffer
buffer(T (&data)[N], std::size_t max_size) noexcept {
  return mutable_buffer{data, (std::min)(N * sizeof(T), max_size)};
}

template<typename T, std::size_t N>
[[nodiscard]] constexpr const_buffer
buffer(const T (&data)[N], std::size_t max_size) noexcept {
  return const_buffer{data, (std::min)(N * sizeof(T), max_size)};
}

/// From container types
/**
 * Container types are here defeined as types that contain the 'data'
 * and 'size' methods, and a 'value_type' type defined. Some containers that
 * fill this requirements are std::vector, std::basic_string, std::array,
 * std::basic_string_view, std::span...
 */
template<typename T,
        typename = std::enable_if_t<is_mutable_buffer_v<T>>>
[[nodiscard]] constexpr mutable_buffer
buffer(T& container) noexcept {     // NOLINT
  return mutable_buffer{container.data(),
                        container.size() * sizeof(typename T::value_type)};
}

template<typename T,
            typename = std::enable_if_t<is_const_buffer_v<T>>>
[[nodiscard]] constexpr const_buffer
buffer(const T& container) noexcept {     // NOLINT
  return const_buffer{container.data(),
                        container.size() * sizeof(typename T::value_type)};
}

/// From container types and clamp
template<typename T,
        typename = std::enable_if_t<is_mutable_buffer_v<T>>>
[[nodiscard]] constexpr mutable_buffer
buffer(T& container, std::size_t max_size) noexcept {     // NOLINT
  return mutable_buffer{container.data(),
                        (std::min)(
                          container.size() * sizeof(typename T::value_type),
                          max_size)};
}

template<typename T,
        typename = std::enable_if_t<is_const_buffer_v<T>>>
[[nodiscard]] constexpr const_buffer
buffer(const T& container, std::size_t max_size) noexcept {     // NOLINT
  return const_buffer{container.data(),
                        (std::min)(
                          container.size() * sizeof(typename T::value_type),
                          max_size)};
}

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_BUFFER_HPP_
