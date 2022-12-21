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
#ifndef COAP_TE_CORE_BUFFER_HPP_
#define COAP_TE_CORE_BUFFER_HPP_

#include <algorithm>    // std::min
#include <type_traits>
#include <limits>
#include <cstring>

#include "coap-te/core/traits.hpp"
#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/core/const_buffer.hpp"

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
template<typename T, std::enable_if_t<core::is_container_v<T>>>
[[nodiscard]] constexpr mutable_buffer
buffer(T& container) noexcept {     // NOLINT
  return mutable_buffer{container.data(),
                        container.size() * sizeof(typename T::value_type)};
}

template<typename T, std::enable_if_t<core::is_container_v<T>>>
[[nodiscard]] constexpr const_buffer
buffer(const T& container) noexcept {     // NOLINT
  return const_buffer{container.data(),
                        container.size() * sizeof(typename T::value_type)};
}

/// From container types and clamp
template<typename T, std::enable_if_t<core::is_container_v<T>>>
[[nodiscard]] constexpr mutable_buffer
buffer(T& container, std::size_t max_size) noexcept {     // NOLINT
  return mutable_buffer{container.data(),
                        (std::min)(
                          container.size() * sizeof(typename T::value_type),
                          max_size)};
}

template<typename T, std::enable_if_t<core::is_container_v<T>>>
[[nodiscard]] constexpr const_buffer
buffer(const T& container, std::size_t max_size) noexcept {     // NOLINT
  return const_buffer{container.data(),
                        (std::min)(
                          container.size() * sizeof(typename T::value_type),
                          max_size)};
}

/**
 * Buffer sequence
 */
/**
 * Begin
 */
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

namespace detail {

// Tag types used to select appropriately optimised overloads.
struct one_buffer {};
struct multiple_buffers {};

// Helper trait to detect single buffers.
template <typename BufferSequence>
struct buffer_sequence_cardinality :
  std::conditional_t<
    std::is_same_v<BufferSequence, mutable_buffer>
    || std::is_same_v<BufferSequence, const_buffer>,
    one_buffer, multiple_buffers> {};

template <typename Iterator>
[[nodiscard]] constexpr std::size_t
buffer_size(one_buffer, Iterator begin, Iterator) noexcept {
  return const_buffer{*begin}.size();
}

template <typename Iterator>
[[nodiscard]] constexpr std::size_t
buffer_size(multiple_buffers, Iterator begin, Iterator end) noexcept {
  std::size_t total_buffer_size = 0;

  Iterator iter = begin;
  for (; iter != end; ++iter) {
    const_buffer b(*iter);
    total_buffer_size += b.size();
  }
  return total_buffer_size;
}

}  // namespace detail

template<typename BufferSequence>
[[nodiscard]] constexpr std::size_t
buffer_size(const BufferSequence& b) noexcept {
  return detail::buffer_size(
    detail::buffer_sequence_cardinality<BufferSequence>(),
    buffer_sequence_begin(b),
    buffer_sequence_end(b));
}

/**
 * Buffer copy
 */

namespace detail {

constexpr std::size_t
buffer_copy_1(const mutable_buffer& target,
              const const_buffer& source) noexcept {
  std::size_t n = (std::min)(target.size(), source.size());
  if (n > 0) {
    std::memcpy(target.data(), source.data(), n);
  }
  return n;
}

template<typename TargetIterator, typename SourceIterator>
constexpr std::size_t buffer_copy(one_buffer, one_buffer,
    TargetIterator target_begin, TargetIterator,
    SourceIterator source_begin, SourceIterator) noexcept {
  return (buffer_copy_1)(*target_begin, *source_begin);
}

template<typename TargetIterator, typename SourceIterator>
constexpr std::size_t buffer_copy(one_buffer, one_buffer,
    TargetIterator target_begin, TargetIterator,
    SourceIterator source_begin, SourceIterator,
    std::size_t max_size) noexcept {
  return (buffer_copy_1)(*target_begin,
                         buffer(*source_begin, max_size));
}

template<typename TargetIterator, typename SourceIterator>
constexpr std::size_t
buffer_copy(one_buffer, multiple_buffers,
            TargetIterator target_begin, TargetIterator,
            SourceIterator source_begin, SourceIterator source_end,
            std::size_t max_size
              = std::numeric_limits<std::size_t>::max()) noexcept {
  std::size_t total_bytes_copied = 0;
  SourceIterator source_iter = source_begin;

  for (mutable_buffer target_buffer(buffer(*target_begin, max_size));
       target_buffer.size() != 0 && source_iter != source_end;
       ++source_iter) {
    const_buffer source_buffer(*source_iter);
    std::size_t bytes_copied = (buffer_copy_1)(target_buffer, source_buffer);
    total_bytes_copied += bytes_copied;
    target_buffer += bytes_copied;
  }
  return total_bytes_copied;
}

template<typename TargetIterator, typename SourceIterator>
constexpr std::size_t
buffer_copy(multiple_buffers, one_buffer,
            TargetIterator target_begin, TargetIterator target_end,
            SourceIterator source_begin, SourceIterator,
            std::size_t max_size
              = std::numeric_limits<std::size_t>::max()) noexcept {
  std::size_t total_bytes_copied = 0;
  TargetIterator target_iter = target_begin;

  for (const_buffer source_buffer(buffer(*source_begin, max_size));
       source_buffer.size() != 0 && target_iter != target_end;
       ++target_iter) {
    mutable_buffer target_buffer(*target_iter);
    std::size_t bytes_copied = (buffer_copy_1)(target_buffer, source_buffer);
    total_bytes_copied += bytes_copied;
    source_buffer += bytes_copied;
  }

  return total_bytes_copied;
}

template<typename TargetIterator, typename SourceIterator>
constexpr std::size_t
buffer_copy(multiple_buffers, multiple_buffers,
            TargetIterator target_begin, TargetIterator target_end,
            SourceIterator source_begin, SourceIterator source_end) noexcept {
  std::size_t total_bytes_copied = 0;

  TargetIterator target_iter = target_begin;
  std::size_t target_buffer_offset = 0;

  SourceIterator source_iter = source_begin;
  std::size_t source_buffer_offset = 0;

  while (target_iter != target_end &&
         source_iter != source_end) {
    mutable_buffer target_buffer = mutable_buffer(*target_iter) +
                                   target_buffer_offset;

    const_buffer source_buffer = const_buffer(*source_iter) +
                                 source_buffer_offset;

    std::size_t bytes_copied = (buffer_copy_1)(target_buffer, source_buffer);
    total_bytes_copied += bytes_copied;

    if (bytes_copied == target_buffer.size()) {
      ++target_iter;
      target_buffer_offset = 0;
    } else {
      target_buffer_offset += bytes_copied;
    }

    if (bytes_copied == source_buffer.size()) {
      ++source_iter;
      source_buffer_offset = 0;
    } else {
      source_buffer_offset += bytes_copied;
    }
  }
  return total_bytes_copied;
}

template<typename TargetIterator, typename SourceIterator>
constexpr std::size_t
buffer_copy(multiple_buffers, multiple_buffers,
            TargetIterator target_begin, TargetIterator target_end,
            SourceIterator source_begin, SourceIterator source_end,
            std::size_t max_size) noexcept {
  std::size_t total_bytes_copied = 0;

  TargetIterator target_iter = target_begin;
  std::size_t target_buffer_offset = 0;

  SourceIterator source_iter = source_begin;
  std::size_t source_buffer_offset = 0;

  while (total_bytes_copied != max_size &&
         target_iter != target_end &&
         source_iter != source_end) {
    mutable_buffer target_buffer = mutable_buffer(*target_iter) +
                                   target_buffer_offset;

    const_buffer source_buffer = const_buffer(*source_iter) +
                                 source_buffer_offset;

    std::size_t bytes_copied = (buffer_copy_1)(target_buffer,
                                buffer(source_buffer,
                                      max_size - total_bytes_copied));
    total_bytes_copied += bytes_copied;

    if (bytes_copied == target_buffer.size()) {
      ++target_iter;
      target_buffer_offset = 0;
    } else {
      target_buffer_offset += bytes_copied;
    }

    if (bytes_copied == source_buffer.size()) {
      ++source_iter;
      source_buffer_offset = 0;
    } else {
      source_buffer_offset += bytes_copied;
    }
  }
  return total_bytes_copied;
}

}  // namespace detail

template<typename MutableBufferSequence, typename ConstBufferSequence>
constexpr std::size_t
buffer_copy(const MutableBufferSequence& target,
            const ConstBufferSequence& source) noexcept {
  return detail::buffer_copy(
    detail::buffer_sequence_cardinality<MutableBufferSequence>(),
    detail::buffer_sequence_cardinality<ConstBufferSequence>(),
    buffer_sequence_begin(target),
    buffer_sequence_end(target),
    buffer_sequence_begin(source),
    buffer_sequence_end(source));
}

template<typename MutableBufferSequence, typename ConstBufferSequence>
constexpr std::size_t
buffer_copy(const MutableBufferSequence& target,
            const ConstBufferSequence& source,
            std::size_t max_size) noexcept {
  return detail::buffer_copy(
    detail::buffer_sequence_cardinality<MutableBufferSequence>(),
    detail::buffer_sequence_cardinality<ConstBufferSequence>(),
    buffer_sequence_begin(target),
    buffer_sequence_end(target),
    buffer_sequence_begin(source),
    buffer_sequence_end(source),
    max_size);
}

}  // namespace coap_te

#endif  // COAP_TE_CORE_BUFFER_HPP_
