#ifndef COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_HPP__

#include "message/reliable/types.hpp"
#include "message/codes.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<bool SetLength>
std::size_t make_response_code_error(CoAP::Message::Reliable::message const& msg,
		std::uint8_t* buffer, std::size_t buffer_len,
		CoAP::Message::code, const char* payload = "") noexcept;

}//Reliable
}//Transmission
}//CoAP

#include "impl/functions_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_HPP__ */
