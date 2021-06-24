#ifndef COAP_TE_MESSAGE_RELIABLE_PARSER_HPP__
#define COAP_TE_MESSAGE_RELIABLE_PARSER_HPP__

#include <cstdint>

#include "../../error.hpp"
#include "types.hpp"

namespace CoAP{
namespace Message{
namespace Reliable{

unsigned parse_header(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error&) noexcept;

unsigned parse(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error&) noexcept;

}//Reliable
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_RELIABLE_PARSER_HPP__ */
