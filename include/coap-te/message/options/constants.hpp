/**
 * @file constants.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief Options constants
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_CONSTANTS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_CONSTANTS_HPP_

#include "coap-te/message/options/config.hpp"

namespace coap_te {
namespace message {
namespace options {

static constexpr const number_type no_cache_key = 0b11100;

static constexpr const unsigned
max_age_default = 60;  //seconds
#if COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1
static constexpr const suppress suppress_default = suppress::none;
#endif /* COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1 */

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_CONSTANTS_HPP_
