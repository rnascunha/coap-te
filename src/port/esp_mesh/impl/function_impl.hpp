#ifndef COAP_TE_PORT_ESP_MESH_FUNCTION_IMPL_HPP__
#define COAP_TE_PORT_ESP_MESH_FUNCTION_IMPL_HPP__

#include "../function.hpp"

#include "message/types.hpp"
#include "message/parser.hpp"
#include "debug/print_message.hpp"

#include "esp_log.h"

#define PROXY_TAG	"PROXY_MESH"

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

#if COAP_TE_PORT_POSIX == 1

template<int BlockTimeMs,
		typename Endpoint,
		typename CoAPEngine,
		bool AddHost = true>
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
		ESP_LOGI(PROXY_TAG, "Recv[" MACSTR " -> %s:%u][f:%d][%d]", MAC2STR(from.addr),
												ip4addr_ntoa(&to.mip.ip4),
												htons(to.mip.port),
												flag,
												data.size);

		CoAP::Error ec;
		std::size_t nsize = CoAP::Port::ESP_Mesh::add_host(data.data, data.size, buffer_size, from, ec);
		if(ec)
		{
			ESP_LOGE(PROXY_TAG, "Error adding host [%d/%s]", ec.value(), ec.message());
		}
		else
		{
			ESP_LOGI(PROXY_TAG, "Host added, new size: %zu", nsize);
			socket.send(data.data, nsize, ep_server, ec);
			if(ec)
			{
				ESP_LOGE(PROXY_TAG, "Error fowarding to server [%d/%s]", ec.value(), ec.message());
			}
			else
			{
				ESP_LOGI(PROXY_TAG, "Forward message to server %zu from " MACSTR "", data.size, MAC2STR(from.addr));
			}
		}
	}
	else if(err != ESP_ERR_MESH_TIMEOUT)
	{
		ESP_LOGE(PROXY_TAG, "Error received MESH socket data: %X (%s)", err, esp_err_to_name(err));
	}

	Endpoint ep;
	size_t size = socket.template receive<BlockTimeMs>(buffer, buffer_size, ep, ec);
	if(ec)
	{
		ESP_LOGE(PROXY_TAG, "Error received UDP socket data: %d/%s", ec.value(), ec.message());
	}
	else if(size > 0)
	{
		ESP_LOGI(PROXY_TAG, "Received[%zu]", size);

		CoAP::Message::message msg;
		CoAP::Message::parse(msg, static_cast<const uint8_t*>(buffer), size, ec);
		if(ec)
		{
			/**
			 * Error parsing message
			 */
			ESP_LOGE(PROXY_TAG, "Error parsing message");
			return;
		}

		typename CoAPEngine::endpoint to;
		std::size_t msize = CoAP::Port::ESP_Mesh::remove_host(msg, buffer, size, *to.native(), ec);
		if(ec)
		{
			/**
			 * Host option not found
			 */
			ESP_LOGE(PROXY_TAG, "Error removing host [%d/%s]", ec.value(), ec.message());

			ec.clear();
			static const char error_message[] = "Host op error/missing";
			std::size_t size_resp = engine.make_response(msg,
								buffer, buffer_size,
								CoAP::Message::code::precondition_failed,
								nullptr,
								error_message, std::strlen(error_message),
								ec);
			if(!ec)
			{
				socket.send(buffer, size_resp, ep, ec);
			}
			else
			{
				ESP_LOGE(PROXY_TAG, "Error creating response remove host [%d/%s]", ec.value(), ec.message());
			}
		}
		else
		{
			ESP_LOGI(PROXY_TAG, "Host remove[" MACSTR "], new size: %zu", MAC2STR(to.address().addr), msize);

			to.type(CoAP::Port::ESP_Mesh::endpoint_type::from_external);
			engine.get_connection().send(buffer, msize, to, ec);
			if(ec)
			{
				ESP_LOGE(PROXY_TAG, "Error sending message to host [%d/%s]", ec.value(), ec.message());
			}
			else
			{
				ESP_LOGI(PROXY_TAG, "Message send[" MACSTR "]: %zu", MAC2STR(to.address().addr), msize);
			}
		}
	}
}

#endif /* COAP_TE_PORT_POSIX == 1 */

}//ESP_Mesh
}//Port
}//CoAP

#endif /* COAP_TE_PORT_ESP_MESH_FUNCTION_IMPL_HPP__ */
