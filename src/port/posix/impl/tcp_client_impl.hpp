#ifndef COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_IMPL_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_IMPL_HPP__

#include "../tcp_client.hpp"
#include "../functions.hpp"

#include <cerrno>

namespace CoAP{
namespace Port{
namespace POSIX{

template<class Endpoint,
		int Flags>
tcp_client<Endpoint, Flags>::
tcp_client() : socket_(0){}

template<class Endpoint,
		int Flags>
tcp_client<Endpoint, Flags>::
~tcp_client()
{
	if(is_open()) close();
}

template<class Endpoint,
		int Flags>
void
tcp_client<Endpoint, Flags>::
open(endpoint& ep, CoAP::Error& ec) noexcept
{
	if((socket_ = ::socket(endpoint::family, SOCK_STREAM, 0)) == -1)
	{
		ec = CoAP::errc::socket_error;
		return;
	}

	if(::connect(socket_,
		reinterpret_cast<struct sockaddr const*>(ep.native()),
		sizeof(typename endpoint::native_type)) == -1)
	{
		ec = CoAP::errc::socket_error;
		close();
		return;
	}
	ep_ = ep;

	if constexpr(Flags & MSG_DONTWAIT)
		nonblock_socket(socket_);
}

template<class Endpoint,
		int Flags>
void
tcp_client<Endpoint, Flags>::
accept(int socket, endpoint const& ep) noexcept
{
	socket_ = socket;
	ep_ = ep;

	if constexpr(Flags & MSG_DONTWAIT)
		nonblock_socket(socket_);
}

template<class Endpoint,
		int Flags>
bool
tcp_client<Endpoint, Flags>::
is_open() noexcept
{
	return socket_ != 0;
}

template<class Endpoint,
		int Flags>
void
tcp_client<Endpoint, Flags>::
close() noexcept
{
	::shutdown(socket_, SHUT_RDWR);
	::close(socket_);
	socket_ = 0;
}

template<class Endpoint,
		int Flags>
Endpoint const&
tcp_client<Endpoint, Flags>::
get_endpoint() const noexcept
{
	return ep_;
}

template<class Endpoint,
		int Flags>
int
tcp_client<Endpoint, Flags>::
native() const noexcept
{
	return socket_;
}

template<class Endpoint,
		int Flags>
std::size_t
tcp_client<Endpoint, Flags>::
send(const void* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
	int sent = ::send(socket_, buffer, buffer_len, 0);
	if(sent < 0)
	{
		ec = CoAP::errc::socket_error;
		return 0;
	}

	return sent;
}

template<class Endpoint,
		int Flags>
std::size_t
tcp_client<Endpoint, Flags>::
receive(void* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
	int recv = ::recv(socket_, buffer, buffer_len, 0);

	if(recv < 0)
	{
		if constexpr(Flags | MSG_DONTWAIT)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK)
				return 0;
		}
		ec = CoAP::errc::socket_error;
		return 0;
	}

	return recv;
}

}//POSIX
}//Port
}//CoAP

#endif /* COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_IMPL_HPP__ */
