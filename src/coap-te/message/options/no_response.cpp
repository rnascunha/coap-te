#include "no_response.hpp"

namespace CoAP{
namespace Message{
namespace Option{

#if COAP_TE_OPTION_NO_RESPONSE == 1

suppress operator|(suppress lhs, suppress rhs) noexcept
{
    return static_cast<suppress>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

bool operator&(suppress lhs, suppress rhs) noexcept
{
	return (static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs)) != 0;
}

bool no_response_suppress(suppress value, CoAP::Message::code mcode) noexcept
{
	if(value == suppress::none)
		return false;
	if(CoAP::Message::is_success(mcode)
		&& (value & suppress::success))
		return true;
	if(CoAP::Message::is_client_error(mcode)
		&& (value & suppress::client_error))
		return true;
	if(CoAP::Message::is_server_error(mcode)
		&& (value & suppress::server_error))
		return true;

	return false;
}

void create(option_template<Option::code>& option,
		suppress const& value) noexcept
{
	option.ocode = code::no_response;
	option.length = value == suppress::none ? 0 : 1;
	option.value = &value;
}

#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

}//Option
}//Message
}//CoAP
