#ifndef COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_HPP__

#include <cstdlib>
#include <cstdint>
#include "../../error.hpp"
#include "../port.hpp"

namespace CoAP{
namespace Port{
namespace POSIX{

template<class Endpoint,
		int Flags = MSG_DONTWAIT>
class tcp_client{
	public:
		static constexpr bool set_length = true;
		static constexpr bool is_server = false;
#ifdef _MSC_VER
		using handler = SOCKET;
#else /* _MSC_VER */
		using handler = int;
#endif /* _MSC_VER */
		using endpoint = Endpoint;
		
		tcp_client();
		~tcp_client();

		handler native() const noexcept;

		void open(endpoint&, CoAP::Error&) noexcept;
		bool async_open(endpoint&, CoAP::Error&) noexcept;
		template<int BlockTimeMs = -1>
		bool wait_connect(CoAP::Error&) const noexcept;

		void bind(endpoint&, CoAP::Error&) noexcept;

		bool is_connected() const noexcept;
		bool is_open() const noexcept;
		void close() noexcept;

		std::size_t send(const void*, std::size_t, CoAP::Error&)  noexcept;
		std::size_t receive(void*, std::size_t, CoAP::Error&) noexcept;
		template<int BlockTimeMs>
		std::size_t receive(void*, std::size_t, CoAP::Error&) noexcept;
	private:
		handler socket_;
};

}//POSIX
}//Port
}//CoAP

#include "impl/tcp_client_impl.hpp"

#endif /* COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_HPP__ */
