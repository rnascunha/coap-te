#ifndef COAP_TE_URI_DECOMPOSE_HPP__
#define COAP_TE_URI_DECOMPOSE_HPP__

#include "types.hpp"
#include <arpa/inet.h>
#include "message/options.hpp"

namespace CoAP{
namespace URI{

template<typename Host,
		typename HostParser>
bool decompose(uri<Host>& uri, const char* uri_string, HostParser parser) noexcept;

template<typename Host>
bool decompose_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
		uri<Host>& uri,
		CoAP::Message::Option::List& list) noexcept;

bool decompose(uri<in_addr>& uri, const char* uri_string) noexcept;
bool decompose(uri<in6_addr>& uri, const char* uri_string) noexcept;
bool decompose(uri<ip_type>& uri, const char* uri_string) noexcept;

bool path_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* path, std::size_t path_length,
				CoAP::Message::Option::List& list) noexcept;
bool query_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* query, std::size_t query_length,
				CoAP::Message::Option::List& list) noexcept;


}//URI
}//CoAP

#include "impl/decompose_impl.hpp"

#endif /* COAP_TE_URI_DECOMPOSE_HPP__ */
