#ifndef COAP_TE_URI_DECOMPOSE_IMPL_HPP__
#define COAP_TE_URI_DECOMPOSE_IMPL_HPP__

#include <cstdlib>
#include "defines/defaults.hpp"
#include "../types.hpp"
#include "../decompose.hpp"
#include "internal/ascii.hpp"
#include "message/options/functions.hpp"

namespace CoAP{
namespace URI{

static bool parse_ipv4(in_addr& uri,
		char* uri_string,
		char** new_uri_string) noexcept;

static bool parse_ipv6(in6_addr& uri,
		char* uri_string,
		char** new_uri_string) noexcept;

static bool parse_ip_v4_v6(ip_type& uri,
		char* uri_string,
		char** new_uri_string) noexcept;

template<typename URI>
using host_parser = bool(URI&, char*, char**) noexcept;

template<bool CheckReliable /* = true */>
bool decompose(uri<in_addr>& uri, char* uri_string) noexcept
{
	return decompose<in_addr, host_parser<in_addr>, CheckReliable>(uri, uri_string, parse_ipv4);
}

template<bool CheckReliable /* = true */>
bool decompose(uri<in6_addr>& uri, char* uri_string) noexcept
{
	return decompose<in6_addr, host_parser<in6_addr>, CheckReliable>(uri, uri_string, parse_ipv6);
}

template<bool CheckReliable /* = true */>
bool decompose(uri<ip_type>& uri, char* uri_string) noexcept
{
	return decompose<ip_type, host_parser<ip_type>, CheckReliable>(uri, uri_string, parse_ip_v4_v6);
}

template<typename Host,
		bool CheckReliable = true>
bool decompose_scheme(uri<Host>& uri, char* uri_string, char** new_uri_string) noexcept
{
	/**
	 * Scheme
	 */
	if(std::strncmp(uri_string, "coap", 4) != 0) return false;
	uri.uri_scheme = scheme::coap;
	uri.port = CoAP::default_port;
	uri_string += 4;
	if(uri_string[0] == 's')
	{
		uri.uri_scheme = scheme::coaps;
		uri.port = CoAP::default_security_port;
		uri_string += 1;
	}
#if COAP_TE_RELIABLE_CONNECTION == 1
	if constexpr(CheckReliable)
	{
		if(uri_string[0] == '+')
		{
			uri_string += 1;
			if(std::strncmp(uri_string, "ws", 2) == 0)
			{
				if(uri.uri_scheme == scheme::coap)
				{
					uri.uri_scheme = scheme::coap_ws;
					uri.port = CoAP::default_ws_port;
				}
				else
				{
					uri.uri_scheme = scheme::coaps_ws;
					uri.port = CoAP::default_wss_port;
				}
				uri_string += 2;
			}
			else if(std::strncmp(uri_string, "tcp", 3) == 0)
			{
				uri.uri_scheme = uri.uri_scheme == scheme::coap ? scheme::coap_tcp : scheme::coaps_tcp;
				uri_string += 3;
			}
			else
				return false;
		}
	}
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
	*new_uri_string = uri_string;

	return true;
}

template<typename Host>
bool decompose_port(uri<Host>& uri, char* uri_string, char** new_uri_string) noexcept
{
	char port[6];
	int i = 0;
	while(uri_string[0] != '\0' || i == 6)
	{
		if(!CoAP::Helper::is_digit(uri_string[0])) break;
		port[i] = uri_string[0];
		uri_string += 1;
		i++;
		port[i] = '\0';
	}
	*new_uri_string = uri_string;

	if(i == 0 || i ==6) return false;
	if(uri_string[0] == '/'
		|| uri_string[0] == '?'
		|| uri_string[0] == '#'
		|| uri_string[0] == '\0') {
		uri.port = static_cast<uint16_t>(std::strtoul(port, nullptr, 10));
		if(!i) return false;
		return true;
	}
	return false;
}

template<typename Host>
bool decompose_path(uri<Host>& uri, char* uri_string, char** new_uri_string) noexcept
{
	uri.path = uri_string;
	while(uri_string[0] != '\0'
			&& uri_string[0] != '?'
			&& uri_string[0] != '#')
	{
		uri_string += 1;
		uri.path_len++;
	}
	*new_uri_string = uri_string;
	return true;
}

template<typename Host>
bool decompose_query(uri<Host>& uri, char* uri_string, char** new_uri_string) noexcept
{
	uri.query = uri_string;
	while(uri_string[0] != '\0'
			&& uri_string[0] != '#')
	{
		uri_string += 1;
		uri.query_len++;
	}
	*new_uri_string = uri_string;
	return true;
}

template<typename Host,
		typename HostParser,
		bool CheckReliable /* = true */>
bool decompose(uri<Host>& uri, char* uri_string, HostParser parser) noexcept
{
	uri.path_len = uri.query_len = 0;

	char* nuri_string = uri_string;
	if(!decompose_scheme<Host, CheckReliable>(uri, uri_string, &nuri_string)) return false;
	uri_string = nuri_string;

	/**
	 * Separator
	 */
	if(std::strncmp(uri_string, "://", 3) != 0) return false;
	uri_string += 3;
	/**
	 * Host
	 */
	if(!parser(uri.host, uri_string, &nuri_string)) return false;
	uri_string = nuri_string;
//	return true;
	/**
	 * Separator
	 */
	if(uri_string[0] == '\0') return true;
	if(uri_string[0] ==  ':')
	{
		uri_string += 1;
		/**
		 * Port
		 */
		if(!decompose_port(uri, uri_string, &nuri_string)) return false;
		uri_string = nuri_string;
	}
	/**
	 * Separator
	 */
	if(uri_string[0] == '\0') return true;
	if(uri_string[0] ==  '/')
	{
		uri_string += 1;
		/**
		 * Path
		 */
		if(!decompose_path(uri, uri_string, &nuri_string)) return false;
		uri_string = nuri_string;
	}

	if(uri_string[0] == '\0') return true;
	if(uri_string[0] == '?')
	{
		uri_string += 1;
		/**
		 * Query
		 */
		if(!decompose_query(uri, uri_string, &nuri_string)) return false;
		uri_string = nuri_string;
	}
	if(uri_string[0] == '#') return false;
	return true;
}

template<typename Host>
bool decompose_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
		uri<Host>& uri,
		CoAP::Message::Option::List& list) noexcept
{
	using namespace CoAP::Message;

	std::size_t used = 0, total = buffer_len;
//	if(add_port)
//	{
//		if(buffer_len < sizeof(Option::node)) return false;
//		Option::node* node = reinterpret_cast<Option::node*>(buffer);
//		buffer_len -= sizeof(Option::node);
//		buffer += sizeof(Option::node);
//
//		if(buffer_len < sizeof(std::uint16_t)) return false;
//		std::memcpy(buffer, &uri.port, sizeof(std::uint16_t));
//
//		node->value.ocode = Option::code::uri_port;
//		node->value.length = 2;
//		node->value.value = buffer;
//
//		buffer_len -= 2;
//		buffer += 2;
//
//		list.add<true>(*node);
//
//		used += sizeof(Option::node) + 2;
//	}

//	buffer_len = total - used;
	if(!path_to_list(buffer, buffer_len, uri.path, uri.path_len, list))
		return false;

	buffer += buffer_len;
	used += buffer_len;

	buffer_len = total - used;
	if(!query_to_list(buffer, buffer_len, uri.query, uri.query_len, list))
		return false;

	used += buffer_len;
	buffer_len = used;

	return true;
}

/***
 *
 *
 */
static bool parse_ipv4(in_addr& host, char* uri_string, char** new_uri_string) noexcept
{
	char addr[16];
	int i = 0;
	while(uri_string[0] != '\0' && i != 16)
	{
		if(!CoAP::Helper::is_digit(uri_string[0]) && uri_string[0] != '.') break;
		addr[i] = uri_string[0];
		uri_string += 1;
		i++;
		addr[i] = '\0';
	}
	*new_uri_string = uri_string;

	if(i == 0 || i == 16) return false;
	if(uri_string[0] == '/'
		|| uri_string[0] == '?'
		|| uri_string[0] == ':'
		|| uri_string[0] == '#'
		|| uri_string[0] == '\0') {
		int ret = inet_pton(AF_INET, addr, &host);
		if(ret <= 0) return false;
		return true;
	}
	return false;
}

static bool parse_ipv6(in6_addr& host, char* uri_string, char** new_uri_string) noexcept
{
	if(uri_string[0] != '[') return false;
	uri_string += 1;

	char addr[40];
	int i = 0;
	while(uri_string[0] != '\0' && i != 40)
	{
		if(!CoAP::Helper::is_digit(uri_string[0]) && uri_string[0] != ':') break;
		addr[i] = uri_string[0];
		uri_string += 1;
		i++;
		addr[i] = '\0';
	}
	*new_uri_string = uri_string;

	if(uri_string[0] != ']') return false;
	uri_string += 1;
	*new_uri_string = uri_string;

	if(i == 0 || i == 40) return false;
	if(uri_string[0] == '/'
		|| uri_string[0] == '?'
		|| uri_string[0] == ':'
		|| uri_string[0] == '#'
		|| uri_string[0] == '\0') {
		int ret = inet_pton(AF_INET6, addr, &host);
		if(ret <= 0) return false;
		return true;
	}
	return false;
}

/**
 * MSVC warning... don't understand...
 */ 
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4505 )
#endif /* defined(_MSC_VER) */

static inline bool parse_ip_v4_v6(ip_type& host,
		char* uri_string,
		char** new_uri_string) noexcept
{
	if(uri_string[0] == '[')
	{
		host.type = host_type::ipv6;
		return parse_ipv6(host.host.ip6, uri_string, new_uri_string);
	}
	host.type = host_type::ipv4;
	return parse_ipv4(host.host.ip4, uri_string, new_uri_string);
}

#if defined(_MSC_VER)
#pragma warning( pop )
#endif /* defined(_MSC_VER) */


}//URI
}//CoAP

#endif /* COAP_TE_URI_DECOMPOSE_IMPL_HPP__ */
