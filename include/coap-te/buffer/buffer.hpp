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
#include <cstring>      // strlen

#include <vector>
#include <string>
#include <array>
#include <string_view>

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

[[nodiscard]] constexpr const_buffer
buffer(const char* data) noexcept {
  return const_buffer{data, std::strlen(data)};
}

/// From array
template<typename T,
         std::size_t N,
         std::enable_if_t<!coap_te::core::is_container_class_v<T>, int> = 0>
[[nodiscard]] constexpr mutable_buffer
buffer(T (&data)[N]) noexcept {
  return mutable_buffer{data, N * sizeof(T)};
}

template<typename T, std::size_t N,
         std::enable_if_t<!coap_te::core::is_container_class_v<T>, int> = 0>
[[nodiscard]] constexpr const_buffer
buffer(const T (&data)[N]) noexcept {
  return const_buffer{data, N * sizeof(T)};
}

/// From array and clamp
template<typename T, std::size_t N,
         std::enable_if_t<!coap_te::core::is_container_class_v<T>, int> = 0>
[[nodiscard]] constexpr mutable_buffer
buffer(T (&data)[N], std::size_t max_size) noexcept {
  return mutable_buffer{data, (std::min)(N * sizeof(T), max_size)};
}

template<typename T, std::size_t N,
         std::enable_if_t<!coap_te::core::is_container_class_v<T>, int> = 0>
[[nodiscard]] constexpr const_buffer
buffer(const T (&data)[N], std::size_t max_size) noexcept {
  return const_buffer{data, (std::min)(N * sizeof(T), max_size)};
}

// string_view
template<typename CharT, typename Traits>
[[nodiscard]] constexpr const_buffer
buffer(std::basic_string_view<CharT, Traits>& container) noexcept {     // NOLINT
  return const_buffer{container.data(),
                      container.size() * sizeof(std::string_view::value_type)};
}

template<typename CharT, typename Traits>
[[nodiscard]] constexpr const_buffer
buffer(std::basic_string_view<CharT, Traits>& container,      // NOLINT
       std::size_t max_size) noexcept {
  return const_buffer{container.data(),
                      (std::min)(
                        container.size() * sizeof(std::string_view::value_type),
                        max_size)};
}

template<typename CharT, typename Traits>
[[nodiscard]] constexpr const_buffer
buffer(const std::basic_string_view<CharT, Traits>& container) noexcept {     // NOLINT
  return const_buffer{container.data(),
                      container.size() * sizeof(std::string_view::value_type)};
}

template<typename CharT, typename Traits>
[[nodiscard]] constexpr const_buffer
buffer(const std::basic_string_view<CharT, Traits>& container,
       std::size_t max_size) noexcept {     // NOLINT
  return const_buffer{container.data(),
                      (std::min)(
                        container.size() * sizeof(std::string_view::value_type),
                        max_size)};
}

/// From container types
/**
 * Container types are here defeined as types that contain the 'data'
 * and 'size' methods, and a 'value_type' type defined. Some containers that
 * fill this requirements are std::vector, std::basic_string, std::array,
 * std::basic_string_view, std::span...
 */
template<typename T,
        std::enable_if_t<coap_te::core::is_container_class_v<T>, int> = 0>
[[nodiscard]] constexpr mutable_buffer
buffer(T& container) noexcept {     // NOLINT
  return mutable_buffer{container.data(),
                        container.size() * sizeof(typename T::value_type)};
}

template<typename T,
         std::enable_if_t<coap_te::core::is_container_class_v<T>, int> = 0>
[[nodiscard]] constexpr const_buffer
buffer(const T& container) noexcept {     // NOLINT
  return const_buffer{container.data(),
                        container.size() * sizeof(typename T::value_type)};
}

/// From container types and clamp
template<typename T,
         std::enable_if_t<coap_te::core::is_container_class_v<T>, int> = 0>
[[nodiscard]] constexpr mutable_buffer
buffer(T& container, std::size_t max_size) noexcept {     // NOLINT
  return mutable_buffer{container.data(),
                        (std::min)(
                          container.size() * sizeof(typename T::value_type),
                          max_size)};
}

template<typename T,
         std::enable_if_t<coap_te::core::is_container_class_v<T>, int> = 0>
[[nodiscard]] constexpr const_buffer
buffer(const T& container, std::size_t max_size) noexcept {     // NOLINT
  return const_buffer{container.data(),
                        (std::min)(
                          container.size() * sizeof(typename T::value_type),
                          max_size)};
}

// vector
// template<typename T, typename Alloc>
// [[nodiscard]] constexpr mutable_buffer
// buffer(std::vector<T, Alloc>& container) noexcept {     // NOLINT
//   return mutable_buffer{container.data(),
//                         container.size() * sizeof(T)};
// }

// template<typename T, typename Alloc>
// [[nodiscard]] constexpr const_buffer
// buffer(const std::vector<T, Alloc>& container) noexcept {     // NOLINT
//   return const_buffer{container.data(),
//                         container.size() * sizeof(T)};
// }

// /// From container types and clamp
// template<typename T, typename Alloc>
// [[nodiscard]] constexpr mutable_buffer
// buffer(std::vector<T, Alloc>& container, std::size_t max_size) noexcept {     // NOLINT
//   return mutable_buffer{container.data(),
//                         (std::min)(
//                           container.size() * sizeof(T),
//                           max_size)};
// }

// template<typename T, typename Alloc>
// [[nodiscard]] constexpr const_buffer
// buffer(const std::vector<T, Alloc>& container, std::size_t max_size) noexcept {     // NOLINT
//   return const_buffer{container.data(),
//                         (std::min)(
//                           container.size() * sizeof(T),
//                           max_size)};
// }

// // array
// template<typename T, std::size_t N>
// [[nodiscard]] constexpr mutable_buffer
// buffer(std::array<T, N>& container) noexcept {     // NOLINT
//   return mutable_buffer{container.data(),
//                         container.size() * sizeof(T)};
// }

// template<typename T, std::size_t N>
// [[nodiscard]] constexpr const_buffer
// buffer(const std::array<T, N>& container) noexcept {     // NOLINT
//   return const_buffer{container.data(),
//                         container.size() * sizeof(T)};
// }

// /// From container types and clamp
// template<typename T, std::size_t N>
// [[nodiscard]] constexpr mutable_buffer
// buffer(std::array<T, N>& container, std::size_t max_size) noexcept {     // NOLINT
//   return mutable_buffer{container.data(),
//                         (std::min)(
//                           container.size() * sizeof(T),
//                           max_size)};
// }

// template<typename T, std::size_t N>
// [[nodiscard]] constexpr const_buffer
// buffer(const std::array<T, N>& container, std::size_t max_size) noexcept {     // NOLINT
//   return const_buffer{container.data(),
//                         (std::min)(
//                           container.size() * sizeof(T),
//                           max_size)};
// }

// // string
// template<typename CharT,
//          typename Traits,
//          typename Alloc>
// [[nodiscard]] mutable_buffer
// buffer(std::basic_string<CharT, Traits, Alloc>& container) noexcept {     // NOLINT
//   return mutable_buffer{container.data(),
//                         container.size() * sizeof(std::string::value_type)};
// }

// template<typename CharT,
//          typename Traits,
//          typename Alloc>
// [[nodiscard]] const_buffer
// buffer(const std::basic_string<CharT, Traits, Alloc>& container) noexcept {     // NOLINT
//   return const_buffer{container.data(),
//                         container.size() * sizeof(std::string::value_type)};
// }

// // From container types and clamp
// template<typename CharT,
//          typename Traits,
//          typename Alloc>
// [[nodiscard]] mutable_buffer
// buffer(std::basic_string<CharT, Traits, Alloc>& container, std::size_t max_size) noexcept {     // NOLINT
//   return mutable_buffer{container.data(),
//                         (std::min)(
//                           container.size() * sizeof(std::string::value_type),
//                           max_size)};
// }

// template<typename CharT,
//          typename Traits,
//          typename Alloc>
// [[nodiscard]] const_buffer
// buffer(const std::basic_string<CharT, Traits, Alloc>& container, std::size_t max_size) noexcept {     // NOLINT
//   return const_buffer{container.data(),
//                         (std::min)(
//                           container.size() * sizeof(std::string::value_type),
//                           max_size)};
// }

}  // namespace coap_te

#include "coap-te/buffer/buffer_sequence.hpp"
#include "coap-te/buffer/is_buffer_sequence.hpp"
#include "coap-te/buffer/traits.hpp"

#endif  // COAP_TE_BUFFER_BUFFER_HPP_
