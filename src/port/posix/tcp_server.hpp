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

		template<auto* ReadCb,
			int BlockTimeMs = 0,
			auto* OpenCb = (void*)nullptr,
			auto* CloseCb = (void*)nullptr,
			unsigned MaxEvents = 32>
		bool receive(void* buffer, std::size_t buffer_len, CoAP::Error&) noexcept;

		template<
			int BlockTimeMs = 0,
			unsigned MaxEvents = 32,
			typename ReadCb,
			typename OpenCb = void*,
			typename CloseCb = void*>
		bool receive(void* buffer, std::size_t buffer_len, CoAP::Error&,
				ReadCb, OpenCb = nullptr, CloseCb = nullptr) noexcept;

		std::size_t send(handler to_socket, const void*, std::size_t, CoAP::Error&)  noexcept;

		void close() noexcept;
		void close_client(handler) noexcept;
	private:
		int accept(CoAP::Error&) noexcept;
		bool open_poll() noexcept;
		bool add_socket_poll(handler socket, std::uint32_t events) noexcept;

		int socket_;
		int epoll_fd_;
};

}//POSIX
}//Port
}//CoAP

#include "impl/tcp_server_impl.hpp"

#endif /* COAP_TE_PORT_POSIX_SOCKET_TCP_SERVER_HPP__ */
