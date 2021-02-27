#ifndef COAP_TE_DEBUG_OUTPUT_STRING_HPP__
#define COAP_TE_DEBUG_OUTPUT_STRING_HPP__

#include "message/types.hpp"
#include "message/codes.hpp"
#include "message/options.hpp"

namespace CoAP{
namespace Debug{

const char* type_string(CoAP::Message::type);
const char* code_string(CoAP::Message::code);
const char* option_string(CoAP::Message::Option::code);
const char* content_format_string(CoAP::Message::content_format);

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_OUTPUT_STRING_HPP__ */
