#ifndef COAP_TE_PORT_LINUX_SOCKET_HPP__
#define COAP_TE_PORT_LINUX_SOCKET_HPP__

#include "error.hpp"
#include "endpoint.hpp"
#include <cstdint>

namespace CoAP{
namespace Port{
namespace Linux{

class socket{
	public:
		socket();

		void open(CoAP::Error&) noexcept;
		void bind(endpoint&, CoAP::Error&) noexcept;
		std::size_t send(const void*, std::size_t, endpoint&, CoAP::Error&)  noexcept;
		std::size_t receive(std::uint8_t*, std::size_t, endpoint&, CoAP::Error&) noexcept;
	private:
		int socket_;
};

}//Linux
}//Port
}//CoAP

#endif /* COAP_TE_PORT_LINUX_SOCKET_HPP__ */
