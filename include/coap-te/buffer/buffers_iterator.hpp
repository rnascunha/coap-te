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

/*******************************/
// added by me
template<typename T>
struct is_mutable_buffer : std::bool_constant<
  std::is_convertible_v<
        typename T::value_type,
        mutable_buffer>
  >{};

template<typename T>
struct is_mutable_buffer<T[]> : std::bool_constant<
  std::is_convertible_v<
        std::remove_all_extents_t<T>,
        mutable_buffer>
  >{};

template<typename T, std::size_t N>
struct is_mutable_buffer<T[N]> : std::bool_constant<
  std::is_convertible_v<
        std::remove_all_extents_t<T>,
        mutable_buffer>
  >{};

template<typename T>
static constexpr bool
is_mutable_buffer_v = is_mutable_buffer<T>::value;

template<typename T>
struct get_const_iterator {
    using type = typename T::const_iterator;
};

template<typename T>
struct get_const_iterator<T[]> {
    using type = const std::remove_all_extents_t<T[]>*;
};

template<typename T, std::size_t N>
struct get_const_iterator<T[N]> {
    using type = const std::remove_all_extents_t<T[N]>*;
};

template<typename T>
using get_const_iterator_t = typename get_const_iterator<T>::type;

/*******************************/

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
  is_mutable = is_mutable_buffer_v<BufferSequence>;         // added by me
  using helper = buffers_iterator_types_helper<is_mutable>;
  using buffer_type = typename helper::buffer_type;
  using byte_type = typename helper::template byte_type<ByteType>::type;
  using const_iterator = get_const_iterator_t<BufferSequence>;    // added by me
};

template<typename ByteType>
struct buffers_iterator_types<mutable_buffer, ByteType> {
  static constexpr bool
  is_mutable = true;                   // added by me
  using buffer_type = mutable_buffer;
  using byte_type = ByteType;
  using const_iterator = const mutable_buffer*;
};

template<typename ByteType>
struct buffers_iterator_types<const_buffer, ByteType> {
  static constexpr bool
  is_mutable = false;                   // added by me
  using buffer_type = const_buffer;
  using byte_type = std::add_const_t<ByteType>;
  using const_iterator = const const_buffer*;
};

}  // namespace detail

template<typename BufferSequence,
         typename ByteType = std::uint8_t,
         bool = std::is_convertible_v<
                  std::decay_t<BufferSequence>, const_buffer> ||
                std::is_convertible_v<
                  std::decay_t<BufferSequence>, mutable_buffer>>
class buffers_iterator;

template<typename BufferSequence, typename ByteType>
class buffers_iterator<BufferSequence, ByteType, false> {
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

  [[nodiscard]] constexpr buffer_sequence_iterator_type
  current() noexcept {
    return current_;
  }

  [[nodiscard]] constexpr std::size_t
  current_position() noexcept {
    return current_buffer_position_;
  }

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
template<typename Buffer, typename ByteType>
class buffers_iterator<Buffer, ByteType, true> {
 private:
  using buffer_type = typename detail::buffers_iterator_types<
                        Buffer, ByteType>::buffer_type;
  using buffer_sequence_iterator_type =
                      typename detail::buffers_iterator_types<
                        Buffer, ByteType>::const_iterator;

 public:
  using difference_type = std::ptrdiff_t;
  using value_type = ByteType;
  using pointer = typename detail::buffers_iterator_types<
                    Buffer, ByteType>::byte_type*;
  using reference = typename detail::buffers_iterator_types<
                    Buffer, ByteType>::byte_type&;

  using iterator_category = std::random_access_iterator_tag;

  constexpr
  buffers_iterator() noexcept = default;

  constexpr
  explicit buffers_iterator(const buffer_type& buffer) noexcept
    : buffer_{buffer} {}

  [[nodiscard]] static constexpr buffers_iterator
  begin(const buffer_type& buffers) noexcept {
    return buffers_iterator{buffers};
  }

  [[nodiscard]] static constexpr buffers_iterator
  end(const buffer_type& buffers) noexcept {
    buffers_iterator new_iter{buffers};
    new_iter.position_ = buffers.size();
    return new_iter;
  }

  [[nodiscard]] constexpr buffer_sequence_iterator_type
  current() noexcept {
    return buffer_sequence_begin(buffer_);
  }

  [[nodiscard]] constexpr std::size_t
  current_position() noexcept {
    return position_;
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
    if (position_ != buffer_.size())
      ++position_;
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
    if (position_ != 0)
      --position_;
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
    return a.position_ - b.position_;
  }

  [[nodiscard]] constexpr friend bool
  operator==(const buffers_iterator& a,
             const buffers_iterator& b) noexcept {
    return a.position_ == b.position_;
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
  [[nodiscard]] constexpr reference
  dereference() const noexcept {
    return static_cast<pointer>(
      buffer_.data())[position_];
  }

  constexpr void
  advance(difference_type n) noexcept {
    position_ += n;
    if (position_ < 0)
      position_ = 0;
    else if (position_ > buffer_.size())
      position_ = buffer_.size();
  }

  buffer_type buffer_;
  std::size_t position_ = 0;
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

/**
 * @brief 
 * 
 * @tparam ypename 
 */
template<typename>
struct is_buffers_iterator : std::false_type{};

template<typename BufferSequence, typename ByteType>
struct is_buffers_iterator<
  buffers_iterator<BufferSequence, ByteType>>
    : std::true_type{};

template<typename T>
static constexpr bool
is_buffers_iterator_v = is_buffers_iterator<T>::value;

/**
 * @brief 
 * 
 * @tparam ypename 
 */
template<typename>
struct is_mutable_buffers_iterator : std::false_type{};

template<typename BufferSequence, typename ByteType>
struct is_mutable_buffers_iterator<buffers_iterator<BufferSequence, ByteType>>
    : std::bool_constant<
        detail::buffers_iterator_types<BufferSequence, ByteType>::is_mutable>{};

template<typename T>
static constexpr bool
is_mutable_buffers_iterator_v = is_mutable_buffers_iterator<T>::value;

/**
 * @brief 
 * 
 * @tparam ypename 
 */
template<typename>
struct is_const_buffers_iterator : std::false_type{};

template<typename BufferSequence, typename ByteType>
struct is_const_buffers_iterator<buffers_iterator<BufferSequence, ByteType>>
    : std::bool_constant<
        !detail::buffers_iterator_types<
          BufferSequence, ByteType>::is_mutable>{};

template<typename T>
static constexpr bool
is_const_buffers_iterator_v = is_const_buffers_iterator<T>::value;

/**
 * @brief 
 * 
 * @tparam BufferIterator 
 */
template<typename BufferIterator>
struct is_multiple_buffers_iterator : std::bool_constant<
  is_buffers_iterator_v<BufferIterator>>{};

template<typename BS, typename BT>
struct is_multiple_buffers_iterator<
  buffers_iterator<BS, BT>> : std::bool_constant<
    !(std::is_convertible_v<std::decay_t<BS>, const_buffer> ||
    std::is_convertible_v<std::decay_t<BS>, mutable_buffer>)>{};

template<typename T>
static constexpr bool
is_multiple_buffers_iterator_v = is_multiple_buffers_iterator<T>::value;

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_BUFFERS_ITERATOR_HPP_
