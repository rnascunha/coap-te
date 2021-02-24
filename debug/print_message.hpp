#ifndef COAP_TE_DEBUG_PRINT_MESSAGE_HPP__
#define COAP_TE_DEBUG_PRINT_MESSAGE_HPP__

#include <cstdint>
#include "message/types.hpp"

namespace CoAP{
namespace Debug{

void print_message(CoAP::Message::message&);
bool print_message(std::uint8_t const* const, std::size_t);

bool print_byte_message(std::uint8_t const* const, std::size_t);

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_PRINT_MESSAGE_HPP__ */
