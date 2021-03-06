#include "compose.hpp"
#include "message/options.hpp"
#include <cstring>
#include <cstdio>

#include "internal/encoder.hpp"
#include "internal/ascii.hpp"

using namespace CoAP::Message;

namespace CoAP{
namespace URI{

static bool to_encode_host_char(char c)
{
	return !(CoAP::Helper::is_alpha_digit(c) || c == '.' || c == '-');
}

static bool to_encode_path_char(char c)
{
	return !(CoAP::Helper::is_unreserved(c)
				|| CoAP::Helper::is_subgen_delimits(c)
				|| c == ':' || c == '@');
}

static bool to_encode_query_char(char c)
{
	return c == '&' || !(CoAP::Helper::is_unreserved(c)
			|| CoAP::Helper::is_subgen_delimits(c)
			|| c == ':' || c == '@' || c == '/' || c == '?');
}

std::size_t compose_host(char* buffer, std::size_t buffer_len,
		const char* host, std::size_t host_len, CoAP::Error& ec)
{
	std::size_t offset = 0;
	if((offset + host_len) > buffer_len)
	{
		ec = CoAP::errc::insufficient_buffer;
		return offset;
	}
	std::memcpy(buffer + offset, host, host_len);
	CoAP::Helper::array_to_lower_case(buffer, host_len);
	int ret = CoAP::Helper::percent_encode(buffer, host_len, buffer_len, to_encode_host_char);
	if(ret < 0)
	{
		ec = CoAP::errc::insufficient_buffer;
		return offset;
	}
	return offset + ret;
}

std::size_t compose_port(char* buffer, std::size_t buffer_len,
		unsigned port, CoAP::Error& ec)
{
	std::size_t offset = std::snprintf(buffer, buffer_len, ":%u", port);
	if(offset > (buffer_len))
	{
		ec = CoAP::errc::insufficient_buffer;
		return offset;
	}
	return offset;
}

std::size_t compose_path(char* buffer, std::size_t buffer_len,
		const char* path, std::size_t path_len, CoAP::Error& ec)
{
	if((path_len + 1) > buffer_len)
	{
		ec = CoAP::errc::insufficient_buffer;
		return 0;
	}
	std::snprintf(buffer, buffer_len, "/%.*s", static_cast<int>(path_len), path);
	int ret = CoAP::Helper::percent_encode(buffer + 1, path_len, buffer_len, to_encode_path_char);
	if(ret < 0)
	{
		ec = CoAP::errc::insufficient_buffer;
		return path_len + 1;
	}
	return ret + 1;
}

std::size_t compose_query(char* buffer, std::size_t buffer_len,
		const char* query, std::size_t query_len, bool is_first, CoAP::Error& ec)
{
	if((query_len + 1) > buffer_len)
	{
		ec = CoAP::errc::insufficient_buffer;
		return 0;//offset;
	}
	std::snprintf(buffer, buffer_len,
							"%c%.*s", is_first ? '?' : '&',
							static_cast<int>(query_len), query);

	int ret = CoAP::Helper::percent_encode(buffer + 1, query_len, buffer_len, to_encode_query_char);
	if(ret < 0)
	{
		ec = CoAP::errc::insufficient_buffer;
		return query_len + 1;
	}

	return ret + 1;
}

std::size_t compose(char* buffer, std::size_t buffer_len,
		Option_Parser& list,
		CoAP::Error& ec)
{
	unsigned offset = 0;
	Option::option const* opt;
	bool is_first_query = true;

	while((opt = list.next()))
	{
		switch(opt->code_)
		{
			case Option::code::uri_host:
				offset += compose_host(buffer + offset, buffer_len - offset,
						reinterpret_cast<char const*>(opt->value_), opt->length_, ec);
				if(ec) return offset;
				break;
			case Option::code::uri_port:
			{
				unsigned port = Option::parse_unsigned(*opt);
				offset += compose_port(buffer + offset, buffer_len - offset, port, ec);
				if(ec) return offset;
			}
				break;
			case Option::code::uri_path:
				offset += compose_path(buffer + offset, buffer_len - offset,
						reinterpret_cast<char const*>(opt->value_), opt->length_, ec);
				if(ec) return offset;
				break;
			case Option::code::uri_query:
				offset += compose_query(buffer + offset, buffer_len - offset,
									reinterpret_cast<char const*>(opt->value_), opt->length_,
									is_first_query, ec);
				is_first_query = false;
				if(ec) return offset;
				break;
			default:
				break;
		}
	}
	return offset;
}

}//URI
}//CoAP
