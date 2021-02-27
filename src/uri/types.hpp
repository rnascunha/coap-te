#ifndef COAP_TE_URI_TYPES_HPP__
#define COAP_TE_URI_TYPES_HPP__

#include <cstdint>
#include "message/options.hpp"

namespace CoAP{
namespace URI{

enum class scheme{
	coap = 0,
	coaps
};

template<typename Address>
struct uri{
	scheme 			sch;
	Address		 	ep;
	uint16_t		port;
	CoAP::Message::Option::List	path_list;
	CoAP::Message::Option::List	query_list;
};

}//URI
}//CoAP

#endif /* COAP_TE_URI_TYPES_HPP__ */
