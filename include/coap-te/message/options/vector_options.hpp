/**
 * @file vector_options.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_VECTOR_OPTIONS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_VECTOR_OPTIONS_HPP_

#include <system_error>   // NOLINT
#include <iostream>

#include "coap-te/core/traits.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/parse.hpp"

namespace coap_te {
namespace message {
namespace options {

class vector_options {
 public:
  using value_type = option;

  class const_iterator {
   public:
    using value_type = vector_options::value_type;

    constexpr
    const_iterator(const const_iterator& buf) noexcept = default;

    constexpr
    explicit const_iterator(const const_buffer& buf) noexcept
      : buf_(buf) {}

    constexpr
    const_iterator(number_type prev, const const_buffer& buf) noexcept
      : prev_(prev), buf_(buf) {}

    const_iterator&
    operator++() noexcept {
      value_type op;
      std::error_code ec;
      parse(prev_, buf_, op, ec);
      prev_ = static_cast<number_type>(op.option_number());

      return *this;
    }

    const_iterator
    operator++(int) noexcept {
      const_iterator temp(*this);
      this->operator++();
      return temp;
    }

    [[nodiscard]] constexpr bool
    is_end() noexcept {
      return buf_[0] == coap_te::message::payload_marker ||
             buf_.size() == 0;
    }

    [[nodiscard]] constexpr
    operator bool() noexcept {
      return is_end();
    }

    [[nodiscard]] value_type
    operator*() noexcept {
      value_type op;
      std::error_code ec;
      coap_te::const_buffer copy_buf(buf_);
      parse(prev_, copy_buf, op, ec);

      return op;
    }

    [[nodiscard]] constexpr const const_buffer&
    buffer() const noexcept {
      return buf_;
    }

   private:
    number_type prev_ = invalid;
    coap_te::const_buffer buf_;
  };

  // constexpr
  // explicit vector_options(const const_buffer& buf) noexcept
  //   : buf_(buf.data(), compute_option_list_size(buf)) {}
  explicit vector_options(const const_buffer& buf) noexcept {
    auto [size, ec] = option_list_size(buf);
    if (ec) {
      return;
    }
    buf_.set(buf.data(), size);
  }

  // iterator interface
  [[nodiscard]] const_iterator
  begin() const noexcept {
    return const_iterator(buf_);
  }

  [[nodiscard]] const_iterator
  end() const noexcept {
    return const_iterator(
      const_buffer((const std::uint8_t*)buf_.data() + buf_.size(), 0));
  }

 private:
  coap_te::const_buffer buf_;
};

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_VECTOR_OPTIONS_HPP_
