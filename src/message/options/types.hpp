#ifndef COAP_TE_MESSAGE_OPTIONS_TYPES_HPP__
#define COAP_TE_MESSAGE_OPTIONS_TYPES_HPP__

#include "defines/defaults.hpp"

namespace CoAP{
namespace Message{
namespace Option{

static constexpr const int invalid_option = 0;

template<typename OptionCode>
constexpr OptionCode invalid() noexcept{
	return static_cast<OptionCode>(invalid_option);
}

enum class type
{
	empty 			= 0,
	opaque,
	uint,
	string
};

template<typename OptionCode>
struct config
{
	OptionCode	ocode;
	bool		repeatable;
	type		otype;
};

enum class delta_special
{
	one_byte_extend 		= 13,
	two_byte_extend 		= 14,
	error		 			= 15
};

enum class length_special
{
	one_byte_extend 		= 13,
	two_byte_extend 		= 14,
	error		 			= 15
};

#if COAP_TE_OPTION_NO_RESPONSE == 1
enum class suppress{
	none 			= 0b00000000,
	success 		= 0b00000010,
	client_error 	= 0b00001000,
	server_error 	= 0b00010000
};
#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

}//Options
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_OPTIONS_TYPES_HPP__ */
