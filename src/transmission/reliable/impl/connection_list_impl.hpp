#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_IMPL_HPP__

#include "../connection_list.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Connection,
		unsigned Size>
connection_list<Connection, Size>::connection_list(){}

template<typename Connection,
		unsigned Size>
Connection* connection_list<Connection, Size>::
find(int socket) noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(nodes_[i].socket() == socket)
			return &nodes_[i];

	return nullptr;
}

template<typename Connection,
		unsigned Size>
Connection* connection_list<Connection, Size>::
find_free_slot() noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(!nodes_[i].is_used())
			return &nodes_[i];

	return nullptr;
}

template<typename Connection,
		unsigned Size>
void
connection_list<Connection, Size>::
close(int socket) noexcept
{
	Connection* conn = find(socket);
	if(conn) conn->clear();
}

template<typename Connection,
		unsigned Size>
void
connection_list<Connection, Size>::
close_all() noexcept
{
	for(int i = 0; i < Size; i++) nodes_[i].clear();
}

template<typename Connection,
		unsigned Size>
Connection*
connection_list<Connection, Size>::
operator[](unsigned index) noexcept
{
	return index >= Size ? nullptr : &nodes_[index];
}

template<typename Connection,
		unsigned Size>
unsigned
connection_list<Connection, Size>::
ocupied() const noexcept
{
	unsigned count = 0;
	for(unsigned i = 0; i < Size; i++)
		if(nodes_[i].is_used())
			count++;

	return count;
}

template<typename Connection,
		unsigned Size>
constexpr unsigned
connection_list<Connection, Size>::
size() const noexcept
{
	return Size;
}

}//CoAP
}//Transmission
}//Reliable

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_IMPL_HPP__ */
