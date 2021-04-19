#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_HPP__

#include "defines/defaults.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

template<typename Conn,
		unsigned Size>
class connection_list{
	public:
		using connection_t = Conn;
		using handler = typename Conn::handler;

		connection_list();

		Conn* find(handler socket) noexcept;
		Conn* find_free_slot() noexcept;

		void close(handler socket) noexcept;
		void close_all() noexcept;

		Conn* operator[](unsigned index) noexcept;

		unsigned ocupied() const noexcept;
		constexpr unsigned size() const noexcept;
	private:
		Conn	nodes_[Size];
};

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//CoAP
}//Transmission
}//Reliable

#include "impl/connection_list_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_HPP__ */
