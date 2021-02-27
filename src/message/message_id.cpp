#include "message_id.hpp"
#include <cstdlib>

namespace CoAP{
namespace Message{

message_id::message_id(unsigned seed)
{
	std::srand(seed);
	id_ = static_cast<uint16_t>(std::rand());
}

uint16_t message_id::operator()() noexcept
{
	return id_++;
}

}//Message
}//CoAP
