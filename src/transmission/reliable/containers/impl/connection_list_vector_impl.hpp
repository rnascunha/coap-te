#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_VECTOR_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_VECTOR_IMPL_HPP__

#include "../connection_list_vector.hpp"
#include <algorithm>

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Connection>
connection_list_vector<Connection>::connection_list_vector(){}

template<typename Connection>
Connection* connection_list_vector<Connection>::
find(handler socket) noexcept
{
	for(auto& c : nodes_)
		if(c.socket() == socket)
			return &c;

	return nullptr;
}

template<typename Connection>
Connection* connection_list_vector<Connection>::
find_free_slot() noexcept
{
	auto& conn = nodes_.emplace_back();

	return &conn;
}

template<typename Connection>
void
connection_list_vector<Connection>::
close(handler socket) noexcept
{
	auto it = std::remove_if(nodes_.begin(), nodes_.end(),
			[&socket](Connection& c){ return c.socket() == socket; });
//	it->clear();
	nodes_.erase(it, nodes_.end());
}

template<typename Connection>
void
connection_list_vector<Connection>::
close_all() noexcept
{
	nodes_.clear();
}

template<typename Connection>
Connection*
connection_list_vector<Connection>::
operator[](unsigned index) noexcept
{
	return index >= size() ? nullptr : &nodes_[index];
}

template<typename Connection>
unsigned
connection_list_vector<Connection>::
ocupied() const noexcept
{
	return size();
}

template<typename Connection>
unsigned
connection_list_vector<Connection>::
size() const noexcept
{
	return nodes_.size();
}

}//CoAP
}//Transmission
}//Reliable

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_VECTOR_IMPL_HPP__ */
