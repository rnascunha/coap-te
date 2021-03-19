#include "decompose.hpp"
#include "internal/ascii.hpp"
#include "internal/decoder.hpp"
#include <cstring>

namespace CoAP{
namespace URI{

static bool parse_ipv4(in_addr& uri,
		const char* uri_string,
		const char** new_uri_string) noexcept;

static bool parse_ipv6(in6_addr& uri,
		const char* uri_string,
		const char** new_uri_string) noexcept;

static bool parse_ip_v4_v6(ip_type& uri,
		const char* uri_string,
		const char** new_uri_string) noexcept;

static bool split_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* data, std::size_t data_length,
				char delimiter, CoAP::Message::Option::code code,
				CoAP::Message::Option::List& list) noexcept;

bool decompose(uri<in_addr>& uri, const char* uri_string) noexcept
{
	return decompose(uri, uri_string, parse_ipv4);
}

bool decompose(uri<in6_addr>& uri, const char* uri_string) noexcept
{
	return decompose(uri, uri_string, parse_ipv6);
}

bool decompose(uri<ip_type>& uri, const char* uri_string) noexcept
{
	return decompose(uri, uri_string, parse_ip_v4_v6);
}

bool path_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* path, std::size_t path_length,
				CoAP::Message::Option::List& list) noexcept
{
	return split_to_list(buffer, buffer_len,
			path, path_length,
			'/', CoAP::Message::Option::code::uri_path,
			list);
}

bool query_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* query, std::size_t query_length,
				CoAP::Message::Option::List& list) noexcept
{
	return split_to_list(buffer, buffer_len,
			query, query_length,
			'&', CoAP::Message::Option::code::uri_query,
			list);
}

/**
 *
 */

static bool parse_ipv4(in_addr& host, const char* uri_string, const char** new_uri_string) noexcept
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

static bool parse_ipv6(in6_addr& host, const char* uri_string, const char** new_uri_string) noexcept
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

static bool parse_ip_v4_v6(ip_type& host,
		const char* uri_string,
		const char** new_uri_string) noexcept
{
	if(uri_string[0] == '[')
	{
		host.type = host_type::ipv6;
		return parse_ipv6(host.host.ip6, uri_string, new_uri_string);
	}
	host.type = host_type::ipv4;
	return parse_ipv4(host.host.ip4, uri_string, new_uri_string);
}

static bool split_to_list(std::uint8_t* buffer, std::size_t& buffer_len,
				const char* data, std::size_t data_length,
				char delimiter, CoAP::Message::Option::code ocode,
				CoAP::Message::Option::List& list) noexcept
{
	using namespace CoAP::Message;

	std::size_t j = 0, total = buffer_len;
	for(std::size_t i = 0; i <= data_length; i++)
	{
		if(i == data_length || data[i] == delimiter)
		{
			if(buffer_len < sizeof(Option::node)) return false;
			Option::node* node = reinterpret_cast<Option::node*>(buffer);
			buffer_len -= sizeof(Option::node);
			std::size_t length = buffer_len;

			buffer += sizeof(Option::node);
			if(!CoAP::Helper::percent_decode(reinterpret_cast<char*>(buffer), length, data + j, i - j))
			{
				return false;
			}
			node->value.ocode = ocode;
			node->value.length = length;
			node->value.value = buffer;

			buffer_len -= length;
			buffer += length;

			list.add<true>(*node);

			j = i + 1;
		}
	}

	buffer_len = total - buffer_len;

	return true;
}


}//URI
}//CoAP
