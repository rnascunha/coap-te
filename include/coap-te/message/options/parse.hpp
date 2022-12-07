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
#include <system_error>   // NOLINT

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/checks.hpp"
#include "coap-te/message/options/option.hpp"

namespace coap_te {
namespace message {
namespace options {

template<typename CheckOptions = check_all,
         typename ConstBuffer>
std::size_t parse(number_type before,
                  ConstBuffer& input,                // NOLINT
                  number_type& current,              // NOLINT
                  ConstBuffer& output,               // NOLINT
                  std::error_code& ec) noexcept;     // NOLINT

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/parse.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_PARSE_HPP_
