#ifndef COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_IMPL_HPP__

#include "../functions.hpp"
#include "error.hpp"
#include "message/reliable/serialize.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<bool SetLength>
std::size_t make_response_code_error(CoAP::Message::Reliable::message const& msg,
		std::uint8_t* buffer, std::size_t buffer_len,
		CoAP::Message::code err_code, const char* payload /* = "" */) noexcept
{
	CoAP::Error ec;
	std::uint8_t token[8];
	std::memcpy(token, msg.token, msg.token_len);

	return CoAP::Message::Reliable::serialize<SetLength>(
			buffer, buffer_len,
			err_code, token, msg.token_len,
			nullptr, payload, std::strlen(payload), ec);
}

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_IMPL_HPP__ */
