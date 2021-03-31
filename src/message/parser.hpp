#ifndef COAP_TE_MESSAGE_PARSER_HPP__
#define COAP_TE_MESSAGE_PARSER_HPP__

#include "error.hpp"
#include "types.hpp"

namespace CoAP{
namespace Message{

unsigned parse(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error&);

bool query_by_key(message const& msg, const char* key, const void** value, unsigned& length) noexcept;

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_PARSER_HPP__ */
