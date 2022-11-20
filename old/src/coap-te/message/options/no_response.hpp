#ifndef COAP_TE_MESSAGE_OPTIONS_NO_RESPONSE_HPP__
#define COAP_TE_MESSAGE_OPTIONS_NO_RESPONSE_HPP__

#include "../codes.hpp"
#include "types.hpp"
#include "template_class.hpp"
#include "../../defines/defaults.hpp"

namespace CoAP{
namespace Message{
namespace Option{

#if COAP_TE_OPTION_NO_RESPONSE == 1

suppress operator|(suppress lhs, suppress rhs) noexcept;
bool operator&(suppress lhs, suppress rhs) noexcept;

template<typename Message>
suppress get_suppress_value(Message const&) noexcept;

bool no_response_suppress(suppress, CoAP::Message::code) noexcept;
template<typename Message>
bool no_response_suppress(Message const&, CoAP::Message::code) noexcept;

void create(option_template<Option::code>& option, suppress const& value) noexcept;

#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

}//Option
}//Message
}//CoAP

#include "impl/no_response_impl.hpp"

#endif /* COAP_TE_MESSAGE_OPTIONS_NO_RESPONSE_HPP__ */
