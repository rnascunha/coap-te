#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_HPP__

#include "types.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

class Connection{
	public:
		Connection();

		void init(int socket) noexcept;
		void init(int socket, csm_configure const& csm) noexcept;

		bool is_used() const noexcept;

		void update(csm_configure const& csm) noexcept;

		int socket() const noexcept;
		csm_configure const& csm() const noexcept;

		void clear() noexcept;
	private:
		int 			socket_ = 0;
		csm_configure	csm_;
};

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_HPP__ */
