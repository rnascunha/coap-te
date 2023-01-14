/**
 * @file iterator.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_BUFFER_DETAIL_ITERATOR_HPP_
#define COAP_TE_BUFFER_DETAIL_ITERATOR_HPP_

#include <limits>

#include "coap-te/buffer/const_buffer.hpp"
#include "coap-te/buffer/mutable_buffer.hpp"
#include "coap-te/buffer/iterator_container.hpp"

namespace coap_te {
namespace detail {

// buffer_offset
template<typename BufferSequence>
[[nodiscard]] constexpr std::size_t
buffer_offset(const BufferSequence&) noexcept {
  return 0;
}

template<typename Iterator>
[[nodiscard]] constexpr std::size_t
buffer_offset(const iterator_container<Iterator>& ic) noexcept {
  return ic.begin().current_position();
}

/// buffer_iterator_begin
template<typename BufferSequence>
[[nodiscard]] constexpr auto
buffer_iterator_begin(const BufferSequence& b) noexcept
  -> decltype(buffer_sequence_begin(b)) {
  return buffer_sequence_begin(b);
}

template<typename Iterator>
[[nodiscard]] constexpr auto
buffer_iterator_begin(const iterator_container<Iterator>& b) noexcept
  -> decltype(b.begin().current()) {
  return b.begin().current();
}

/// buffer_iterator_end
template<typename BufferSequence>
[[nodiscard]] constexpr auto
buffer_iterator_end(const BufferSequence& b) noexcept
  -> decltype(buffer_sequence_end(b)) {
  return buffer_sequence_end(b);
}

template<typename Iterator>
[[nodiscard]] constexpr auto
buffer_iterator_end(const iterator_container<Iterator>& b) noexcept
  -> decltype(b.end().end()) {
  return b.end().end();
}

// buffer_max_size
template<typename It1, typename It2>
[[nodiscard]] constexpr std::size_t
buffer_max_size(const It1& it1, const It2& it2,
                std::size_t max_size
                  = std::numeric_limits<std::size_t>::max()) noexcept {
  if constexpr (is_iterator_container_v<It1> &&
                is_iterator_container_v<It2>) {
    return (std::max)({max_size, it1.size(), it2.size()});
  } else if constexpr (is_iterator_container_v<It1>) {
    return (std::max)(max_size, it1.size());
  } else if constexpr (is_iterator_container_v<It2>) {
    return (std::max)(max_size, it2.size());
  } else {
    return max_size;
  }
}

}  // namespace detail
}  // namespace coap_te

#endif  // COAP_TE_BUFFER_DETAIL_ITERATOR_HPP_
