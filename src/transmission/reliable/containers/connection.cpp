#include "connection.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

Connection::Connection(){}

void Connection::init(handler socket) noexcept
{
	socket_ = socket;
	csm_.reset();
}

void Connection::init(handler socket, csm_configure const& csm) noexcept
{
	socket_ = socket;
	csm_ = csm;
}

bool Connection::is_used() const noexcept
{
	return socket_ != 0;
}

void Connection::update(csm_configure const& csm) noexcept
{
	csm_ = csm;
}

Connection::handler Connection::socket() const noexcept
{
	return socket_;
}

csm_configure const& Connection::csm() const noexcept
{
	return csm_;
}

csm_configure& Connection::csm() noexcept
{
	return csm_;
}

void Connection::clear() noexcept
{
	socket_ = 0;
	csm_.reset();
}

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//Reliable
}//Transmission
}//CoAP
