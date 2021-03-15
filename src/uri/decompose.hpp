#ifndef COAP_TE_URI_DECOMPOSE_HPP__
#define COAP_TE_URI_DECOMPOSE_HPP__

#include "types.hpp"
#include <arpa/inet.h>

namespace CoAP{
namespace URI{

template<typename Host,
		typename HostParser>
bool decompose(uri<Host>& uri, const char* uri_string, HostParser parser) noexcept;

bool decompose(uri<in_addr>& uri, const char* uri_string) noexcept;
bool decompose(uri<in6_addr>& uri, const char* uri_string) noexcept;
bool decompose(uri<ip_type>& uri, const char* uri_string) noexcept;

}//URI
}//CoAP

#include "impl/decompose_impl.hpp"

#endif /* COAP_TE_URI_DECOMPOSE_HPP__ */
