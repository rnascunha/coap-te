#ifndef COAP_TE_PORT_ESP_MESH_ENDPOINT_MAC_HPP__
#define COAP_TE_PORT_ESP_MESH_ENDPOINT_MAC_HPP__

#include <cstring>
#include <cstdint>
#include <cstdio>
#include <esp_mesh.h>

#include "port.hpp"

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

static constexpr const unsigned mac_str_size = 18; //17 + 1

enum class endpoint_type{
	to_root = 0,
	from_root = MESH_DATA_FROMDS,
	internal = MESH_DATA_P2P,
	external = MESH_DATA_TODS
};

class endpoint_mesh{
	public:
		using native_type = mesh_addr_t;

		endpoint_mesh()
		: type_(endpoint_type::to_root)
		{
			std::memset(&addr_, 0, sizeof(native_type));
		}

		endpoint_mesh(mesh_addr_t& ep, endpoint_type type)
		: type_(type)
		{
			std::memcpy(&addr_, &ep, sizeof(native_type));
		}

		void set(mesh_addr_t& ep, endpoint_type type)
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

		native_type* native() noexcept{ return type_ == endpoint_type::to_root ? nullptr : &addr_; }

		const char* address(char* addr_str, std::size_t len = mac_str_size) noexcept
		{
			if(type_ == endpoint_type::to_root)
			{
				snprintf(addr_str, len, "root");
			}
			else
			{
				std::snprintf(addr_str, len, MACSTR, MAC2STR(addr_));
			}
			return addr_str;
		}

		const char* host(char* host_addr, std::size_t len = mac_str_size) noexcept
		{
			return address(host_addr, len);
		}

		native_type& address() noexcept{ return addr_; }
		endpoint_type type() noexcept{ return type_; }
		std::uint16_t port() const noexcept{ return 5683; }

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
//			if(ep.type_ != type_) return false;
//			if(type_ == endpoint_type::to_root) return true;
			return std::memcmp(&addr_, &ep.addr_, sizeof(native_type)) == 0;
		}

		bool operator!=(endpoint_mesh const& ep) const noexcept
		{
			return !(*this == ep);
		}
	private:
		endpoint_type	type_;
		native_type		addr_;
};

}//ESP_Mesh
}//Port
}//CoAP

#endif /* COAP_TE_PORT_ESP_MESH_ENDPOINT_MAC_HPP__ */
