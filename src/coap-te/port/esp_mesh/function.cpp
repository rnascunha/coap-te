#include <cstdio>

#include "function.hpp"
#include "error.hpp"
#include "message/types.hpp"
#include "message/parser.hpp"
#include "message/serialize.hpp"
#include "message/options/options.hpp"

#ifndef MACSTR
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif /* MACSTR */
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#endif /* MAC2STR */

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

std::size_t add_host(void* buffer,
			std::size_t used,
			std::size_t buffer_size,
			mesh_addr_t const& addr,
			CoAP::Error& ec) noexcept
{
	CoAP::Message::message msg;
	CoAP::Message::parse(msg, static_cast<const uint8_t*>(buffer), used, ec);
	if(ec) return used;

	char buffer_addr[20];
	std::snprintf(buffer_addr, 20, MACSTR, MAC2STR(addr.addr));
	CoAP::Message::Serialize serial(static_cast<uint8_t*>(buffer), buffer_size, msg);
	serial.add_option({CoAP::Message::Option::code::uri_host, buffer_addr}, ec);

	return serial.used();
}

std::size_t remove_host(
			CoAP::Message::message& msg,
			void* buffer,
			std::size_t used,
			mesh_addr_t& host,
			CoAP::Error& ec) noexcept
{
	CoAP::Message::Option::option op;
	if(!CoAP::Message::Option::get_option(msg, op, CoAP::Message::Option::code::uri_host, 0))
	{
		ec = CoAP::errc::option_not_found;
		return 0;
	}

	if(endpoint_mesh::string_to_native(static_cast<const char*>(op.value), op.length, host) != 6)
	{
		ec = CoAP::errc::invalid_data;
		return 0;
	}

	CoAP::Message::Serialize serial(static_cast<uint8_t*>(buffer), used, msg);
	serial.remove_option(CoAP::Message::Option::code::uri_host, ec);

	return serial.used();
}

}//ESP_Mesh
}//Port
}//CoAP
