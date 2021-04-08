#ifndef COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_HPP__

#include <cstdlib>
#include <cstdint>
#include "error.hpp"
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
		using handler = int;

		using endpoint = Endpoint;
		tcp_client();
		~tcp_client();

		int native() const noexcept;

		void open(endpoint&, CoAP::Error&) noexcept;

		bool is_open() const noexcept;
		void close() noexcept;

		endpoint const& get_endpoint() const noexcept;

		std::size_t send(const void*, std::size_t, CoAP::Error&)  noexcept;
		std::size_t receive(void*, std::size_t, CoAP::Error&) noexcept;
	private:
		handler socket_;
		endpoint ep_;
};

}//POSIX
}//Port
}//CoAP

#include "impl/tcp_client_impl.hpp"

#endif /* COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_HPP__ */
