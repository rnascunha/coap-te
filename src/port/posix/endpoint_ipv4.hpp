#ifndef COAP_TE_PORT_POSIX_ENDPOINT_IPV4_HPP__
#define COAP_TE_PORT_POSIX_ENDPOINT_IPV4_HPP__

#include <cstring>
#include <cstdint>
#include "error.hpp"

#include "port.hpp"

namespace CoAP{
namespace Port{
namespace POSIX{

class endpoint_ipv4{
	public:
		using native_type = sockaddr_in;
		static constexpr const sa_family_t family = AF_INET;

		endpoint_ipv4()
		{
			std::memset(&addr_, 0, sizeof(native_type));
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
			addr_.sin_family = endpoint_ipv4::family;
			addr_.sin_port = htons(port);
			addr_.sin_addr.s_addr = addr;
		}

		bool set(const char* addr_str, std::uint16_t port) noexcept
		{
			in_addr_t addr;
			int ret = inet_pton(endpoint_ipv4::family, addr_str, &addr);
			if(ret <= 0)
			{
				std::memset(&addr_, 0, sizeof(native_type));
				return false;
			}

			addr_.sin_family = endpoint_ipv4::family;
			addr_.sin_port = htons(port);
			addr_.sin_addr.s_addr = addr;

			return true;
		}

		native_type* native() noexcept{ return &addr_; }

		const char* address(char* addr_str, std::size_t len = INET_ADDRSTRLEN) noexcept
		{
#ifdef _WIN32
			return InetNtop(family, &addr_.sin_addr, addr_str, len);
#else
			return inet_ntop(family, &addr_.sin_addr, addr_str, len);
#endif
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
		native_type		addr_;
};

}//POSIX
}//Port
}//CoAP

#endif /* COAP_TE_PORT_POSIX_ENDPOINT_IPV4_HPP__ */
