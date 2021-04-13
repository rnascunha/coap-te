#ifndef COAP_TE_OBSERVE_FUNCTIONS_IMPL_HPP__
#define COAP_TE_OBSERVE_FUNCTIONS_IMPL_HPP__

#include "../functions.hpp"
#include "message/options/parser.hpp"
#include "message/options/functions2.hpp"

#include "transmission/types.hpp"

namespace CoAP{
namespace Observe{

template<typename Message>
message_status process_message(Message const& msg) noexcept
{
	using namespace CoAP::Message;
	Option::option opt;
	if(!Option::get_option(msg, opt, Option::code::observe))
	{
		return message_status::not_found;
	}

	unsigned value = Option::parse_unsigned(opt);
	if(value == reg) return message_status::register_;
	return message_status::deregister;
}

}//CoAP
}//Observe

#endif /* COAP_TE_OBSERVE_FUNCTIONS_IMPL_HPP__ */
