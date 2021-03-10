#ifndef COAP_TE_TRANSMISSION_FUNCTION_HPP__
#define COAP_TE_TRANSMISSION_FUNCTION_HPP__

#include "types.hpp"

namespace CoAP{
namespace Transmission{

static constexpr const unsigned int max_latency_seconds = 100;	//MAX_LATENCY

double expiration_timeout(configure const& config) noexcept;
double expiration_timeout_retransmit(
		double timeout,
		unsigned restransmit_count) noexcept;
double max_transmit_span(configure const& config) noexcept;
double max_transmist_wait(configure const& config) noexcept;
unsigned int maximum_round_trip_time(unsigned int max_latency, unsigned int processing_delay) noexcept;
double exchange_lifetime(configure const& config, unsigned int max_latency, unsigned int processing_delay) noexcept;
double non_lifetime(configure const& config, unsigned int max_latency) noexcept;

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_FUNCTION_HPP__ */
