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
#include "coap-te/buffer/buffer_range.hpp"

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
buffer_offset(const buffer_range<Iterator>& ic) noexcept {
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
buffer_iterator_begin(const buffer_range<Iterator>& b) noexcept
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
buffer_iterator_end(const buffer_range<Iterator>& b) noexcept
  -> decltype(b.end().current()) {
  // For buffer_range, the end is controlled by max size
  // so it doesn't matter the iterator end
  return decltype(b.end().current()){};
}

// buffer_max_size
template<typename It1, typename It2>
[[nodiscard]] constexpr std::size_t
buffer_max_size(const It1& it1, const It2& it2,
                std::size_t max_size
                  = std::numeric_limits<std::size_t>::max()) noexcept {
  if constexpr (is_buffer_range_v<It1> &&
                is_buffer_range_v<It2>) {
    return (std::min)({max_size, it1.size(), it2.size()});
  } else if constexpr (is_buffer_range_v<It1>) {
    return (std::min)(max_size, it1.size());
  } else if constexpr (is_buffer_range_v<It2>) {
    return (std::min)(max_size, it2.size());
  } else {
    return max_size;
  }
}

}  // namespace detail
}  // namespace coap_te

#endif  // COAP_TE_BUFFER_DETAIL_ITERATOR_HPP_
