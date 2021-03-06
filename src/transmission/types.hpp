#ifndef COAP_TE_TRANSMISSION_TYPES_HPP__
#define COAP_TE_TRANSMISSION_TYPES_HPP__

#include "message/types.hpp"

namespace CoAP{
namespace Transmission{

//https://tools.ietf.org/html/rfc7252#section-4.8
struct configure{
	unsigned int	ack_timeout_seconds 			= 2;	//ACK_TIMEOUT
	float			ack_ramdom_factor 				= 1.5;	//ACK_RANDOM_FACTOR
	unsigned int	max_restransmission 			= 4;	//MAX_RETRANSMIT
	unsigned int	max_interaction 				= 1;	//NSTART
	unsigned int	default_leisure_seconds 		= 5;	//DEFAULT_LEISURE
	float			probing_rate_byte_per_seconds 	= 1;	//PROBING_RATE
	//Implementation
	unsigned int	max_packet_size 				= 1024;
};

using transaction_cb = void(*)(void const*,
							CoAP::Message::message const*,
							void*);

using Response = CoAP::Message::message;

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_TYPES_HPP__ */
