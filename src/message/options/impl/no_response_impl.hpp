#ifndef COAP_TE_MESSAGE_OPTIONS_NO_REPONSE_IMPL_HPP__
#define COAP_TE_MESSAGE_OPTIONS_NO_REPONSE_IMPL_HPP__

#include "../no_response.hpp"
#include "../functions2.hpp"
#include "../options.hpp"

namespace CoAP{
namespace Message{
namespace Option{

#if COAP_TE_OPTION_NO_RESPONSE == 1

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

#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

}//Option
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_OPTIONS_NO_REPONSE_IMPL_HPP__ */
