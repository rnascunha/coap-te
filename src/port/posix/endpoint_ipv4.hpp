#ifndef COAP_TE_PORT_POSIX_ENDPOINT_IPV4_HPP__
#define COAP_TE_PORT_POSIX_ENDPOINT_IPV4_HPP__

#include <cstring>
#include <cstdint>
#include "error.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>

namespace CoAP{
namespace Port{
namespace POSIX{

class endpoint_ipv4{
	public:
		using native_type = struct sockaddr_in;
		static constexpr const sa_family_t family = AF_INET;

		endpoint_ipv4()
		{
			::bzero(&addr_, sizeof(struct sockaddr_in));
		}

		endpoint_ipv4(in_addr_t addr, std::uint16_t port)
		{
			set(addr, port);
		}

		endpoint_ipv4(const char* addr_str, std::uint16_t port, CoAP::Error& ec)
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

		const char* address(char* addr_str, std::size_t len = INET_ADDRSTRLEN) noexcept
		{
			return inet_ntop(family, &addr_.sin_addr, addr_str, len);
		}
		const char* host(char* host_addr) noexcept { return address(host_addr); }

		in_addr_t address() noexcept{ return addr_.sin_addr.s_addr; }
		std::uint16_t port() const noexcept{ return ntohs(addr_.sin_port); }

		endpoint_ipv4& operator=(endpoint_ipv4 const& ep) noexcept
		{
			addr_.sin_family = ep.addr_.sin_family;
			addr_.sin_port = ep.addr_.sin_port;
			addr_.sin_addr.s_addr = ep.addr_.sin_addr.s_addr;
			return *this;
		}

		bool operator==(endpoint_ipv4 const& ep) const noexcept
		{
			return addr_.sin_port == ep.addr_.sin_port &&
					addr_.sin_addr.s_addr == ep.addr_.sin_addr.s_addr;
		}

		bool operator!=(endpoint_ipv4 const& ep) const noexcept
		{
			return !(*this == ep);
		}
	private:
		struct sockaddr_in		addr_;
};

}//POSIX
}//Port
}//CoAP

#endif /* COAP_TE_PORT_POSIX_ENDPOINT_IPV4_HPP__ */
