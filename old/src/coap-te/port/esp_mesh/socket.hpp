#ifndef COAP_TE_PORT_ESP_MESH_SOCKET_HPP__
#define COAP_TE_PORT_ESP_MESH_SOCKET_HPP__

#include <cstdlib>
#include <cstdint>
#include "../../error.hpp"

#include "endpoint_mesh.hpp"

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

template<mesh_tos_t TOS = MESH_TOS_P2P>
class socket{
	public:
//		using handler = int;
		using endpoint = endpoint_mesh;
		socket(){}

		template<int BlockTimeMs = 0>
		std::size_t send(const void*, std::size_t, endpoint&, CoAP::Error&)  noexcept;
		template<int BlockTimeMs = 0>
		std::size_t receive(void*, std::size_t, endpoint&, CoAP::Error&) noexcept;
};

}//ESP_Mesh
}//Port
}//CoAP

#include "impl/socket_impl.hpp"

#endif /* COAP_TE_PORT_ESP_MESH_SOCKET_HPP__ */
