#ifndef COAP_TE_OBSERVE_FUNCTIONS_IMPL_HPP__
#define COAP_TE_OBSERVE_FUNCTIONS_IMPL_HPP__

#include "../functions.hpp"
#include "../../message/options/parser.hpp"
#include "../../message/options/functions2.hpp"

#include "../../transmission/types.hpp"

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

template<typename Message, unsigned MaxLantency>
bool freshness(order& fresher_value, Message const& msg, bool update /* = true */) noexcept
{
	using namespace CoAP::Message;
	Option::option opt;
	if(!Option::get_option(msg, opt, Option::code::observe))
	{
		return false;
	}
	unsigned value = Option::parse_unsigned(opt);

	CoAP::time_t t = CoAP::time();
	if(t > static_cast<CoAP::time_t>(fresher_value.time + MaxLantency))
	{
		if(update)
		{
			fresher_value.value = value;
			fresher_value.time = t;
		}
		return true;
	}

	if((fresher_value.value < value
			&& (value - fresher_value.value) < max_option_value)
		||
		(fresher_value.value > value
			&& (fresher_value.value - value) > max_option_value))
	{
		if(update)
		{
			fresher_value.value = value;
			fresher_value.time = t;
		}
		return true;
	}
	return false;
}

}//CoAP
}//Observe

#endif /* COAP_TE_OBSERVE_FUNCTIONS_IMPL_HPP__ */
