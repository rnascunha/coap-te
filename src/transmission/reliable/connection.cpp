#include "connection.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

Connection::Connection(){}

void Connection::init(int socket) noexcept
{
	socket_ = socket;
	csm_.reset();
}

void Connection::init(int socket, csm_configure const& csm) noexcept
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

int Connection::socket() const noexcept
{
	return socket;
}

csm_configure const& Connection::csm() const noexcept
{
	return csm_;
}

void Connection::clear()
{
	socket_ = 0;
	csm_.reset();
}

}//Reliable
}//Transmission
}//CoAP
