#ifndef COAP_TE_PORT_ESP_MESH_FUNCTION_IMPL_HPP__
#define COAP_TE_PORT_ESP_MESH_FUNCTION_IMPL_HPP__

#include "../function.hpp"

#include "../../../message/types.hpp"
#include "../../../message/parser.hpp"

#include "esp_log.h"

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

#if COAP_TE_PORT_POSIX == 1

template<int BlockTimeMs,
		typename Endpoint,
		typename CoAPEngine,
		bool AddHost /* = true */,
		bool RemoveHost /* = true */>
void proxy_forward_udp_mesh(CoAP::Port::POSIX::udp<Endpoint>& socket,
		Endpoint& ep_server,
		CoAPEngine& engine,
		void* buffer,
		std::size_t buffer_size,
		CoAP::Error& ec) noexcept
{
	mesh_data_t data;
	data.data = static_cast<uint8_t*>(buffer);
	data.size = static_cast<uint16_t>(buffer_size);
	mesh_addr_t from, to;
	int flag = 0;

	//Receiving from mesh net to external net
	esp_err_t err = esp_mesh_recv_toDS(&from,
										&to,
										&data,
										BlockTimeMs,
										&flag,
										NULL,
										0);

	if(err == ESP_OK)
	{
		if constexpr (AddHost)
		{
			data.size = CoAP::Port::ESP_Mesh::add_host(data.data,
																data.size,
																buffer_size,
																from,
																ec);
			if(ec) return;
		}

		socket.send(data.data, data.size, ep_server, ec);
		if(ec) return;
	}
	else if(err != ESP_ERR_MESH_TIMEOUT)
	{
		ec = CoAP::errc::socket_receive;
		return;
	}

	Endpoint ep;
	size_t size = socket.template receive<BlockTimeMs>(buffer, buffer_size, ep, ec);
	if(ec) return;

	if(size > 0)
	{
		CoAP::Message::message msg;
		CoAP::Message::parse(msg, static_cast<const uint8_t*>(buffer), size, ec);
		if(ec) return;

		typename CoAPEngine::endpoint to;
		std::size_t msize = CoAP::Port::ESP_Mesh::remove_host(msg, buffer, size, *to.native(), ec);
		if(ec)
		{
			CoAP::Error ecp;
			std::size_t size_resp = engine.make_response(msg,
								buffer, buffer_size,
								CoAP::Message::code::precondition_failed,
								nullptr,
								ec.message(), std::strlen(ec.message()),
								ecp);
			if(ecp) return;
			socket.send(buffer, size_resp, ep, ec);
		}
		else
		{
			to.type(CoAP::Port::ESP_Mesh::endpoint_type::from_external);
			engine.get_connection().send(buffer, msize, to, ec);
//			if(ec) return;
			//already last thing
		}
	}
}

#endif /* COAP_TE_PORT_POSIX == 1 */

}//ESP_Mesh
}//Port
}//CoAP

#endif /* COAP_TE_PORT_ESP_MESH_FUNCTION_IMPL_HPP__ */
