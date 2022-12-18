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

#include <utility>

#include "coap-te/core/traits.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/serialize.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/option_list_func.hpp"

namespace coap_te {
namespace message {

template<typename OptionList,
         typename ConstBufferToken = coap_te::const_buffer,
         typename ConstBufferPayload = coap_te::const_buffer>
class message {
 public:
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBufferToken>,
                "Must be const buffer type");
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBufferPayload>,
                "Must be const buffer type");
  // static_assert(is_option_list_type_v<OptionList>,
  //              "Must be option list type")

  using option_list_type = OptionList;
  using token_type = ConstBufferToken;
  using payload_type = ConstBufferPayload;
  using const_iterator = typename OptionList::const_iterator;

  message() = default;
  message(type tp, code co, const token_type& token = token_type{})
    : type_{tp}, code_{co}, token_{token} {}

  // Getters
  // Header
  [[nodiscard]] constexpr type
  get_type() const noexcept {
    return type_;
  }

  [[nodiscard]] constexpr code
  get_code() const noexcept {
    return code_;
  }

  [[nodiscard]] constexpr const token_type&
  token() const noexcept {
    return token_;
  }

  [[nodiscard]] constexpr message_id
  mid() const noexcept {
    return mid_;
  }

  // Option
  [[nodiscard]] constexpr const option_list_type&
  option_list() const noexcept {
    return opt_list_;
  }

  [[nodiscard]] constexpr option_list_type&
  option_list() noexcept {
    return opt_list_;
  }

  [[nodiscard]] constexpr std::size_t
  count_options() const noexcept {
    return options::count(opt_list_);
  }

  [[nodiscard]] const_iterator
  begin() const noexcept {
    return opt_list_.begin();
  }

  [[nodiscard]] const_iterator
  end() const noexcept {
    return opt_list_.end();
  }

  // Payload
  [[nodiscard]] constexpr const payload_type&
  payload() const noexcept {
    return payload_;
  }

  [[nodiscard]] constexpr std::size_t
  size() const noexcept {
    return minimum_header_size +             // header
           token_.size() +                   // token
           options::size(opt_list_) +        // options
           (payload_.size() != 0 ? 1 : 0) +  // payload_marker
           payload_.size();                  // payload
  }

  // Setters
  constexpr message&
  header(type tp, code co) noexcept {
    type_ = tp;
    code_ = co;
    return *this;
  }

  constexpr message&
  set(type tp) noexcept {
    type_ = tp;
    return *this;
  }

  constexpr message&
  set(code tp) noexcept {
    code_ = tp;
    return *this;
  }

  constexpr message&
  mid(message_id mmid) noexcept {
    mid_ = mmid;
    return *this;
  }

  constexpr message&
  token(const token_type& tk) noexcept {
    token_ = {tk.data(), (std::min)(tk.size(), max_token_size)};
    return *this;
  }

  constexpr message&
  options(const option_list_type& opt_list) noexcept {
    opt_list_ = opt_list;
    return *this;
  }

  template<typename Option>
  constexpr message&
  add_option(Option&& op) noexcept {
    // static_assert(is_option_type_v<Option>,
    //               "Must be option type")
    options::insert(opt_list_, std::forward<Option>(op));
    return *this;
  }

  constexpr message&
  payload(const payload_type& pl) noexcept {
    payload_ = pl;
    return *this;
  }

  template<typename MutableBuffer>
  std::size_t serialize(message_id mid,
              MutableBuffer& output,      // NOLINT
              std::error_code& ec) const noexcept {   // NOLINT
    return coap_te::message::serialize(
                    type_, code_, mid, token_,
                    opt_list_, payload_, output,
                    ec);
  }

  template<typename MutableBuffer>
  std::size_t serialize(message_id mid,
                        MutableBuffer& output) const {  // NOLINT
    return coap_te::message::serialize(
                    type_, code_, mid, token_,
                    opt_list_, payload_, output);
  }

 private:
  type              type_ = type::reset;
  code              code_ = code::empty;
  message_id        mid_ = 0;
  token_type        token_{};
  option_list_type  opt_list_{};
  payload_type      payload_{};
};

}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_MESSAGE_HPP_
