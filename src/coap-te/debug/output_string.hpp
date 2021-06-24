#ifndef COAP_TE_DEBUG_OUTPUT_STRING_HPP__
#define COAP_TE_DEBUG_OUTPUT_STRING_HPP__

#include "../message/types.hpp"
#include "../message/codes.hpp"
#include "../message/options/options.hpp"
#include "../transmission/types.hpp"
#include "../uri/types.hpp"

namespace CoAP{
namespace Debug{

const char* type_string(CoAP::Message::type) noexcept;
const char* code_string(CoAP::Message::code) noexcept;

const char* option_string(CoAP::Message::Option::code) noexcept;
#if	COAP_TE_BLOCKWISE_TRANSFER == 1
const char* option_string(CoAP::Message::Option::csm) noexcept;
const char* option_string(CoAP::Message::Option::ping_pong) noexcept;
const char* option_string(CoAP::Message::Option::release) noexcept;
const char* option_string(CoAP::Message::Option::abort) noexcept;
#endif/* COAP_TE_BLOCKWISE_TRANSFER == 1 */

const char* content_format_string(CoAP::Message::content_format) noexcept;
const char* transaction_status_string(CoAP::Transmission::status_t) noexcept;
const char* scheme_string(CoAP::URI::scheme) noexcept;

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_OUTPUT_STRING_HPP__ */
