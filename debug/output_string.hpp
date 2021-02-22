#ifndef COAP_TE_DEBUG_OUTPUT_STRING_HPP__
#define COAP_TE_DEBUG_OUTPUT_STRING_HPP__

#include "../codes.hpp"
#include "../options.hpp"

namespace CoAP{
namespace Debug{

const char* option_string(CoAP::Message::option_code op);
const char* response_string(CoAP::Message::code code);

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_OUTPUT_STRING_HPP__ */
