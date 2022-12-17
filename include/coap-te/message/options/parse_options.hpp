/**
 * @file parse_options.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_PARSE_OPTIONS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_PARSE_OPTIONS_HPP_

#include <system_error>     // NOLINT

#include "coap-te/core/const_buffer.hpp"

namespace coap_te {
namespace message {
namespace options {

/**
 * @brief 
 * 
 * @tparam OptionList 
 * @param buf 
 * @param list 
 * @param ec 
 * @return std::size_t 
 */
template<typename OptionList>
std::size_t
parse_options(const coap_te::const_buffer& buf,
              OptionList& list,               // NOLINT
              std::error_code& ec) noexcept;  // NOLINT

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/parse_options.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_PARSE_OPTIONS_HPP_
