#ifndef COAP_TE_DEBUG_PRINT_MESSAGE_HPP__
#define COAP_TE_DEBUG_PRINT_MESSAGE_HPP__

#include "../defines/defaults.hpp"
#include <cstdint>
#include "../message/types.hpp"

#if COAP_TE_ENABLE_STREAM_CONNECTION == 1
#include "../message/reliable/types.hpp"
#endif /* COAP_TE_ENABLE_STREAM_CONNECTION */

namespace CoAP{
namespace Debug{

void print_message(CoAP::Message::message const&) noexcept;
bool print_message(std::uint8_t const* const, std::size_t) noexcept;

void print_message_encoded_string(CoAP::Message::message const&) noexcept;
void print_message_string(CoAP::Message::message const&) noexcept;

bool print_byte_message(std::uint8_t const*, std::size_t) noexcept;

#if COAP_TE_ENABLE_STREAM_CONNECTION == 1
void print_message(CoAP::Message::Reliable::message const&) noexcept;
void print_message_string(CoAP::Message::Reliable::message const& msg) noexcept;
bool print_byte_reliable_message(std::uint8_t const*, std::size_t) noexcept;
#endif /* COAP_TE_ENABLE_STREAM_CONNECTION */

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_PRINT_MESSAGE_HPP__ */
