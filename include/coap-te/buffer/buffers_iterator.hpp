/**
 * @file buffers_iterator.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_BUFFER_BUFFERS_ITERATOR_HPP_
#define COAP_TE_BUFFER_BUFFERS_ITERATOR_HPP_

#include <type_traits>
#include <cstring>
#include <iterator>

#include "coap-te/buffer/const_buffer.hpp"
#include "coap-te/buffer/mutable_buffer.hpp"
#include "coap-te/buffer/buffer.hpp"

namespace coap_te {

namespace detail {

template<bool IsMutable>
struct buffers_iterator_types_helper;

template<>
struct buffers_iterator_types_helper<false> {
  using buffer_type = const_buffer;
  template<typename ByteType>
  struct byte_type {
    using type = typename std::add_const_t<ByteType>;
  };
};

template<>
struct buffers_iterator_types_helper<true> {
  using buffer_type = mutable_buffer;
  template<typename ByteType>
  struct byte_type {
    using type = ByteType;
  };
};

template<typename BufferSequence, typename ByteType>
struct buffers_iterator_types {
  static constexpr bool
  is_mutable = std::is_convertible_v<
                typename BufferSequence::value_type,
                mutable_buffer>;
  using helper = buffers_iterator_types_helper<is_mutable>;
  using buffer_type = typename helper::buffer_type;
  using byte_type = typename helper::template byte_type<ByteType>::type;
  using const_iterator = typename BufferSequence::const_iterator;
};

template<typename ByteType>
struct buffers_iterator_types<mutable_buffer, ByteType> {
  using buffer_type = mutable_buffer;
  using byte_type = ByteType;
  using const_iterator = const mutable_buffer*;
};

template<typename ByteType>
struct buffers_iterator_types<const_buffer, ByteType> {
  using buffer_type = const_buffer;
  using byte_type = std::add_const_t<ByteType>;
  using const_iterator = const const_buffer*;
};

}  // namespace detail

template<typename BufferSequence,
         typename ByteType = std::uint8_t>
class buffers_iterator {
 private:
  using buffer_type = typename detail::buffers_iterator_types<
                        BufferSequence, ByteType>::buffer_type;
  using buffer_sequence_iterator_type =
                      typename detail::buffers_iterator_types<
                        BufferSequence, ByteType>::const_iterator;

 public:
  using difference_type = std::ptrdiff_t;
  using value_type = ByteType;
  using pointer = typename detail::buffers_iterator_types<
                    BufferSequence, ByteType>::byte_type*;
  using reference = typename detail::buffers_iterator_types<
                    BufferSequence, ByteType>::byte_type&;

  using iterator_category = std::random_access_iterator_tag;

  buffers_iterator()
    : current_buffer_{},
      current_buffer_position_{0},
      begin_{},
      current_{},
      end_{},
      position_{0} {}

  [[nodiscard]] static constexpr buffers_iterator
  begin(const BufferSequence& buffers) noexcept {
    buffers_iterator new_iter;
    new_iter.begin_ = buffer_sequence_begin(buffers);
    new_iter.current_ = buffer_sequence_begin(buffers);
    new_iter.end_ = buffer_sequence_end(buffers);

    while (new_iter.current_ != new_iter.end_) {
      new_iter.current_buffer_ = *new_iter.current_;
      if (new_iter.current_buffer_.size() > 0)
        break;
      ++new_iter.current_;
    }
    return new_iter;
  }

  [[nodiscard]] static constexpr buffers_iterator
  end(const BufferSequence& buffers) noexcept {
    buffers_iterator new_iter;
    new_iter.begin_ = buffer_sequence_begin(buffers);
    new_iter.current_ = buffer_sequence_begin(buffers);
    new_iter.end_ = buffer_sequence_end(buffers);

    while (new_iter.current_ != new_iter.end_) {
      buffer_type buffer = *new_iter.current_;
      new_iter.position_ += buffer.size();
      ++new_iter.current_;
    }
    return new_iter;
  }

  [[nodiscard]] constexpr reference
  operator*() const noexcept {
    return dereference();
  }

  [[nodiscard]] constexpr pointer
  operator->() const noexcept {
    return &dereference();
  }

  [[nodiscard]] constexpr reference
  operator[](difference_type difference) const noexcept {
    buffers_iterator tmp(*this);
    tmp.advance(difference);
    return *tmp;
  }

  constexpr buffers_iterator&
  operator++() noexcept {
    increment();
    return *this;
  }

  constexpr buffers_iterator
  operator++(int) noexcept {
    buffers_iterator tmp(*this);
    ++*this;
    return tmp;
  }

  constexpr buffers_iterator&
  operator--() noexcept {
    decrement();
    return *this;
  }

  constexpr buffers_iterator
  operator--(int) noexcept {
    buffers_iterator tmp(*this);
    --*this;
    return tmp;
  }

  constexpr buffers_iterator&
  operator+=(difference_type difference) noexcept {
    advance(difference);
    return *this;
  }

  constexpr buffers_iterator&
  operator-=(difference_type difference) noexcept {
    advance(-difference);
    return *this;
  }

  [[nodiscard]] constexpr friend buffers_iterator
  operator+(const buffers_iterator& iter,
            difference_type difference) noexcept {
    buffers_iterator tmp(iter);
    tmp.advance(difference);
    return tmp;
  }

  [[nodiscard]] constexpr friend buffers_iterator
  operator+(difference_type difference,
            const buffers_iterator& iter) noexcept {
    buffers_iterator tmp(iter);
    tmp.advance(difference);
    return tmp;
  }

  [[nodiscard]] friend constexpr buffers_iterator
  operator-(const buffers_iterator& iter,
            difference_type difference) noexcept {
    buffers_iterator tmp(iter);
    tmp.advance(-difference);
    return tmp;
  }

  [[nodiscard]] constexpr friend difference_type
  operator-(const buffers_iterator& a,
            const buffers_iterator& b) noexcept {
    return b.distance_to(a);
  }

  [[nodiscard]] constexpr friend bool
  operator==(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return a.equal(b);
  }

  [[nodiscard]] constexpr friend bool
  operator!=(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return !a.equal(b);
  }

  [[nodiscard]] constexpr friend bool
  operator<(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return a.distance_to(b) > 0;
  }

  [[nodiscard]] constexpr friend bool
  operator<=(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return !(b < a);
  }

  [[nodiscard]] constexpr friend bool
  operator>(const buffers_iterator& a,
            const buffers_iterator& b) noexcept {
    return b < a;
  }

  [[nodiscard]] constexpr friend bool
  operator>=(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return !(a < b);
  }

 private:
  [[nodiscard]] constexpr reference
  dereference() const noexcept {
    return static_cast<pointer>(
      current_buffer_.data())[current_buffer_position_];
  }

  [[nodiscard]] constexpr bool
  equal(const buffers_iterator& other) const noexcept {
    return position_ == other.position_;
  }

  constexpr void
  increment() noexcept {
    ++position_;

    ++current_buffer_position_;
    if (current_buffer_position_ != current_buffer_.size())
      return;

    ++current_;
    current_buffer_position_ = 0;
    while (current_ != end_) {
      current_buffer_ = *current_;
      if (current_buffer_.size() > 0)
        return;
      ++current_;
    }
  }

  constexpr void
  decrement() noexcept {
    --position_;

    if (current_buffer_position_ != 0) {
      --current_buffer_position_;
      return;
    }

    buffer_sequence_iterator_type iter = current_;
    while (iter != begin_) {
      --iter;
      buffer_type buffer = *iter;
      std::size_t buffer_size = buffer.size();
      if (buffer_size > 0) {
        current_ = iter;
        current_buffer_ = buffer;
        current_buffer_position_ = buffer_size - 1;
        return;
      }
    }
  }

  constexpr void
  advance(difference_type n) noexcept {
    if (n > 0) {
      for (;;) {
        difference_type current_buffer_balance =
          current_buffer_.size() - current_buffer_position_;

        if (current_buffer_balance > n) {
          position_ += n;
          current_buffer_position_ += n;
          return;
        }

        n -= current_buffer_balance;
        position_ += current_buffer_balance;
        if (++current_ == end_) {
          current_buffer_ = buffer_type{};
          current_buffer_position_ = 0;
          return;
        }
        current_buffer_ = *current_;
        current_buffer_position_ = 0;
      }
    } else if (n < 0) {
      std::size_t abs_n = -n;
      for (;;) {
        if (current_buffer_position_ >= abs_n) {
          position_ -= abs_n;
          current_buffer_position_ -= abs_n;
          return;
        }

        abs_n -= current_buffer_position_;
        position_ -= current_buffer_position_;

        if (current_ == begin_) {
          current_buffer_position_ = 0;
          return;
        }

        buffer_sequence_iterator_type iter = current_;
        while (iter != begin_) {
          --iter;
          buffer_type buffer = *iter;
          std::size_t buffer_size = buffer.size();
          if (buffer_size > 0) {
            current_ = iter;
            current_buffer_ = buffer;
            current_buffer_position_ = buffer_size;
            break;
          }
        }
      }
    }
  }

  [[nodiscard]] constexpr difference_type
  distance_to(const buffers_iterator& other) const noexcept {
    return other.position_ - position_;
  }

  buffer_type current_buffer_;
  std::size_t current_buffer_position_;
  buffer_sequence_iterator_type begin_;
  buffer_sequence_iterator_type current_;
  buffer_sequence_iterator_type end_;
  std::size_t position_;
};

/**
 * Added by me [init]
 */
template<>
class buffers_iterator<mutable_buffer, std::uint8_t> {
 private:
  using byte_type = std::uint8_t;
  using buffer_type = mutable_buffer;
  using buffer_sequence_iterator_type = const mutable_buffer*;

 public:
  using difference_type = std::ptrdiff_t;
  using value_type = byte_type;
  using pointer = byte_type*;
  using reference = byte_type&;

  using iterator_category = std::random_access_iterator_tag;

  constexpr
  buffers_iterator() noexcept = default;

  constexpr
  explicit buffers_iterator(const buffer_type& buf) noexcept
    : ptr_{static_cast<pointer>(buf.data())} {}

  [[nodiscard]] static constexpr buffers_iterator
  begin(const buffer_type& buffers) noexcept {
    return buffers_iterator{buffers};
  }

  [[nodiscard]] static constexpr buffers_iterator
  end(const buffer_type& buffers) noexcept {
    return buffers_iterator{buffer_type{static_cast<pointer>(buffers.data()) +
                            buffers.size(),
                            0}};
  }

  [[nodiscard]] constexpr reference
  operator*() const noexcept {
    return *ptr_;
  }

  [[nodiscard]] constexpr pointer
  operator->() const noexcept {
    return ptr_;
  }

  [[nodiscard]] constexpr reference
  operator[](difference_type difference) const noexcept {
    return *(ptr_ + difference);
  }

  constexpr buffers_iterator&
  operator++() noexcept {
    ptr_ += 1;
    return *this;
  }

  constexpr buffers_iterator
  operator++(int) noexcept {
    buffers_iterator tmp(*this);
    ++*this;
    return tmp;
  }

  constexpr buffers_iterator&
  operator--() noexcept {
    ptr_ -= 1;
    return *this;
  }

  constexpr buffers_iterator
  operator--(int) noexcept {
    buffers_iterator tmp(*this);
    --*this;
    return tmp;
  }

  constexpr buffers_iterator&
  operator+=(difference_type difference) noexcept {
    ptr_ += difference;
    return *this;
  }

  constexpr buffers_iterator&
  operator-=(difference_type difference) noexcept {
    ptr_ -= difference;
    return *this;
  }

  [[nodiscard]] constexpr friend buffers_iterator
  operator+(const buffers_iterator& iter,
            difference_type difference) noexcept {
    buffers_iterator tmp(iter);
    tmp += difference;
    return tmp;
  }

  [[nodiscard]] constexpr friend buffers_iterator
  operator+(difference_type difference,
            const buffers_iterator& iter) noexcept {
    buffers_iterator tmp(iter);
    tmp += difference;
    return tmp;
  }

  [[nodiscard]] friend constexpr buffers_iterator
  operator-(const buffers_iterator& iter,
            difference_type difference) noexcept {
    buffers_iterator tmp(iter);
    tmp -= difference;
    return tmp;
  }

  [[nodiscard]] constexpr friend difference_type
  operator-(const buffers_iterator& a,
            const buffers_iterator& b) noexcept {
    return a.ptr_ - b.ptr_;
  }

  [[nodiscard]] constexpr friend bool
  operator==(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return a.ptr_ == b.ptr_;
  }

  [[nodiscard]] constexpr friend bool
  operator!=(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return !(a == b);
  }

  [[nodiscard]] constexpr friend bool
  operator<(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return a < b;
  }

  [[nodiscard]] constexpr friend bool
  operator<=(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return !(b < a);
  }

  [[nodiscard]] constexpr friend bool
  operator>(const buffers_iterator& a,
            const buffers_iterator& b) noexcept {
    return a > b;
  }

  [[nodiscard]] constexpr friend bool
  operator>=(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return !(a < b);
  }

 private:
  pointer ptr_ = nullptr;
};

template<>
class buffers_iterator<const_buffer, std::uint8_t> {
 private:
  using byte_type = std::uint8_t;
  using buffer_type = const_buffer;
  using buffer_sequence_iterator_type = const const_buffer*;

 public:
  using difference_type = std::ptrdiff_t;
  using value_type = byte_type;
  using pointer = const byte_type*;
  using reference = const byte_type&;

  using iterator_category = std::random_access_iterator_tag;

  constexpr
  buffers_iterator() noexcept = default;

  constexpr
  explicit buffers_iterator(const buffer_type& buf) noexcept
    : ptr_{static_cast<pointer>(buf.data())} {}

  constexpr
  explicit buffers_iterator(const mutable_buffer& buf) noexcept
    : ptr_{static_cast<pointer>(buf.data())} {}

  [[nodiscard]] static constexpr buffers_iterator
  begin(const buffer_type& buffers) noexcept {
    return buffers_iterator{buffers};
  }

  [[nodiscard]] static constexpr buffers_iterator
  end(const buffer_type& buffers) noexcept {
    return buffers_iterator{buffer_type{static_cast<pointer>(buffers.data()) +
                            buffers.size(),
                            0}};
  }

  [[nodiscard]] constexpr reference
  operator*() const noexcept {
    return *ptr_;
  }

  [[nodiscard]] constexpr pointer
  operator->() const noexcept {
    return ptr_;
  }

  [[nodiscard]] constexpr reference
  operator[](difference_type difference) const noexcept {
    return *(ptr_ + difference);
  }

  constexpr buffers_iterator&
  operator++() noexcept {
    ptr_ += 1;
    return *this;
  }

  constexpr buffers_iterator
  operator++(int) noexcept {
    buffers_iterator tmp(*this);
    ++*this;
    return tmp;
  }

  constexpr buffers_iterator&
  operator--() noexcept {
    ptr_ -= 1;
    return *this;
  }

  constexpr buffers_iterator
  operator--(int) noexcept {
    buffers_iterator tmp(*this);
    --*this;
    return tmp;
  }

  constexpr buffers_iterator&
  operator+=(difference_type difference) noexcept {
    ptr_ += difference;
    return *this;
  }

  constexpr buffers_iterator&
  operator-=(difference_type difference) noexcept {
    ptr_ -= difference;
    return *this;
  }

  [[nodiscard]] constexpr friend buffers_iterator
  operator+(const buffers_iterator& iter,
            difference_type difference) noexcept {
    buffers_iterator tmp(iter);
    tmp += difference;
    return tmp;
  }

  [[nodiscard]] constexpr friend buffers_iterator
  operator+(difference_type difference,
            const buffers_iterator& iter) noexcept {
    buffers_iterator tmp(iter);
    tmp += difference;
    return tmp;
  }

  [[nodiscard]] friend constexpr buffers_iterator
  operator-(const buffers_iterator& iter,
            difference_type difference) noexcept {
    buffers_iterator tmp(iter);
    tmp -= difference;
    return tmp;
  }

  [[nodiscard]] constexpr friend difference_type
  operator-(const buffers_iterator& a,
            const buffers_iterator& b) noexcept {
    return a.ptr_ - b.ptr_;
  }

  [[nodiscard]] constexpr friend bool
  operator==(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return a.ptr_ == b.ptr_;
  }

  [[nodiscard]] constexpr friend bool
  operator!=(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return !(a == b);
  }

  [[nodiscard]] constexpr friend bool
  operator<(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return a < b;
  }

  [[nodiscard]] constexpr friend bool
  operator<=(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return !(b < a);
  }

  [[nodiscard]] constexpr friend bool
  operator>(const buffers_iterator& a,
            const buffers_iterator& b) noexcept {
    return a > b;
  }

  [[nodiscard]] constexpr friend bool
  operator>=(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return !(a < b);
  }

 private:
  pointer ptr_ = nullptr;
};

/**
 * Added by me [end]
 */

template<typename BufferSequence>
[[nodiscard]] constexpr buffers_iterator<BufferSequence>
buffers_begin(const BufferSequence& buffers) noexcept {
  return buffers_iterator<BufferSequence>::begin(buffers);
}

template<typename BufferSequence>
[[nodiscard]] constexpr buffers_iterator<BufferSequence>
buffers_end(const BufferSequence& buffers) noexcept {
  return buffers_iterator<BufferSequence>::end(buffers);
}

template<typename>
struct is_buffer_iterator : std::false_type{};

template<typename BufferSequence, typename ByteType>
struct is_buffer_iterator<
  buffers_iterator<BufferSequence, ByteType>>
    : std::true_type{};

template<typename T>
static constexpr bool
is_buffers_iterator_v = is_buffer_iterator<T>::value;

template<typename BufferIterator>
struct is_multiple_buffer_iterator : std::true_type{};

template<typename BS, typename BT>
struct is_multiple_buffer_iterator<
  buffers_iterator<BS, BT>> : std::bool_constant<
    !(std::is_convertible_v<std::decay_t<BS>, const_buffer> ||
    std::is_convertible_v<std::decay_t<BS>, mutable_buffer>)>{};

template<typename T>
static constexpr bool
is_multiple_buffer_iterator_v = is_multiple_buffer_iterator<T>::value;

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_BUFFERS_ITERATOR_HPP_
