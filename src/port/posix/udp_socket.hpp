#ifndef COAP_TE_PORT_POSIX_SOCKET_UDP_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_UDP_HPP__

#include <cstdlib>
#include <cstdint>
#include "error.hpp"

namespace CoAP{
namespace Port{
namespace POSIX{

template<class Endpoint,
		int Flags = 0>
class udp{
	public:
		using endpoint = Endpoint;
		udp();

		void open(CoAP::Error&) noexcept;
		void bind(endpoint&, CoAP::Error&) noexcept;
		std::size_t send(const void*, std::size_t, endpoint&, CoAP::Error&)  noexcept;
		std::size_t receive(void*, std::size_t, endpoint&, CoAP::Error&) noexcept;
	private:
		int socket_;
};

}//POSIX
}//Port
}//CoAP

#include "impl/udp_socket_impl.hpp"

#endif /* COAP_TE_PORT_POSIX_SOCKET_UDP_HPP__ */