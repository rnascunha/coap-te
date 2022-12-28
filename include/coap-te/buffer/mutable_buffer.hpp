/**
 * @file mutable_buffer.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_BUFFER_MUTABLE_BUFFER_HPP_
#define COAP_TE_BUFFER_MUTABLE_BUFFER_HPP_

#include <cstdint>
#include <iterator>

#include "coap-te/buffer/traits.hpp"

namespace coap_te {

/**
 * @ingroup buffer
 * @{
 */

class mutable_buffer {
 public:
  using value_type = std::uint8_t;
  using pointer = value_type*;
  using reference = value_type&;
  using size_type = std::size_t;

  class iterator {
   public:
    using value_type = mutable_buffer::value_type;
    using reference = mutable_buffer::reference;
    using pointer = mutable_buffer::pointer;
    using iterator_category = std::random_access_iterator_tag;

    constexpr
    explicit iterator(pointer ptr) noexcept
      : ptr_(ptr)
    {}

    constexpr
    explicit iterator(const iterator& other) noexcept = default;

    [[nodiscard]] constexpr bool
    operator==(const iterator& rhs) const noexcept {
      return ptr_ == rhs.ptr_;
    }

    [[nodiscard]] constexpr bool
    operator!=(const iterator& rhs) const noexcept {
      return !(ptr_ == rhs.ptr_);
    }

    [[nodiscard]] constexpr iterator&
    operator+=(int n) noexcept {
      ptr_ += n;
      return *this;
    }

    [[nodiscard]] constexpr iterator
    operator+(int n) const noexcept {
      return iterator(ptr_ + n);
    }

    [[nodiscard]] constexpr iterator&
    operator=(const iterator& rhs) noexcept {
      ptr_ = rhs.ptr_;
      return *this;
    }

    [[nodiscard]] constexpr iterator&
    operator++() noexcept {
      ++ptr_;
      return *this;
    }

    constexpr iterator&
    operator--() noexcept {
      --ptr_;
      return *this;
    }

    constexpr iterator
    operator++(int) noexcept {
      pointer temp = ptr_;
      ++ptr_;
      return iterator(temp);
    }

    constexpr iterator
    operator--(int) noexcept {
      pointer temp = ptr_;
      --ptr_;
      return iterator(temp);
    }

    [[nodiscard]] reference
    operator*() const noexcept {
      return *ptr_;
    }

    [[nodiscard]] reference
    operator*() noexcept {
      return *ptr_;
    }

   private:
    pointer ptr_;
  };

  constexpr
  mutable_buffer(void* data, std::size_t size) noexcept
    : data_(data), size_(size)
  {}

  constexpr
  mutable_buffer(void* data,
               std::size_t size,
               std::size_t type_size) noexcept
    : mutable_buffer(data, size * type_size)
  {}

  template<typename T>
  constexpr
  explicit mutable_buffer(T& container) noexcept      //NOLINT
    : mutable_buffer(container.data(),
                   container.size(),
                   sizeof(typename T::value_type)) {
    static_assert(is_mutable_buffer_v<T>, "Is not buffer type");
  }

  template<std::size_t N, typename T>
  constexpr
  explicit mutable_buffer(T (&arr)[N]) noexcept
    : mutable_buffer(arr, N, sizeof(T))
  {}

  [[nodiscard]] constexpr reference
  operator[](std::size_t n) const noexcept {
    return static_cast<pointer>(data_)[n];
  }

  [[nodiscard]] constexpr reference
  operator[](std::size_t n) noexcept {
    return static_cast<pointer>(data_)[n];
  }

  constexpr mutable_buffer&
  operator+=(std::size_t n) noexcept {
    data_ = static_cast<pointer>(data_) + n;
    size_ -= n;
    return *this;
  }

  [[nodiscard]] constexpr void*
  data() const noexcept {
    return data_;
  }

  [[nodiscard]] constexpr size_type
  size() const noexcept {
    return size_;
  }

  // iterator iterface
  [[nodiscard]] constexpr iterator
  begin() noexcept {
    return iterator(static_cast<pointer>(data_));
  }

  [[nodiscard]] constexpr iterator
  end() noexcept {
    return iterator(static_cast<pointer>(data_) + size_);
  }

  [[nodiscard]] constexpr iterator
  begin() const noexcept {
    return iterator(static_cast<pointer>(data_));
  }

  [[nodiscard]] constexpr iterator
  end() const noexcept {
    return iterator(static_cast<pointer>(data_) + size_);
  }

  [[nodiscard]] constexpr iterator
  cbegin() const noexcept {
    return iterator(static_cast<pointer>(data_));
  }

  [[nodiscard]] constexpr iterator
  cend() const noexcept {
    return iterator(static_cast<pointer>(data_) + size_);
  }

  // friends
  [[nodiscard]] friend constexpr mutable_buffer
  operator+(const mutable_buffer& buf, std::size_t n) noexcept {
    mutable_buffer b{buf};
    b += n;
    return b;
  }

  [[nodiscard]] friend constexpr mutable_buffer
  operator+(std::size_t n, const mutable_buffer& buf) noexcept {
    return buf + n;
  }

 private:
  void* data_ = nullptr;
  size_type size_ = 0;
};

/** @} */  // end of buffer

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_MUTABLE_BUFFER_HPP_
