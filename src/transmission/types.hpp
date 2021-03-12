#ifndef COAP_TE_TRANSMISSION_TYPES_HPP__
#define COAP_TE_TRANSMISSION_TYPES_HPP__

#include "port/port.hpp"
#include "internal/helper.hpp"
#include "message/types.hpp"

namespace CoAP{
namespace Transmission{

//https://tools.ietf.org/html/rfc7252#section-4.8
struct configure{
	double			ack_timeout_seconds 			= 2.0;	//ACK_TIMEOUT
	double			ack_ramdom_factor 				= 1.5;	//ACK_RANDOM_FACTOR
	unsigned int	max_restransmission 			= 4;	//MAX_RETRANSMIT
	unsigned int	max_interaction 				= 1;	//NSTART
	unsigned int	default_leisure_seconds 		= 5;	//DEFAULT_LEISURE
	double			probing_rate_byte_per_seconds 	= 1;	//PROBING_RATE
};

struct transaction_param{
	double			max_span;
	double			next_expiration;
	double			expiration_factor;
	unsigned int	retransmission_count;
};

using transaction_cb = void(*)(void const*,
							CoAP::Message::message const*,
							void*);

enum class status_t{
	none = 0,
	sending,
	canceled,
	success,
	timeout,
	failed		//reset received
};

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_TYPES_HPP__ */
