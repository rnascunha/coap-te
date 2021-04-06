#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_HPP__

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Connection,
		unsigned Size>
class connection_list{
	public:
		using connection_t = Connection;

		connection_list();

		Connection* find(int socket) noexcept;
		Connection* find_free_slot() noexcept;

		Connection* operator[](unsigned index) noexcept
		{
			return index >= Size ? nullptr : &nodes_[index];
		}

		constexpr unsigned size() const noexcept{ return Size; }
	private:
		Connection	nodes_[Size];
};

}//CoAP
}//Transmission
}//Reliable

#include "impl/connection_list_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_LIST_HPP__ */
