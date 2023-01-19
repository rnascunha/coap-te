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

namespace coap_te {

/**
 * @ingroup buffer
 * @{
 */

class mutable_buffer {
 public:
  using size_type = std::size_t;

  constexpr
  mutable_buffer() noexcept = default;

  constexpr
  mutable_buffer(void* data, size_type size) noexcept
    : data_(data), size_(size)
  {}

  constexpr mutable_buffer&
  operator+=(size_type n) noexcept {
    size_type offset = n < size_ ? n : size_;
    data_ = static_cast<std::uint8_t*>(data_) + offset;
    size_ -= offset;
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

  // friends
  [[nodiscard]] friend constexpr mutable_buffer
  operator+(const mutable_buffer& buf, size_type n) noexcept {
    mutable_buffer b{buf};
    b += n;
    return b;
  }

  [[nodiscard]] friend constexpr mutable_buffer
  operator+(size_type n, const mutable_buffer& buf) noexcept {
    return buf + n;
  }

 private:
  void* data_ = nullptr;
  size_type size_ = 0;
};

/** @} */  // end of buffer

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_MUTABLE_BUFFER_HPP_
