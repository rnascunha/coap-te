#ifndef COAP_TE_OPTIONS_HPP__
#define COAP_TE_OPTIONS_HPP__

#include <cstdint>
#include "other/list.hpp"
#include "types.hpp"

namespace CoAP{
namespace Message{

//https://tools.ietf.org/html/rfc7252#section-5.10
enum class option_code {
	if_match 		= 1,	//If-Match
	uri_host		= 3,	//Uri-Host
	etag			= 4,	//ETag
	if_none_match	= 5,	//If-None-Match
	uri_port		= 7,	//Uri-Port
	location_path	= 8,	//Location-Path
	uri_path		= 11,	//Uri-Path
	content_format 	= 12,	//Content-Format
	max_age			= 14,	//Max-Age
	uri_query		= 15,	//Uri-Query
	accept			= 17,	//Accept
	location_query	= 20,	//Location-Query
	proxy_uri		= 35,	//Proxy-Uri
	proxy_scheme	= 39,	//Proxy-Scheme
	size1			= 60,	//Size1
	invalid			= 255	//invalid option
};

enum class option_type
{
	empty 			= 0,
	opaque,
	uint,
	string
};

struct option_config
{
	option_code		code;
	bool			critical;
	bool			unsafe;
	bool			no_chache_key;
	bool			repeatable;
	//const char*		name;
	option_type		type;
};

static constexpr const option_config options[] = {
	{option_code::if_match, 		true,	false, 	false, 	true, 	option_type::opaque},
	{option_code::uri_host, 		true,	true, 	false, 	false,	option_type::string},
	{option_code::etag, 			false,	false, 	false, 	true, 	option_type::opaque},
	{option_code::if_none_match, 	true,	false, 	false, 	false, 	option_type::empty},
	{option_code::uri_port, 		true,	true, 	false, 	false, 	option_type::uint},
	{option_code::location_path, 	false,	false, 	false, 	true, 	option_type::string},
	{option_code::uri_path, 		true,	true, 	false, 	true, 	option_type::string},
	{option_code::content_format, 	false,	false, 	false, 	false, 	option_type::uint},
	{option_code::max_age, 			false,	true, 	false, 	false, 	option_type::uint},
	{option_code::uri_query, 		true,	true, 	false, 	true, 	option_type::string},
	{option_code::accept, 			true,	false, 	false, 	false, 	option_type::uint},
	{option_code::location_query, 	false,	false, 	false, 	true, 	option_type::string},
	{option_code::proxy_uri, 		true,	true, 	false, 	false, 	option_type::string},
	{option_code::proxy_scheme, 	true,	true, 	false, 	false, 	option_type::string},
	{option_code::size1, 			false,	false,	true, 	false, 	option_type::uint},
};

enum class option_delta_special
{
	one_byte_extend 		= 13,
	two_byte_extend 		= 14,
	error		 			= 15
};

enum class option_length_special
{
	one_byte_extend 		= 13,
	two_byte_extend 		= 14,
	error		 			= 15
};

enum class option_critical : std::uint8_t
{
	elective = 0,
	critical = 1
};

enum class option_safe : std::uint8_t
{
	unsafe = 0b10,
	safe_to_forward = 0b00
};

struct option
{
	option();
	//Copy
	option(option_code, unsigned, const void*);
	option(option_code);
	option(option_code, const char*);
	option(option_code, unsigned&);
	option(content_format const&);
	//Opaque
	option(option_code, const void*, unsigned);

	option_code code = option_code::invalid;
	unsigned	length = 0;
	const void*	value = nullptr;

	inline bool operator==(option const& rhs) noexcept
	{
		return code == rhs.code;
	}

	inline bool operator!=(option const& rhs) noexcept
	{
		return !(*this == rhs);
	}

	inline bool operator<(option const& rhs) noexcept
	{
		return code < rhs.code;
	}

	inline bool operator>(option const& rhs) noexcept
	{
		return *this > rhs;
	}

	inline bool operator<=(option const& rhs) noexcept
	{
		return !(*this > rhs);
	}

	inline bool operator>=(option const& rhs) noexcept
	{
		return !(*this < rhs);
	}
};

using Option_List = CoAP::list<option>;
using option_node = Option_List::node;

option_config const * get_option_config(option_code code) noexcept;

void exchange(option* first, option* second) noexcept;
void sort_options(option* options, std::size_t num) noexcept;
void sort_options(option_node*) noexcept;

template<bool CheckType = true>
bool create_option(option&, option_code) noexcept;
template<bool CheckType = true>
bool create_option(option&, option_code, const char*) noexcept;
template<bool CheckType = true>
bool create_option(option&, option_code, unsigned&) noexcept;
void create_option(option&, content_format const&) noexcept;
template<bool CheckType = true>
bool create_option(option&, option_code code, const void*, unsigned) noexcept;

}//Message
}//CoAP

#include "impl/options_impl.hpp"

#endif /* COAP_TE_OPTIONS_HPP__ */
