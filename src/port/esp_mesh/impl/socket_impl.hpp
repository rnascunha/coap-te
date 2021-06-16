#ifndef COAP_TE_PORT_ESP_MESH_SOCKET_IMPL_HPP__
#define COAP_TE_PORT_ESP_MESH_SOCKET_IMPL_HPP__

#include "../socket.hpp"

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

template<mesh_tos_t TOS>
template<int BlockTimeMs /* = 0 */>
std::size_t socket<TOS>::send(const void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	const mesh_data_t data = {
		(uint8_t*)buffer,							//data
		static_cast<std::uint16_t>(buffer_len),		//size
		MESH_PROTO_BIN,								//proto
		TOS											//tos
	};

	if(ep.type() == endpoint_type::to_external)
	{
		/**
		 * Putting some dumb data. I don't know why, but if I but a valid
		 * MAC (from a device of the network) with the flag "to_external" it doesn't work.
		 * My guess is that you need to put something else. But it can't be all 0's also...
		 * If put everthing zero is like "to_root" flag.
		 *
		 * I don't know if this is a bug or I'm not knowning how to use the functions...
		 */
		ep.native()->mip.ip4.addr = 0;
		ep.native()->mip.port = htons(5683);
	}

	esp_err_t err = esp_mesh_send(ep.type() == endpoint_type::to_root ? nullptr : ep.native(),
						&data,
						static_cast<int>(ep.type()) | (BlockTimeMs <= 0 ? MESH_DATA_NONBLOCK : 0),
						nullptr,
						0);
	if(err != ESP_OK)
	{
		ec = CoAP::errc::socket_error;
		return 0;
	}
	return buffer_len;
}

template<mesh_tos_t TOS>
template<int BlockTimeMs /* = 0 */>
std::size_t socket<TOS>::receive(void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	mesh_data_t data;
	data.data = static_cast<uint8_t*>(buffer);
	data.size = static_cast<std::uint16_t>(buffer_len);

	int flag = 0;

	esp_err_t err = esp_mesh_recv(ep.native(), &data, BlockTimeMs, &flag, nullptr, 0);
	if(err != ESP_OK)
	{
		if(err != ESP_ERR_MESH_TIMEOUT)
		{
			ec = CoAP::errc::socket_error;
		}
		return 0;
	}
	else if(data.size > 0)
	{
		/**
		 * We need to invert the flags. If we receive the flag "from external", we
		 * need to set it "to extenal", and vice-versa, so the response can be done
		 * appropriately.
		 */
		if(flag & MESH_DATA_FROMDS) ep.type(endpoint_type::to_external);
		else if (flag & MESH_DATA_TODS) ep.type(endpoint_type::from_external);
	}

	return data.size;
}

}//ESP_Mesh
}//Port
}//CoAP

#endif /* COAP_TE_PORT_ESP_MESH_SOCKET_IMPL_HPP__ */
