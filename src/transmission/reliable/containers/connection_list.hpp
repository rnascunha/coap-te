#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_HPP__

#include "defines/defaults.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

template<typename Connection,
		unsigned Size>
class connection_list{
	public:
		using connection_t = Connection;
		using handler = typename Connection::handler;

		connection_list();

		Connection* find(handler socket) noexcept;
		Connection* find_free_slot() noexcept;

		void close(handler socket) noexcept;
		void close_all() noexcept;

		Connection* operator[](unsigned index) noexcept;

		unsigned ocupied() const noexcept;
		constexpr unsigned size() const noexcept;
	private:
		Connection	nodes_[Size];
};

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//CoAP
}//Transmission
}//Reliable

#include "impl/connection_list_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_HPP__ */
