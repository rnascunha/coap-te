#ifndef COAP_TE_TRANSMISSION_FUNCTION_IMPL_HPP__
#define COAP_TE_TRANSMISSION_FUNCTION_IMPL_HPP__

#include "../functions.hpp"

namespace CoAP{
namespace Transmission{

template<bool UseInternalBuffer = false,
		typename Engine>
bool send_async_ack(Engine& engine,
		typename Engine::endpoint const& ep, CoAP::Message::message const& response, CoAP::Error& ec) noexcept
{
	if(response.mtype == CoAP::Message::type::confirmable)
	{
		typename Engine::request req{ep};
		req.header(CoAP::Message::type::acknowledgment,
				CoAP::Message::code::empty);

		engine.template send<UseInternalBuffer>(req, response.mid, ec);
		return true;
	}
	return false;
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_FUNCTION_IMPL_HPP__ */
