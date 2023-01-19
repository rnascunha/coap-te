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
#ifndef COAP_TE_BUFFER_CONST_BUFFER_HPP_
#define COAP_TE_BUFFER_CONST_BUFFER_HPP_

#include <cstdint>

#include "coap-te/buffer/mutable_buffer.hpp"

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
  using size_type = std::size_t;

  constexpr
  const_buffer() noexcept = default;

  constexpr
  const_buffer(const void* data, size_type size) noexcept
    : data_(data), size_(size) {}

  constexpr
  const_buffer(const mutable_buffer& buf) noexcept   //NOLINT
    : const_buffer(buf.data(), buf.size()) {}

  constexpr const_buffer&
  operator+=(size_type n) noexcept {
    size_type offset = n < size_ ? n : size_;
    data_ = static_cast<const std::uint8_t*>(data_) + offset;
    size_ -= offset;
    return *this;
  }

  [[nodiscard]] constexpr const void*
  data() const noexcept {
    return data_;
  }

  [[nodiscard]] constexpr size_type
  size() const noexcept {
    return size_;
  }

  // friends
  [[nodiscard]] friend constexpr const_buffer
  operator+(const const_buffer& buf, size_type n) noexcept {
    const_buffer b{buf};
    b += n;
    return b;
  }

  [[nodiscard]] friend constexpr const_buffer
  operator+(size_type n, const const_buffer& buf) noexcept {
    return buf + n;
  }

 private:
  const void* data_ = nullptr;
  size_type size_ = 0;
};

/** @} */  // end of buffer

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_CONST_BUFFER_HPP_
