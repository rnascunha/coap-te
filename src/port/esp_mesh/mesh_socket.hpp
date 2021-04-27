#ifndef COAP_TE_PORT_ESP_MESH_SOCKET_HPP__
#define COAP_TE_PORT_ESP_MESH_SOCKET_HPP__

#include <cstdlib>
#include <cstdint>
#include "error.hpp"
#include "port.hpp"

#include "endpoint_mesh.hpp"

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

class mesh{
	public:
//		using handler = int;
		using endpoint = endpoint_mesh;
		mesh();

		std::size_t send(const void*, std::size_t, endpoint&, CoAP::Error&)  noexcept;
		std::size_t receive(void*, std::size_t, endpoint&, CoAP::Error&) noexcept;
};

}//ESP_Mesh
}//Port
}//CoAP

#endif /* COAP_TE_PORT_ESP_MESH_SOCKET_HPP__ */
