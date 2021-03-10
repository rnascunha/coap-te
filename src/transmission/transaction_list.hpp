#ifndef COAP_TE_TRANSMISSION_TRANSACTION_LIST_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_LIST_HPP__

namespace CoAP{
namespace Transmission{

template<unsigned Size,
		typename Transaction>
class transaction_list{
	public:
		using transaction_t = Transaction;
		using endpoint = typename Transaction::endpoint_t;

		struct node{
			transaction_t 	transaction;
		};

		transaction_list();

		Transaction* find(std::uint16_t mid) noexcept;
		Transaction* find(endpoint const&, std::uint16_t mid) noexcept;
		Transaction* find_free_slot() noexcept;

		void check_all(configure const&) noexcept;
		Transaction* check_all_response(CoAP::Message::message const&) noexcept;
		Transaction* check_all_response(endpoint const&, CoAP::Message::message const&) noexcept;

		Transaction* operator[](unsigned index) noexcept
		{
			return index >= Size ? nullptr : &nodes_[index].transaction;
		}

		constexpr unsigned size() const noexcept{ return Size; }
	private:
		node	nodes_[Size];
};

}//Transmission
}//CoAP

#include "impl/transaction_list_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_LIST_HPP__ */
