/**
 * @file buffer_size.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_BUFFER_BUFFER_SIZE_HPP_
#define COAP_TE_BUFFER_BUFFER_SIZE_HPP_

#include "coap-te/buffer/const_buffer.hpp"
#include "coap-te/buffer/mutable_buffer.hpp"
#include "coap-te/buffer/iterator_container.hpp"
#include "coap-te/buffer/detail/cardinality.hpp"

namespace coap_te {

namespace detail {

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

template<typename Iterator>
[[nodiscard]] constexpr std::size_t
buffer_size(const iterator_container<Iterator>& b) noexcept {
  return b.size();
}

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_BUFFER_SIZE_HPP_
