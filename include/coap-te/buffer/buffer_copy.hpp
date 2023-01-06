/**
 * @file buffer_copy.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_BUFFER_BUFFER_COPY_HPP_
#define COAP_TE_BUFFER_BUFFER_COPY_HPP_

#include <limits>

#include "coap-te/buffer/const_buffer.hpp"
#include "coap-te/buffer/mutable_buffer.hpp"
#include "coap-te/buffer/iterator_container.hpp"
#include "coap-te/buffer/detail/cardinality.hpp"

namespace coap_te {

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

#endif  // COAP_TE_BUFFER_BUFFER_COPY_HPP_