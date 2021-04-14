#ifndef COAP_TE_TRANSMISSION_RELIABLE_TYPES_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_TYPES_HPP__

#include <cstdlib>
#include "message/reliable/types.hpp"
#include "response.hpp"
#include <limits>

namespace CoAP{
namespace Transmission{
namespace Reliable{

using expiration_time_type = unsigned;

static constexpr const unsigned no_transaction = 0;
static constexpr const unsigned no_expiration = std::numeric_limits<unsigned>::max();
static constexpr const unsigned default_expiration = no_expiration;

static constexpr unsigned default_max_message_size = 1152;

struct csm_configure{
	unsigned	max_message_size = default_max_message_size;
	bool		block_wise_transfer = false;

	void reset()
	{
		max_message_size = default_max_message_size;
		block_wise_transfer = false;
	}

	csm_configure& operator=(csm_configure const& rhs) noexcept
	{
		if(rhs.max_message_size != 0)
			max_message_size = rhs.max_message_size;
		if(rhs.block_wise_transfer)
			block_wise_transfer = rhs.block_wise_transfer;

		return *this;
	}
};

struct separate_response{
	int				 	socket;
	std::size_t			token_len;
	const void*			token[8];

	separate_response(CoAP::Message::Reliable::message const& request, int sockett)
	: socket{socket}, token_len{request.token_len}
	{
		std::memcpy(token, request.token, token_len);
	}

	separate_response(CoAP::Message::Reliable::message const& request, Response const& response)
	: socket{response.socket()}, token_len{request.token_len}
	{
		std::memcpy(token, request.token, token_len);
	}
};

using default_cb = void(*)(int socket,
							CoAP::Message::Reliable::message const*,
							void*) noexcept;

using transaction_cb = void(*)(void const*,
							CoAP::Message::Reliable::message const*,
							void*) noexcept;


}//CoAP
}//Transmission
}//Reliable

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TYPES_HPP__ */
