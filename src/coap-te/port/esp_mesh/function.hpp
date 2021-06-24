#ifndef COAP_TE_PORT_ESP_MESH_FUNCTION_HPP__
#define COAP_TE_PORT_ESP_MESH_FUNCTION_HPP__

#include <cstdlib>
#include "../../error.hpp"
#include "../../message/types.hpp"

#include <esp_mesh.h>

#if COAP_TE_PORT_POSIX == 1
#include "../posix/udp_socket.hpp"
#endif /* COAP_TE_PORT_POSIX == 1 */

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

std::size_t add_host(void* buffer,
			std::size_t used,
			std::size_t buffer_size,
			mesh_addr_t const&,
			CoAP::Error& ec) noexcept;

std::size_t remove_host(CoAP::Message::message&,
			void* buffer,
			std::size_t used,
			mesh_addr_t&,
			CoAP::Error& ec) noexcept;

#if COAP_TE_PORT_POSIX == 1
template<int BlockTimeMs,
		typename Endpoint,
		typename CoAPEngine,
		bool AddHost = true,
		bool RemoveHost = true>
void proxy_forward_udp_mesh(CoAP::Port::POSIX::udp<Endpoint>& socket,
		Endpoint& ep_server,
		CoAPEngine& engine,
		void* buffer,
		std::size_t buffer_size,
		CoAP::Error& ec) noexcept;
#endif /* COAP_TE_PORT_POSIX == 1 */

}//ESP_Mesh
}//Port
}//CoAP

#include "impl/function_impl.hpp"

#endif /* COAP_TE_PORT_ESP_MESH_FUNCTION_HPP__ */
