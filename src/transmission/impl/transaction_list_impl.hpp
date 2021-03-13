#ifndef COAP_TE_TRANSMISSION_TRANSACTION_LIST_IMPL_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_LIST_IMPL_HPP__

#include "../transaction_list.hpp"

namespace CoAP{
namespace Transmission{

template<unsigned Size,
		typename Transaction>
transaction_list<Size, Transaction>::transaction_list(){}

template<unsigned Size,
		typename Transaction>
Transaction*
transaction_list<Size, Transaction>::
find(std::uint16_t mid) noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(nodes_[i].is_busy() && nodes_[i].transaction.mid() == mid)
			return &nodes_[i].transaction;

	return nullptr;
}

template<unsigned Size,
		typename Transaction>
Transaction*
transaction_list<Size, Transaction>::
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

template<unsigned Size,
		typename Transaction>
Transaction*
transaction_list<Size, Transaction>::
find_free_slot() noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(!nodes_[i].transaction.is_busy())
			return &nodes_[i].transaction;

	return nullptr;
}

template<unsigned Size,
		typename Transaction>
void
transaction_list<Size, Transaction>::
check_all(configure const& config) noexcept
{
	for(unsigned i = 0; i < Size; i++)
		nodes_[i].transaction.check(config);
}

template<unsigned Size,
		typename Transaction>
template<bool CheckEndpoint, bool CheckToken>
Transaction*
transaction_list<Size, Transaction>::
check_all_response(transaction_list<Size, Transaction>::endpoint const& ep,
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
