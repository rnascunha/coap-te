#ifndef COAP_TE_MESSAGES_RELIABLE_TYPES_HPP__
#define COAP_TE_MESSAGES_RELIABLE_TYPES_HPP__

#include <cstdlib>
#include <cstdint>
#include "../codes.hpp"

namespace CoAP{
namespace Message{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

enum class extend_length
{
	normal			= 0,
	one_byte 		= 13,
	two_bytes		= 14,
	three_bytes		= 15
};

struct message{
	unsigned		len = 0;
	std::size_t		token_len = 0;		///< Token length
	code			mcode;				///< Message code (check enum class code)
	void const*		token = nullptr;	///< Pointer to token
	std::uint8_t const* option_init = nullptr;	///< Pointer to init of options
	std::size_t		options_len = 0;	///< Size at buffer of options
	std::size_t		option_num = 0;		///< Number of options
	void const*		payload = nullptr;	///< Pointer to payload init
	std::size_t		payload_len = 0;	///< Size of payload

	std::size_t size() const noexcept;
	void reset()
	{
		len = 0;
		token_len = 0;
		token = nullptr;
		option_init = nullptr;
		options_len = 0;
		option_num = 0;
		payload = nullptr;
		payload_len = 0;
	}
};

template<CoAP::Message::code Code>
struct option_type;

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//Reliable
}//Message
}//CoAP

#include "impl/types_impl.hpp"

#endif /* COAP_TE_MESSAGES_RELIABLE_TYPES_HPP__ */
