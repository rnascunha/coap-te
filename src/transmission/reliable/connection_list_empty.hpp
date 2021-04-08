#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_EMPTY_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_EMPTY_HPP__

#include "defines/defaults.hpp"
#include "connection_empty.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

class connection_list_empty{
	public:
		using connection_t = Connection_Empty;

		connection_list_empty(){}

		connection_t* find(int) noexcept{ return nullptr; }
		connection_t* find_free_slot() noexcept{ return nullptr; }

		void close(int) noexcept{}
		void close_all() noexcept{}

		connection_t* operator[](unsigned) noexcept{ return nullptr; }

		unsigned ocupied() const noexcept{ return 0; }
		constexpr unsigned size() const noexcept { return 0; }
};

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//CoAP
}//Transmission
}//Reliable

#include "impl/connection_list_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_EMPTY_HPP__ */
