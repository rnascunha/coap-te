#ifndef COAP_TE_MESSAGE_ID_HPP__
#define COAP_TE_MESSAGE_ID_HPP__

#include <cstdint>

namespace CoAP{
namespace Message{

class message_id
{
	public:
		message_id(unsigned seed);

		std::uint16_t operator()() noexcept;
	private:
		std::uint16_t id_;
};

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_ID_HPP__ */
