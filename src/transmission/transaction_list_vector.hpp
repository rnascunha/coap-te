#ifndef COAP_TE_TRANSMISSION_TRANSACTION_LIST_VECTOR_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_LIST_VECTOR_HPP__

#include "message/types.hpp"
#include <cstdint>
#include <vector>

namespace CoAP{
namespace Transmission{

template<typename Transaction>
class transaction_list_vector{
	public:
		using transaction_t = Transaction;
		using endpoint = typename Transaction::endpoint_t;

		struct node{
			transaction_t 	transaction;
		};

		transaction_list_vector();

		Transaction* find(std::uint16_t mid) noexcept;
		Transaction* find(endpoint const&, std::uint16_t mid) noexcept;
		Transaction* find_free_slot() noexcept;

		void check_all() noexcept;
		template<bool CheckEndpoint, bool CheckToken>
		Transaction* check_all_response(endpoint const&, CoAP::Message::message const&) noexcept;

		Transaction* operator[](unsigned index) noexcept
		{
			return index >= nodes_.size() ? nullptr : &nodes_[index].transaction;
		}

		/**
		 * Exclusive to transaction_list_vector
		 */
		void clear_unsused() noexcept;

	private:
		std::vector<node>	nodes_;
};

}//Transmission
}//CoAP

#include "impl/transaction_list_vector_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_LIST_VECTOR_HPP__ */
