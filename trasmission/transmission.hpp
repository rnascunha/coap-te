#ifndef COAP_TE_TRANSMISSION_HPP__
#define COAP_TE_TRANSMISSION_HPP__

#include <cstdint>
#include "helper.hpp"

namespace CoAP{
namespace Transmission{

static constexpr std::uint16_t default_port = 5683;

//https://tools.ietf.org/html/rfc7252#section-4.8
struct configure{
	unsigned int	ack_timeout_seconds = 2;	//ACK_TIMEOUT
	float			ack_ramdom_factor = 1.5;	//ACK_RANDOM_FACTOR
	unsigned int	max_restransmission = 4;	//MAX_RETRANSMIT
	unsigned int	max_interaction = 1;		//NSTART
	unsigned int	default_leisure_seconds = 5;//DEFAULT_LEISURE
	float			probing_rate_byte_per_seconds = 1;//PROBING_RATE
};

struct record{
	std::uint16_t	mid;				//message id
	unsigned int	timeout;
	unsigned int	retransmission_count;
};

float
max_transmit_span(configure const& config)
{
	return config.ack_timeout_seconds *
			CoAP::Helper::pow(2, config.max_restransmission) *
			config.ack_ramdom_factor;
}

float
max_transmist_wait(configure const& config)
{
	return config.ack_timeout_seconds *
				(CoAP::Helper::pow(2, config.max_restransmission + 1) - 1) *
				config.ack_ramdom_factor;
}

static constexpr const unsigned int max_latency_seconds = 100;	//MAX_LATENCY

unsigned int
maximum_round_trip_time(unsigned int max_latency, unsigned int processing_delay)
{
	return (2* max_latency) * processing_delay;
}

float
exchange_lifetime(configure const& config, unsigned int max_latency, unsigned int processing_delay)
{
	return max_transmit_span(config) * (2 * max_latency) * processing_delay;
}

float
non_lifetime(configure const& config, unsigned int max_latency)
{
	return max_transmit_span(config) * (max_latency);
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_HPP__ */
