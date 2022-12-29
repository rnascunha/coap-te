/**
 * @file parse.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_PARSE_HPP_
#define COAP_TE_MESSAGE_PARSE_HPP_

#include "coap-te/core/error.hpp"
#include "coap-te/message/message.hpp"

namespace coap_te {
namespace message {

template<typename ConstBuffer,
         typename Message>
constexpr std::size_t
parse_header(ConstBuffer&,
             Message&,
             coap_te::error_code&) noexcept;

template<typename ConstBuffer,
         typename Message>
constexpr std::size_t
parse(ConstBuffer&,
      Message&,
      coap_te::error_code&) noexcept;

}  // namespace message
}  // namespace coap_te

#include "impl/parse.ipp"

#endif  // COAP_TE_MESSAGE_PARSE_HPP_
