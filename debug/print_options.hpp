#ifndef COAP_TE_DEBUG_PRINT_OPTIONS_HPP__
#define COAP_TE_DEBUG_PRINT_OPTIONS_HPP__

#include "../options.hpp"
#include <cstddef>

namespace CoAP{
namespace Debug{

void print_option(CoAP::Message::option const* op);
void print_options(CoAP::Message::option const* op, std::size_t option_num);
void print_options(CoAP::Message::option_node const*);

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_PRINT_OPTIONS_HPP__ */
