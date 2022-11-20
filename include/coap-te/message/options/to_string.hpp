#ifndef COAP_TE_MESSAGE_OPTIONS_TO_STRING_HPP
#define COAP_TE_MESSAGE_OPTIONS_TO_STRING_HPP

#include <string_view>

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/definitions.hpp"

namespace coap_te {
namespace message {
namespace options {

[[nodiscard]] constexpr std::string_view 
to_string(format t) noexcept {
	switch(t) {
		case format::empty:
			return "empty";
		case format::opaque:
			return "opaque";
		case format::uint:
			return "uint";
		case format::string:
			return "string";
		default:
			break;
	}
	return "unknown";
}

[[nodiscard]] constexpr std::string_view 
to_string(const definition& def) noexcept {
    return def.name;
}

[[nodiscard]] constexpr std::string_view 
to_string(number op) noexcept {
    return to_string(get_definition(op));
}

}//options 
}//message
}//coap_te

#endif /* COAP_TE_MESSAGE_OPTIONS_TO_STRING_HPP */