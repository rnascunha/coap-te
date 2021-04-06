#ifndef COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_IMPL_HPP__

#include "../transaction_list.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Transaction,
		unsigned Size>
transaction_list<Transaction, Size>::
transaction_list(){}

template<typename Transaction,
		unsigned Size>
Transaction*
transaction_list<Transaction, Size>::
find_free_slot() noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(!list_[i].is_busy()) return &list_[i];

	return nullptr;
}

template<typename Transaction,
		unsigned Size>
void
transaction_list<Transaction, Size>::
check_all() noexcept
{
	for(unsigned i = 0; i < Size; i++)
		list_[i].check();
}

template<typename Transaction,
		unsigned Size>
Transaction*
transaction_list<Transaction, Size>::
check_all_response(int socket, CoAP::Message::Reliable::message const& msg) noexcept
{
	for(unsigned i = 0; i < Size; i++)
		if(list_[i].check_response(socket, msg))
			return &list_[i];

	return nullptr;
}

template<typename Transaction,
		unsigned Size>
Transaction*
transaction_list<Transaction, Size>::
operator[](unsigned index) noexcept
{
	return index >= Size ? nullptr : &list_[index];
}

template<typename Transaction,
		unsigned Size>
constexpr unsigned
transaction_list<Transaction, Size>::
size() const noexcept{ return Size; }

}//CoAP
}//Transmission
}//Reliable

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_IMPL_HPP__ */
