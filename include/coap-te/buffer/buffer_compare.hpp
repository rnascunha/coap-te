/**
 * @file buffer_compare.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_BUFFER_BUFFER_COMPARE_HPP_
#define COAP_TE_BUFFER_BUFFER_COMPARE_HPP_

#include <utility>
#include <limits>

#include "coap-te/buffer/const_buffer.hpp"
#include "coap-te/buffer/mutable_buffer.hpp"
#include "coap-te/buffer/iterator_container.hpp"
#include "coap-te/buffer/detail/cardinality.hpp"
#include "coap-te/buffer/detail/iterator.hpp"

namespace coap_te {
namespace detail {

constexpr std::pair<std::size_t, int>
buffer_compare_1(const const_buffer& buf1,
              const const_buffer& buf2) noexcept {
  std::size_t n = (std::min)(buf1.size(), buf2.size());
  int res = 0;
  if (n > 0) {
    res = std::memcmp(buf1.data(), buf2.data(), n);
  }
  return {n, res};
}

template<typename Iterator1, typename Iterator2>
constexpr int
buffer_compare(one_buffer, one_buffer,
    Iterator1 buf1, Iterator1,
    std::size_t,
    Iterator2 buf2, Iterator2,
    std::size_t) noexcept {
  return ((buffer_compare_1)(*buf1, *buf2)).second;
}

template<typename Iterator1, typename Iterator2>
constexpr int
buffer_compare(one_buffer, one_buffer,
    Iterator1 buf1, Iterator1,
    std::size_t,
    Iterator2 buf2, Iterator2,
    std::size_t,
    std::size_t max_size) noexcept {
  return ((buffer_compare_1)(*buf1, buffer(*buf2, max_size))).second;
}

template<typename Iterator1, typename Iterator2>
constexpr int
buffer_compare(one_buffer, multiple_buffers,
            Iterator1 buf1_begin, Iterator1,
            std::size_t,
            Iterator2 buf2_begin, Iterator2 buf2_end,
            std::size_t buf2_offset,
            std::size_t max_size
              = std::numeric_limits<std::size_t>::max()) noexcept {
  Iterator2 buf2_iter = buf2_begin;

  const_buffer buf1_buffer{buffer(*buf1_begin, max_size)};
  for (const_buffer buf2_buffer =
        const_buffer(*buf2_iter) + buf2_offset;
       buf1_buffer.size() != 0 && buf2_iter != buf2_end;
       buf2_buffer = *++buf2_iter) {
    auto [bytes_compared, res] = (buffer_compare_1)(buf1_buffer, buf2_buffer);
    if (res != 0) {
      return res;
    }
    buf1_buffer += bytes_compared;
  }
  return 0;
}

template<typename Iterator1, typename Iterator2>
constexpr int
buffer_compare(multiple_buffers, one_buffer,
            Iterator1 buf1_begin, Iterator1 buf1_end,
            std::size_t buf1_offset,
            Iterator2 buf2_begin, Iterator2,
            std::size_t,
            std::size_t max_size
              = std::numeric_limits<std::size_t>::max()) noexcept {
  return -buffer_compare(one_buffer{}, multiple_buffers{},
                      buf2_begin, {},
                      {},
                      buf1_begin, buf1_end,
                      buf1_offset,
                      max_size);
}

template<typename Iterator1, typename Iterator2>
constexpr int
buffer_compare(multiple_buffers, multiple_buffers,
            Iterator1 buf1_begin, Iterator1 buf1_end,
            std::size_t buf1_buffer_offset,
            Iterator2 buf2_begin, Iterator2 buf2_end,
            std::size_t buf2_buffer_offset) noexcept {
  Iterator1 buf1_iter = buf1_begin;
  Iterator2 buf2_iter = buf2_begin;

  while (buf1_iter != buf1_end &&
         buf2_iter != buf2_end) {
    const_buffer buf1_buffer = const_buffer(*buf1_iter) +
                                   buf1_buffer_offset;

    const_buffer buf2_buffer = const_buffer(*buf2_iter) +
                                 buf2_buffer_offset;

    auto [bytes_compared, res] = (buffer_compare_1)(buf1_buffer, buf2_buffer);
    if (res != 0)
      return res;

    if (bytes_compared == buf1_buffer.size()) {
      ++buf1_iter;
      buf1_buffer_offset = 0;
    } else {
      buf1_buffer_offset += bytes_compared;
    }

    if (bytes_compared == buf2_buffer.size()) {
      ++buf2_iter;
      buf2_buffer_offset = 0;
    } else {
      buf2_buffer_offset += bytes_compared;
    }
  }
  return 0;
}

template<typename Iterator1, typename Iterator2>
constexpr int
buffer_compare(multiple_buffers, multiple_buffers,
            Iterator1 buf1_begin, Iterator1 buf1_end,
            std::size_t buf1_buffer_offset,
            Iterator2 buf2_begin, Iterator2 buf2_end,
            std::size_t buf2_buffer_offset,
            std::size_t max_size) noexcept {
  std::size_t total_bytes_compared = 0;

  Iterator1 buf1_iter = buf1_begin;
  Iterator2 buf2_iter = buf2_begin;

  while (total_bytes_compared != max_size &&
         buf1_iter != buf1_end &&
         buf2_iter != buf2_end) {
    const_buffer buf1_buffer = const_buffer(*buf1_iter) +
                                   buf1_buffer_offset;

    const_buffer buf2_buffer = const_buffer(*buf2_iter) +
                                 buf2_buffer_offset;

    auto [bytes_compared, res] = (buffer_compare_1)(buf1_buffer,
                                  buffer(buf2_buffer,
                                      max_size - total_bytes_compared));
    if (res != 0)
      return res;
    total_bytes_compared += bytes_compared;

    if (bytes_compared == buf1_buffer.size()) {
      ++buf1_iter;
      buf1_buffer_offset = 0;
    } else {
      buf1_buffer_offset += bytes_compared;
    }

    if (bytes_compared == buf2_buffer.size()) {
      ++buf2_iter;
      buf2_buffer_offset = 0;
    } else {
      buf2_buffer_offset += bytes_compared;
    }
  }
  return 0;
}

}  // namespace detail

template<typename ConstBufferSequence1,
         typename ConstBufferSequence2>
constexpr int
buffer_compare(const ConstBufferSequence1& buf1,
               const ConstBufferSequence2& buf2) noexcept {
  if constexpr (is_iterator_container_v<ConstBufferSequence1> ||
              is_iterator_container_v<ConstBufferSequence2>) {
    return buffer_compare(
      buf1, buf2,
      detail::buffer_max_size(buf1, buf2));
  } else {
    return detail::buffer_compare(
      detail::buffer_sequence_cardinality<ConstBufferSequence1>(),
      detail::buffer_sequence_cardinality<ConstBufferSequence2>(),
      detail::buffer_iterator_begin(buf1),
      detail::buffer_iterator_end(buf1),
      detail::buffer_offset(buf1),
      detail::buffer_iterator_begin(buf2),
      detail::buffer_iterator_end(buf2),
      detail::buffer_offset(buf2));
  }
}

template<typename ConstBufferSequence1,
         typename ConstBufferSequence2>
constexpr int
buffer_compare(const ConstBufferSequence1& buf1,
            const ConstBufferSequence2& buf2,
            std::size_t max_size) noexcept {
  return detail::buffer_compare(
    detail::buffer_sequence_cardinality<ConstBufferSequence1>(),
    detail::buffer_sequence_cardinality<ConstBufferSequence2>(),
    detail::buffer_iterator_begin(buf1),
    detail::buffer_iterator_end(buf1),
    detail::buffer_offset(buf1),
    detail::buffer_iterator_begin(buf2),
    detail::buffer_iterator_end(buf2),
    detail::buffer_offset(buf2),
    max_size);
}

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_BUFFER_COMPARE_HPP_
