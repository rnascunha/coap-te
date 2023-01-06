/**
 * @file iterator_container.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_BUFFER_ITERATOR_CONTAINER_HPP_
#define COAP_TE_BUFFER_ITERATOR_CONTAINER_HPP_

#include <cstdint>
#include <type_traits>

#include "coap-te/buffer/buffers_iterator.hpp"

namespace coap_te {

template<typename Iterator>
class iterator_container {
 public:
  static_assert(is_buffers_iterator_v<Iterator>, "Is not buffer_iterator");

  using const_iterator = Iterator;
  using value_type = typename Iterator::value_type;
  using pointer = typename Iterator::pointer;
  using reference = typename Iterator::reference;

  static constexpr bool
  is_multiple = coap_te::is_multiple_buffer_iterator_v<Iterator>;

  constexpr
  iterator_container(Iterator begin, Iterator end) noexcept
    : begin_(begin), end_(end) {}

  template<typename BufferSequence>
  constexpr
  explicit iterator_container(const BufferSequence& buf) noexcept
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
iterator_container(BufferSequence buf) ->
  iterator_container<decltype(buffers_begin(buf))>;

template<typename T>
struct is_iterator_container : std::false_type{};

template<typename T>
struct is_iterator_container<iterator_container<T>> : std::true_type{};

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_ITERATOR_CONTAINER_HPP_
