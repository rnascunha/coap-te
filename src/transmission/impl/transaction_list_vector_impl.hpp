#ifndef COAP_TE_TRANSMISSION_TRANSACTION_LIST_VECTOR_IMPL_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_LIST_VECTOR_IMPL_HPP__

#include "../transaction_list_vector.hpp"
#include "algorithm"

namespace CoAP{
namespace Transmission{

template<typename Transaction>
transaction_list_vector<Transaction>::transaction_list_vector()
{}

template<typename Transaction>
Transaction*
transaction_list_vector<Transaction>::
find(std::uint16_t mid) noexcept
{
	for(auto& node : nodes_)
		if(nodes_.is_busy() && nodes_.transaction.mid() == mid)
			return &nodes_.transaction;

	return nullptr;
}

template<typename Transaction>
Transaction*
transaction_list_vector<Transaction>::
find(endpoint const& ep,
		std::uint16_t mid) noexcept
{
	for(auto& node : nodes_)
		if(node.transaction.is_busy() &&
			node.transaction.mid() == mid &&
			node.transaction.endpoint() == ep)
			return &node.transaction;

	return nullptr;
}

template<typename Transaction>
Transaction*
transaction_list_vector<Transaction>::
find_free_slot() noexcept
{
	for(auto& node : nodes_)
		if(!node.transaction.is_busy())
			return &node.transaction;

	return nullptr;
}

template<typename Transaction>
void
transaction_list_vector<Transaction>::
check_all() noexcept
{
	for(auto& node : nodes_)
	{
		node.transaction.check();
	}
}

template<typename Transaction>
template<bool CheckEndpoint,
	bool CheckToken>
Transaction*
transaction_list_vector<Transaction>::
check_all_response(transaction_list_vector<Transaction>::endpoint const& ep,
		CoAP::Message::message const& msg) noexcept
{
	for(auto& node : nodes_)
		if(node.transaction.template check_response<CheckEndpoint, CheckToken>(ep, msg))
			return &node.transaction;

	return nullptr;
}

template<typename Transaction>
void
transaction_list_vector<Transaction>::
clear_unsused() noexcept
{
	auto it = std::remove_if(nodes_.begin(), nodes_.end(), [](node t){ return !t.transaction.is_busy(); });
	nodes_.erase(it, nodes_.end());
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_LIST_VECTOR_IMPL_HPP__ */
