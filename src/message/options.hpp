#ifndef COAP_TE_MESSAGE_OPTIONS_HPP__
#define COAP_TE_MESSAGE_OPTIONS_HPP__

#include <cstdint>
#include "internal/list.hpp"
#include "types.hpp"

namespace CoAP{
namespace Message{
namespace Option{

//https://tools.ietf.org/html/rfc7252#section-5.10
enum class code {
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

enum class type
{
	empty 			= 0,
	opaque,
	uint,
	string
};

struct config
{
	code		ocode;
	bool		repeatable;
	type		otype;
};

static constexpr const config options[] = {
	{code::if_match, 		true, 	type::opaque},
	{code::uri_host, 		false,	type::string},
	{code::etag, 			true, 	type::opaque},
	{code::if_none_match, 	false, 	type::empty},
	{code::uri_port, 		false, 	type::uint},
	{code::location_path, 	true, 	type::string},
	{code::uri_path, 		true, 	type::string},
	{code::content_format, 	false, 	type::uint},
	{code::max_age, 		false, 	type::uint},
	{code::uri_query, 		true, 	type::string},
	{code::accept, 			false, 	type::uint},
	{code::location_query, 	true, 	type::string},
	{code::proxy_uri, 		false, 	type::string},
	{code::proxy_scheme, 	false, 	type::string},
	{code::size1, 			false, 	type::uint},
};

config const * get_config(code code) noexcept;

bool is_critical(code code) noexcept;
bool is_unsafe(code code) noexcept;
bool is_no_cache_key(code code) noexcept;

enum class delta_special
{
	one_byte_extend 		= 13,
	two_byte_extend 		= 14,
	error		 			= 15
};

enum class length_special
{
	one_byte_extend 		= 13,
	two_byte_extend 		= 14,
	error		 			= 15
};

struct option
{
	option();
	//Copy
	option(code, unsigned, const void*);
	option(code);
	option(code, const char*);
	option(code, unsigned&);
	option(content_format const&, bool is_request = false);
	//Opaque
	option(code, const void*, unsigned);

	bool is_critical() const noexcept;
	bool is_unsafe() const noexcept;
	bool is_no_cache_key() const noexcept;

	bool operator==(option const& rhs) noexcept;
	bool operator!=(option const& rhs) noexcept;
	bool operator<(option const& rhs) noexcept;
	bool operator>(option const& rhs) noexcept;
	bool operator<=(option const& rhs) noexcept;
	bool operator>=(option const& rhs) noexcept;

	code 		code_ = code::invalid;
	unsigned	length_ = 0;
	const void*	value_ = nullptr;
};

using List = CoAP::list<option>;
using node = List::node;

void exchange(option* first, option* second) noexcept;
void sort(option* options, std::size_t num) noexcept;
void sort(node*) noexcept;

unsigned parse_unsigned(option const&);

template<bool CheckType = true>
bool create(option&, code) noexcept;
template<bool CheckType = true>
bool create(option&, code, const char*) noexcept;
template<bool CheckType = true>
bool create(option&, code, unsigned&) noexcept;
void create(option&, content_format const&, bool is_request = false) noexcept;
template<bool CheckType = true>
bool create(option&, code code, const void*, unsigned) noexcept;

}//Option
}//Message
}//CoAP

#include "impl/options_impl.hpp"

#endif /* COAP_TE_MESSAGE_OPTIONS_HPP__ */
