#ifndef COAP_TE_TRANSMISSION_TRANSACTION_LIST_IMPL_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_LIST_IMPL_HPP__

#include "../transaction_list.hpp"

namespace CoAP{
namespace Transmission{

template<typename Transaction,
		unsigned Size>
transaction_list<Transaction, Size>::transaction_list()
{
	static_assert(Size > 0, "Transaction size (capacity) must be > 0");
}

template<typename Transaction,
		unsigned Size>
Transaction*
transaction_list<Transaction, Size>::
find(std::uint16_t mid) noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(nodes_[i].is_busy() && nodes_[i].transaction.mid() == mid)
			return &nodes_[i].transaction;

	return nullptr;
}

template<typename Transaction,
		unsigned Size>
Transaction*
transaction_list<Transaction, Size>::
find(endpoint const& ep,
		std::uint16_t mid) noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(nodes_[i].transaction.is_busy() &&
			nodes_[i].transaction.mid() == mid &&
			nodes_[i].transaction.endpoint() == ep)
			return &nodes_[i].transaction;

	return nullptr;
}

template<typename Transaction,
		unsigned Size>
Transaction*
transaction_list<Transaction, Size>::
find_free_slot() noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(!nodes_[i].transaction.is_busy())
			return &nodes_[i].transaction;

	return nullptr;
}

template<typename Transaction,
		unsigned Size>
void
transaction_list<Transaction, Size>::
check_all() noexcept
{
	for(unsigned i = 0; i < Size; i++)
		nodes_[i].transaction.check();
}

template<typename Transaction,
		unsigned Size>
template<bool CheckEndpoint,
	bool CheckToken>
Transaction*
transaction_list<Transaction, Size>::
check_all_response(transaction_list<Transaction, Size>::endpoint const& ep,
		CoAP::Message::message const& msg) noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(nodes_[i].transaction.template check_response<CheckEndpoint, CheckToken>(ep, msg))
			return &nodes_[i].transaction;

	return nullptr;
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_LIST_IMPL_HPP__ */
