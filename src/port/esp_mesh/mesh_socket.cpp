#include "mesh_socket.hpp"
#include <esp_err.h>
#include <esp_mesh.h>

namespace CoAP{
namespace Port{
namespace ESP_Mesh{

mesh::mesh(){}

std::size_t mesh::send(const void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec)  noexcept
{
	mesh_data_t data;
	data.data = buffer;
	data.size = static_cast<std::uint16_t>(buffer_len);
	data.proto = MESH_PROTO_BIN;
	data.tos = MESH_TOS_DEF; //no retransmission

	esp_err_t err = esp_mesh_send(ep.native(),
						&data, ep.type() | MESH_DATA_NONBLOCK,
						nullptr,
						0);
	if(err != ESP_OK)
	{
		ec = CoAP::errc::socket_error;
		return 0;
	}
	return buffer_len;
}

std::size_t mesh::receive(void* buffer, std::size_t buffer_len, endpoint& ep, CoAP::Error& ec) noexcept
{
	mesh_data_t data;
	data.data = buffer;
	data.size = static_cast<std::uint16_t>(buffer_len);

	int flag = 0;

	esp_err_t err = esp_mesh_recv(ep.native(), &data, 0, &flag, nullptr, 0);
	if(err != ESP_OK)
	{
		ec = CoAP::errc::socket_error;
		return 0;
	}

	if(flag & MESH_DATA_FROMDS) ep.type(endpoint_type::external);
	else if (flag & MESH_DATA_TODS) ep.type(endpoint_type::internal);

	return data.size;
}

}//ESP_Mesh
}//Port
}//CoAP
