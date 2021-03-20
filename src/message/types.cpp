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

}//Message
}//CoAP
