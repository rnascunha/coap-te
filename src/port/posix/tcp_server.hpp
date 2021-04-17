#ifndef COAP_TE_PORT_POSIX_SOCKET_TCP_SERVER_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_TCP_SERVER_HPP__

#include <cstdlib>
#include <cstdint>

#include "defines/defaults.hpp"

#include "error.hpp"
#include "port.hpp"

namespace CoAP{
namespace Port{
namespace POSIX{

template<class Endpoint,
		int Flags = MSG_DONTWAIT>
class tcp_server{
	public:
		static constexpr bool set_length = true;
		static constexpr bool is_server = true;
		using handler = int;
		using endpoint = Endpoint;

		tcp_server();

		template<int PendingQueueSize = 10>
		void open(endpoint&, CoAP::Error&) noexcept;
		bool is_open() const noexcept;

		template<
			int BlockTimeMs = 0,
			unsigned MaxEvents = 32,
			typename ReadCb,
			typename OpenCb = void*,
			typename CloseCb = void*>
		bool run(CoAP::Error&,
				ReadCb, OpenCb = nullptr, CloseCb = nullptr) noexcept;

		std::size_t receive(handler socket, void* buffer, std::size_t, CoAP::Error&) noexcept;
		std::size_t send(handler to_socket, const void*, std::size_t, CoAP::Error&)  noexcept;

		void close() noexcept;
		void close_client(handler) noexcept;

#if COAP_TE_USE_SELECT == 1 || COAP_TE_TCP_SERVER_CLIENT_LIST == 1
		fd_set const& client_list() const noexcept;
#endif /* COAP_TE_USE_SELECT == 1 || COAP_TE_TCP_SERVER_CLIENT_LIST == 1 */
	private:
		int accept(CoAP::Error&) noexcept;
		bool open_poll() noexcept;
		bool add_socket_poll(handler socket, std::uint32_t events) noexcept;

		handler socket_;
#if COAP_TE_USE_SELECT != 1
		int epoll_fd_;
#endif /* COAP_TE_USE_SELECT != 1 */
#if COAP_TE_USE_SELECT == 1 || COAP_TE_TCP_SERVER_CLIENT_LIST == 1
		fd_set	list_;
#endif /* COAP_TE_USE_SELECT != 1 || COAP_TE_TCP_SERVER_CLIENT_LIST == 1*/
};

}//POSIX
}//Port
}//CoAP

#include "impl/tcp_server_impl.hpp"

#endif /* COAP_TE_PORT_POSIX_SOCKET_TCP_SERVER_HPP__ */
