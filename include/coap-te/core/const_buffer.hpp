/**
 * @file const_buffer.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_CORE_CONST_BUFFER_HPP_
#define COAP_TE_CORE_CONST_BUFFER_HPP_

#include <cstdint>
#include <iterator>
#include <string_view>

#include "coap-te/core/traits.hpp"
#include "coap-te/core/mutable_buffer.hpp"

namespace coap_te {

/** 
 * @defgroup buffer Buffer
 * Defines buffer view functions
 * 
 * @ingroup buffer
 * @{
 */

class const_buffer {
 public:
  using value_type = std::uint8_t;
  using pointer = const value_type*;
  using reference = const value_type&;
  using size_type = std::size_t;

  class const_iterator {
   public:
    using value_type = const_buffer::value_type;
    using reference = const_buffer::reference;
    using pointer = const_buffer::pointer;
    using iterator_category = std::random_access_iterator_tag;

    constexpr
    explicit const_iterator(pointer ptr) noexcept
      : ptr_(ptr)
    {}

    constexpr
    explicit const_iterator(const const_iterator& other) noexcept = default;

    bool operator==(const const_iterator& rhs) const noexcept {
      return ptr_ == rhs.ptr_;
    }

    bool operator!=(const const_iterator& rhs) const noexcept {
      return !(ptr_ == rhs.ptr_);
    }

    const_iterator& operator+=(int n) noexcept {
      ptr_ += n;
      return *this;
    }

    const_iterator operator+(int n) const noexcept {
      return const_iterator(ptr_ + n);
    }

    const_iterator& operator=(const const_iterator& rhs) noexcept {
      ptr_ = rhs.ptr_;
      return *this;
    }

    const_iterator& operator++() noexcept {
      ++ptr_;
      return *this;
    }

    const_iterator& operator--() noexcept {
      --ptr_;
      return *this;
    }

    const_iterator operator++(int) noexcept {
      pointer temp = ptr_;
      ++ptr_;
      return const_iterator(temp);
    }

    const_iterator operator--(int) noexcept {
      pointer temp = ptr_;
      --ptr_;
      return const_iterator(temp);
    }

    reference operator*() const noexcept {
      return *ptr_;
    }

   private:
    pointer ptr_;
  };

  constexpr
  const_buffer() noexcept = default;

  constexpr
  const_buffer(const const_buffer& buf) = default;

  constexpr
  const_buffer(const void* data, std::size_t size) noexcept
    : data_(data), size_(size)
  {}

  constexpr
  const_buffer(const void* data,
               std::size_t size,
               std::size_t type_size) noexcept
    : const_buffer(data, size * type_size)
  {}

  constexpr
  const_buffer(const mutable_buffer& buf) noexcept   //NOLINT
    : const_buffer(buf.data(), buf.size())
  {}

  template<typename T>
  constexpr
  explicit const_buffer(const T& container) noexcept
    : const_buffer(container.data(),
                   container.size(),
                   sizeof(typename T::value_type)) {
    static_assert(core::is_const_buffer_v<T>, "Is not buffer type");
  }

  constexpr
  explicit const_buffer(const char* data) noexcept
    : const_buffer(std::string_view(data))
  {}

  template<std::size_t N, typename T>
  constexpr
  explicit const_buffer(const T (&arr)[N]) noexcept
    : const_buffer(arr, N, sizeof(T))
  {}

  constexpr value_type
  operator[](std::size_t n) const noexcept {
    return static_cast<pointer>(data_)[n];
  }

  constexpr const_buffer&
  operator+=(std::size_t n) noexcept {
    data_ = static_cast<pointer>(data_) + n;
    size_ -= n;
    return *this;
  }

  // copy assignment
  constexpr const_buffer&
  operator=(const const_buffer& other) noexcept {
    data_ = other.data_;
    size_ = other.size_;

    return *this;
  }

  constexpr void
  set(const void* data, std::size_t size) noexcept {
    data_ = data;
    size_ = size;
  }

  constexpr
  const void* data() const noexcept {
    return data_;
  }

  constexpr
  size_type size() const noexcept {
    return size_;
  }

  constexpr
  const_iterator begin() const noexcept {
    return const_iterator(static_cast<pointer>(data_));
  }

  constexpr
  const_iterator end() const noexcept {
    return const_iterator(static_cast<pointer>(data_) + size_);
  }

  constexpr
  const_iterator cbegin() const noexcept {
    return const_iterator(static_cast<pointer>(data_));
  }

  constexpr
  const_iterator cend() const noexcept {
    return const_iterator(static_cast<pointer>(data_) + size_);
  }

 private:
  const void* data_ = nullptr;
  size_type size_ = 0;
};

/** @} */  // end of buffer

}  // namespace coap_te

#endif  // COAP_TE_CORE_CONST_BUFFER_HPP_
