#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_VECTOR_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_VECTOR_HPP__

#include "../../../defines/defaults.hpp"
#include <vector>

namespace CoAP{
namespace Transmission{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

template<typename Connection>
class connection_list_vector{
	public:
		using connection_t = Connection;
		using handler = typename Connection::handler;

		connection_list_vector();

		Connection* find(handler socket) noexcept;
		Connection* find_free_slot() noexcept;

		void close(handler socket) noexcept;
		void close_all() noexcept;

		Connection* operator[](unsigned index) noexcept;

		unsigned ocupied() const noexcept;
		unsigned size() const noexcept;
	private:
		std::vector<Connection>	nodes_;
};

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//CoAP
}//Transmission
}//Reliable

#include "impl/connection_list_vector_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_VECTOR_HPP__ */
