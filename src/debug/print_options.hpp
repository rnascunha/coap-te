#ifndef COAP_TE_DEBUG_PRINT_OPTIONS_HPP__
#define COAP_TE_DEBUG_PRINT_OPTIONS_HPP__

#include "defines/defaults.hpp"
#include "message/options/options.hpp"
#include <cstddef>

namespace CoAP{
namespace Debug{

template<typename OptionCode = CoAP::Message::Option::code>
void print_option(CoAP::Message::Option::option_template<OptionCode> const& op, bool print_payload = true);
template<typename OptionCode = CoAP::Message::Option::code>
void print_options(CoAP::Message::Option::option_template<OptionCode> const* op, std::size_t option_num);
template<typename OptionCode = CoAP::Message::Option::code>
void print_options(CoAP::Message::Option::node_option<OptionCode> const*, const char* prefix = "");

}//Debug
}//CoAP

#include "impl/print_options_impl.hpp"

#endif /* COAP_TE_DEBUG_PRINT_OPTIONS_HPP__ */
