#ifndef COAP_TE_PORT_LINUX_ENDPOINT_HPP__
#define COAP_TE_PORT_LINUX_ENDPOINT_HPP__

#include <cstring>
#include <cstdint>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace CoAP{
namespace Port{
namespace Linux{

class socket_endpoint{
	public:
		using native_type = struct sockaddr_in;
		static constexpr const sa_family_t family = AF_INET;

		socket_endpoint()
		{
			::bzero(&addr_, sizeof(struct sockaddr_in));
		}

		socket_endpoint(in_addr_t addr, std::uint16_t port)
		{
			set(addr, port);
		}

		socket_endpoint(const char* addr_str, std::uint16_t port, CoAP::Error& ec)
		{
			if(!set(addr_str, port))
				ec = CoAP::errc::endpoint_error;
		}

		void set(in_addr_t addr, std::uint16_t port) noexcept
		{
			addr_.sin_family = family;
			addr_.sin_port = htons(port);
			addr_.sin_addr.s_addr = addr;
		}

		bool set(const char* addr_str, std::uint16_t port) noexcept
		{
			in_addr_t addr;
			int ret = inet_pton(family, addr_str, &addr);
			if(ret <= 0)
			{
				::bzero(&addr_, sizeof(struct sockaddr_in));
				return false;
			}

			addr_.sin_family = family;
			addr_.sin_port = htons(port);
			addr_.sin_addr.s_addr = addr;

			return true;
		}

		struct sockaddr_in* native() noexcept{ return &addr_; }

		const char* address(char* addr_str) noexcept
		{
			return inet_ntop(family, &addr_.sin_addr, addr_str, sizeof(native_type));
		}
		const char* host(char* host_addr) noexcept { return address(host_addr); }

		in_addr_t address() noexcept{ return addr_.sin_addr.s_addr; }
		std::uint16_t port() const noexcept{ return ntohs(addr_.sin_port); }

		socket_endpoint& operator=(socket_endpoint const& ep) noexcept
		{
			addr_.sin_family = ep.addr_.sin_family;
			addr_.sin_port = ep.addr_.sin_port;
			addr_.sin_addr.s_addr = ep.addr_.sin_addr.s_addr;
			return *this;
		}

		bool operator==(socket_endpoint const& ep) const noexcept
		{
			return addr_.sin_port == ep.addr_.sin_port &&
					addr_.sin_addr.s_addr == ep.addr_.sin_addr.s_addr;
		}

		bool operator!=(socket_endpoint const& ep) const noexcept
		{
			return !(*this == ep);
		}
	private:
		struct sockaddr_in		addr_;
};

}//Linux
}//Port
}//CoAP

#endif /* COAP_TE_PORT_LINUX_ENDPOINT_HPP__ */
