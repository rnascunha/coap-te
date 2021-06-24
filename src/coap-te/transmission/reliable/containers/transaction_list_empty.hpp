#ifndef COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_EMPTY_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_EMPTY_HPP__

#include <cstdint>
#include <cstdlib>
#include "../../../message/reliable/types.hpp"
#include "transaction_empty.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

class transaction_list_empty{
	public:
		using transaction_t = transaction_empty;

		transaction_list_empty(){}

		transaction_t* find_free_slot() noexcept{ return nullptr; }

		void check_all() noexcept{}
		transaction_t* check_all_response(int, CoAP::Message::Reliable::message const&) noexcept
		{ return nullptr; }

		void cancel_all() noexcept;
		void cancel_all(int) noexcept{}

		transaction_t* operator[](unsigned) noexcept{ return nullptr; }
		constexpr unsigned size() const noexcept{ return 0; }
};

}//CoAP
}//Transmission
}//Reliable

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_LIST_EMPTY_HPP__ */
