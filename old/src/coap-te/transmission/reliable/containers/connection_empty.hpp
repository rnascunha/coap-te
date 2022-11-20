#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_EMPTY_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_EMPTY_HPP__

#include "../types.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Handler>
class Connection_Empty{
	public:
		using handler = Handler;
		Connection_Empty(){}

		void init(handler) noexcept{}
		void init(handler, csm_configure const&) noexcept{}

		bool is_used() const noexcept{ return false; }

		void update(csm_configure const&) noexcept{}

		handler socket() const noexcept{ return 0; }
		csm_configure const& csm() const noexcept
		{
			static csm_configure csm; return csm;
		}
		csm_configure& csm()noexcept
		{
			static csm_configure csm; return csm;
		}

		void clear() noexcept{}
};

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_EMPTY_HPP__ */
