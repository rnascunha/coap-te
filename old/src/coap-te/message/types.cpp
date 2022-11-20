#include "types.hpp"

namespace CoAP{
namespace Message{

bool check_type(type mtype)
{
	switch(mtype)
	{
		case type::confirmable:
		case type::nonconfirmable:
		case type::acknowledgment:
		case type::reset:
			return true;
		default: break;
	}
	return false;
}

std::size_t
message::size() const noexcept
{
		/* header */									/* payload marker */
	return 4 + token_len + options_len + payload_len + (payload_len ? 1 : 0);
}

void
message::clear() noexcept
{
	token = nullptr;
	token_len = 0;
	option_init = nullptr;
	options_len = 0;
	option_num = 0;
	payload = nullptr;
	payload_len = 0;
}

}//Message
}//CoAP
