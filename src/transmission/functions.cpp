#include "functions.hpp"
#include "port/port.hpp"
#include "internal/helper.hpp"
#include "message/serialize.hpp"

namespace CoAP{
namespace Transmission{

std::size_t
make_response_code_error(CoAP::Message::message const& msg,
				std::uint8_t* buffer, std::size_t buffer_len,
				CoAP::Message::code err_code, const char* payload /* = "" */) noexcept
{
	CoAP::Error ec;
	std::uint8_t token[8];
	std::memcpy(token, msg.token, msg.token_len);

	return CoAP::Message::serialize(buffer, buffer_len,
					CoAP::Message::type::acknowledgement,
					err_code, msg.mid, token, msg.token_len,
					nullptr, payload, std::strlen(payload), ec);
}

double expiration_timeout(configure const& config) noexcept
{
	return config.ack_timeout_seconds +
			(static_cast<double>(CoAP::random_generator()) /
					(RAND_MAX/(config.ack_timeout_seconds * config.ack_ramdom_factor)));
}

double expiration_timeout_retransmit(
		double timeout,
		unsigned restransmit_count) noexcept
{
	return timeout * (CoAP::Helper::pow(2, restransmit_count));
}

double
max_transmit_span(configure const& config) noexcept
{
	return config.ack_timeout_seconds *
			static_cast<double>(CoAP::Helper::pow(2, config.max_restransmission)) *
			config.ack_ramdom_factor;
}

double
max_transmist_wait(configure const& config) noexcept
{
	return config.ack_timeout_seconds *
				(static_cast<double>(CoAP::Helper::pow(2, config.max_restransmission + 1)) - 1) *
				config.ack_ramdom_factor;
}

unsigned int
maximum_round_trip_time(unsigned int max_latency, unsigned int processing_delay) noexcept
{
	return (2* max_latency) * processing_delay;
}

double
exchange_lifetime(configure const& config, unsigned int max_latency, unsigned int processing_delay) noexcept
{
	return max_transmit_span(config) * (2 * max_latency) * processing_delay;
}

double
non_lifetime(configure const& config, unsigned int max_latency) noexcept
{
	return max_transmit_span(config) * (max_latency);
}

}//Transmission
}//CoAP
