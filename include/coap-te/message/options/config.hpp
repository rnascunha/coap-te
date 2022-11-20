#ifndef COAP_TE_MESSAGE_OPTIONS_CONFIG_HPP
#define COAP_TE_MESSAGE_OPTIONS_CONFIG_HPP

#if COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1
#include <cstdint>
#endif /* COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1 */

namespace coap_te {
namespace message {
namespace options {

using number_type = unsigned short;
static constexpr const number_type invalid = 0;

enum class format {
	empty 			= 0,
	opaque,
	uint,
	string
};

enum class number : number_type;

struct definition {
	number		    		op;
	const std::string_view	name;
	bool			    	repeatable;
	format			    	type;
    unsigned        		length_min;
    unsigned        		length_max;

	operator number_type() const noexcept {
		return static_cast<number_type>(op);
	}

	operator std::string_view() const noexcept {
		return name;
	}

	operator bool() const noexcept {
		return static_cast<number_type>(op) != invalid;
	}
	
	friend constexpr bool operator <(const definition& lhs, const definition& rhs) noexcept {
		return lhs.op < rhs.op;
	}

	friend constexpr bool operator ==(const definition& lhs, const definition& rhs) noexcept {
		return lhs.op == rhs.op;
	}

	friend constexpr bool operator <(const definition& lhs, number op) noexcept {
		return lhs.op < op;
	}

	friend constexpr bool operator ==(const definition& lhs, number op) noexcept {
		return lhs.op == op;
	}
};

enum class delta {
	one_byte 		= 13,
	two_byte 		= 14,
	error 			= 15
};

enum class length {
	one_byte 		= 13,
	two_byte 		= 14,
	error 			= 15
};

#if COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1
enum class suppress : std::uint8_t{
	none 			= 0b0'0000,
	success 		= 0b0'0010,
	client_error 	= 0b0'1000,
	server_error 	= 0b1'0000
};
#endif /* COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1 */

}//options
}//message
}//coap_te

#endif /* COAP_TE_MESSAGE_OPTIONS_CONFIG_HPP */