#include "../udp_socket.hpp"
#include <cerrno>

namespace CoAP{
namespace Port{
namespace POSIX{

template<class Endpoint,
		int RecvFlags,
		int SendFlags>
udp<Endpoint, RecvFlags, SendFlags>::
udp() : socket_(0){}

template<class Endpoint,
		int RecvFlags,
		int SendFlags>
void
udp<Endpoint, RecvFlags, SendFlags>::
open(CoAP::Error& ec) noexcept
{
	if((socket_ = ::socket(endpoint::family, SOCK_DGRAM, 0)) == -1)
		ec = CoAP::errc::socket_error;
}

template<class Endpoint,
		int RecvFlags,
		int SendFlags>
void
udp<Endpoint, RecvFlags, SendFlags>::
bind(endpoint& ep, CoAP::Error& ec) noexcept
{
	if (::bind(socket_,
		reinterpret_cast<struct sockaddr const*>(ep.native()),
		sizeof(typename endpoint::native_type)) == -1)
		ec = CoAP::errc::socket_error;
}

template<class Endpoint,
		int RecvFlags,
		int SendFlags>
std::size_t
udp<Endpoint, RecvFlags, SendFlags>::
send(const void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	int sent = ::sendto(socket_, buffer, buffer_len, SendFlags,
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
		int RecvFlags,
		int SendFlags>
std::size_t
udp<Endpoint, RecvFlags, SendFlags>::
receive(void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	unsigned addr_len = sizeof(struct sockaddr);
	int recv = ::recvfrom(socket_,
			buffer, buffer_len, RecvFlags,
			reinterpret_cast<struct sockaddr*>(ep.native()), &addr_len);

	if(recv < 0)
	{
		if constexpr(RecvFlags | MSG_DONTWAIT)
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
