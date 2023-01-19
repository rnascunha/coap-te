/**
 * @file parse.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_PARSE_HPP_
#define COAP_TE_MESSAGE_OPTIONS_PARSE_HPP_

#include <cstdint>
#include <utility>

#include "coap-te/core/error.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/checks.hpp"
#include "coap-te/message/options/option.hpp"

namespace coap_te {
namespace message {
namespace options {

template<typename CheckOptions = check_all>
constexpr std::size_t
parse(number_type before,
      const const_buffer& data,
      number_type& current,                 // NOLINT
      const_buffer& output,                 // NOLINT
      coap_te::error_code& ec) noexcept;    // NOLINT

template<typename CheckOptions = check_all,
         typename Option>
constexpr std::size_t
parse(number_type before,
      const const_buffer& input,
      Option& output,                       // NOLINT
      coap_te::error_code& ec) noexcept;    // NOLINT

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/parse.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_PARSE_HPP_
