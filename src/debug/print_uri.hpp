#ifndef COAP_TE_DEBUG_PRINT_URI_HPP__
#define COAP_TE_DEBUG_PRINT_URI_HPP__

#include "uri/types.hpp"
#include <arpa/inet.h>

namespace CoAP{
namespace Debug{

void print_uri(CoAP::URI::uri<in_addr> const& uri) noexcept;
void print_uri(CoAP::URI::uri<in6_addr> const& uri) noexcept;
void print_uri(CoAP::URI::uri<CoAP::URI::ip_type> const& uri) noexcept;

void print_uri_decomposed(CoAP::URI::uri<in_addr> const& uri) noexcept;
void print_uri_decomposed(CoAP::URI::uri<in6_addr> const& uri) noexcept;
void print_uri_decomposed(CoAP::URI::uri<CoAP::URI::ip_type> const& uri) noexcept;

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_PRINT_URI_HPP__ */
