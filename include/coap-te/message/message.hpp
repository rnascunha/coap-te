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
#include "coap-te/message/options/option.hpp"

namespace coap_te {
namespace message {

template<typename ConstBuffer,
         typename OptionList>
class message {
 public:
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBuffer>,
                "Must be const buffer type");

  message() = default;
  message(type tp, code co, const ConstBuffer& token = ConstBuffer{})
    : type_{tp}, code_{co}, token_{token} {}

  // Getters
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

  [[nodiscard]] constexpr ConstBuffer&
  payload() const noexcept {
    return payload_;
  }

  // Setters
  message& header(type tp, code co) noexcept {
    type_ = tp;
    code_ = co;
    return *this;
  }

  message& set(type tp) noexcept {
    type_ = tp;
    return *this;
  }

  message& set(code tp) noexcept {
    code_ = tp;
    return *this;
  }

  message& token(const ConstBuffer& tk) noexcept {
    token_ = tk;
    return *this;
  }

  template<typename OptionType>
  message& add_option(OptionType op) noexcept {
    opt_list_.add(op);
    return *this;
  }

  message& payload(const ConstBuffer& pl) noexcept {
    payload_ = pl;
    return *this;
  }

  template<typename MutableBuffer>
  std::size_t serialize(message_id mid,
              MutableBuffer& output,      // NOLINT
              std::error_code& ec) const noexcept {   // NOLINT
    return serialize(type_, code_, mid, token_,
                     opt_list_, payload_, output,
                     ec);
  }

  template<typename MutableBuffer>
  std::size_t serialize(message_id mid,
                        MutableBuffer& output) const {  // NOLINT
    return serialize(type_, code_, mid, token_,
                     opt_list_, payload_, output);
  }

 private:
  type          type_ = type::reset;
  code          code_ = code::empty;
  ConstBuffer   token_{};
  OptionList    opt_list_{};
  ConstBuffer   payload_{};
};

}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_MESSAGE_HPP_
