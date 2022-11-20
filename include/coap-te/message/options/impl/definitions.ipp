#ifndef COAP_TE_MESSAGE_OPTIONS_DEFINITIONS_IMPL_IPP
#define COAP_TE_MESSAGE_OPTIONS_DEFINITIONS_IMPL_IPP

#include "coap-te/core/utility.hpp"

namespace coap_te {
namespace message {
namespace options {

[[nodiscard]] constexpr const definition&
get_definition(number n) noexcept {
	auto it = ::coap_te::core::binary_search(std::begin(definitions), std::end(definitions), n);
	if (it == std::end(definitions)) 
		return definitions[0];
	return *it;
}

}//options
}//message
}//coap_te

#endif /* COAP_TE_MESSAGE_OPTIONS_DEFINITIONS_IMPL_IPP */