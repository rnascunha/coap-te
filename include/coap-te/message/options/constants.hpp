#ifndef COAP_TE_MESSAGE_OPTIONS_CONSTANTS_HPP
#define COAP_TE_MESSAGE_OPTIONS_CONSTANTS_HPP

#include "coap-te/message/options/config.hpp"

namespace coap_te {
namespace message {
namespace options {

static constexpr const number_type no_cache_key = 0b11100;

static constexpr const unsigned max_age_default = 60; //seconds
#if COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1
static constexpr const suppress suppress_default = suppress::none;
#endif /* COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1 */

}//options
}//message
}//coap_te

#endif /* COAP_TE_MESSAGE_OPTIONS_CONSTANTS_HPP */