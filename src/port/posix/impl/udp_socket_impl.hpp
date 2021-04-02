#ifndef COAP_TE_PORT_POSIX_SOCKET_UDP_IMPL_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_UDP_IMPL_HPP__

#include "../udp_socket.hpp"
#include "../functions.hpp"

#include <cerrno>

namespace CoAP{
namespace Port{
namespace POSIX{

template<class Endpoint,
		int Flags>
udp<Endpoint, Flags>::
udp() : socket_(0){}

template<class Endpoint,
		int Flags>
void
udp<Endpoint, Flags>::
open(CoAP::Error& ec) noexcept
{
	if((socket_ = ::socket(endpoint::family, SOCK_DGRAM, 0)) == -1)
	{
		ec = CoAP::errc::socket_error;
		return;
	}
	if constexpr(Flags & MSG_DONTWAIT)
		nonblock_socket(socket_);
}

template<class Endpoint,
		int Flags>
void
udp<Endpoint, Flags>::
bind(endpoint& ep, CoAP::Error& ec) noexcept
{
	if (::bind(socket_,
		reinterpret_cast<struct sockaddr const*>(ep.native()),
		sizeof(typename endpoint::native_type)) == -1)
		ec = CoAP::errc::socket_error;
}

template<class Endpoint,
		int Flags>
std::size_t
udp<Endpoint, Flags>::
send(const void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	int sent = ::sendto(socket_, buffer, buffer_len, 0,
				reinterpret_cast<struct sockaddr const*>(ep.native()),
				sizeof(typename endpoint::native_type));
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
udp<Endpoint, Flags>::
receive(void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	unsigned addr_len = sizeof(struct sockaddr);
	int recv = ::recvfrom(socket_,
			buffer, buffer_len, 0,
			reinterpret_cast<struct sockaddr*>(ep.native()), &addr_len);

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

#endif /* COAP_TE_PORT_POSIX_SOCKET_UDP_IMPL_HPP__ */
