#ifndef COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_IMPL_HPP__
#define COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_IMPL_HPP__

#include "../functions.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<typename OptionCode>
bool is_critical(OptionCode code) noexcept { return static_cast<int>(code) & 1; }
template<typename OptionCode>
bool is_unsafe(OptionCode code) noexcept { return static_cast<int>(code) & 2; }
template<typename OptionCode>
bool is_no_cache_key(OptionCode code) noexcept { return (static_cast<int>(code) & 0x1e) == 0x1c; }

}//Option
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_OPTIONS_FUNCTIONS_IMPL_HPP__ */
