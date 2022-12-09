/**
 * @file message.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_MESSAGE_HPP_
#define COAP_TE_MESSAGE_MESSAGE_HPP_

#include "coap-te/core/traits.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"

namespace coap_te {
namespace message {

template<typename ConstBuffer,
         typename OptionList>
class message {
 public:
  message() = default;
  message(type tp, code co, token = ConstBuffer{})
    : type_{tp}, code_{co}, token_{token} {}

  [[nodiscard]] constexpr type
  get_type() const noexcept {
    return type_;
  }

  [[nodiscard]] constexpr code
  get_code() const noexcept {
    return code_;
  }

  [[nodiscard]] constexpr ConstBuffer&
  token() const noexcept {
    return token_;
  }

  [[nodiscard]] constexpr message_id&
  mid() const noexcept {
    return mid_;
  }

  [[nodiscard]] constexpr ConstBuffer&
  payload() const noexcept {
    return payload_;
  }

 private:
  type          type_ = type::reset;
  code          code_ = code::empty;
  message_id    mid_ = 0x00;
  ConstBuffer   token_{};
  OptionList    opt_list_{};
  ConstBuffer   payload_{};
};

}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_MESSAGE_HPP_
