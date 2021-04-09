#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_HPP__

#include "../types.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

class Connection{
	public:
		using handler = int;
		Connection();

		void init(handler socket) noexcept;
		void init(handler socket, csm_configure const& csm) noexcept;

		bool is_used() const noexcept;

		void update(csm_configure const& csm) noexcept;

		handler socket() const noexcept;
		csm_configure const& csm() const noexcept;
		csm_configure& csm()noexcept;

		void clear() noexcept;
	private:
		handler 			socket_ = 0;
		csm_configure	csm_;
};

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_HPP__ */
