#include "message_id.hpp"
#include <cstdlib>
#include "../port/port.hpp"

namespace CoAP{
namespace Message{

message_id::message_id(unsigned seed)
{
	std::srand(seed);
	id_ = static_cast<uint16_t>(CoAP::random_generator());
}

message_id::message_id()
{
	id_ = static_cast<uint16_t>(CoAP::random_generator());
}

uint16_t message_id::operator()() noexcept
{
	return id_++;
}

}//Message
}//CoAP
