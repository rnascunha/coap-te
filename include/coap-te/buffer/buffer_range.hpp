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
  using buffer_type = typename std::conditional_t<
                          is_mutable_buffers_iterator_v<Iterator>,
                          mutable_buffer,
                          const_buffer>;

  static constexpr bool
  is_multiple = is_multiple_buffers_iterator_v<Iterator>;

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

  constexpr reference
  operator+=(std::size_t n) noexcept {
    auto size_ = this->size();
    std::size_t offset = n > size_ ? size_ : n;
    begin_ += offset;
    return *this;
  }

  [[nodiscard]] friend constexpr buffer_range
  operator+(const buffer_range& buf, std::size_t n) noexcept {
    auto size_ = buf.size();
    std::size_t offset = n > size_ ? size_ : n;
    return buffer_range{buf.begin_ + offset, buf.end_};
  }

  [[nodiscard]] friend constexpr buffer_range
  operator+(std::size_t n, const buffer_range& buf) noexcept {
    return buf.begin_ + n;
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
buffer_range(const BufferSequence& buf) ->
  buffer_range<decltype(buffers_begin(buf))>;

// buffer_range type traits
template<typename T>
struct is_buffer_range : std::false_type{};

template<typename T>
struct is_buffer_range<buffer_range<T>> : std::true_type{};

template<typename T>
static constexpr bool
is_buffer_range_v = is_buffer_range<T>::value;

//
template<typename>
struct is_mutable_buffer_range : std::false_type{};

template<typename Iterator>
struct is_mutable_buffer_range<buffer_range<Iterator>> :
  std::bool_constant<
    is_mutable_buffers_iterator_v<Iterator>>{};

template<typename T>
static constexpr bool
is_mutable_buffer_range_v = is_mutable_buffer_range<T>::value;

//
template<typename>
struct is_const_buffer_range : std::false_type{};

template<typename Iterator>
struct is_const_buffer_range<buffer_range<Iterator>> :
  std::bool_constant<
    is_const_buffers_iterator_v<Iterator>>{};

template<typename T>
static constexpr bool
is_const_buffer_range_v = is_const_buffer_range<T>::value;

// Overload of buffers_[begin, end]
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
