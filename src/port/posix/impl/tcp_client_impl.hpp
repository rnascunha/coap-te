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
	if((socket_ = ::socket(endpoint::family, SOCK_STREAM, IPPROTO_TCP)) == -1)
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

	if constexpr((Flags & MSG_DONTWAIT) != 0)
		nonblock_socket(socket_);
}

template<class Endpoint,
		int Flags>
bool
tcp_client<Endpoint, Flags>::
is_open() const noexcept
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
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	::closesocket(socket_);
#else /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	::close(socket_);
#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	socket_ = 0;
}

template<class Endpoint,
		int Flags>
typename tcp_client<Endpoint, Flags>::handler
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
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	int sent = ::send(socket_, static_cast<const char*>(buffer), buffer_len, 0);
#else /* #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	int sent = ::send(socket_, buffer, buffer_len, 0);
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
tcp_client<Endpoint, Flags>::
receive(void* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	int recv = ::recv(socket_, static_cast<char*>(buffer), buffer_len, 0);
#else /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	int recv = ::recv(socket_, buffer, buffer_len, 0);
#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	if(recv < 1)
	{
		if constexpr((Flags & MSG_DONTWAIT) != 0)
		{
#if	defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
			if(recv == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
#else /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
			if(recv == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
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

#endif /* COAP_TE_PORT_POSIX_SOCKET_TCP_CLIENT_IMPL_HPP__ */
