#ifndef COAP_TE_PORT_POSIX_SOCKET_UDP_IMPL_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_UDP_IMPL_HPP__

#include "../udp_socket.hpp"
#include "../functions.hpp"

#include <cerrno>

#include <cstdio>

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
	if constexpr((Flags & MSG_DONTWAIT) != 0)
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
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	int sent = ::sendto(socket_, static_cast<const char*>(buffer), buffer_len, 0,
				reinterpret_cast<struct sockaddr const*>(ep.native()),
				sizeof(typename endpoint::native_type));
#else /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	int sent = ::sendto(socket_, buffer, buffer_len, 0,
				reinterpret_cast<struct sockaddr const*>(ep.native()),
				sizeof(typename endpoint::native_type));
#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
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
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	int addr_len = sizeof(struct sockaddr);
	int recv = ::recvfrom(socket_,
			static_cast<char*>(buffer), buffer_len, 0,
			reinterpret_cast<struct sockaddr*>(ep.native()), &addr_len);
#else /* #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	unsigned addr_len = sizeof(struct sockaddr);
	int recv = ::recvfrom(socket_,
			buffer, buffer_len, 0,
			reinterpret_cast<struct sockaddr*>(ep.native()), &addr_len);
#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */

	if(recv < 0)
	{
		if constexpr((Flags & MSG_DONTWAIT) != 0)
		{
#if	defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
			if(WSAGetLastError() == WSAEWOULDBLOCK)
#else
			if(errno == EAGAIN || errno == EWOULDBLOCK)
#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
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
