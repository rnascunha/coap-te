#include "socket.hpp"

namespace CoAP{
namespace Port{
namespace Linux{

socket::socket() : socket_(0){}

void
socket::
open(CoAP::Error& ec) noexcept
{
	if((socket_ = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ec = CoAP::errc::socket_error;
}

void
socket::
bind(endpoint& ep, CoAP::Error& ec) noexcept
{
	if (::bind(socket_,
		reinterpret_cast<struct sockaddr*>(ep.native()),
		sizeof(endpoint::native_type)) == -1)
		ec = CoAP::errc::socket_error;
}

std::size_t
socket::
send(const void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	int sent = ::sendto(socket_, buffer, buffer_len, 0,
				reinterpret_cast<struct sockaddr*>(ep.native()),
				sizeof(endpoint::native_type));
	if(sent < 0)
	{
		ec = CoAP::errc::socket_error;
		return 0;
	}

	return sent;
}

std::size_t
socket::
receive(std::uint8_t* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	unsigned addr_len;
	int recv = ::recvfrom(socket_,
			buffer, buffer_len, 0,
			reinterpret_cast<struct sockaddr*>(ep.native()), &addr_len);

	if(recv < 0)
	{
		ec = CoAP::errc::socket_error;
		return 0;
	}

	return recv;
}

}//Linux
}//Port
}//CoAP
