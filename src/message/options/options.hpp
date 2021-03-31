#ifndef COAP_TE_MESSAGE_OPTIONS_HPP__
#define COAP_TE_MESSAGE_OPTIONS_HPP__

#include <type_traits>
#include <cstdint>

#include "defines/defaults.hpp"
#include "../types.hpp"

#include "functions.hpp"
#include "types.hpp"
#include "list.hpp"

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
#if	COAP_TE_BLOCKWISE_TRANSFER == 1
	block2			= 23,	//Block2
	block1			= 27,	//Block1
	size2			= 28,	//Size2
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
	proxy_uri		= 35,	//Proxy-Uri
	proxy_scheme	= 39,	//Proxy-Scheme
	size1			= 60,	//Size1
};

static constexpr const config<code> options[] = {
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
#if	COAP_TE_BLOCKWISE_TRANSFER == 1
	{code::block2,			false,	type::uint},
	{code::block1,			false,	type::uint},
	{code::size2,			false,	type::uint},
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
	{code::proxy_uri, 		false, 	type::string},
	{code::proxy_scheme, 	false, 	type::string},
	{code::size1, 			false, 	type::uint},
};

using option = option_template<code>;
using List = List_Option<code>;
using node = node_option<code>;

void create(option&, content_format const&, bool is_request = false) noexcept;

#if COAP_TE_RELIABLE_CONNECTION == 1

enum class csm{
	max_message_size	= 2,
	block_wise_transfer	= 4,
};

static constexpr const config<csm> options_csm[] = {
	{csm::max_message_size, 	false,	type::uint},
	{csm::block_wise_transfer,	false,	type::empty}
};

enum class ping_pong{
	custody				= 2,
};

static constexpr const config<ping_pong> options_ping_pong[] = {
	{ping_pong::custody, 		false,	type::empty}
};

enum class release{
	alternative_address	= 2,
	hold_off			= 4,
};

static constexpr const config<release> options_release[] = {
	{release::alternative_address,	true,	type::string},
	{release::hold_off,				false,	type::uint}
};

enum class abort{
	bad_csm_option		= 2,
};

static constexpr const config<abort> options_abort[] = {
	{abort::bad_csm_option,		false,	type::uint}
};

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

template<typename OptionCode>
config<OptionCode> const * get_config(OptionCode ocode) noexcept
{
	if constexpr(std::is_same<OptionCode, code>::value)
	{
		for(unsigned i = 0; i < sizeof(options) / sizeof(config<code>); i++)
			if(ocode == options[i].ocode)
				return &options[i];
	}
#if COAP_TE_RELIABLE_CONNECTION == 1
	else if constexpr(std::is_same<OptionCode, csm>::value)
	{
		for(unsigned i = 0; i < sizeof(options_csm) / sizeof(config<csm>); i++)
			if(ocode == options[i].ocode)
				return &options[i];
	}
	else if constexpr(std::is_same<OptionCode, ping_pong>::value)
	{
		for(unsigned i = 0; i < sizeof(options_ping_pong) / sizeof(config<ping_pong>); i++)
			if(ocode == options[i].ocode)
				return &options[i];
	}
	else if constexpr(std::is_same<OptionCode, release>::value)
	{
		for(unsigned i = 0; i < sizeof(options_release) / sizeof(config<release>); i++)
			if(ocode == options[i].ocode)
				return &options[i];
	}
	else if constexpr(std::is_same<OptionCode, abort>::value)
	{
		for(unsigned i = 0; i < sizeof(options_abort) / sizeof(config<abort>); i++)
			if(ocode == options[i].ocode)
				return &options[i];
	}
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
	return nullptr;
}

}//Option
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_OPTIONS_HPP__ */
