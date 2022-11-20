#ifndef COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP
#define COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/constants.hpp"

namespace coap_te {
namespace message {
namespace options {

[[nodiscard]] constexpr bool 
is_critical(number op) noexcept {
    return (static_cast<number_type>(op) & 1) != 0;
}

[[nodiscard]] constexpr bool 
is_elective(number op) noexcept {
    return ! (is_critical(op));
}

[[nodiscard]] constexpr bool 
is_unsafe_to_forward(number op) noexcept {
    return (static_cast<number_type>(op) & 0b10) != 0;
}

[[nodiscard]] constexpr bool 
is_safe_to_forward(number op) noexcept {
    return ! (is_unsafe_to_forward(op));
}

[[nodiscard]] constexpr bool
is_no_cache_key(number op) noexcept {
    return (static_cast<number_type>(op) & no_cache_key) == no_cache_key;
}

}//coap_te   
}//message
}//options

#endif /* COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_HPP */