#ifndef COAP_TE_PORT_POSIX_SOCKET_TCP_SERVER_IMPL_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_TCP_SERVER_IMPL_HPP__

#include "../tcp_server.hpp"
#include "../port.hpp"

#include <type_traits>

namespace CoAP{
namespace Port{
namespace POSIX{

template<class Endpoint,
		int Flags>
tcp_server<Endpoint, Flags>::tcp_server()
	: socket_(0), epoll_fd_(0){}

template<class Endpoint,
		int Flags>
template<int PendingQueueSize /* = 10 */>
void
tcp_server<Endpoint, Flags>::
open(endpoint& ep, CoAP::Error& ec) noexcept
{
	if((socket_ = ::socket(endpoint::family, SOCK_STREAM, 0)) == -1)
	{
		ec = CoAP::errc::socket_error;
		return;
	}

	int opt = 1;
	if(::setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		close();
		ec = CoAP::errc::socket_error;
		return;
	}

	if (::bind(socket_,
		reinterpret_cast<struct sockaddr const*>(ep.native()),
		sizeof(typename endpoint::native_type)) == -1)
	{
		close();
		ec = CoAP::errc::socket_error;
		return;
	}

	if(::listen(socket_, PendingQueueSize) == -1)
	{
		close();
		ec = CoAP::errc::socket_error;
		return;
	}

	if constexpr(Flags & MSG_DONTWAIT)
		nonblock_socket(socket_);

	if(!open_poll())
	{
		close();
		ec = CoAP::errc::socket_error;
	}
}

template<class Endpoint,
		int Flags>
bool tcp_server<Endpoint, Flags>::
is_open() const noexcept
{
	return socket_ != 0;
}

template<class Endpoint,
		int Flags>
bool
tcp_server<Endpoint, Flags>::
open_poll() noexcept
{
	epoll_fd_ = epoll_create1(0);
	if(epoll_fd_ == -1)
		return false;

	if(!add_socket_poll(socket_, EPOLLIN | EPOLLOUT | EPOLLET))
		return false;

	return true;
}

template<class Endpoint,
		int Flags>
bool
tcp_server<Endpoint, Flags>::
add_socket_poll(int socket, std::uint32_t events) noexcept
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = socket;
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket, &ev) == -1)
		return false;

	return true;
}

template<class Endpoint,
		int Flags>
void tcp_server<Endpoint, Flags>::
close() noexcept
{
	epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, socket_, NULL);
	if(socket_) ::close(socket_);
	if(epoll_fd_) ::close(epoll_fd_);
	socket_ = 0;
}

template<class Endpoint,
	int Flags>
void tcp_server<Endpoint, Flags>::
close_client(handler socket) noexcept
{
	epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, socket, NULL);
	::close(socket);
}

template<class Endpoint,
		int Flags>
int
tcp_server<Endpoint, Flags>::
accept(CoAP::Error& ec) noexcept
{
	handler s = 0;
	endpoint ep;
	socklen_t len = sizeof(typename endpoint::native_type);
	if((s = ::accept(socket_, reinterpret_cast<struct sockaddr*>(ep.native()), &len)) == -1)
	{
		ec = CoAP::errc::socket_error;
	}
	else
	{
		if(!add_socket_poll(s, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP))
			ec = CoAP::errc::socket_error;
		if(Flags & MSG_DONTWAIT)
			nonblock_socket(s);
	}
	return s;
}

template<class Endpoint,
		int Flags>
template<
		int BlockTimeMs /* = 0 */,
		unsigned MaxEvents /* = 32 */,
		typename ReadCb,
		typename OpenCb /* = void* */,
		typename CloseCb /* = void* */>
bool
tcp_server<Endpoint, Flags>::
run(CoAP::Error& ec,
		ReadCb read_cb,
		OpenCb open_cb/* = nullptr */ [[maybe_unused]],
		CloseCb close_cb/* = nullptr */ [[maybe_unused]]) noexcept
{
	struct epoll_event events[MaxEvents];

	int event_num = epoll_wait(epoll_fd_, events, MaxEvents, BlockTimeMs);
	for (int i = 0; i < event_num; i++)
	{
		if (events[i].data.fd == socket_)
		{
			[[maybe_unused]] int c = accept(ec);
			if constexpr(!std::is_same<void*, OpenCb>::value)
			{
				open_cb(c);
			}
		}
		else if (events[i].events & EPOLLIN)
		{
			/* handle EPOLLIN event */
			handler s = events[i].data.fd;
			read_cb(s);
		}
		/* check if the connection is closing */
		if (events[i].events & (EPOLLRDHUP | EPOLLHUP))
		{
			handler s = events[i].data.fd;
			if constexpr(!std::is_same<void*, CloseCb>::value)
			{
				close_cb(s);
			}
			close_client(s);
		}
	}
	return ec ? false : true;
}


template<class Endpoint,
		int Flags>
std::size_t
tcp_server<Endpoint, Flags>::
receive(handler socket, void* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
	ssize_t bytes = read(socket, buffer, buffer_len);
	if (bytes < 0)
	{
		if constexpr(Flags & MSG_DONTWAIT)
		{
			if(!(errno == EAGAIN || errno == EWOULDBLOCK))
				ec = CoAP::errc::socket_error;
		}
		return 0;
	}
	return bytes;
}

template<class Endpoint,
		int Flags>
std::size_t
tcp_server<Endpoint, Flags>::
send(handler to_socket, const void* buffer, std::size_t buffer_len, CoAP::Error& ec)  noexcept
{
	int size = ::send(to_socket, buffer, buffer_len, 0);
	if(size == -1)
	{
		if constexpr(Flags & MSG_DONTWAIT)
			if(!(errno == EAGAIN || errno == EWOULDBLOCK))
				ec = CoAP::errc::socket_error;
		return 0;
	}
	return size;
}

}//POSIX
}//Port
}//CoAP

#endif /* COAP_TE_PORT_POSIX_SOCKET_TCP_SERVER_IMPL_HPP__ */
