#ifndef COAP_TE_PORT_ESP_MESH_ENDPOINT_MAC_HPP__
#define COAP_TE_PORT_ESP_MESH_ENDPOINT_MAC_HPP__

#include <cstring>
#include <cstdint>
#include <cstdio>
#include <esp_mesh.h>

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

#ifndef MACSTR
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif /* MACSTR */
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#endif /* MAC2STR */

static constexpr const unsigned mac_str_size = 18; //17 + 1
static constexpr const uint16_t	default_port = 5683;

enum class endpoint_type{
	to_root = 0,
	internal = MESH_DATA_P2P,
	from_external = MESH_DATA_FROMDS,
	to_external = MESH_DATA_TODS
};

class endpoint_mesh{
	public:
		using native_type = mesh_addr_t;

		endpoint_mesh()
		{
			set();
		}

		endpoint_mesh(mesh_addr_t const& ep, endpoint_type type)
		{
			set(ep, type);
		}

		endpoint_mesh(endpoint_mesh const& ep)
		{
			set(ep);
		}

		void set(endpoint_mesh const& ep) noexcept
		{
			type_ = ep.type();
			std::memcpy(&addr_, &ep.address(), sizeof(native_type));
		}

		void set(mesh_addr_t const& ep, endpoint_type type)
		{
			if(type == endpoint_type::to_root)
			{
				set();
				return;
			}
			std::memcpy(&addr_, &ep, sizeof(native_type));
			type_ = type;
		}

		void set()
		{
			std::memset(&addr_, 0, sizeof(native_type));
			type_ = endpoint_type::to_root;
		}

		native_type* native() noexcept
		{
			return &addr_;
		}

		const char* address(char* addr_str, std::size_t len = mac_str_size) noexcept
		{
			if(type_ == endpoint_type::to_root)
			{
				snprintf(addr_str, len, "root");
			}
			else
			{
				std::snprintf(addr_str, len, MACSTR, MAC2STR(addr_.addr));
			}
			return addr_str;
		}

		const char* host(char* host_addr, std::size_t len = mac_str_size) noexcept
		{
			return address(host_addr, len);
		}

		native_type const& address() const noexcept{ return addr_; }
		endpoint_type type() const noexcept{ return type_; }
		std::uint16_t port() const noexcept{ return default_port; }

		void type(endpoint_type mtype) noexcept
		{
			type_ = mtype;
		}

		endpoint_mesh& operator=(endpoint_mesh const& ep) noexcept
		{
			type_ = ep.type_;
			std::memcpy(&addr_, &ep.addr_, sizeof(native_type));
			return *this;
		}

		bool operator==(endpoint_mesh const& ep) const noexcept
		{
			if(ep.type_ != type_) return false;
			if(type_ == endpoint_type::to_root) return true;
			return std::memcmp(&addr_, &ep.addr_, sizeof(native_type)) == 0;
		}

		bool operator!=(endpoint_mesh const& ep) const noexcept
		{
			return !(*this == ep);
		}

		static unsigned string_to_native(const char* str, unsigned size, native_type& addr) noexcept
		{
			unsigned count = 0, count_part = 0;
			uint8_t value = 0;
			for(unsigned i = 0; i <= size; i++)
			{
				if(str[i] == ':' || i == size)
				{
					if(count_part == 0) addr.addr[count] = 0;
					else if (count_part == 1 || count_part == 2) addr.addr[count] = value;
					else return count;

					count_part = 0;
					count++;
					value = 0;

					continue;
				}
				if(count_part >= 2)
				{
					return count;
				}

				if(str[i] >= '0' && str[i] <= '9')
				{
					value = (str[i] - '0') | (value << (count_part * 4));
				}
				else if(str[i] >= 'a' && str[i] <= 'f')
				{
					value = (str[i] - 'a' + 10) | (value << (count_part * 4));
				}
				else if(str[i] >= 'A' && str[i] <= 'F')
				{
					value = (str[i] - 'A' + 10) | (value << (count_part * 4));
				}
				else
				{
					return count;
				}
				count_part++;
			}
			return count;
		}
	private:
		endpoint_type	type_;
		native_type		addr_;
};

}//ESP_Mesh
}//Port
}//CoAP

#endif /* COAP_TE_PORT_ESP_MESH_ENDPOINT_MAC_HPP__ */
