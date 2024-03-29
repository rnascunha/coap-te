#ifndef COAP_TE_TRANSMISSION_TYPES_HPP__
#define COAP_TE_TRANSMISSION_TYPES_HPP__

#include "../port/port.hpp"
#include "../internal/helper.hpp"
#include "../message/types.hpp"
#include "response.hpp"

#include <functional>

namespace CoAP{
namespace Transmission{

//https://tools.ietf.org/html/rfc7252#section-4.8
struct configure{
	double			ack_timeout_seconds 			= 2.0;	//ACK_TIMEOUT
	double			ack_random_factor 				= 1.5;	//ACK_RANDOM_FACTOR
	unsigned int	max_restransmission 			= 4;	//MAX_RETRANSMIT
//	unsigned int	max_interaction 				= 1;	//NSTART
//	unsigned int	default_leisure_seconds 		= 5;	//DEFAULT_LEISURE
//	double			probing_rate_byte_per_seconds 	= 1;	//PROBING_RATE
};

struct transaction_param{
	double			max_span;
	double			next_expiration;
	double			expiration_factor;
	unsigned int	retransmission_remaining;
};

template<typename Endpoint>
struct separate_response{
	Endpoint		 	ep;
	Message::type		type = CoAP::Message::type::nonconfirmable;
	std::size_t			token_len = 0;
	const void*			token[8];

	separate_response(){}

	separate_response(Message::message const& request, Endpoint const& endp)
	: ep{endp}, type{request.mtype}, token_len{request.token_len}
	{
		std::memcpy(token, request.token, token_len);
	}

	separate_response(Message::message const& request, Response<Endpoint> const& response)
	: ep{response.endpoint()}, type{request.mtype}, token_len{request.token_len}
	{
		std::memcpy(token, request.token, token_len);
	}

	void set(Message::message const& request, Response<Endpoint> const& response) noexcept
	{
		ep = response.endpoint();
		type  = request.mtype;
		token_len = request.token_len;
		std::memcpy(token, request.token, token_len);
	}
};

template<typename Endpoint>
using default_cb = void(*)(Endpoint const&,
							CoAP::Message::message const*,
							void*) noexcept;

using transaction_cb = void(*)(void const*,
							CoAP::Message::message const*,
							void*) noexcept;

using transaction_cb_functional = std::function<void(void const*,
							CoAP::Message::message const*,
							void*)>;

enum class status_t{
	none = 0,	///< not been used
	to_send,	///< transaction to be send (lock before send)
	sending,	///< in used
	canceled,	///< canceled
	success,	///< received a response
	timeout,	///< timeout
	empty		///< received a empty response,
};

enum class profile{
	client = 0,
	server
};

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_TYPES_HPP__ */
