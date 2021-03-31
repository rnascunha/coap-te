#ifndef COAP_TE_DEBUG_PRINT_OPTIONS_HPP__
#define COAP_TE_DEBUG_PRINT_OPTIONS_HPP__

#include "message/options/options.hpp"
#include <cstddef>

namespace CoAP{
namespace Debug{

void print_option(CoAP::Message::Option::option const& op, bool print_payload = true);
void print_options(CoAP::Message::Option::option const* op, std::size_t option_num);
void print_options(CoAP::Message::Option::node const*);

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_PRINT_OPTIONS_HPP__ */
