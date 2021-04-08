#ifndef COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_HPP__

#include <cstdint>
#include <cstdlib>
#include "message/reliable/types.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Transaction,
		unsigned Size>
class transaction_list{
	public:
		using transaction_t = Transaction;

		transaction_list();

		Transaction* find_free_slot() noexcept;

		void check_all() noexcept;
		Transaction* check_all_response(int socket, CoAP::Message::Reliable::message const&) noexcept;
		void cancel_all() noexcept;
		void cancel_all(int socket) noexcept;

		Transaction* operator[](unsigned index) noexcept;
		constexpr unsigned size() const noexcept;
	private:
		transaction_t list_[Size];
};

}//CoAP
}//Transmission
}//Reliable

#include "impl/transaction_list_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_HPP__ */
