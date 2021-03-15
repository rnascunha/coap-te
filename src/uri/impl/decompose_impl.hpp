#ifndef COAP_TE_URI_DECOMPOSE_IMPL_HPP__
#define COAP_TE_URI_DECOMPOSE_IMPL_HPP__

#include <cstdlib>
#include "../types.hpp"
#include "../decompose.hpp"
#include "internal/ascii.hpp"

namespace CoAP{
namespace URI{

template<typename Host>
bool decompose_scheme(uri<Host>& uri, const char* uri_string, const char** new_uri_string) noexcept
{
	/**
	 * Scheme
	 */
	if(std::strncmp(uri_string, "coap", 4) != 0) return false;
	uri.uri_scheme = scheme::coap;
	uri.port = 5683;
	uri_string += 4;
	if(uri_string[0] == 's')
	{
		uri.uri_scheme = scheme::coaps;
		uri.port = 5684;
		uri_string += 1;
	}

	*new_uri_string = uri_string;

	return true;
}

template<typename Host>
bool decompose_port(uri<Host>& uri, const char* uri_string, const char** new_uri_string) noexcept
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
		uri.port = std::strtoul(port, nullptr, 10);
		if(!i) return false;
		return true;
	}
	return false;
}

template<typename Host>
bool decompose_path(uri<Host>& uri, const char* uri_string, const char** new_uri_string) noexcept
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
bool decompose_query(uri<Host>& uri, const char* uri_string, const char** new_uri_string) noexcept
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
		typename HostParser>
bool decompose(uri<Host>& uri, const char* uri_string, HostParser parser) noexcept
{
	uri.path_len = uri.query_len = 0;

	const char* nuri_string = uri_string;
	if(!decompose_scheme(uri, uri_string, &nuri_string)) return false;
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

}//URI
}//CoAP

#endif /* COAP_TE_URI_DECOMPOSE_IMPL_HPP__ */
