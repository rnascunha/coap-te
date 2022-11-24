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

#include <cstring>    // std::strlen
#include <iterator>

#include "coap-te/core/traits.hpp"

namespace coap_te {

class const_buffer {
 public:
  using value_type = unsigned char;
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

    const_iterator operator+(int n) noexcept {
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

  template<typename T>
  constexpr
  const_buffer(const T* data, std::size_t size) noexcept
    : data_(data), size_(size * sizeof(T))
  {}

  constexpr
  explicit const_buffer(const char* data) noexcept
    : data_(data), size_(std::strlen(data))
  {}

  template<typename T>
  constexpr
  explicit const_buffer(const T& container) noexcept
    : data_(container.data()),
      size_(container.size() * sizeof(typename T::value_type)) {
    static_assert(core::is_buffer_type_v<T>, "Is not buffer type");
  }

  template<std::size_t N, typename T>
  constexpr
  explicit const_buffer(const T (&arr)[N]) noexcept
    : data_(arr), size_(N * sizeof(T))
  {}

  value_type operator[](std::size_t n) const noexcept {
    return static_cast<pointer>(data_)[n];
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

}  // namespace coap_te

#endif  // COAP_TE_CORE_CONST_BUFFER_HPP_
