/**
 * @file token.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_TOKEN_HPP_
#define COAP_TE_MESSAGE_TOKEN_HPP_

#include <algorithm>    // std::min
#include <string_view>
#include <cstring>
#include <cstdint>

#include "coap-te/core/traits.hpp"
#include "coap-te/message/config.hpp"

namespace coap_te {
namespace message {

[[nodiscard]] inline constexpr std::size_t
token_size(std::size_t size) noexcept {
  return (std::min)(size, max_token_size);
}

class token_view {
 public:
  token_view() = default;
  token_view(const token_view&) = default;

  constexpr
  token_view(const void* data, std::size_t size) noexcept
    : size_(token_size(size)), data_(data) {}

  constexpr
  token_view(const std::string_view& str) noexcept    // NOLINT
    : size_(token_size(str.size())), data_(str.data()) {}

  template<typename ConstBuffer>
  constexpr token_view&
  operator=(const ConstBuffer& other) noexcept {
    static_assert(coap_te::core::is_const_buffer_type_v<ConstBuffer>,
                  "Must be const buffer type");
    size_ = token_size(other.size());
    data_ = other.data();
    return *this;
  }

  template<typename ConstBuffer>
  constexpr bool
  operator==(const ConstBuffer& other) const noexcept {
    static_assert(coap_te::core::is_const_buffer_type_v<ConstBuffer>,
                  "Must be const buffer type");
    if (size_ != other.size())
      return false;

    if (std::memcmp(data_, other.data(), size_) != 0)
      return false;

    return true;
  }

 private:
  std::size_t size_ = 0;
  const void* data_ = nullptr;
};

class token {
 public:
  token() = default;

  constexpr
  token(const void* data, std::size_t size) noexcept
    : size_(token_size(size)) {
      std::memcpy(data_, data, size_);
  }

  constexpr
  token(const std::string_view& str) noexcept       // NOLINT
    : size_(token_size(str.size())) {
      std::memcpy(data_, str.data(), size_);
  }

  template<typename ConstBuffer>
  constexpr token&
  operator=(const ConstBuffer& other) noexcept {
    static_assert(coap_te::core::is_const_buffer_type_v<ConstBuffer>,
                  "Must be const buffer type");
    size_ = token_size(other.size());
    std::memcpy(data_, other.data(), size_);
    return *this;
  }

  template<typename ConstBuffer>
  constexpr bool
  operator==(const ConstBuffer& other) const noexcept {
    static_assert(coap_te::core::is_const_buffer_type_v<ConstBuffer>,
                  "Must be const buffer type");
    if (size_ != other.size())
      return false;

    if (std::memcmp(data_, other.data(), size_) != 0)
      return false;

    return true;
  }

 private:
  std::size_t size_ = 0;
  std::uint8_t data_[max_token_size]{};   // NOLINT
};

}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_TOKEN_HPP_
