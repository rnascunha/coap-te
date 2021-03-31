#ifndef COAP_TE_URI_DECOMPOSE_HPP__
#define COAP_TE_URI_DECOMPOSE_HPP__

#include <cstdlib>

#include "types.hpp"
#include "message/options/options.hpp"

namespace CoAP{
namespace URI{

template<typename Host,
		typename HostParser,
		bool CheckReliable = true>
bool decompose(uri<Host>& uri, char* uri_string, HostParser parser) noexcept;

template<typename Host>
bool decompose_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
		uri<Host>& uri,
		CoAP::Message::Option::List& list) noexcept;

template<bool CheckReliable = true>
bool decompose(uri<in_addr>& uri, char* uri_string) noexcept;
template<bool CheckReliable = true>
bool decompose(uri<in6_addr>& uri, char* uri_string) noexcept;
template<bool CheckReliable = true>
bool decompose(uri<ip_type>& uri, char* uri_string) noexcept;

/**
 * The couple functions below use as input const data (const char* path/query).
 * All data is percent-decode at the external buffer
 */
bool path_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* path, std::size_t path_length,
				CoAP::Message::Option::List& list) noexcept;
bool query_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* query, std::size_t query_length,
				CoAP::Message::Option::List& list) noexcept;

/**
 * The couple functions below use as input NOT const data (char* path/query).
 * All data is percent decoded in-loco
 */
bool path_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				char* path, std::size_t path_length,
				CoAP::Message::Option::List& list) noexcept;
bool query_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				char* query, std::size_t query_length,
				CoAP::Message::Option::List& list) noexcept;

}//URI
}//CoAP

#include "impl/decompose_impl.hpp"

#endif /* COAP_TE_URI_DECOMPOSE_HPP__ */
