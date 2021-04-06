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
Connection* connection_list<Connection, Size>::find(int socket) noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(nodes_[i].is_busy() && nodes_[i].socket() == socket)
			return &nodes_[i];

	return nullptr;
}

template<typename Connection,
		unsigned Size>
Connection* connection_list<Connection, Size>::find_free_slot() noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(!nodes_[i].is_used())
			return &nodes_[i];

	return nullptr;
}

template<typename Connection,
		unsigned Size>
Connection* connection_list<Connection, Size>::operator[](unsigned index) noexcept
{
	return index >= Size ? nullptr : &nodes_[index];
}

template<typename Connection,
		unsigned Size>
constexpr unsigned connection_list<Connection, Size>::size() const noexcept
{
	return Size;
}

}//CoAP
}//Transmission
}//Reliable

#include "impl/connection_list_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_IMPL_HPP__ */
