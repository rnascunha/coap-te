#ifndef COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_VECTOR_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_VECTOR_IMPL_HPP__

#include "../transaction_list_vector.hpp"
#include <algorithm>

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Transaction>
transaction_list_vector<Transaction>::transaction_list_vector()
{}

template<typename Transaction>
Transaction*
transaction_list_vector<Transaction>::
find_free_slot() noexcept
{
	for(auto& node : nodes_)
		if(!node.is_busy())
			return &node;

	auto& node = nodes_.emplace_back();

	return &node;
}

template<typename Transaction>
void
transaction_list_vector<Transaction>::
check_all() noexcept
{
	for(auto& node : nodes_)
	{
		node.check();
	}
}

template<typename Transaction>
Transaction*
transaction_list_vector<Transaction>::
check_all_response(handler socket,
		CoAP::Message::Reliable::message const& msg) noexcept
{
	for(auto& node : nodes_)
		if(node.check_response(socket, msg))
			return &node;

	return nullptr;
}

template<typename Transaction>
Transaction*
transaction_list_vector<Transaction>::
operator[](unsigned index) noexcept
{
	return index >= nodes_.size() ? nullptr : &nodes_[index];
}

template<typename Transaction>
constexpr unsigned
transaction_list_vector<Transaction>::
size() const noexcept
{
	return nodes_.size();
}

template<typename Transaction>
void
transaction_list_vector<Transaction>::
clear_unsused() noexcept
{
	auto it = std::remove_if(nodes_.begin(), nodes_.end(), [](Transaction& t){ return !t.is_busy(); });
	nodes_.erase(it, nodes_.end());
}

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_VECTOR_IMPL_HPP__ */
