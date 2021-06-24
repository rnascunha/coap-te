#ifndef COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_IMPL_HPP__

#include "../connection.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

template<typename Handler>
Connection<Handler>::Connection(){}

template<typename Handler>
void Connection<Handler>::init(handler socket) noexcept
{
	socket_ = socket;
	csm_.reset();
}

template<typename Handler>
void Connection<Handler>::init(handler socket, csm_configure const& csm) noexcept
{
	socket_ = socket;
	csm_ = csm;
}

template<typename Handler>
bool Connection<Handler>::is_used() const noexcept
{
	return socket_ != 0;
}

template<typename Handler>
void Connection<Handler>::update(csm_configure const& csm) noexcept
{
	csm_ = csm;
}

template<typename Handler>
typename Connection<Handler>::handler 
Connection<Handler>::socket() const noexcept
{
	return socket_;
}

template<typename Handler>
csm_configure const& Connection<Handler>::csm() const noexcept
{
	return csm_;
}

template<typename Handler>
csm_configure& Connection<Handler>::csm() noexcept
{
	return csm_;
}

template<typename Handler>
void Connection<Handler>::clear() noexcept
{
	socket_ = 0;
	csm_.reset();
}

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_CONNECTION_IMPL_HPP__ */