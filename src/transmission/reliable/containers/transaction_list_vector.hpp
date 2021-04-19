#ifndef COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_VECTOR_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_VECTOR_HPP__

#include "message/reliable/types.hpp"
#include <cstdint>
#include <vector>

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Transaction>
class transaction_list_vector{
	public:
		using transaction_t = Transaction;
		using handler = typename Transaction::handler;

		transaction_list_vector();

		Transaction* find_free_slot() noexcept;

		void check_all() noexcept;
		Transaction* check_all_response(handler socket, CoAP::Message::Reliable::message const&) noexcept;

		Transaction* operator[](unsigned index) noexcept;
		constexpr unsigned size() const noexcept;

		/**
		 * Exclusive to transaction_list_vector
		 */
		void clear_unsused() noexcept;

	private:
		std::vector<Transaction>	nodes_;
};

}//Reliable
}//Transmission
}//CoAP

#include "impl/transaction_list_vector_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_VECTOR_HPP__ */
