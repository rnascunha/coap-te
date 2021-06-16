#ifndef COAP_TE_URI_TYPES_HPP__
#define COAP_TE_URI_TYPES_HPP__

#include <cstdlib>
#include <cstdint>

#include "defines/defaults.hpp"
#include "message/options/options.hpp"
#include "port/port.hpp"

namespace CoAP{
namespace URI{

enum class scheme{
	coap = 0,
	coaps,
#if COAP_TE_RELIABLE_CONNECTION == 1
	coap_tcp,
	coaps_tcp,
	coap_ws,
	coaps_ws
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
};

enum class host_type{
	ipv4 = 0,
	ipv6
};

struct ip_type{
	host_type		type;
	union {
		in_addr		ip4;
		in6_addr	ip6;
	} host;
};

template<typename Host>
struct uri{
	scheme			uri_scheme;
	Host			host;
	uint16_t		port;
	char*			path;
	std::size_t		path_len;
	char* 			query;
	std::size_t		query_len;
};

}//URI
}//CoAP

#endif /* COAP_TE_URI_TYPES_HPP__ */
