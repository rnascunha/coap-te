#ifndef COAP_TE_MESSAGE_OPTIONS_NO_REPONSE_IMPL_HPP__
#define COAP_TE_MESSAGE_OPTIONS_NO_REPONSE_IMPL_HPP__

#include "../no_response.hpp"
#include "../functions2.hpp"
#include "../options.hpp"

#include "../template_class.hpp"

namespace CoAP{
namespace Message{
namespace Option{

#if COAP_TE_OPTION_NO_RESPONSE == 1

suppress operator|(suppress lhs, suppress rhs) noexcept
{
    return static_cast<suppress>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

bool operator&(suppress lhs, suppress rhs) noexcept
{
	return (static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs)) != 0;
}

bool no_response_suppress(suppress value, CoAP::Message::code mcode) noexcept
{
	if(value == suppress::none)
		return false;
	if(CoAP::Message::is_success(mcode)
		&& (value & suppress::success))
		return true;
	if(CoAP::Message::is_client_error(mcode)
		&& (value & suppress::client_error))
		return true;
	if(CoAP::Message::is_server_error(mcode)
		&& (value & suppress::server_error))
		return true;

	return false;
}

template<typename Message>
suppress get_suppress_value(Message const& msg) noexcept
{
	suppress value = suppress::none;
	option opt;
	if(get_option(msg, opt, Option::code::no_response))
	{
		value = static_cast<suppress>(parse_unsigned(opt));
	}

	return value;
}

template<typename Message>
bool no_response_suppress(Message const& msg, CoAP::Message::code mcode) noexcept
{
	suppress value = get_suppress_value(msg);
	return no_response_suppress(value, mcode);
}

void create(option_template<Option::code>& option,
		suppress& value) noexcept
{
	option.ocode = code::no_response;
	option.length = value == suppress::none ? 0 : 1;
	option.value = &value;
}

#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

}//Option
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_OPTIONS_NO_REPONSE_IMPL_HPP__ */
