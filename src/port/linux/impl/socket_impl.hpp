#include "../socket.hpp"
#include <cerrno>

namespace CoAP{
namespace Port{
namespace Linux{

template<int RecvFlags, int SendFlags>
socket<RecvFlags, SendFlags>::socket() : socket_(0){}

template<int RecvFlags, int SendFlags>
void
socket<RecvFlags, SendFlags>::
open(CoAP::Error& ec) noexcept
{
	if((socket_ = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ec = CoAP::errc::socket_error;
}

template<int RecvFlags, int SendFlags>
void
socket<RecvFlags, SendFlags>::
bind(endpoint& ep, CoAP::Error& ec) noexcept
{
	if (::bind(socket_,
		reinterpret_cast<struct sockaddr const*>(ep.native()),
		sizeof(endpoint::native_type)) == -1)
		ec = CoAP::errc::socket_error;
}

template<int RecvFlags, int SendFlags>
std::size_t
socket<RecvFlags, SendFlags>::
send(const void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	int sent = ::sendto(socket_, buffer, buffer_len, SendFlags,
				reinterpret_cast<struct sockaddr const*>(ep.native()),
				sizeof(endpoint::native_type));
	if(sent < 0)
	{
		ec = CoAP::errc::socket_error;
		return 0;
	}

	return sent;
}

template<int RecvFlags, int SendFlags>
std::size_t
socket<RecvFlags, SendFlags>::
receive(std::uint8_t* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
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

}//Linux
}//Port
}//CoAP
