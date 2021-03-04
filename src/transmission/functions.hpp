#ifndef COAP_TE_TRANSMISSION_FUNCTION_HPP__
#define COAP_TE_TRANSMISSION_FUNCTION_HPP__

#include "internal/helper.hpp"
#include "types.hpp"

namespace CoAP{
namespace Transmission{

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

#endif /* COAP_TE_TRANSMISSION_FUNCTION_HPP__ */
