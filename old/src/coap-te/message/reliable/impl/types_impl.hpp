#ifndef COAP_TE_MESSAGES_RELIABLE_TYPES_IMPL_HPP__
#define COAP_TE_MESSAGES_RELIABLE_TYPES_IMPL_HPP__

#include "../types.hpp"
#include "../../options/options.hpp"

namespace CoAP{
namespace Message{
namespace Reliable{

#if COAP_TE_ENABLE_STREAM_CONNECTION == 1

template<CoAP::Message::code Code>
struct option_type
{
	using type = CoAP::Message::Option::code;
};

template<>
struct option_type<CoAP::Message::code::csm>
{
	using type = CoAP::Message::Option::csm;
};

template<>
struct option_type<CoAP::Message::code::ping>
{
	using type = CoAP::Message::Option::ping_pong;
};

template<>
struct option_type<CoAP::Message::code::pong>
{
	using type = CoAP::Message::Option::ping_pong;
};

template<>
struct option_type<CoAP::Message::code::release>
{
	using type = CoAP::Message::Option::release;
};

template<>
struct option_type<CoAP::Message::code::abort>
{
	using type = CoAP::Message::Option::abort;
};

#endif /* COAP_TE_ENABLE_STREAM_CONNECTION == 1 */

}//Reliable
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGES_RELIABLE_TYPES_IMPL_HPP__ */
