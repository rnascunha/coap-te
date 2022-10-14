#ifndef COAP_TE_PORT_POSIX_SOCKET_UDP_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_UDP_HPP__

#include <cstdlib>
#include <cstdint>
#include "../../error.hpp"
#include "port.hpp"

namespace CoAP{
namespace Port{
namespace POSIX{

template<class Endpoint,
		int Flags = MSG_DONTWAIT>
class udp{
	public:
#ifdef _MSC_VER
		using handler = SOCKET;
#else /* _MSC_VER */
		using handler = int;
#endif /* _MSC_VER */
		using endpoint = Endpoint;
		udp();

		void open(CoAP::Error&) noexcept;
		void open(sa_family_t, CoAP::Error&) noexcept;
		void open(endpoint&, CoAP::Error&) noexcept;

		void bind(endpoint&, CoAP::Error&) noexcept;

		void close() noexcept;

		std::size_t send(const void*, std::size_t, endpoint&, CoAP::Error&)  noexcept;
		std::size_t receive(void*, std::size_t, endpoint&, CoAP::Error&) noexcept;
		template<int BlockTimeMs>
		std::size_t receive(void*, std::size_t, endpoint&, CoAP::Error&) noexcept;
	private:
		handler socket_;
};

}//POSIX
}//Port
}//CoAP

#include "impl/udp_socket_impl.hpp"

#endif /* COAP_TE_PORT_POSIX_SOCKET_UDP_HPP__ */
