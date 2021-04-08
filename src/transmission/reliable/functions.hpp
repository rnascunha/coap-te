#ifndef COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_HPP__

#include "error.hpp"
#include "message/options/options.hpp"
#include "message/reliable/types.hpp"
#include "message/codes.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<bool SetLength>
std::size_t make_response_code_error(CoAP::Message::Reliable::message const& msg,
		std::uint8_t* buffer, std::size_t buffer_len,
		CoAP::Message::code, const char* payload = "") noexcept;

template<bool SetLength>
std::size_t make_csm_message(csm_configure const&,
		void* buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept;

template<bool SetLength>
std::size_t make_abort_message(CoAP::Message::Option::option_abort&,
		const char* payload,
		void* buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept;

}//Reliable
}//Transmission
}//CoAP

#include "impl/functions_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_FUNCTION_HPP__ */
