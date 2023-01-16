/**
 * @file buffer_range.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_BUFFER_BUFFER_RANGE_HPP_
#define COAP_TE_BUFFER_BUFFER_RANGE_HPP_

#include <cstdint>
#include <type_traits>

#include "coap-te/buffer/buffers_iterator.hpp"

namespace coap_te {

template<typename Iterator>
class buffer_range {
 public:
  static_assert(is_buffers_iterator_v<Iterator>, "Is not buffer_iterator");

  using const_iterator = Iterator;
  using value_type = typename Iterator::value_type;
  using pointer = typename Iterator::pointer;
  using reference = typename Iterator::reference;

  static constexpr bool
  is_multiple = coap_te::is_multiple_buffer_iterator_v<Iterator>;

  constexpr
  buffer_range(Iterator begin, Iterator end) noexcept
    : begin_(begin), end_(end) {}

  template<typename BufferSequence>
  constexpr
  explicit buffer_range(const BufferSequence& buf) noexcept
    : begin_(buffers_begin(buf)), end_(buffers_end(buf)) {}

  [[nodiscard]] constexpr std::size_t
  size() const noexcept {
    return end_ - begin_;
  }

  [[nodiscard]] constexpr reference
  operator[](std::size_t n) const noexcept {
    return begin_[n];
  }

  /**
   * Iterator interface
   */

  [[nodiscard]] constexpr const_iterator
  begin() noexcept {
    return begin_;
  }

  [[nodiscard]] constexpr const_iterator
  end() noexcept {
    return end_;
  }

  [[nodiscard]] constexpr const_iterator
  begin() const noexcept {
    return begin_;
  }

  [[nodiscard]] constexpr const_iterator
  end() const noexcept {
    return end_;
  }

 private:
  const_iterator begin_;
  const_iterator end_;
};

/// constructor deduction guide
template<class BufferSequence>
buffer_range(BufferSequence buf) ->
  buffer_range<decltype(buffers_begin(buf))>;

template<typename T>
struct is_buffer_range : std::false_type{};

template<typename T>
struct is_buffer_range<buffer_range<T>> : std::true_type{};

template<typename T>
static constexpr bool
is_buffer_range_v = is_buffer_range<T>::value;

template<typename Iterator>
[[nodiscard]] constexpr Iterator
buffers_begin(const buffer_range<Iterator>& buffers) noexcept {
  return buffers.begin();
}

template<typename Iterator>
[[nodiscard]] constexpr Iterator
buffers_end(const buffer_range<Iterator>& buffers) noexcept {
  return buffers.end();
}

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_BUFFER_RANGE_HPP_
